/*
 * MPU9250.c
 *
 *  Created on: 27-Jul-2018
 *      Author: sagarshubham
 */


#include "MPU9250.h"


static uint8_t rawAccelTempGyroRegisterDatas[14] = {0, 0, 0, 0, 0, 0, 0,
                                                    0, 0, 0, 0, 0, 0, 0};



extern MPU9250_DEVICE* initialize_MPU9250(MPU9250_DEVICE* MPU9250DevicePointer,
                                          I2C_PERIPHERAL I2CPeripheralCode,
                                          uint8_t MPU9250SlaveAddress,
                                          GYRO_FULLSCALE_RANGES gyroFullScaleRangeValue,
                                          ACCEL_FULLSCALE_RANGES accelFullScaleRangeValue)
{
    uint8_t registerValues = 0;

    // Step 1. Initialize the I2C Device.
    initI2CMaster(&MPU9250DevicePointer->MPU9250I2CDevice, I2CPeripheralCode, MPU9250SlaveAddress, FAST_400k_I2C) ;

    // Step 2. Set Full Scale Range Values.
    MPU9250DevicePointer->gyroFSSEL = gyroFullScaleRangeValue ;
    MPU9250DevicePointer->accelFSSEL = accelFullScaleRangeValue ;

    /*
     * Step 3. Wake up Device.
     * Set MPU9250_PWR_MGNT_1_REGISTER to 0x80.
     * A. Resets Sleep, Cycle, and Gyro Standby bits of the register.
     * A.1 This also sets Clock source to Internal 20Mhz Oscillator.
     * A.2 Also resets all internal registers. Bit [7] will autoclear.
     * A.3 Wait for 100ms after this.
     * B. Clear Signal Paths using SIGNAL_PATH_REGISTER
     * C. Clear data registers using SIG_COND_RST Bit of USER_CNTRL_REGISTER.
     * D. Set or UNSET I2C_IF_DIS in USER_CNTRL_REGISTER if using SSI or not.
     *
     */

    writeByte_mpu9250(MPU9250DevicePointer, MPU9250_PWR_MGNT_1_REGISTER, 0x80) ;    // A, A.1, A.2
    milliSecondDelay(100) ; // A.3
    registerValues = readByte_mpu9250(MPU9250DevicePointer, MPU9250_SIGNAL_PATH_RESET_REGISTER) ; // B
    registerValues |= 0x07 ; // B
    writeByte_mpu9250(MPU9250DevicePointer, MPU9250_SIGNAL_PATH_RESET_REGISTER, registerValues) ; // B
    registerValues = readByte_mpu9250(MPU9250DevicePointer, MPU9250_USER_CTRL_REGISTER) ; // C. & D.
    registerValues |= 0x01 ; // C.
#ifdef MPU9250_USE_SSI
    registerValues |= 0b00010000 ;  // D.
#endif
    writeByte_mpu9250(MPU9250DevicePointer, MPU9250_USER_CTRL_REGISTER, registerValues) ; // C. & D.

    /*
     * Step 4. Set Clock Source to PLL.
     * SET CLKSEL[2:0] to 0b001 = 0x01
     */
    registerValues = readByte_mpu9250(MPU9250DevicePointer, MPU9250_PWR_MGNT_1_REGISTER) ;
    registerValues |= (0b00000001);
    writeByte_mpu9250(MPU9250DevicePointer, MPU9250_PWR_MGNT_1_REGISTER, registerValues);
    milliSecondDelay(100);

    /*
     * Step 5. Set CONFIG Register.
     * 7th Bit is RESERVED.
     * 6th Don't care about this bit now. Leave it as it is.
     * Disable FSYNC Capture.
     *  i.e. [5:3] bits EXT_SYNC_SET[2:0] = 0b000
     * DLPF Settings.
     *
     *
     *   *  Set for 1Khz Sample Rate, 5.9ms delay, 41Hz Bandwidth.
     *      i.e. [2:0] DLPF_CFG[2:0] = 0b011 && in MPU9250_GYRO_CONFIG_REGISTER, [1:0] bits FCHOICE_b[1:0] = 0b00
     *      This configuration has delay time of 0.97 ms, i.e. nearly 1KHz  // Set SMPLRT_DIV REGISTER according to this.
     */
    registerValues = readByte_mpu9250(MPU9250DevicePointer, MPU9250_CONFIG_REGISTER);
    registerValues &= ~(0b00111000);            // Set FSYNC Bits as 0b000
    registerValues &= ~(0b00000111);            // First Clear DLPF_CFG bits.
    registerValues |=  (0b00000011);            // Set it to 0b011
    writeByte_mpu9250(MPU9250DevicePointer, MPU9250_CONFIG_REGISTER, registerValues);
    // Set Sample Rate in SMPLRT_DIV Register. Should be near 1Khz as Sample Rate is 1Khz, SMPLRT_DIV is 0.
    writeByte_mpu9250(MPU9250DevicePointer, MPU9250_SAMPLE_RATE_DIVIDER_REGISTER, 0x00);

    /*
     * Step 6. Set Accel CONFIG 2 REGISTER.
     * [7:6] and [5:4] are RESERVED.
     * [3] is accel_fchoice_b, set it to 0 so as accel_fchoice is 1.
     * [2:0] are for accel low pass filter settings.
     *  Set it for 1Khz, 5.80ms delay, 184hz Bandwidth.
     *          i.e. set accel_fchoice to 1 (bit [3] accel_fchoice_b of accel conf 2 register to 0).
     *          Then set bits [2:0] to 0b001.
     */
    registerValues = readByte_mpu9250(MPU9250DevicePointer, MPU9250_ACCEL_CONFIG_2_REGISTER);
    registerValues &= ~(0b00001000);  // Set fchoice_b bit [3] to 0.
    registerValues &= ~(0b00000111);  // First Clear DLPF Bits.
    registerValues |= ~(0b00000001);  // Then set it to 0b001.
    writeByte_mpu9250(MPU9250DevicePointer, MPU9250_ACCEL_CONFIG_2_REGISTER,registerValues);
    //  *****


    /*
     * Step 7. Set GYRO_CONFIG REGISTER.
     * [7:5] is for self test bits. disable it by setting it to 0b000
     * [4:3] is for full scale range settings. set it according to MPU9250DevicePointer->gyroFSSEL.
     * [2] is RESERVED.
     * [1:0] is FChoice_b[1:0] bits. Set it to 0b00 as per above.
     */
    registerValues = readByte_mpu9250(MPU9250DevicePointer, MPU9250_GYRO_CONFIG_REGISTER);
    registerValues &= ~(0b11100000);                        // Set [7:5] as 0b000
    registerValues &= ~(0b00011000);                        // First clear [4:3]
    registerValues |=  MPU9250DevicePointer->gyroFSSEL;     // Then Set FSSEL.
    registerValues &= ~(0b00000011);                        // Clear FChoice_b
    writeByte_mpu9250(MPU9250DevicePointer, MPU9250_GYRO_CONFIG_REGISTER,registerValues);

    /*
     * Step 8. Set ACCEL_CONFIG_1_REGISTER
     * [7:5] are for self test, disable them by setting it to 0b000
     * [4:3] are for full scale range setting. set it according to MPU9250DevicePointer->accelFSSEL.
     * [2:0] are RESERVED. Let them be.
     */
    registerValues = readByte_mpu9250(MPU9250DevicePointer, MPU9250_ACCEL_CONFIG_1_REGISTER);
    registerValues &= ~(0b11100000);                    // Set [7:5] as 0b000
    registerValues &= ~(0b00011000);                    // Fisrt clear [4:3]
    registerValues |= MPU9250DevicePointer->accelFSSEL; // Then set FSSEL.
    writeByte_mpu9250(MPU9250DevicePointer, MPU9250_ACCEL_CONFIG_1_REGISTER, registerValues);


    /*
     * Step 9. Set MPU9250_INT_PIN_BYPASS_CONF_REGISTER for interrupts and Bypass.
     * Keep bit [1], [4], and [5] as 1
     *  This puts device in bypass mode and the interrupt clears only when data is read.
     * [7:6] and [3:2] as 0.
     * Bit [0] is RESERVED.
     */
    registerValues = readByte_mpu9250(MPU9250DevicePointer, MPU9250_INT_PIN_BYPASS_CONF_REGISTER);
    registerValues |=  (0b10010010);            // Set Bits [1],[4], and [5] as 1.
    registerValues &= ~(0b01101100);            // Set [7:6] and [3:2] as 0.
    writeByte_mpu9250(MPU9250DevicePointer,MPU9250_INT_PIN_BYPASS_CONF_REGISTER, registerValues);


    /*
     * Step 10. Set Interrupt Enable Register.
     * [7], [2:1] are RESERVED.
     * Set [6:3] as 0.
     * Set [0] as 1. This propagates interrupt from aux. sensors to INT PIN.
     */
    registerValues = readByte_mpu9250(MPU9250DevicePointer, MPU9250_INTERRUPT_ENABLE_REGISTER);
    registerValues &= ~(0b01111000);            // Set Bits [6:3] as 0.
    registerValues |= (0b00000001);         // Set Bit [0] as 1.
    writeByte_mpu9250(MPU9250DevicePointer,MPU9250_INTERRUPT_ENABLE_REGISTER, registerValues);


    /*
     * Step 11. Disable FIFO for now using USER_CTRL_REGISTER + FIFO_ENABLE_REGISTER
     * (Couldn't find source to disable DMP and prevent it from writing to FIFO.)
     *  In USER_CTRL_REGISTER
     *      Set Bit[6] to 0.
     *  In FIFO_ENABLE_REGISTER
     *      Set all bits to 0.
     */
    registerValues = readByte_mpu9250(MPU9250DevicePointer, MPU9250_USER_CTRL_REGISTER);
    registerValues &= ~(0b01000000);
    writeByte_mpu9250(MPU9250DevicePointer, MPU9250_USER_CTRL_REGISTER, registerValues);

    registerValues = readByte_mpu9250(MPU9250DevicePointer, MPU9250_FIFO_ENABLE_REGISTER);
    registerValues &= ~(0b11111111);
    writeByte_mpu9250(MPU9250DevicePointer, MPU9250_FIFO_ENABLE_REGISTER, registerValues);


    /*
     * Step 12. Now all settings are done. Only resolution values are to be set.
     * The below shenanigans of not using actual values of resolution but instead equal proportions of them is just that, a childish shenanigan.
     */
    switch (MPU9250DevicePointer->accelFSSEL) {
    case A_FS_SEL_2_G:
        MPU9250DevicePointer->accelerometerResolution = 2.0f / 32768.0f;
        break;
    case A_FS_SEL_4_G:
        MPU9250DevicePointer->accelerometerResolution = 4.0f / 32768.0f;
        break;
    case A_FS_SEL_8_G:
        MPU9250DevicePointer->accelerometerResolution = 8.0f / 32768.0f;
        break;
    case A_FS_SEL_16_G:
        MPU9250DevicePointer->accelerometerResolution = 16.0f / 32768.0f;
        break;
    }
    switch (MPU9250DevicePointer->gyroFSSEL) {
    case G_FS_SEL_250_DPS:
        MPU9250DevicePointer->gyroscopeResolution = 2.0f / 262.0f;
        break;
    case G_FS_SEL_500_DPS:
        MPU9250DevicePointer->gyroscopeResolution = 4.0f / 262.0f;
        break;
    case G_FS_SEL_1000_DPS:
        MPU9250DevicePointer->gyroscopeResolution = 8.0f / 262.0f;
        break;
    case G_FS_SEL_2000_DPS:
        MPU9250DevicePointer->gyroscopeResolution = 16.0f / 262.0f;
        break;
    }
#ifdef MPU9250_CALIBRATION_NEEDED
    calibrate_MPU9250(MPU9250DevicePointer);
#endif
    return MPU9250DevicePointer;
}


extern uint8_t whoAmi_MPU9250(MPU9250_DEVICE* MPU9250DevicePointer)
{
    uint8_t whoAmIRegisterValue = 0;
#ifdef MPU9250_USE_SSI
    setChipSelectState_mpu9250(MPU9250DevicePointer, MPU9250_CHIP_SELECT) ;
    SSIWriteValue(&MPU9250DevicePointer->MPU9250SSIDevice, MPU9250_WHO_AM_I_REGISTER | MPU9250_SSI_READ_REGISTER) ;
    whoAmIRegisterValue = SSIReadValue(&MPU9250DevicePointer->MPU9250SSIDevice) ;
    setChipSelectState_mpu9250(MPU9250DevicePointer, MPU9250_CHIP_UNSELECT) ;
#endif
#ifdef MPU9250_USE_I2C
    whoAmIRegisterValue = I2CWhoAmI(&MPU9250DevicePointer->MPU9250I2CDevice, MPU9250_WHO_AM_I_REGISTER) ;
#endif
    return whoAmIRegisterValue ;
}

#ifdef MPU9250_CALIBRATION_NEEDED
static void calibrate_MPU9250(MPU9250_DEVICE* MPU9250DevicePointer)
{

    // We shall read a fixed number of data, finds its average, and save it to bias arrays.

    /*
     * Step 1.
     * Make space for incoming data.
     */
    uint8_t rawDataArray[14];                           // Array to store raw data from accelerometer, temperature, and gyroscope sensor.
    // Reading temperature values helps to read all 14 registers in one go
    int32_t accelDataSumArray[3] = {0 , 0, 0};          // Array to store sum of accelerometer data values.
    int32_t gyroDataSumArray[3]  = {0 , 0, 0};          // Array to store sum of gyroscope data values.
    uint16_t looper = 0, looper2 = 0;                   // Variables to help looping in the algorithm,
    uint16_t numOfSamples = 1000;                       // Number of samples.
    // Can be more, but then calibration is lengthy;
    // can be less, but then we might get erroneous data.




    // Step 2. Take Data.
    for(looper = 0; looper < numOfSamples; ++looper)
    {
        //      if(isDataReady(MPU9250DevicePointer, UARTDevicePointer))
        {
            readBytes_mpu9250(MPU9250DevicePointer, MPU9250_ACCEL_XOUT_H_REGISTER, 14, &rawDataArray[0]);
            accelDataSumArray[MPU9250_AX_AXIS] += (int32_t)((int16_t)( rawDataArray[MPU9250_AXH_REGISTER_ARRAY_POS] << 8 ) |
                    rawDataArray[MPU9250_AXL_REGISTER_ARRAY_POS] );

            accelDataSumArray[MPU9250_AY_AXIS] += (int32_t)((int16_t)( rawDataArray[MPU9250_AYH_REGISTER_ARRAY_POS] << 8 ) |
                    rawDataArray[MPU9250_AYL_REGISTER_ARRAY_POS] );

            accelDataSumArray[MPU9250_AZ_AXIS] += (int32_t)((int16_t)( rawDataArray[MPU9250_AZH_REGISTER_ARRAY_POS] << 8 ) |
                    rawDataArray[MPU9250_AZL_REGISTER_ARRAY_POS] );

            gyroDataSumArray[MPU9250_GX_AXIS] += (int32_t)((int16_t)( rawDataArray[MPU9250_GXH_REGISTER_ARRAY_POS] << 8 ) |
                    rawDataArray[MPU9250_GXL_REGISTER_ARRAY_POS] );

            gyroDataSumArray[MPU9250_GY_AXIS] += (int32_t)((int16_t)( rawDataArray[MPU9250_GYH_REGISTER_ARRAY_POS] << 8 ) |
                    rawDataArray[MPU9250_GYL_REGISTER_ARRAY_POS] );

            gyroDataSumArray[MPU9250_GZ_AXIS] += (int32_t)((int16_t)( rawDataArray[MPU9250_GZH_REGISTER_ARRAY_POS] << 8 ) |
                    rawDataArray[MPU9250_GZL_REGISTER_ARRAY_POS] );

            milliSecondDelay(5);
        }
    }

    // Step 3. Take Average of Data.
    for(looper2 = 0; looper2 < 3; ++looper2)
    {
        accelDataSumArray[looper2] /= (int32_t)(numOfSamples);
        gyroDataSumArray[looper2] /= (int32_t)(numOfSamples);
    }

    // Adjust for g along Z axis.

    int32_t accelSensitivityValue = 0;
    switch (MPU9250DevicePointer->accelFSSEL) {
    case A_FS_SEL_2_G:
        accelSensitivityValue = 16384;
        break;
    case A_FS_SEL_4_G:
        accelSensitivityValue = 8192;
        break;
    case A_FS_SEL_8_G:
        accelSensitivityValue = 4096;
        break;
    case A_FS_SEL_16_G:
        accelSensitivityValue = 2048;
        break;
    }
    if(accelDataSumArray[MPU9250_AZ_AXIS] > 0L)
    {
        accelDataSumArray[MPU9250_AZ_AXIS] = accelDataSumArray[MPU9250_AZ_AXIS] - accelSensitivityValue;
    }
    else
    {

        accelDataSumArray[MPU9250_AZ_AXIS] = accelDataSumArray[MPU9250_AZ_AXIS] + accelSensitivityValue;
    }


    MPU9250DevicePointer->accelerometerBiasValues[MPU9250_AX_AXIS] = (float)(accelDataSumArray[MPU9250_AX_AXIS]) * MPU9250DevicePointer->accelerometerResolution;
    MPU9250DevicePointer->accelerometerBiasValues[MPU9250_AY_AXIS] = (float)(accelDataSumArray[MPU9250_AY_AXIS]) * MPU9250DevicePointer->accelerometerResolution;
    MPU9250DevicePointer->accelerometerBiasValues[MPU9250_AZ_AXIS] = (float)(accelDataSumArray[MPU9250_AZ_AXIS]) * MPU9250DevicePointer->accelerometerResolution;


    MPU9250DevicePointer->gyroscopeBiasValues[MPU9250_GX_AXIS] = (float)(gyroDataSumArray[MPU9250_GX_AXIS]) * MPU9250DevicePointer->gyroscopeResolution;
    MPU9250DevicePointer->gyroscopeBiasValues[MPU9250_GY_AXIS] = (float)(gyroDataSumArray[MPU9250_AY_AXIS]) * MPU9250DevicePointer->gyroscopeResolution;
    MPU9250DevicePointer->gyroscopeBiasValues[MPU9250_GZ_AXIS] = (float)(gyroDataSumArray[MPU9250_GZ_AXIS]) * MPU9250DevicePointer->gyroscopeResolution;

}
#endif

extern float readAllValues_MPU9250(MPU9250_DEVICE* MPU9250DevicePointer)
{
    readRawAllRegisterData_MPU9250(MPU9250DevicePointer);


    MPU9250DevicePointer->accelerometerAccelerationValues[MPU9250_AX_AXIS] = (float)((int16_t)( rawAccelTempGyroRegisterDatas[MPU9250_AXH_REGISTER_ARRAY_POS] << 8 )
            | rawAccelTempGyroRegisterDatas[MPU9250_AXL_REGISTER_ARRAY_POS] ) * MPU9250DevicePointer->accelerometerResolution
            - MPU9250DevicePointer->accelerometerBiasValues[MPU9250_AX_AXIS];

    MPU9250DevicePointer->accelerometerAccelerationValues[MPU9250_AY_AXIS] = (float)((int16_t)( rawAccelTempGyroRegisterDatas[MPU9250_AYH_REGISTER_ARRAY_POS] << 8 )
            | rawAccelTempGyroRegisterDatas[MPU9250_AYL_REGISTER_ARRAY_POS] ) * MPU9250DevicePointer->accelerometerResolution
            - MPU9250DevicePointer->accelerometerBiasValues[MPU9250_AY_AXIS];

    MPU9250DevicePointer->accelerometerAccelerationValues[MPU9250_AZ_AXIS] = (float)((int16_t)( rawAccelTempGyroRegisterDatas[MPU9250_AZH_REGISTER_ARRAY_POS] << 8 )
            | rawAccelTempGyroRegisterDatas[MPU9250_AZL_REGISTER_ARRAY_POS] ) * MPU9250DevicePointer->accelerometerResolution
            - MPU9250DevicePointer->accelerometerBiasValues[MPU9250_AZ_AXIS];


    MPU9250DevicePointer->gyroscopeAngularVelocityValues[MPU9250_GX_AXIS] = (float)((int16_t)( rawAccelTempGyroRegisterDatas[MPU9250_GXH_REGISTER_ARRAY_POS] << 8 )
            | rawAccelTempGyroRegisterDatas[MPU9250_GXL_REGISTER_ARRAY_POS] ) * MPU9250DevicePointer->gyroscopeResolution
            - MPU9250DevicePointer->gyroscopeBiasValues[MPU9250_GX_AXIS];

    MPU9250DevicePointer->gyroscopeAngularVelocityValues[MPU9250_GY_AXIS] = (float)((int16_t)( rawAccelTempGyroRegisterDatas[MPU9250_GYH_REGISTER_ARRAY_POS] << 8 )
            | rawAccelTempGyroRegisterDatas[MPU9250_GYL_REGISTER_ARRAY_POS] ) * MPU9250DevicePointer->gyroscopeResolution
            - MPU9250DevicePointer->gyroscopeBiasValues[MPU9250_GY_AXIS];

    MPU9250DevicePointer->gyroscopeAngularVelocityValues[MPU9250_GZ_AXIS] = (float)((int16_t)( rawAccelTempGyroRegisterDatas[MPU9250_GZH_REGISTER_ARRAY_POS] << 8 )
            | rawAccelTempGyroRegisterDatas[MPU9250_GZL_REGISTER_ARRAY_POS] ) * MPU9250DevicePointer->gyroscopeResolution
            - MPU9250DevicePointer->gyroscopeBiasValues[MPU9250_GZ_AXIS];
#ifdef MPU9250_USE_RADIANS
    MPU9250DevicePointer->gyroscopeAngularVelocityValues[MPU9250_GX_AXIS] *= DEGREES_TO_RADIANS ;
    MPU9250DevicePointer->gyroscopeAngularVelocityValues[MPU9250_GY_AXIS] *= DEGREES_TO_RADIANS ;
    MPU9250DevicePointer->gyroscopeAngularVelocityValues[MPU9250_GZ_AXIS] *= DEGREES_TO_RADIANS ;
#endif

#ifdef TEMP_DATA_NEEDED
    float temperatureValue = (float)((int16_t)( rawAccelTempGyroRegisterDatas[MPU9250_TH_REGISTER_ARRAY_POS] << 8 )
            | rawAccelTempGyroRegisterDatas[MPU9250_TL_REGISTER_ARRAY_POS] ) / 333.87f + 21.0f;
    return temperatureValue;
#endif
    return 0.0f;

}

extern void readAccelerationValues_MPU9250(MPU9250_DEVICE* MPU9250DevicePointer)
{
    readRawAccelerationData_MPU9250(MPU9250DevicePointer);

    MPU9250DevicePointer->accelerometerAccelerationValues[MPU9250_AX_AXIS] = (float)((int16_t)( rawAccelTempGyroRegisterDatas[MPU9250_AXH_REGISTER_ARRAY_POS] << 8 )
            | rawAccelTempGyroRegisterDatas[MPU9250_AXL_REGISTER_ARRAY_POS] ) * MPU9250DevicePointer->accelerometerResolution
            - MPU9250DevicePointer->accelerometerBiasValues[MPU9250_AX_AXIS];

    MPU9250DevicePointer->accelerometerAccelerationValues[MPU9250_AY_AXIS] = (float)((int16_t)( rawAccelTempGyroRegisterDatas[MPU9250_AYH_REGISTER_ARRAY_POS] << 8 )
            | rawAccelTempGyroRegisterDatas[MPU9250_AYL_REGISTER_ARRAY_POS] ) * MPU9250DevicePointer->accelerometerResolution
            - MPU9250DevicePointer->accelerometerBiasValues[MPU9250_AY_AXIS];

    MPU9250DevicePointer->accelerometerAccelerationValues[MPU9250_AZ_AXIS] = (float)((int16_t)( rawAccelTempGyroRegisterDatas[MPU9250_AZH_REGISTER_ARRAY_POS] << 8 )
            | rawAccelTempGyroRegisterDatas[MPU9250_AZL_REGISTER_ARRAY_POS] ) * MPU9250DevicePointer->accelerometerResolution
            - MPU9250DevicePointer->accelerometerBiasValues[MPU9250_AZ_AXIS];
}

extern void readGyrationValues_MPU9250(MPU9250_DEVICE* MPU9250DevicePointer)
{
    readRawGyrationData_MPU9250(MPU9250DevicePointer);

    MPU9250DevicePointer->gyroscopeAngularVelocityValues[MPU9250_GX_AXIS] = (float)((int16_t)( rawAccelTempGyroRegisterDatas[MPU9250_GXH_REGISTER_ARRAY_POS] << 8 )
            | rawAccelTempGyroRegisterDatas[MPU9250_GXL_REGISTER_ARRAY_POS] ) * MPU9250DevicePointer->gyroscopeResolution
            - MPU9250DevicePointer->gyroscopeBiasValues[MPU9250_GX_AXIS];

    MPU9250DevicePointer->gyroscopeAngularVelocityValues[MPU9250_GY_AXIS] = (float)((int16_t)( rawAccelTempGyroRegisterDatas[MPU9250_GYH_REGISTER_ARRAY_POS] << 8 )
            | rawAccelTempGyroRegisterDatas[MPU9250_GYL_REGISTER_ARRAY_POS] ) * MPU9250DevicePointer->gyroscopeResolution
            - MPU9250DevicePointer->gyroscopeBiasValues[MPU9250_GY_AXIS];

    MPU9250DevicePointer->gyroscopeAngularVelocityValues[MPU9250_GZ_AXIS] = (float)((int16_t)( rawAccelTempGyroRegisterDatas[MPU9250_GZH_REGISTER_ARRAY_POS] << 8 )
            | rawAccelTempGyroRegisterDatas[MPU9250_GZL_REGISTER_ARRAY_POS] ) * MPU9250DevicePointer->gyroscopeResolution
            - MPU9250DevicePointer->gyroscopeBiasValues[MPU9250_GZ_AXIS];
#ifdef MPU9250_USE_RADIANS
    MPU9250DevicePointer->gyroscopeAngularVelocityValues[MPU9250_GX_AXIS] *= DEGREES_TO_RADIANS ;
    MPU9250DevicePointer->gyroscopeAngularVelocityValues[MPU9250_GY_AXIS] *= DEGREES_TO_RADIANS ;
    MPU9250DevicePointer->gyroscopeAngularVelocityValues[MPU9250_GZ_AXIS] *= DEGREES_TO_RADIANS ;
#endif
}

#ifdef TEMP_DATA_NEEDED
extern float readTemperatureValues_MPU9250(MPU9250_DEVICE* MPU9250DevicePointer)
{
    readRawTemeratureData_MPU9250(MPU9250DevicePointer);
    float temperatureValue = (float)((int16_t)( rawAccelTempGyroRegisterDatas[MPU9250_TH_REGISTER_ARRAY_POS] << 8 )
            | rawAccelTempGyroRegisterDatas[MPU9250_TL_REGISTER_ARRAY_POS] ) / 333.87f + 21.0f;
    return temperatureValue;
}
#endif

#ifdef MPU9250_SELFTEST_NEEDED


extern bool checkSelfTestResults_MPU9250(MPU9250_DEVICE* MPU9250DevicePointer, UARTDEVICE* UARTDevicePointer)
{
    float accelerometerSelfTestResults[3];
    float gyroscopeSelfTestResults[3];
    bool isSelfTestPassed = isSelfTestPassed_MPU9250(MPU9250DevicePointer,&accelerometerSelfTestResults[0], &gyroscopeSelfTestResults[0]);
    UARTWrite(UARTDevicePointer,"X-axis self test: acceleration trim within: %f %% of factory value.\n",accelerometerSelfTestResults[0]);
    UARTWrite(UARTDevicePointer,"Y-axis self test: acceleration trim within: %f %% of factory value.\n",accelerometerSelfTestResults[1]);
    UARTWrite(UARTDevicePointer,"Z-axis self test: acceleration trim within: %f %% of factory value.\n",accelerometerSelfTestResults[2]);
    UARTWrite(UARTDevicePointer,"X-axis self test: gyration     trim within: %f %% of factory value.\n",gyroscopeSelfTestResults[0]);
    UARTWrite(UARTDevicePointer,"Y-axis self test: gyration     trim within: %f %% of factory value.\n",gyroscopeSelfTestResults[1]);
    UARTWrite(UARTDevicePointer,"Z-axis self test: gyration     trim within: %f %% of factory value.\n",gyroscopeSelfTestResults[2]);

    return isSelfTestPassed;
}


static bool isSelfTestPassed_MPU9250(   MPU9250_DEVICE* MPU9250DevicePointer ,
                                        float* accelerometerSelfTestResults,
                                        float* gyroscopeSelfTestResults
/*, UARTDEVICE* UARTDevicePointer*/)
{
    uint8_t lastGyroDLPFConfig,                         // Variables to store last configurations.
    lastGyroFSConfig,
    lastGyroFChoiceConfig,
    lastSmplrtDivConfig,
    lastAccelDLPFConfig,
    lastAccelFSConfig,
    lastAccelFchoiceConfig;

    uint8_t regiserValue;                               // Variable to store register values.

    uint8_t rawAccelGyroRegisterValues[14];             // Array to store raw register values.

    int32_t avgNONSTAccelValues[3] = {0, 0, 0};         // Array to store sum and average of non ST accel values.
    int32_t avgNONSTGyroValues[3] =  {0, 0, 0};         // Array to store sum and average of non ST gyro values.

    int32_t avgSTAccelValues[3] = {0, 0, 0};            // Array to store sum and average of ST accel values.
    int32_t avgSTGyroValues[3] =  {0, 0, 0};            // Array to store sum and average of ST gyro values.

    int32_t selfTestAccelValues[3] =  {0, 0, 0};        // Array to store difference between ST accel values and NON ST accel values.
    int32_t selfTestGyroValues[3] =  {0, 0, 0};         // Array to store difference between ST gyro values and NON ST gyro values.

    uint8_t stCodeAccelValues[3];                       // Array to store accel self test output generated during manufacturing tests.
    uint8_t stCodeGyroValues[3];                        // Array to store gyro self test output generated during manufacturing tests.

    float stOTPAccelValues[3];                          // Array to store ST_OTP Accel Values.
    float stOTPGyroValues[3];                           // Array to store ST_OTP Gyro Values.

    uint8_t numOfSamples = 200;                         // Save 200 samples of each NON ST and ST Values.
    float fullScaleValue = 2620.0f;                     // Actual value is (float)(2620/(2^FS)), where FS = 0 .
    // We here save the direct value.

    bool isSelfTestPassed = true;

    /*
     * Step 1. Save the last configuration values:-
     *  A. Last gyro
     *      i.      DLPF Configs  (MPU9250_CONFIG_REGISTER)
     *      ii.     SMPLRT_DIV values (MPU9250_SAMPLE_RATE_DIVIDER_REGISTER)
     *      iii.    FSSEL (MPU9250_GYRO_CONFIG_REGISTER) and
     *      iv.     FChoice_b values (MPU9250_GYRO_CONFIG_REGISTER)
     *  B. Last accel
     *      i.      DLPF Configs (MPU9250_ACCEL_CONFIG_2_REGISTER)
     *      ii.     accel FChoice_b (MPU9250_ACCEL_CONFIG_2_REGISTER)
     *      iii.    FSSEL (MPU9250_ACCEL_CONFIG_1_REGISTER)
     */
    lastSmplrtDivConfig     = readByte_mpu9250(MPU9250DevicePointer, MPU9250_SAMPLE_RATE_DIVIDER_REGISTER);  // Save all. will change to 1.
    lastGyroDLPFConfig      = readByte_mpu9250(MPU9250DevicePointer, MPU9250_CONFIG_REGISTER);               // DLPF Gyro Config is in [2:0] bits.
    lastGyroFChoiceConfig   = readByte_mpu9250(MPU9250DevicePointer,MPU9250_GYRO_CONFIG_REGISTER);           // FChoice_b values are in [1:0] bits.
    lastGyroFSConfig        = readByte_mpu9250(MPU9250DevicePointer, MPU9250_GYRO_CONFIG_REGISTER);          // FSSEL values are in [4:3] bits.
    lastAccelDLPFConfig     = readByte_mpu9250(MPU9250DevicePointer, MPU9250_ACCEL_CONFIG_2_REGISTER);       // DLPF Accel values are in [2:0] bits.
    lastAccelFchoiceConfig  = readByte_mpu9250(MPU9250DevicePointer, MPU9250_ACCEL_CONFIG_2_REGISTER);       // Fchoice_b values is in [3] bit.
    lastAccelFSConfig       = readByte_mpu9250(MPU9250DevicePointer, MPU9250_ACCEL_CONFIG_1_REGISTER);       // FSSEL values are in [4:3] bits.

    //  UARTWrite(UARTDevicePointer, "lastSmplrtDivConfig: 0b%b\n", lastSmplrtDivConfig);
    //  UARTWrite(UARTDevicePointer, "lastGyroDLPFConfig: 0b%b\n", lastGyroDLPFConfig);
    //  UARTWrite(UARTDevicePointer, "lastGyroFChoiceConfig: 0b%b\n", lastGyroFChoiceConfig);
    //  UARTWrite(UARTDevicePointer, "lastGyroFSConfig: 0b%b\n", lastGyroFSConfig);
    //  UARTWrite(UARTDevicePointer, "lastAccelDLPFConfig: 0b%b\n", lastAccelDLPFConfig);
    //  UARTWrite(UARTDevicePointer, "lastAccelFchoiceConfig: 0b%b\n", lastAccelFchoiceConfig);
    //  UARTWrite(UARTDevicePointer, "lastAccelFSConfig: 0b%b\n", lastAccelFSConfig);

    /*
     * Step 2. Prepare the registers for initiating self test mode.
     *  A. SMPLRT_DIV Register to 0x00.
     *  B. Gyro DLPF bits to 0b010
     *  C. Gyro FChoice_b bits to 0b00
     *  D. Gyro FSSEL bits to 0b00 (+-250 DPS)
     *
     *  E. Accel DLPF bits to 0b10
     *  F. Accel FChoice_b bits to 0b0
     *  G. Accel FSSEL to bits 0b00 (+-2 g)
     */
    writeByte_mpu9250(MPU9250DevicePointer,MPU9250_SAMPLE_RATE_DIVIDER_REGISTER, 0x00);
    //  UARTWrite(UARTDevicePointer, "MPU9250_SAMPLE_RATE_DIVIDER_REGISTER is: 0b%b\n",readByte_mpu9250(MPU9250DevicePointer, MPU9250_SAMPLE_RATE_DIVIDER_REGISTER));
    regiserValue = readByte_mpu9250(MPU9250DevicePointer, MPU9250_CONFIG_REGISTER);
    //  UARTWrite(UARTDevicePointer, "MPU9250_CONFIG_REGISTER was: 0b%b is: ",regiserValue);
    regiserValue &= ~(0b00000111);      // First Clear the [2:0] bits of DLPF.
    regiserValue |=  (0b00000010);      // Then set it to 0b010 of DLPF
    writeByte_mpu9250(MPU9250DevicePointer, MPU9250_CONFIG_REGISTER, regiserValue);
    //  UARTWrite(UARTDevicePointer, "0b%b\n",readByte_mpu9250(MPU9250DevicePointer, MPU9250_CONFIG_REGISTER));

    regiserValue = readByte_mpu9250(MPU9250DevicePointer, MPU9250_GYRO_CONFIG_REGISTER);
    //  UARTWrite(UARTDevicePointer, "MPU9250_GYRO_CONFIG_REGISTER was: 0b%b is: ",regiserValue);
    regiserValue &= ~(0b00011011);      // First Clear the [4:3] bits of FSSEL and [1:0] bits of FChoice_b
    regiserValue |=  (0b00000000);      // Then set 0b00 to FSSEL [4:3] and 0b00 to FChoice_b [1:0]
    writeByte_mpu9250(MPU9250DevicePointer, MPU9250_GYRO_CONFIG_REGISTER, regiserValue);
    //  UARTWrite(UARTDevicePointer, "0b%b\n",readByte_mpu9250(MPU9250DevicePointer, MPU9250_GYRO_CONFIG_REGISTER));

    regiserValue = readByte_mpu9250(MPU9250DevicePointer, MPU9250_ACCEL_CONFIG_2_REGISTER);
    //  UARTWrite(UARTDevicePointer, "MPU9250_ACCEL_CONFIG_2_REGISTER was: 0b%b is: ",regiserValue);
    regiserValue &= ~(0b00001111);      // First Clear the [2:0] DLPF bits and [3] Fchoice_b Bit.
    regiserValue |=  (0b00000010);      // the set 0b010 to [2:0] DLPF bits and 0 to [3] FChoice Bit.
    writeByte_mpu9250(MPU9250DevicePointer, MPU9250_ACCEL_CONFIG_2_REGISTER, regiserValue);
    //  UARTWrite(UARTDevicePointer, "0b%b\n",readByte_mpu9250(MPU9250DevicePointer, MPU9250_ACCEL_CONFIG_2_REGISTER));

    regiserValue = readByte_mpu9250(MPU9250DevicePointer, MPU9250_ACCEL_CONFIG_1_REGISTER);
    //  UARTWrite(UARTDevicePointer, "MPU9250_ACCEL_CONFIG_1_REGISTER was: 0b%b is: ",regiserValue);
    regiserValue &= ~(0b00011000);      // First Clear the [4:3] bits of FSSEL.
    regiserValue |=  (0b00000000);      // Then set 0b00 to FSSEL [4:3].
    writeByte_mpu9250(MPU9250DevicePointer, MPU9250_ACCEL_CONFIG_1_REGISTER, regiserValue);
    //  UARTWrite(UARTDevicePointer, "0b%b\n",readByte_mpu9250(MPU9250DevicePointer, MPU9250_ACCEL_CONFIG_1_REGISTER));


    /*
     * Step 3. read 200 NON ST gyro and accel values and sum them.
     * As per DLPF configurations of Accelerometer and Gyroscope, output rate is 1Khz.
     * So we sample data every 1000ms.
     * For this we use ss_TIME.h function getTicks() and the fact that difference of TIMIG_FACTOR is equivalent to 1 millisecond gap.
     */
    uint8_t looper = 0;
    uint8_t t0 =0;
    for(looper = 0; looper <numOfSamples; ++looper)
    {
        if(getTicks() - t0 >= TIMING_FACTOR)
        {
            readBytes_mpu9250(MPU9250DevicePointer, MPU9250_ACCEL_XOUT_H_REGISTER,14,&rawAccelGyroRegisterValues[0]);
            t0 = getTicks();

            avgNONSTAccelValues[MPU9250_AX_AXIS] += (int32_t)((int16_t)( rawAccelGyroRegisterValues[MPU9250_AXH_REGISTER_ARRAY_POS] << 8 ) |
                    rawAccelGyroRegisterValues[MPU9250_AXL_REGISTER_ARRAY_POS] );

            avgNONSTAccelValues[MPU9250_AY_AXIS] += (int32_t)((int16_t)( rawAccelGyroRegisterValues[MPU9250_AYH_REGISTER_ARRAY_POS] << 8 ) |
                    rawAccelGyroRegisterValues[MPU9250_AYL_REGISTER_ARRAY_POS] );

            avgNONSTAccelValues[MPU9250_AZ_AXIS] += (int32_t)((int16_t)( rawAccelGyroRegisterValues[MPU9250_AZH_REGISTER_ARRAY_POS] << 8 ) |
                    rawAccelGyroRegisterValues[MPU9250_AZL_REGISTER_ARRAY_POS] );

            avgNONSTGyroValues[MPU9250_GX_AXIS] += (int32_t)((int16_t)( rawAccelGyroRegisterValues[MPU9250_GXH_REGISTER_ARRAY_POS] << 8 ) |
                    rawAccelGyroRegisterValues[MPU9250_GXL_REGISTER_ARRAY_POS] );

            avgNONSTGyroValues[MPU9250_GY_AXIS] += (int32_t)((int16_t)( rawAccelGyroRegisterValues[MPU9250_GYH_REGISTER_ARRAY_POS] << 8 ) |
                    rawAccelGyroRegisterValues[MPU9250_GYL_REGISTER_ARRAY_POS] );

            avgNONSTGyroValues[MPU9250_GZ_AXIS] += (int32_t)((int16_t)( rawAccelGyroRegisterValues[MPU9250_GZH_REGISTER_ARRAY_POS] << 8 ) |
                    rawAccelGyroRegisterValues[MPU9250_GZL_REGISTER_ARRAY_POS] );

            //          UARTWrite(UARTDevicePointer, "%d ",(int32_t)((int16_t)( rawAccelGyroRegisterValues[MPU9250_AXH_REGISTER_ARRAY_POS] << 8 ) |
            //                  rawAccelGyroRegisterValues[MPU9250_AXL_REGISTER_ARRAY_POS] ));
            //          UARTWrite(UARTDevicePointer, "%d ",(int32_t)((int16_t)( rawAccelGyroRegisterValues[MPU9250_AYH_REGISTER_ARRAY_POS] << 8 ) |
            //                  rawAccelGyroRegisterValues[MPU9250_AYL_REGISTER_ARRAY_POS] ));
            //          UARTWrite(UARTDevicePointer, "%d\t",(int32_t)((int16_t)( rawAccelGyroRegisterValues[MPU9250_AZH_REGISTER_ARRAY_POS] << 8 ) |
            //                  rawAccelGyroRegisterValues[MPU9250_AZL_REGISTER_ARRAY_POS] ));
            //
            //          UARTWrite(UARTDevicePointer, "%d ",(int32_t)((int16_t)( rawAccelGyroRegisterValues[MPU9250_GXH_REGISTER_ARRAY_POS] << 8 ) |
            //                  rawAccelGyroRegisterValues[MPU9250_GXL_REGISTER_ARRAY_POS] ));
            //          UARTWrite(UARTDevicePointer, "%d ",(int32_t)((int16_t)( rawAccelGyroRegisterValues[MPU9250_GYH_REGISTER_ARRAY_POS] << 8 ) |
            //                  rawAccelGyroRegisterValues[MPU9250_GYL_REGISTER_ARRAY_POS] ));
            //          UARTWrite(UARTDevicePointer, "%d\n",(int32_t)((int16_t)( rawAccelGyroRegisterValues[MPU9250_GZH_REGISTER_ARRAY_POS] << 8 ) |
            //                  rawAccelGyroRegisterValues[MPU9250_GZL_REGISTER_ARRAY_POS] ));

        }
    }

    //  UARTWrite(UARTDevicePointer, "\n\n%d ",accelValues[0]);
    //  UARTWrite(UARTDevicePointer, "%d ",accelValues[1]);
    //  UARTWrite(UARTDevicePointer, "%d\t",accelValues[2]);
    //
    //  UARTWrite(UARTDevicePointer, "%d ",gyroValues[0]);
    //  UARTWrite(UARTDevicePointer, "%d ",gyroValues[1]);
    //  UARTWrite(UARTDevicePointer, "%d\n",gyroValues[2]);

    /*
     * Step 4. Get Average Non ST Values (Divide by the number of samples).
     */
    avgNONSTAccelValues[MPU9250_AX_AXIS] = avgNONSTAccelValues[MPU9250_AX_AXIS] / ((int32_t)(numOfSamples));
    avgNONSTAccelValues[MPU9250_AY_AXIS] = avgNONSTAccelValues[MPU9250_AY_AXIS] / ((int32_t)(numOfSamples));
    avgNONSTAccelValues[MPU9250_AZ_AXIS] = avgNONSTAccelValues[MPU9250_AZ_AXIS] / ((int32_t)(numOfSamples));

    avgNONSTGyroValues[MPU9250_GX_AXIS] = avgNONSTGyroValues[MPU9250_GX_AXIS] / ((int32_t)(numOfSamples));
    avgNONSTGyroValues[MPU9250_GY_AXIS] = avgNONSTGyroValues[MPU9250_GY_AXIS] / ((int32_t)(numOfSamples));
    avgNONSTGyroValues[MPU9250_GZ_AXIS] = avgNONSTGyroValues[MPU9250_GZ_AXIS] / ((int32_t)(numOfSamples));



    //  UARTWrite(UARTDevicePointer, "\n\n%d ",avgNONSTAccelValues[0]);
    //  UARTWrite(UARTDevicePointer, "%d ",avgNONSTAccelValues[1]);
    //  UARTWrite(UARTDevicePointer, "%d\t",avgNONSTAccelValues[2]);
    //
    //  UARTWrite(UARTDevicePointer, "%d ",avgNONSTGyroValues[0]);
    //  UARTWrite(UARTDevicePointer, "%d ",avgNONSTGyroValues[1]);
    //  UARTWrite(UARTDevicePointer, "%d\n",avgNONSTGyroValues[2]);

    /*
     * Step 5. Enable Self Test on MPU9250_ACCEL_CONFIG_1_REGISTER and MPU9250_GYRO_CONFIG_REGISTER and wait 20ms for mode to set in.
     */
    regiserValue = readByte_mpu9250(MPU9250DevicePointer, MPU9250_ACCEL_CONFIG_1_REGISTER);
    //  UARTWrite(UARTDevicePointer, "MPU9250_ACCEL_CONFIG_1_REGISTER was: 0b%b is: ",regiserValue);
    regiserValue |= (0b11100000);
    writeByte_mpu9250(MPU9250DevicePointer,MPU9250_ACCEL_CONFIG_1_REGISTER, regiserValue);
    //  UARTWrite(UARTDevicePointer, "0b%b\n",readByte_mpu9250(MPU9250DevicePointer, MPU9250_ACCEL_CONFIG_1_REGISTER));

    regiserValue = readByte_mpu9250(MPU9250DevicePointer, MPU9250_GYRO_CONFIG_REGISTER);
    //  UARTWrite(UARTDevicePointer, "MPU9250_GYRO_CONFIG_REGISTER was: 0b%b is: ",regiserValue);
    regiserValue |= (0b11100000);
    writeByte_mpu9250(MPU9250DevicePointer,MPU9250_GYRO_CONFIG_REGISTER, regiserValue);
    //  UARTWrite(UARTDevicePointer, "0b%b\n",readByte_mpu9250(MPU9250DevicePointer, MPU9250_GYRO_CONFIG_REGISTER));
    milliSecondDelay(20);

    /*
     * Step 6. Read 200 ST gyro and accel values and and sum them.
     * As per DLPF configurations of Accelerometer and Gyroscope, output rate is 1Khz.
     * So we sample data every 1000ms.
     * For this we use ss_TIME.h function getTicks().
     * The difference of TIMIG_FACTOR in present ticks value and last tick value is equivalent to 1 millisecond gap.
     */
    t0 = 0;     // set last ticks to 0.

    for(looper = 0; looper < numOfSamples; ++looper)
    {
        if(getTicks() - t0 >= TIMING_FACTOR)
        {
            readBytes_mpu9250(MPU9250DevicePointer, MPU9250_ACCEL_XOUT_H_REGISTER,14,&rawAccelGyroRegisterValues[0]);
            t0 = getTicks();

            avgSTAccelValues[MPU9250_AX_AXIS] += (int32_t)((int16_t)( rawAccelGyroRegisterValues[MPU9250_AXH_REGISTER_ARRAY_POS] << 8 ) |
                    rawAccelGyroRegisterValues[MPU9250_AXL_REGISTER_ARRAY_POS] );

            avgSTAccelValues[MPU9250_AY_AXIS] += (int32_t)((int16_t)( rawAccelGyroRegisterValues[MPU9250_AYH_REGISTER_ARRAY_POS] << 8 ) |
                    rawAccelGyroRegisterValues[MPU9250_AYL_REGISTER_ARRAY_POS] );

            avgSTAccelValues[MPU9250_AZ_AXIS] += (int32_t)((int16_t)( rawAccelGyroRegisterValues[MPU9250_AZH_REGISTER_ARRAY_POS] << 8 ) |
                    rawAccelGyroRegisterValues[MPU9250_AZL_REGISTER_ARRAY_POS] );

            avgSTGyroValues[MPU9250_GX_AXIS] += (int32_t)((int16_t)( rawAccelGyroRegisterValues[MPU9250_GXH_REGISTER_ARRAY_POS] << 8 ) |
                    rawAccelGyroRegisterValues[MPU9250_GXL_REGISTER_ARRAY_POS] );

            avgSTGyroValues[MPU9250_GY_AXIS] += (int32_t)((int16_t)( rawAccelGyroRegisterValues[MPU9250_GYH_REGISTER_ARRAY_POS] << 8 ) |
                    rawAccelGyroRegisterValues[MPU9250_GYL_REGISTER_ARRAY_POS] );

            avgSTGyroValues[MPU9250_GZ_AXIS] += (int32_t)((int16_t)( rawAccelGyroRegisterValues[MPU9250_GZH_REGISTER_ARRAY_POS] << 8 ) |
                    rawAccelGyroRegisterValues[MPU9250_GZL_REGISTER_ARRAY_POS] );

            //          UARTWrite(UARTDevicePointer, "%d ",(int32_t)((int16_t)( rawAccelGyroRegisterValues[MPU9250_AXH_REGISTER_ARRAY_POS] << 8 ) |
            //                  rawAccelGyroRegisterValues[MPU9250_AXL_REGISTER_ARRAY_POS] ));
            //          UARTWrite(UARTDevicePointer, "%d ",(int32_t)((int16_t)( rawAccelGyroRegisterValues[MPU9250_AYH_REGISTER_ARRAY_POS] << 8 ) |
            //                  rawAccelGyroRegisterValues[MPU9250_AYL_REGISTER_ARRAY_POS] ));
            //          UARTWrite(UARTDevicePointer, "%d\t",(int32_t)((int16_t)( rawAccelGyroRegisterValues[MPU9250_AZH_REGISTER_ARRAY_POS] << 8 ) |
            //                  rawAccelGyroRegisterValues[MPU9250_AZL_REGISTER_ARRAY_POS] ));
            //
            //          UARTWrite(UARTDevicePointer, "%d ",(int32_t)((int16_t)( rawAccelGyroRegisterValues[MPU9250_GXH_REGISTER_ARRAY_POS] << 8 ) |
            //                  rawAccelGyroRegisterValues[MPU9250_GXL_REGISTER_ARRAY_POS] ));
            //          UARTWrite(UARTDevicePointer, "%d ",(int32_t)((int16_t)( rawAccelGyroRegisterValues[MPU9250_GYH_REGISTER_ARRAY_POS] << 8 ) |
            //                  rawAccelGyroRegisterValues[MPU9250_GYL_REGISTER_ARRAY_POS] ));
            //          UARTWrite(UARTDevicePointer, "%d\n",(int32_t)((int16_t)( rawAccelGyroRegisterValues[MPU9250_GZH_REGISTER_ARRAY_POS] << 8 ) |
            //                  rawAccelGyroRegisterValues[MPU9250_GZL_REGISTER_ARRAY_POS] ));

        }
    }



    //  UARTWrite(UARTDevicePointer, "\n\n%d ",accelValues[0]);
    //  UARTWrite(UARTDevicePointer, "%d ",accelValues[1]);
    //  UARTWrite(UARTDevicePointer, "%d\t",accelValues[2]);
    //
    //  UARTWrite(UARTDevicePointer, "%d ",gyroValues[0]);
    //  UARTWrite(UARTDevicePointer, "%d ",gyroValues[1]);
    //  UARTWrite(UARTDevicePointer, "%d\n",gyroValues[2]);

    /*
     * Step 7. Get Average ST Values (Divide by the number of samples)..
     */
    avgSTAccelValues[MPU9250_AX_AXIS] = avgSTAccelValues[MPU9250_AX_AXIS] / ((int32_t)(numOfSamples));
    avgSTAccelValues[MPU9250_AY_AXIS] = avgSTAccelValues[MPU9250_AY_AXIS] / ((int32_t)(numOfSamples));
    avgSTAccelValues[MPU9250_AZ_AXIS] = avgSTAccelValues[MPU9250_AZ_AXIS] / ((int32_t)(numOfSamples));

    avgSTGyroValues[MPU9250_GX_AXIS] = avgSTGyroValues[MPU9250_GX_AXIS] / ((int32_t)(numOfSamples));
    avgSTGyroValues[MPU9250_GY_AXIS] = avgSTGyroValues[MPU9250_GY_AXIS] / ((int32_t)(numOfSamples));
    avgSTGyroValues[MPU9250_GZ_AXIS] = avgSTGyroValues[MPU9250_GZ_AXIS] / ((int32_t)(numOfSamples));

    //  UARTWrite(UARTDevicePointer, "\n\n%d ",avgSTAccelValues[0]);
    //  UARTWrite(UARTDevicePointer, "%d ",avgSTAccelValues[1]);
    //  UARTWrite(UARTDevicePointer, "%d\t",avgSTAccelValues[2]);
    //
    //  UARTWrite(UARTDevicePointer, "%d ",avgSTGyroValues[0]);
    //  UARTWrite(UARTDevicePointer, "%d ",avgSTGyroValues[1]);
    //  UARTWrite(UARTDevicePointer, "%d\n",avgSTGyroValues[2]);

    /*
     * Step 8. Get selfTest values according to formula :
     *  selfTestValues  = average Self Test Values - non Average Self Test Values.
     */
    selfTestAccelValues[MPU9250_AX_AXIS] = avgSTAccelValues[MPU9250_AX_AXIS] - avgNONSTAccelValues[MPU9250_AX_AXIS];
    selfTestAccelValues[MPU9250_AY_AXIS] = avgSTAccelValues[MPU9250_AY_AXIS] - avgNONSTAccelValues[MPU9250_AY_AXIS];
    selfTestAccelValues[MPU9250_AZ_AXIS] = avgSTAccelValues[MPU9250_AZ_AXIS] - avgNONSTAccelValues[MPU9250_AZ_AXIS];


    selfTestGyroValues[MPU9250_GX_AXIS] = avgSTGyroValues[MPU9250_GX_AXIS] - avgNONSTGyroValues[MPU9250_GX_AXIS];
    selfTestGyroValues[MPU9250_GY_AXIS] = avgSTGyroValues[MPU9250_GY_AXIS] - avgNONSTGyroValues[MPU9250_GY_AXIS];
    selfTestGyroValues[MPU9250_GZ_AXIS] = avgSTGyroValues[MPU9250_GZ_AXIS] - avgNONSTGyroValues[MPU9250_GZ_AXIS];

    //  UARTWrite(UARTDevicePointer, "\n\n%d ",accelValues[0]);
    //  UARTWrite(UARTDevicePointer, "%d ",accelValues[1]);
    //  UARTWrite(UARTDevicePointer, "%d\t",accelValues[2]);
    //
    //  UARTWrite(UARTDevicePointer, "%d ",gyroValues[0]);
    //  UARTWrite(UARTDevicePointer, "%d ",gyroValues[1]);
    //  UARTWrite(UARTDevicePointer, "%d\n",gyroValues[2]);
    //
    //  UARTWrite(UARTDevicePointer, "\n\n%d ",selfTestAccelValues[0]);
    //  UARTWrite(UARTDevicePointer, "%d ",selfTestAccelValues[1]);
    //  UARTWrite(UARTDevicePointer, "%d\t",selfTestAccelValues[2]);
    //
    //  UARTWrite(UARTDevicePointer, "%d ",selfTestGyroValues[0]);
    //  UARTWrite(UARTDevicePointer, "%d ",selfTestGyroValues[1]);
    //  UARTWrite(UARTDevicePointer, "%d\n",selfTestGyroValues[2]);

    /*
     * Step 9. Clean up new data and restore old data.
     */
    regiserValue = readByte_mpu9250(MPU9250DevicePointer, MPU9250_ACCEL_CONFIG_1_REGISTER);
    regiserValue  &= ~(0b11100000);
    writeByte_mpu9250(MPU9250DevicePointer,MPU9250_ACCEL_CONFIG_1_REGISTER, regiserValue);

    regiserValue = readByte_mpu9250(MPU9250DevicePointer, MPU9250_GYRO_CONFIG_REGISTER);
    regiserValue &= ~(0b11100000);
    writeByte_mpu9250(MPU9250DevicePointer,MPU9250_GYRO_CONFIG_REGISTER, regiserValue);

    milliSecondDelay(20);
    writeByte_mpu9250(MPU9250DevicePointer, MPU9250_SAMPLE_RATE_DIVIDER_REGISTER, lastSmplrtDivConfig);                 // Save all. will change to 1.
    writeByte_mpu9250(MPU9250DevicePointer, MPU9250_CONFIG_REGISTER, lastGyroDLPFConfig);       // DLPF Gyro Config is in [2:0] bits. Mask Rest.
    writeByte_mpu9250(MPU9250DevicePointer,MPU9250_GYRO_CONFIG_REGISTER, lastGyroFChoiceConfig);   // FChoice_b values are in [1:0] bits. Mask Rest.
    writeByte_mpu9250(MPU9250DevicePointer, MPU9250_GYRO_CONFIG_REGISTER, lastGyroFSConfig);  // FSSEL values are in [4:3] bits. Mask Rest.

    writeByte_mpu9250(MPU9250DevicePointer, MPU9250_ACCEL_CONFIG_2_REGISTER, lastAccelDLPFConfig); // DLPF Accel values are in [2:0] bits. Mask Rest.
    writeByte_mpu9250(MPU9250DevicePointer, MPU9250_ACCEL_CONFIG_2_REGISTER, lastAccelFchoiceConfig); // Fchoice_b values is in [3] bit. Mask Rest.
    writeByte_mpu9250(MPU9250DevicePointer, MPU9250_ACCEL_CONFIG_1_REGISTER, lastAccelFSConfig); // FSSEL values are in [4:3] bits. Mask Rest.


    /*
     * Step 10. Read SELF_TEST_REGISTER values as stCodes.
     */
    readBytes_mpu9250(MPU9250DevicePointer, MPU9250_SELFTEST_XACCEL_REGISTER,3, &stCodeAccelValues[0]);
    readBytes_mpu9250(MPU9250DevicePointer, MPU9250_SELFTEST_XGYRO_REGISTER,3, &stCodeGyroValues[0]);


    //  UARTWrite(UARTDevicePointer, "\n\n%d ",stCodeAccelValues[0]);
    //  UARTWrite(UARTDevicePointer, "%d ",stCodeAccelValues[1]);
    //  UARTWrite(UARTDevicePointer, "%d\t",stCodeAccelValues[2]);
    //
    //  UARTWrite(UARTDevicePointer, "%d ",stCodeGyroValues[0]);
    //  UARTWrite(UARTDevicePointer, "%d ",stCodeGyroValues[1]);
    //  UARTWrite(UARTDevicePointer, "%d\n",stCodeGyroValues[2]);

    /*
     * Step 11. Calculate Self Test OTP Values according to formula:
     *  stOTP = fullScaleValue * 1.01^(stCodeValue - 1)
     */

    stOTPAccelValues[MPU9250_AX_AXIS] = fullScaleValue * powf(1.01f , (float)( stCodeAccelValues[MPU9250_AX_AXIS] - 1 ) );
    stOTPAccelValues[MPU9250_AY_AXIS] = fullScaleValue * powf(1.01f , (float)( stCodeAccelValues[MPU9250_AY_AXIS] - 1 ) );
    stOTPAccelValues[MPU9250_AZ_AXIS] = fullScaleValue * powf(1.01f , (float)( stCodeAccelValues[MPU9250_AZ_AXIS] - 1 ) );

    stOTPGyroValues[MPU9250_GX_AXIS] = fullScaleValue * powf(1.01f , (float)( stCodeGyroValues[MPU9250_GX_AXIS] - 1 ) );
    stOTPGyroValues[MPU9250_GY_AXIS] = fullScaleValue * powf(1.01f , (float)( stCodeGyroValues[MPU9250_GY_AXIS] - 1 ) );
    stOTPGyroValues[MPU9250_GZ_AXIS] = fullScaleValue * powf(1.01f , (float)( stCodeGyroValues[MPU9250_GZ_AXIS] - 1 ) );

    //  UARTWrite(UARTDevicePointer, "\n\n%f ",stOTPAccelValues[0]);
    //  UARTWrite(UARTDevicePointer, "%f ",stOTPAccelValues[1]);
    //  UARTWrite(UARTDevicePointer, "%f\t",stOTPAccelValues[2]);
    //
    //  UARTWrite(UARTDevicePointer, "%f ",stOTPGyroValues[0]);
    //  UARTWrite(UARTDevicePointer, "%f ",stOTPGyroValues[1]);
    //  UARTWrite(UARTDevicePointer, "%f\n",stOTPGyroValues[2]);



    /*
     * Step 12. Check for Self Test Pass
     * In case st_otp != 0
     *      For Gyroscope
     *          (selfTestGyroValues / stOTPGyroValues) > 0.5
     *      For Accelerometer
     *          (selfTestAccelValues / stOTPAccelVaues) < 1.5
     *
     * In Case, st_otp == 0,
     *      For Gyroscope
     *          | (selfTestGyroValues) | > 60 dps
     *      For Accelerometer
     *          225 mgee <= | (selfTestAccelValues) | <= 675 mgee
     * Then, if above condition is met,
     *      For Gyroscope
     *          | (GyroOffsetValues) | <= 20 dps
     */
    for(looper = 0 ; looper < 3 && isSelfTestPassed; ++ looper)
    {
        float accelTest, gyroTest;
        if(stOTPAccelValues[looper] != 0)
        {
            accelTest = (float)selfTestAccelValues[looper] / stOTPAccelValues[looper];
            gyroTest   = (float)selfTestGyroValues[looper] / stOTPGyroValues[looper];
            accelerometerSelfTestResults[looper] = 100.0f*(accelTest-1.0f);
            gyroscopeSelfTestResults[looper]   = 100.0f*(gyroTest-1.0f);
            //         UARTWrite(UARTDevicePointer, "looper: %d\taccelTestVal: %f\taccelTest%%: %f\t\tgyroTest: %f\tgyroTest%%: %f\n",looper, accelTest, 100.0f*(accelTest-1),gyroTest, 100.0f*(gyroTest-1));
        }
        else
        {
            if(    (abs(selfTestAccelValues[looper])*2000.0f/32568.0f >= 675) ||
                    (abs(selfTestAccelValues[looper])*2000.0f/32568.0f <= 225) ||
                    (abs(selfTestGyroValues[looper])*2.0f/262.0f >= 60)    )
            {
                isSelfTestPassed = false;
            }
        }
        if(    (accelTest >= 1.5) ||
                (accelTest <= 0.5) ||
                (gyroTest <= 0.5 )     )
        {

            isSelfTestPassed = false;
            //         UARTWrite(UARTDevicePointer, "looper: %d\taccelTest: %f\tgyroTest: %f\n",looper, accelTest, gyroTest);
        }
        //     UARTWrite(UARTDevicePointer, "looper: %d\t isSelfTestPassed: %s\n", looper, (isSelfTestPassed)?("TRUE"):("FALSE"));
    }
    // UARTWrite(UARTDevicePointer, "looper: %d\t isSelfTestPassed: %s\n", looper, (isSelfTestPassed)?("TRUE"):("FALSE"));
    if(isSelfTestPassed)
    {
        //     uint8_t gyroOffsetRawRegisterValues[6];
        readBytes_mpu9250(MPU9250DevicePointer, MPU9250_XG_OFFSET_H_REGISTER,6,&rawAccelGyroRegisterValues[MPU9250_GXH_REGISTER_ARRAY_POS]);
        //     UARTWrite(UARTDevicePointer,"\n\n%d ",gyroOffsetRawRegisterValues[0]);
        //     UARTWrite(UARTDevicePointer,"%d ",gyroOffsetRawRegisterValues[1]);
        //     UARTWrite(UARTDevicePointer,"%d ",gyroOffsetRawRegisterValues[2]);
        //     UARTWrite(UARTDevicePointer,"%d ",gyroOffsetRawRegisterValues[3]);
        //     UARTWrite(UARTDevicePointer,"%d ",gyroOffsetRawRegisterValues[4]);
        //     UARTWrite(UARTDevicePointer,"%d\n\n",gyroOffsetRawRegisterValues[5]);
        avgNONSTGyroValues[MPU9250_GX_AXIS] = (int32_t)((int16_t)( rawAccelGyroRegisterValues[MPU9250_GXH_REGISTER_ARRAY_POS] << 8 ) |
                rawAccelGyroRegisterValues[MPU9250_GXL_REGISTER_ARRAY_POS] );

        avgNONSTGyroValues[MPU9250_GY_AXIS] = (int32_t)((int16_t)( rawAccelGyroRegisterValues[MPU9250_GYH_REGISTER_ARRAY_POS] << 8 ) |
                rawAccelGyroRegisterValues[MPU9250_GYL_REGISTER_ARRAY_POS] );

        avgNONSTGyroValues[MPU9250_GZ_AXIS] = (int32_t)((int16_t)( rawAccelGyroRegisterValues[MPU9250_GZH_REGISTER_ARRAY_POS] << 8 ) |
                rawAccelGyroRegisterValues[MPU9250_GZL_REGISTER_ARRAY_POS] );
        //     UARTWrite(UARTDevicePointer, "\n\n%d ", gyroValues[MPU9250_GX_AXIS]);
        //     UARTWrite(UARTDevicePointer, "%d ", gyroValues[MPU9250_GY_AXIS]);
        //     UARTWrite(UARTDevicePointer, "%d\n\n", gyroValues[MPU9250_GZ_AXIS]);
        //
        //     UARTWrite(UARTDevicePointer, "\n\n%f ", abs((float)gyroValues[0]));
        //     UARTWrite(UARTDevicePointer, "%f ", abs((float)gyroValues[1]));
        //     UARTWrite(UARTDevicePointer, "%f\n\n", abs((float)gyroValues[2]));
        if(  (abs((float)avgNONSTGyroValues[MPU9250_GX_AXIS])*2.0f/262.0f >= 20) ||
                (abs((float)avgNONSTGyroValues[MPU9250_GY_AXIS])*2.0f/262.0f >= 20) ||
                (abs((float)avgNONSTGyroValues[MPU9250_GZ_AXIS])*2.0f/262.0f >= 20)  )
        {
            isSelfTestPassed = false;
        }

    }
    // UARTWrite(UARTDevicePointer, "looper: %d\t isSelfTestPassed: %s\n", looper, (isSelfTestPassed)?("TRUE"):("FALSE"));

    return isSelfTestPassed;
}

#endif



static inline void readRawAllRegisterData_MPU9250(MPU9250_DEVICE* MPU9250DevicePointer)
{
    readBytes_mpu9250(MPU9250DevicePointer, MPU9250_ACCEL_XOUT_H_REGISTER, 14, &rawAccelTempGyroRegisterDatas[0]);
}

static void readRawAccelerationData_MPU9250(MPU9250_DEVICE* MPU9250DevicePointer)
{
    readBytes_mpu9250(MPU9250DevicePointer, MPU9250_ACCEL_XOUT_H_REGISTER, 6, &rawAccelTempGyroRegisterDatas[0]);
}

#ifdef TEMP_DATA_NEEDED
static inline void readRawTemeratureData_MPU9250(MPU9250_DEVICE* MPU9250DevicePointer)
{
    readBytes_mpu9250(MPU9250DevicePointer, MPU9250_TEMP_OUT_H_REGISTER, 2, &rawAccelTempGyroRegisterDatas[6]);
}
#endif

static void readRawGyrationData_MPU9250(MPU9250_DEVICE* MPU9250DevicePointer)
{
    readBytes_mpu9250(MPU9250DevicePointer, MPU9250_GYRO_XOUT_H_REGISTER, 6, &rawAccelTempGyroRegisterDatas[8]);
}

static inline uint8_t readByte_mpu9250(MPU9250_DEVICE* MPU9250DevicePointer, uint8_t registerAddress)
{
    uint8_t registerValue = 0;
    registerValue = I2CReadByte(&MPU9250DevicePointer->MPU9250I2CDevice, registerAddress) ;
    return registerValue ;

}

static uint8_t* readBytes_mpu9250(MPU9250_DEVICE* MPU9250DevicePointer, uint8_t startRegisterAddress, uint8_t numOfRegisters, uint8_t* dataArray )
{
#ifdef MPU9250_USE_I2C
    I2CReadBytes(&MPU9250DevicePointer->MPU9250I2CDevice, startRegisterAddress, numOfRegisters, dataArray) ;
#endif
    return dataArray ;
}

static inline void writeByte_mpu9250(MPU9250_DEVICE* MPU9250DevicePointer, uint8_t registerAddress, uint8_t registerValue)
{
    I2CWriteByte(&MPU9250DevicePointer->MPU9250I2CDevice, registerAddress, registerValue) ;
}







