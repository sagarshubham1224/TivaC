/*
 * AK8963.c
 *
 *  Created on: 15-Mar-2018
 *      Author: sagar
 */


#include "AK8963.h"



//
//static uint8_t rawMagRegisterDatas[7] = {0, 0, 0, 0, 0, 0, 0};







extern AK8963_DEVICE* initialize_AK8963(AK8963_DEVICE* AK8963DevicePointer,
                                        I2C_PERIPHERAL I2CPeripheralCode,
                                        AK8963_OPMODES opModeVal,
                                        AK8963_DATARES dataResVal)
{


    //If AK8963 is part of MPU-9250, enable pass through mode for the MPU9250.
#ifdef MPU9250_PRESENT
    enableMPU9250Bypass(I2CPeripheralCode);
#endif

    //Initialize I2C peripheral and module to be
    initI2CMaster(&AK8963DevicePointer->I2CDevice, I2CPeripheralCode, AK8963_SLAVE_ADDRESS, FAST_400k_I2C);

    //	Soft Reset all Registers once.
    //	Though it will set device to PD mode, yet we will do it once again to initiate FUSE_ROM_MODE.
    I2CWriteByte(&AK8963DevicePointer->I2CDevice, AK8963_CONTROL_REGISTER2,AK8963_SOFT_RESET);
    milliSecondDelay(10);

    AK8963DevicePointer->operationMode = opModeVal;
    AK8963DevicePointer->dataResolution = dataResVal;
    // POWER DOWN AK8963.
    setMode_AK8963(AK8963DevicePointer, POWER_DOWN_MODE);


    // Set Fuse ROM Mode for reading Sensitivity adjustment values.
    // 	Remember to go back to power down mode once done. Then only change to another mode.
    setMode_AK8963(AK8963DevicePointer, FUSE_ROM_MODE);


    // Read adjustment sensitivity values.
    uint8_t sensitivityAdjRegisterValuesArray[3] = {0 , 0, 0};
    I2CReadBytes(&AK8963DevicePointer->I2CDevice, AK8963_SENSE_ADJ_X_REGISTER, 3, &sensitivityAdjRegisterValuesArray[0]);

    // Form senseAdjValues.
    AK8963DevicePointer->sensitivityAdjustment_ResolutionValues[AK8963_X_AXIS] = (float)(sensitivityAdjRegisterValuesArray[AK8963_X_AXIS] -128)/ 256.0f + 1.0f;
    AK8963DevicePointer->sensitivityAdjustment_ResolutionValues[AK8963_Y_AXIS] = (float)(sensitivityAdjRegisterValuesArray[AK8963_Y_AXIS] -128)/ 256.0f + 1.0f;
    AK8963DevicePointer->sensitivityAdjustment_ResolutionValues[AK8963_Z_AXIS] = (float)(sensitivityAdjRegisterValuesArray[AK8963_Z_AXIS] -128)/ 256.0f + 1.0f;

    // Return to POWER DOWN MODE.
    setMode_AK8963(AK8963DevicePointer, POWER_DOWN_MODE);

    // Set operation Mode and data resolution.
    setMode_AK8963(AK8963DevicePointer, opModeVal);




    // Set magnetic Resolution according to data width.
    switch (AK8963DevicePointer->dataResolution) {
    case RESOLUTION_14_BIT:
        AK8963DevicePointer->sensitivityAdjustment_ResolutionValues[AK8963_X_AXIS] *= (4912.0f / 8190.0f);   	//  ~= 0.6uT/LSB
        AK8963DevicePointer->sensitivityAdjustment_ResolutionValues[AK8963_Y_AXIS] *= (4912.0f / 8190.0f);   	//  ~= 0.6uT/LSB
        AK8963DevicePointer->sensitivityAdjustment_ResolutionValues[AK8963_Z_AXIS] *= (4912.0f / 8190.0f);   	//  ~= 0.6uT/LSB
        break;
    case RESOLUTION_16_BIT:
        AK8963DevicePointer->sensitivityAdjustment_ResolutionValues[AK8963_X_AXIS] *= (4912.0f/32760.0f);		//  ~= 0.15uT/LSB
        AK8963DevicePointer->sensitivityAdjustment_ResolutionValues[AK8963_Y_AXIS] *= (4912.0f/32760.0f);		//  ~= 0.15uT/LSB
        AK8963DevicePointer->sensitivityAdjustment_ResolutionValues[AK8963_Z_AXIS] *= (4912.0f/32760.0f);		//  ~= 0.15uT/LSB
        break;
    }

    // Set Hard Iron Offset Values.
    //	setHardIronOffsetValues(AK8963DevicePointer, -16.529289108769877f, 39.64472517265603f, 14.968989182873017f);
    setHardIronOffsetValues(AK8963DevicePointer,
                            -0.10342108503832748f + -16.529289108769877f+ 1.9443690875153585f,
                            0.14458552365075072f + 39.64472517265603f + 1.3183049116007899f,
                            0.22103032829652136f + 14.968989182873017f + 3.0937458164787373f);


    return AK8963DevicePointer;
}


extern void setHardIronOffsetValues(AK8963_DEVICE* AK8963DevicePointer, float xOffsetValue, float yOffsetValue, float zOffsetValue)
{
    AK8963DevicePointer->hardIronOffsetValues[AK8963_X_AXIS] = xOffsetValue;
    AK8963DevicePointer->hardIronOffsetValues[AK8963_Y_AXIS] = yOffsetValue;
    AK8963DevicePointer->hardIronOffsetValues[AK8963_Z_AXIS] = zOffsetValue;

}


#ifdef SELFTEST_NEEDED
extern bool isSelfTestPassed_AK8963(AK8963_DEVICE* AK8963DevicePointer, UARTDEVICE* UARTDevicePointer)
{

    /*
     *
     * 	<Self-test Sequence>
	(1)	Set Power-down mode
	(2)	Write “1”   to SELF bit of ASTC register
	(3)	Set Self-test Mode
	(4)	Check Data Ready or not by any of the following method.
			- Polling DRDY bit of ST1 register
			- Monitor DRDY pin
		When Data Ready, proceed to the next step.
	(5)	Read measurement data (HXL to HZH)
	(6)	Write “0”   to SELF bit of ASTC register
	(7)	Set Power-down mode

	<Self-test Judgment>
	When measurement data read by the above sequence is in the range of following table after
	sensitivity adjustment, AK8963 is working normally.
	14-	bit output (BIT=“0”)
	HX[15:0] -50 =< HX =< 50
	HY[15:0] -50 =< HY =< 50
	HZ[15:0] -800 =< HZ =< -200

	16-bit output (BIT=“1”)
	HX[15:0] -200 =< HX =< 200
	HY[15:0] -200 =< HY =< 200
	HZ[15:0] -3200 =< HZ =< -800
     *
     *
     */

    bool selfTestOK = false;				// boolean variable to save result.
    uint8_t selfTestRegisterVal = 0;		// variable to save register values.
    uint8_t lastOperationMode = 0;			// variable to save present operation mode and restore it once done here.

    /*
     * Save the last state of device before changing it to Power Down Mode.
     */
    lastOperationMode = I2CReadByte(&AK8963DevicePointer->I2CDevice, AK8963_CONTROL_REGISTER1);
    UARTWrite(UARTDevicePointer, "Mode Res: 0b%b\n",lastOperationMode);

    /*
     *	Step 1. Set Power Down Mode.
     */
    setMode_AK8963(AK8963DevicePointer, POWER_DOWN_MODE);

    /*
     * Step 2. Set only SELF bit to "1" of self test register for self test.
     * For this, read its value, set SELF bit to 1 and write it back.
     * It is done so because,
     * 	1. This would be portable for future.
     * 	2. I am a wuss.
     */
    selfTestRegisterVal = I2CReadByte(&AK8963DevicePointer->I2CDevice, AK8963_SELF_TEST_CONTROL_REGISTER);  		// read the register.
    selfTestRegisterVal |= (1 << AK8963_SELF_TEST_CONTROL_REGISTER_SELF_BIT);							// mask all but SELF Bit and set it to "1".
    I2CWriteByte(&AK8963DevicePointer->I2CDevice, AK8963_SELF_TEST_CONTROL_REGISTER, selfTestRegisterVal);			// Write it back to register.

    /*
     *	Step 3. Set Self Test Mode.
     */
    setMode_AK8963(AK8963DevicePointer, SELF_TEST_MODE);

    /*
     * Step 4. Poll DRDY bit of ST1 Register.
     */
    selfTestRegisterVal = I2CReadByte(&AK8963DevicePointer->I2CDevice, AK8963_STATUS1_REGISTER);		// read the register.
    selfTestRegisterVal &= (1 << AK8963_STATUS1_REGISTER_DRDY_BIT);							// mask all but DRDY Bit.
    selfTestRegisterVal = selfTestRegisterVal >> AK8963_STATUS1_REGISTER_DRDY_BIT;			// Bring DRDY bit to units place.
    while(! selfTestRegisterVal) // keep checking till DRDY becomes 1.
    {
        selfTestRegisterVal = I2CReadByte(&AK8963DevicePointer->I2CDevice, AK8963_STATUS1_REGISTER);	// read the register.
        selfTestRegisterVal &= (1 << AK8963_STATUS1_REGISTER_DRDY_BIT);						// mask all but DRDY Bit.
        selfTestRegisterVal = selfTestRegisterVal >> AK8963_STATUS1_REGISTER_DRDY_BIT;		// Bring DRDY bit to units place.
    }


    /*
     * Step 5. DRDY BIT became 1, so now read data from data registers.
     */
    I2CReadBytes(&AK8963DevicePointer->I2CDevice, AK8963_XOUT_L_REGISTER,6,&AK8963DevicePointer->rawMagRegisterDatas[0]);

    /*
     * Read ST2 Register to confirm read and check for overflow error.
     */
    selfTestRegisterVal = I2CReadByte(&AK8963DevicePointer->I2CDevice, AK8963_STATUS2_REGISTER);
    /*
     * Get HOFL Bit and check for errors.
     */
    selfTestRegisterVal &= (1 << AK8963_STATUS2_REGISTER_HOFL_BIT);							//  Mask all but HOFL Bit.
    selfTestRegisterVal = selfTestRegisterVal >> AK8963_STATUS2_REGISTER_HOFL_BIT;			//  Bring the Bit to unit's place.
    uint8_t looper = 0;
    for(looper = 0; looper <6;++looper){
        UARTWrite(UARTDevicePointer, "%d ", AK8963DevicePointer->rawMagRegisterDatas[looper]);
    }
    UARTWrite(UARTDevicePointer, "\t");

    if(selfTestRegisterVal != 1) // If no overflow occurred.
    {

        // Combine H_REGISTER and L_REGISTER Values as per Little Endian Scheme.
        AK8963DevicePointer->magneticFluxDensityValues[AK8963_X_AXIS] = (float)((int16_t)( AK8963DevicePointer->rawMagRegisterDatas[AK8963_XH_REGISTER_POS] << 8 )
                | AK8963DevicePointer->rawMagRegisterDatas[AK8963_XL_REGISTER_POS] );


        AK8963DevicePointer->magneticFluxDensityValues[AK8963_Y_AXIS] = (float)((int16_t)( AK8963DevicePointer->rawMagRegisterDatas[AK8963_YH_REGISTER_POS] << 8 )
                | AK8963DevicePointer->rawMagRegisterDatas[AK8963_YL_REGISTER_POS] );


        AK8963DevicePointer->magneticFluxDensityValues[AK8963_Z_AXIS] = (float)((int16_t)( AK8963DevicePointer->rawMagRegisterDatas[AK8963_ZH_REGISTER_POS] << 8 )
                | AK8963DevicePointer->rawMagRegisterDatas[AK8963_ZL_REGISTER_POS] );

        for(looper = 0; looper <3;++looper){
            UARTWrite(UARTDevicePointer, "%f ", AK8963DevicePointer->magneticFluxDensityValues[looper]);
        }
        UARTWrite(UARTDevicePointer, "\t");

        //	Adjust for sensitivity Adjustment values.
        AK8963DevicePointer->magneticFluxDensityValues[AK8963_X_AXIS] *= AK8963DevicePointer->sensitivityAdjustment_ResolutionValues[AK8963_X_AXIS];
        AK8963DevicePointer->magneticFluxDensityValues[AK8963_Y_AXIS] *= AK8963DevicePointer->sensitivityAdjustment_ResolutionValues[AK8963_Y_AXIS];
        AK8963DevicePointer->magneticFluxDensityValues[AK8963_Z_AXIS] *= AK8963DevicePointer->sensitivityAdjustment_ResolutionValues[AK8963_Z_AXIS];

        //Adjust for Resoultion values which have been multiplied in advance.
        switch (AK8963DevicePointer->dataResolution) {
        case RESOLUTION_14_BIT:
            AK8963DevicePointer->magneticFluxDensityValues[AK8963_X_AXIS] =	(AK8963DevicePointer->magneticFluxDensityValues[AK8963_X_AXIS]) / (4912.0f / 8190.0f);
            AK8963DevicePointer->magneticFluxDensityValues[AK8963_Y_AXIS] =	(AK8963DevicePointer->magneticFluxDensityValues[AK8963_Y_AXIS]) / (4912.0f / 8190.0f);
            AK8963DevicePointer->magneticFluxDensityValues[AK8963_Z_AXIS] =	(AK8963DevicePointer->magneticFluxDensityValues[AK8963_Z_AXIS]) / (4912.0f / 8190.0f);

            break;
        case RESOLUTION_16_BIT:
            AK8963DevicePointer->magneticFluxDensityValues[AK8963_X_AXIS] = (AK8963DevicePointer->magneticFluxDensityValues[AK8963_X_AXIS]) / (4912.0f / 32760.0f);
            AK8963DevicePointer->magneticFluxDensityValues[AK8963_Y_AXIS] = (AK8963DevicePointer->magneticFluxDensityValues[AK8963_Y_AXIS]) / (4912.0f / 32760.0f);
            AK8963DevicePointer->magneticFluxDensityValues[AK8963_Z_AXIS] = (AK8963DevicePointer->magneticFluxDensityValues[AK8963_Z_AXIS]) / (4912.0f / 32760.0f);
            break;
        }

        for(looper = 0; looper <3;++looper){
            UARTWrite(UARTDevicePointer, "%f ", AK8963DevicePointer->magneticFluxDensityValues[looper]);
        }
        UARTWrite(UARTDevicePointer, "\n");

        // Judgment. I know, comparing floats like this is not wise.
        // This is quick and dirty, and I will bathe in coffee once this is done.
        // TODO: Create a library/function to handle floating point comparisons.
        int16_t XValue = (int16_t)(AK8963DevicePointer->magneticFluxDensityValues[AK8963_X_AXIS]);
        int16_t YValue = (int16_t)(AK8963DevicePointer->magneticFluxDensityValues[AK8963_Y_AXIS]);
        int16_t ZValue = (int16_t)(AK8963DevicePointer->magneticFluxDensityValues[AK8963_Z_AXIS]);
        char *isXOK, *isYOK, *isZOK;

        switch (AK8963DevicePointer->dataResolution) {  //Compare Ranges according to type of Data Resolution.
        case RESOLUTION_14_BIT:
            UARTWrite(UARTDevicePointer, "IN 14\n");
            isXOK = (XValue >= -50 && XValue <= 50)? "TRUE":"FALSE";
            isYOK = (YValue >= -50 && YValue <= 50)? "TRUE":"FALSE";
            isZOK = (ZValue >= -800 && ZValue <= -200)? "TRUE":"FALSE";
            UARTWrite(UARTDevicePointer, "X: %s Y: %s Z: %s\n",isXOK,isYOK,isZOK);
            if( (XValue >= -50 && XValue <= 50) &&
                    (YValue >= -50 && YValue <= 50) &&
                    (ZValue >= -800 && ZValue <= -200) ){
                selfTestOK = true;
            }
            break;
        case RESOLUTION_16_BIT:
            UARTWrite(UARTDevicePointer, "IN 16\n");
            isXOK = (XValue >= -200 && XValue <= 200)? "TRUE":"FALSE";
            isYOK = (YValue >= -200 && YValue <= 200)? "TRUE":"FALSE";
            isZOK = (ZValue >= -3200 && ZValue <= -800)? "TRUE":"FALSE";
            UARTWrite(UARTDevicePointer, "X: %s Y: %s Z: %s\n",isXOK,isYOK,isZOK);
            if( (XValue >= -200 && XValue <= 200)
                    && (YValue >= -200 && YValue <= 200)
                    && (ZValue >= -3200 && ZValue <= -800) ){
                selfTestOK = true;
            }
            break;
        }
    }


    // Step 6. Set SELF Bit in Self Test Control register back to "0".
    // Again follow procedure of Step 2.
    selfTestRegisterVal = I2CReadByte(&AK8963DevicePointer->I2CDevice, AK8963_SELF_TEST_CONTROL_REGISTER);  // read the register.
    selfTestRegisterVal &= ~(1 << AK8963_SELF_TEST_CONTROL_REGISTER_SELF_BIT);								// mask all but SELF Bit and set it to "0".
    I2CWriteByte(&AK8963DevicePointer->I2CDevice, AK8963_SELF_TEST_CONTROL_REGISTER, selfTestRegisterVal);	// Write it to register.

    //	Step 7. Set Power Down Mode.
    setMode_AK8963(AK8963DevicePointer, /*UARTDevicePointer,*/ POWER_DOWN_MODE);

    // Return to the mode that was previously set.
    setMode_AK8963(AK8963DevicePointer, /*UARTDevicePointer,*/ lastOperationMode);


    //	Return the Result.
    return selfTestOK;

}

#endif


extern uint8_t wai_AK8963(AK8963_DEVICE* AK8963DevicePointer)
{
    uint8_t whoAmIRegisterValue = I2CReadByte(&AK8963DevicePointer->I2CDevice, AK8963_WHO_AM_I_REGISTER);
    return whoAmIRegisterValue;
}


extern bool readRawMagFluxDataNB_AK8963(AK8963_DEVICE* AK8963DevicePointer)
{
    bool isDataReady = false;
    //	Step 1. Check if Data is available to be read using DRDY Bit of ST1.
    uint8_t st1RegisterVal = I2CReadByte(&AK8963DevicePointer->I2CDevice, AK8963_STATUS1_REGISTER);	// read the register.
    st1RegisterVal &= (1 << AK8963_STATUS1_REGISTER_DRDY_BIT);							// mask all but DRDY Bit.

    //	Step 2. If new data is ready, read it, else give error false
    if(st1RegisterVal) 	//	DRDY is "1" and hence proceed to read Data.
    {
        // Step 3. Read All Data Registers as well as ST2 in one go.
        I2CReadBytes(&AK8963DevicePointer->I2CDevice, AK8963_XOUT_L_REGISTER,7,&AK8963DevicePointer->rawMagRegisterDatas[0]);

        // Step 4. Check HOFL Bit of ST2, if it is 1, Magnetic Overflow has occurred, return a false.
        uint8_t st2RegisterVal = AK8963DevicePointer->rawMagRegisterDatas[AK8963_ST2_REGISTER_POS];		// read the register.
        //		UARTWrite(UARTDevicePointer,"ST2: 0b%b\n",st2RegisterVal);
        st2RegisterVal &= ( 1 << AK8963_STATUS2_REGISTER_HOFL_BIT);					// mask all but HOFL bit.
        st2RegisterVal = (st2RegisterVal << AK8963_STATUS2_REGISTER_HOFL_BIT);		// Shift to get bit to unit's place.
        if(! st2RegisterVal)	//	HOFL Bit is not "1" and hence Overflow did not occurred
        {
            isDataReady = true;
        }
    }
    return isDataReady;
}

extern bool readMagFluxDensities_AK8963(AK8963_DEVICE* AK8963DevicePointer)
{
    bool isDataReady = false;
    // Step 1. Read Register Values. If Data is not ready, value returned is false, else true.
    isDataReady = readRawMagFluxDataNB_AK8963(AK8963DevicePointer);
    // Step 2. If data read is successful, change it to correct form and return a true.
    if(isDataReady)	// Data Successfully read, transform them to magnetic flux density values.
    {
        // Combine H_REGISTER and L_REGISTER Values as per Little Endian Scheme.
        AK8963DevicePointer->magneticFluxDensityValues[AK8963_X_AXIS] = (float)((int16_t)( AK8963DevicePointer->rawMagRegisterDatas[AK8963_XH_REGISTER_POS] << 8 )
                | AK8963DevicePointer->rawMagRegisterDatas[AK8963_XL_REGISTER_POS] );

        AK8963DevicePointer->magneticFluxDensityValues[AK8963_Y_AXIS] = (float)((int16_t)( AK8963DevicePointer->rawMagRegisterDatas[AK8963_YH_REGISTER_POS] << 8 )
                | AK8963DevicePointer->rawMagRegisterDatas[AK8963_YL_REGISTER_POS] );


        AK8963DevicePointer->magneticFluxDensityValues[AK8963_Z_AXIS] = (float)((int16_t)( AK8963DevicePointer->rawMagRegisterDatas[AK8963_ZH_REGISTER_POS] << 8 )
                | AK8963DevicePointer->rawMagRegisterDatas[AK8963_ZL_REGISTER_POS] );



        //Adjust Magnetometer sensitivity and Resolution values to get values in uT.
        AK8963DevicePointer->magneticFluxDensityValues[AK8963_X_AXIS] *= AK8963DevicePointer->sensitivityAdjustment_ResolutionValues[AK8963_X_AXIS];
        AK8963DevicePointer->magneticFluxDensityValues[AK8963_Y_AXIS] *= AK8963DevicePointer->sensitivityAdjustment_ResolutionValues[AK8963_Y_AXIS];
        AK8963DevicePointer->magneticFluxDensityValues[AK8963_Z_AXIS] *= AK8963DevicePointer->sensitivityAdjustment_ResolutionValues[AK8963_Z_AXIS];



        //Adjust for Hard Iron Offset values.
        AK8963DevicePointer->magneticFluxDensityValues[AK8963_X_AXIS] -= AK8963DevicePointer->hardIronOffsetValues[AK8963_X_AXIS];
        AK8963DevicePointer->magneticFluxDensityValues[AK8963_Y_AXIS] -= AK8963DevicePointer->hardIronOffsetValues[AK8963_Y_AXIS];
        AK8963DevicePointer->magneticFluxDensityValues[AK8963_Z_AXIS] -= AK8963DevicePointer->hardIronOffsetValues[AK8963_Z_AXIS];

    }
    return isDataReady;
}


extern float getHeading_AK8963(AK8963_DEVICE* AK8963DevicePointer)
{
    readMagFluxDensities_AK8963(AK8963DevicePointer) ;

    float yawAngle = atan2f(AK8963DevicePointer->magneticFluxDensityValues[AK8963_Y_AXIS],
                           AK8963DevicePointer->magneticFluxDensityValues[AK8963_X_AXIS])*180.0f / PI;
    if(AK8963DevicePointer->magneticFluxDensityValues[AK8963_X_AXIS] < 0.0000f) yawAngle = 180 - yawAngle ;
    else if (AK8963DevicePointer->magneticFluxDensityValues[AK8963_X_AXIS] > 0)
    {
        if(AK8963DevicePointer->magneticFluxDensityValues[AK8963_Y_AXIS] < 0)
        {
            yawAngle = -yawAngle ;
        }
        else
        {
            yawAngle = 360 -yawAngle ;
        }
    }
    else if (AK8963DevicePointer->magneticFluxDensityValues[AK8963_X_AXIS] < 0.00001f &&
            AK8963DevicePointer->magneticFluxDensityValues[AK8963_X_AXIS]  > -0.00001f)
    {
        if(AK8963DevicePointer->magneticFluxDensityValues[AK8963_Y_AXIS] < 0)
        {
            yawAngle = 90 ;
        }
        else
        {
            yawAngle = 270 ;
        }
    }
    yawAngle += DECLINATION_ANGLE ;
    return yawAngle ;
}

static void setMode_AK8963(AK8963_DEVICE* AK8963DevicePointer,AK8963_OPMODES modeValue)
{
    //Set Mode value and set BIT field according to modeVal & dataResolution.

    AK8963DevicePointer->operationMode = modeValue;
    I2CWriteByte(&AK8963DevicePointer->I2CDevice, AK8963_CONTROL_REGISTER1, (modeValue) | (AK8963DevicePointer->dataResolution));

    //Provide a little Delay to ensure the mode sets in.
    milliSecondDelay(1);
}

#ifdef CALIBRATION_NEEDED
void calibrate_AK8963(AK8963_DEVICE* AK8963DevicePointer, UARTDEVICE* UARTDevicePointer)
{
    uint16_t looper = 0;
    bool isDataReady = false;
    uint8_t delTime = 0;
    if(AK8963DevicePointer->operationMode == CONTINOUS_MEASUREMENT_MODE_100Hz) delTime = 15;
    else if (AK8963DevicePointer->operationMode == CONTINOUS_MEASUREMENT_MODE_8Hz) delTime = 130;
    UARTWrite(UARTDevicePointer, "DelTime: %d", delTime);
    for(looper = 0; looper < 2000; ++looper)
    {
        isDataReady = readRawMagFluxDataNB_AK8963(AK8963DevicePointer);
        if(isDataReady)
        {
            uint8_t looper1 = 0;
            //			for(looper1 = 0; looper1<6;++looper1) UARTWrite(UARTDevicePointer,"%d ",rawMagRegisterDatas[looper1]);
            //			UARTWrite(UARTDevicePointer, "\n");

            AK8963DevicePointer->magneticFluxDensityValues[AK8963_X_AXIS] = (float)((int16_t)( AK8963DevicePointer->rawMagRegisterDatas[AK8963_XH_REGISTER_POS] << 8 )
                    | AK8963DevicePointer->rawMagRegisterDatas[AK8963_XL_REGISTER_POS] );

            AK8963DevicePointer->magneticFluxDensityValues[AK8963_Y_AXIS] = (float)((int16_t)( AK8963DevicePointer->rawMagRegisterDatas[AK8963_YH_REGISTER_POS] << 8 )
                    | AK8963DevicePointer->rawMagRegisterDatas[AK8963_YL_REGISTER_POS] );


            AK8963DevicePointer->magneticFluxDensityValues[AK8963_Z_AXIS] = (float)((int16_t)( AK8963DevicePointer->rawMagRegisterDatas[AK8963_ZH_REGISTER_POS] << 8 )
                    | AK8963DevicePointer->rawMagRegisterDatas[AK8963_ZL_REGISTER_POS] );


            //	Adjust for sensitivity Adjustment values.
            AK8963DevicePointer->magneticFluxDensityValues[AK8963_X_AXIS] *= AK8963DevicePointer->sensitivityAdjustment_ResolutionValues[AK8963_X_AXIS];
            AK8963DevicePointer->magneticFluxDensityValues[AK8963_Y_AXIS] *= AK8963DevicePointer->sensitivityAdjustment_ResolutionValues[AK8963_Y_AXIS];
            AK8963DevicePointer->magneticFluxDensityValues[AK8963_Z_AXIS] *= AK8963DevicePointer->sensitivityAdjustment_ResolutionValues[AK8963_Z_AXIS];
            UARTWrite(UARTDevicePointer, "%d ",looper);
            for(looper1 = 0; looper1<3;++looper1) UARTWrite(UARTDevicePointer, "%f\t",AK8963DevicePointer->magneticFluxDensityValues[looper1]);
            UARTWrite(UARTDevicePointer,"\n");
            milliSecondDelay(delTime);
        }
    }


}
#endif

#ifdef MPU9250_PRESENT
static void enableMPU9250Bypass(I2C_PERIPHERAL I2CPeripheralCode)
{
    I2CDEVICE dummyMPU9250;
    initI2CMaster(&dummyMPU9250, I2CPeripheralCode, MPU9250_ADDRESS, FAST_400k_I2C);
    uint8_t byPassRegisterVal = I2CReadByte(&dummyMPU9250, MPU9250_INT_PIN_CFG_REGISTER);
    uint8_t intEnableRegisterVal = I2CReadByte(&dummyMPU9250, MPU9250_INT_ENABLE_REGISTER);
    byPassRegisterVal |= 0b00010010;
    intEnableRegisterVal |= 0b00000001;
    I2CWriteByte(&dummyMPU9250, MPU9250_INT_PIN_CFG_REGISTER, 0x12);
    I2CWriteByte(&dummyMPU9250, MPU9250_INT_ENABLE_REGISTER, 0x01);
}
#endif
