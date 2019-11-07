/*
 * BME280.c
 *
 *  Created on: 16-Apr-2018
 *      Author: sagar
 */


#include "BME280.h"







BME280* initialize_BME280(BME280* BME280DevicePointer,
#ifdef BME280_USE_SSI
                          char SSIPeripheralLetter,
                          uint8_t SSIPeripheralNumber,
                          char chipSelectPinPortLetter,
                          uint8_t chipSelectPinNumber,
#endif
#ifdef BME280_USE_I2C
                          uint8_t I2CBaseNumber,
#endif
                          UARTDEVICE* UARTDevicePointer)
{
#ifdef BME280_USE_I2C
    initI2CMaster(&BME280DevicePointer->BME280I2CDevice,I2CBaseNumber,BME280_I2C_ADDRESS_SDO_HIGH,FAST_400k_I2C);
#endif
#ifdef BME280_USE_SSI
    initSSIDEVICE(&BME280DevicePointer->BME280SSIDevice,
                  SSIPeripheralNumber,
                  SSIPeripheralLetter,
                  BME280_SSI_MAX_FREQUENCY,
                  BME280_SSI_DATA_WIDTH,
                  BME280_SSI_CPOL_CPHA_SETTINGS,
                  LEAVE_FSS_PIN,
                  SET_SSIRX_PIN) ;
    initGPIO(&BME280DevicePointer->chipSelectPin, chipSelectPinPortLetter, chipSelectPinNumber, OUTPUT) ;
    setChipSelectPinState_BME280(BME280DevicePointer, BME280_CHIP_UNSELECT) ;

#endif
    return BME280DevicePointer ;

//        reset_BME280(BME280DevicePointer);
//        uint8_t whoAmIValue = readWHOAMIValue_BME280(BME280DevicePointer, UARTDevicePointer) ;
//        UARTWrite(UARTDevicePointer, "Init Done: WhoAmIValue: 0x%x is %s\n", whoAmIValue
//                  , (whoAmIValue == BME280_WHO_AM_I_REGISTER_VALUE)?"correct":"incorrect");
//        if(whoAmIValue != BME280_WHO_AM_I_REGISTER_VALUE) return BME280DevicePointer ;
//
//        uint8_t  registerValue = 0;
//
//        ticksDelay(10);
//
//
//        //    readRegister_BME280(BME280DevicePointer
//        //  writeRegister_BME280(BME280DevicePointer
//        /*
//         * Step 1. Write config values in sleep mode.
//         */
//        registerValue = readRegister_BME280(BME280DevicePointer,BME280_CONFIG_REGISTER) ;
//        registerValue &= (0b00000010);   // Preserve Bit 1 and set all rest to 0.
//        registerValue |= (BME280_STANDBY_TIME_0_5ms | BME280_FILTER_COEFFICIENT_OFF | BME280_DISABLE_3WIRE_SPI); // Set new Values.
//        writeRegister_BME280(BME280DevicePointer,BME280_CONFIG_REGISTER, registerValue) ;
//
//        uint8_t whoAmIValue = readWHOAMIValue_BME280(BME280DevicePointer, UARTDevicePointer) ;
//        UARTWrite(UARTDevicePointer, "Init Done: WhoAmIValue: 0x%x is %s\n", whoAmIValue
//                  , (whoAmIValue == BME280_WHO_AM_I_REGISTER_VALUE)?"correct":"incorrect");
//
//
//        /*
//         * Step 2. Set Humidity Control Values.
//         */
//        registerValue = readRegister_BME280(BME280DevicePointer,BME280_HUMIDITY_DATA_ACQ_CONTROL_REGISTER) ;
//        registerValue &= (0b11111000); //Preserve Bits[7:3] and set Bits[2:0] to 0.
//        registerValue |= (BME280_HUMIDITY_OVERSAMPLING_x1) ; // Set Bits[2:0] as per sampling requirements.
//        writeRegister_BME280(BME280DevicePointer, BME280_HUMIDITY_DATA_ACQ_CONTROL_REGISTER, registerValue) ;
//
//
//        whoAmIValue = readWHOAMIValue_BME280(BME280DevicePointer, UARTDevicePointer) ;
//        UARTWrite(UARTDevicePointer, "Init Done: WhoAmIValue: 0x%x is %s\n", whoAmIValue
//                  , (whoAmIValue == BME280_WHO_AM_I_REGISTER_VALUE)?"correct":"incorrect");
//
//        /*
//         * Step 3. Set Pressure and Temperature Control Values.
//         */
//        registerValue = ( (BME280_PRESSURE_OVERSAMPLING_x1) |  (BME280_TEMPERATURE_OVERSAMPLING_x1) | BME280_OPERATION_MODE_NORMAL_MODE ) ;
//        writeRegister_BME280(BME280DevicePointer,BME280_PRESSURE_TEMP_DATA_ACQ_CONTROL_REGISTER, registerValue) ;
//        registerValue = readRegister_BME280(BME280DevicePointer,BME280_PRESSURE_TEMP_DATA_ACQ_CONTROL_REGISTER);
//        UARTWrite(UARTDevicePointer, "CNTRL MEAS: %b\n",registerValue) ;
//
//        ticksDelay(10);
//
//
//
//        whoAmIValue = readWHOAMIValue_BME280(BME280DevicePointer, UARTDevicePointer) ;
//        UARTWrite(UARTDevicePointer, "Init Done: WhoAmIValue: 0x%x is %s\n", whoAmIValue
//                  , (whoAmIValue == BME280_WHO_AM_I_REGISTER_VALUE)?"correct":"incorrect");
//
//        readCompensationParameters(BME280DevicePointer, UARTDevicePointer);
//        UARTWrite(UARTDevicePointer, "dig_T1: %d dig_T2: %d dig_T3: %d\n",BME280DevicePointer->CompParams.dig_T1,BME280DevicePointer->CompParams.dig_T2,BME280DevicePointer->CompParams.dig_T3);
//
//
//
//        return BME280DevicePointer;
}

uint8_t readWHOAMIValue_BME280(BME280* BME280DevicePointer, UARTDEVICE* UARTDevicePointer)
{
    uint8_t whoAmIValue = 0xFF;
#ifdef BME280_USE_I2C
    whoAmIValue = I2CWhoAmI(&BME280DevicePointer->BME280I2CDevice,BME280_WHO_AM_I_REGISTER) ;
#endif
#ifdef BME280_USE_SSI
    setChipSelectPinState_BME280(BME280DevicePointer, BME280_CHIP_SELECT) ;
    SSIWriteValue(&BME280DevicePointer->BME280SSIDevice, (BME280_WHO_AM_I_REGISTER) | BME280_SSI_READ_FROM_REGISTER) ;
//    whoAmIValue = SSIReadValue(&BME280DevicePointer->BME280SSIDevice) ;
    UARTWrite(UARTDevicePointer, "WHOAMI: 0x%x\n",SSIReadValue(&BME280DevicePointer->BME280SSIDevice) ) ;
    setChipSelectPinState_BME280(BME280DevicePointer, BME280_CHIP_UNSELECT) ;
#endif
    return whoAmIValue ;
}

void reset_BME280(BME280* BME280DevicePointer, uint8_t * sent1, uint8_t* sent2)
{
#ifdef BME280_USE_I2C
    I2CWriteByte(&BME280DevicePointer->BME280I2CDevice, BME280_RESET_REGISTER_ADDRESS, BME280_SOFT_RESET_VALUE) ;
#endif
#ifdef BME280_USE_SSI
    setChipSelectPinState_BME280(BME280DevicePointer, BME280_CHIP_SELECT) ;
    SSIWriteValue(&BME280DevicePointer->BME280SSIDevice, ( BME280_RESET_REGISTER_ADDRESS & 0b01111111 ) ) ;
    *sent1 = SSIReadValue(&BME280DevicePointer->BME280SSIDevice);
    SSIWriteValue(&BME280DevicePointer->BME280SSIDevice, BME280_SOFT_RESET_VALUE) ;
    *sent2 = SSIReadValue(&BME280DevicePointer->BME280SSIDevice);
    setChipSelectPinState_BME280(BME280DevicePointer, BME280_CHIP_UNSELECT) ;
#endif
    milliSecondDelay(2);
}

void readRawData_BME280(BME280* BME280DevicePointer)
{
    readRegisters_BME280(BME280DevicePointer,
                         BME280_PRESSURE_DATA_MSB,          8,
                         &BME280DevicePointer->rawDataArray[0]) ;
}

float getTemperature_BME280(BME280* BME280DevicePointer, UARTDEVICE* UARTDevicePointer)
{

    /*  readRawData_BME280(BME280DevicePointer);
    int32_t var1,var2;
    int32_t adc_T = (BME280DevicePointer->rawDataArray[BME280_T_MSB_POS] << 12)
                | (BME280DevicePointer->rawDataArray[BME280_T_LSB_POS] << 4 )
                | (BME280DevicePointer->rawDataArray[BME280_T_XLSB_POS] >> 4);
    //  UARTWrite(UARTDevicePointer,"M: %b\tL: %b\tX: %b\tadc_T: %d\t",BME280DevicePointer->rawDataArray[BME280_T_MSB_POS],
    //          BME280DevicePointer->rawDataArray[BME280_T_LSB_POS],
    //          BME280DevicePointer->rawDataArray[BME280_T_XLSB_POS],adc_T);

    var1 =  ((((adc_T>>3) - ((int32_t)BME280DevicePointer->CompParams.dig_T1<<1))) * ((int32_t)BME280DevicePointer->CompParams.dig_T2)) >> 11;

    //  UARTWrite(UARTDevicePointer, "var1: %d\t",var1);

    var2 = (((((adc_T>>4) - ((int32_t)BME280DevicePointer->CompParams.dig_T1)) * ((adc_T>>4) - ((int32_t)BME280DevicePointer->CompParams.dig_T1))) >> 12) *
            ((int32_t)BME280DevicePointer->CompParams.dig_T3)) >> 14;

    //  UARTWrite(UARTDevicePointer, "var2: %d\t",var2);
    BME280DevicePointer->t_fine = var1 + var2 ;


    BME280DevicePointer->temperature = ((BME280DevicePointer->t_fine*5 + 128 ) >> 8) / 100.0f;
    //  UARTWrite(UARTDevicePointer, "Temp: %f\t",presentTemperature) ;
    //  var1 = ( ( (adc_T >> 3) - ((uint32_t)BME280DevicePointer->CompParams.dig_T1 << 1)) * ((uint32_t)BME280DevicePointer->CompParams.dig_T2) ) >> 11 ;
     */

    int32_t adc_T = (BME280DevicePointer->rawDataArray[BME280_T_MSB_POS] << 12)
                                                                                            | (BME280DevicePointer->rawDataArray[BME280_T_LSB_POS] << 4 )
                                                                                            | (BME280DevicePointer->rawDataArray[BME280_T_XLSB_POS] >> 4);
    double var1, var2;
    var1 = (((double)adc_T)/16384.0 - ((double)BME280DevicePointer->CompParams.dig_T1)/1024.0) * ((double)BME280DevicePointer->CompParams.dig_T2);
    var2 = ((((double)adc_T)/131072.0 - ((double)BME280DevicePointer->CompParams.dig_T1)/8192.0) *
            (((double)adc_T)/131072.0 - ((double) BME280DevicePointer->CompParams.dig_T1)/8192.0)) * ((double)BME280DevicePointer->CompParams.dig_T3);
    BME280DevicePointer->t_fine = (int32_t)(var1 + var2);
    BME280DevicePointer->temperature = (float)((var1 + var2) / 5120.0);

    return BME280DevicePointer->temperature ;
}






float getPressure_BME280(BME280* BME280DevicePointer, UARTDEVICE* UARTDevicePointer)
{
    /*
    BME280_S32_t var1, var2;
    BME280_U32_t p;
    var1 = (((BME280_S32_t)t_fine)>>1) – (BME280_S32_t)64000;
    var2 = (((var1>>2) * (var1>>2)) >> 11 ) * ((BME280_S32_t)dig_P6);
    var2 = var2 + ((var1*((BME280_S32_t)dig_P5))<<1);
    var2 = (var2>>2)+(((BME280_S32_t)dig_P4)<<16);
    var1 = (((dig_P3 * (((var1>>2) * (var1>>2)) >> 13 )) >> 3) + ((((BME280_S32_t)dig_P2) * var1)>>1))>>18;
    var1 =((((32768+var1))*((BME280_S32_t)dig_P1))>>15);
    if (var1 == 0)
    {
    return 0; // avoid exception caused by division by zero
    }
    p = (((BME280_U32_t)(((BME280_S32_t)1048576)-adc_P)-(var2>>12)))*3125;
    if (p < 0x80000000)
    {
    p = (p << 1) / ((BME280_U32_t)var1);
    }
    else
    {
    p = (p / (BME280_U32_t)var1) * 2;
    }
    var1 = (((BME280_S32_t)dig_P9) * ((BME280_S32_t)(((p>>3) * (p>>3))>>13)))>>12;
    var2 = (((BME280_S32_t)(p>>2)) * ((BME280_S32_t)dig_P8))>>13;
    p = (BME280_U32_t)((BME280_S32_t)p + ((var1 + var2 + dig_P7) >> 4));
    return p;
     */

    /*
    int64_t var1, var2, p;
    var1 = ((int64_t)BME280DevicePointer->t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)BME280DevicePointer->CompParams.dig_P6;
    var2 = var2 + ((var1*(int64_t)BME280DevicePointer->CompParams.dig_P5)<<17);
    var2 = var2 + (((int64_t)BME280DevicePointer->CompParams.dig_P4)<<35);
    var1 = ((var1 * var1 * (int64_t)BME280DevicePointer->CompParams.dig_P3)>>8) + ((var1 * (int64_t)BME280DevicePointer->CompParams.dig_P2)<<12);
    var1 = (((((int64_t)1)<<47)+var1))*((int64_t)BME280DevicePointer->CompParams.dig_P1)>>33;
    if (var1 == 0)
    {
        return 0; // avoid exception caused by division by zero
    }
    int32_t adc_P = (BME280DevicePointer->rawDataArray[BME280_P_MSB_POS] << 12)
                                    | (BME280DevicePointer->rawDataArray[BME280_P_LSB_POS] << 4 )
                                    | (BME280DevicePointer->rawDataArray[BME280_P_XLSB_POS] >> 4);
    p = 1048576-adc_P;
    p = (((p<<31)-var2)*3125)/var1;
    var1 = (((int64_t)BME280DevicePointer->CompParams.dig_P9) * (p>>13) * (p>>13)) >> 25;
    var2 = (((int64_t)BME280DevicePointer->CompParams.dig_P8) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((int64_t)BME280DevicePointer->CompParams.dig_P7)<<4);
    BME280DevicePointer->pressure = (uint32_t)p / 256.0f;

     */
    int32_t adc_P =     (BME280DevicePointer->rawDataArray[BME280_P_MSB_POS] << 12)
                                                                            |   (BME280DevicePointer->rawDataArray[BME280_P_LSB_POS] << 4 )
                                                                            |   (BME280DevicePointer->rawDataArray[BME280_P_XLSB_POS] >> 4);
    double var1, var2, p;
    var1 = ((double)BME280DevicePointer->t_fine/2.0) - 64000.0;
    var2 = var1 * var1 * ((double)BME280DevicePointer->CompParams.dig_P6) / 32768.0;
    var2 = var2 + var1 * ((double)BME280DevicePointer->CompParams.dig_P5) * 2.0;
    var2 = (var2/4.0)+(((double)BME280DevicePointer->CompParams.dig_P4) * 65536.0);
    var1 = (((double)BME280DevicePointer->CompParams.dig_P3) * var1 * var1 / 524288.0 + ((double)BME280DevicePointer->CompParams.dig_P2) * var1) / 524288.0;
    var1 = (1.0 + var1 / 32768.0)*((double)BME280DevicePointer->CompParams.dig_P1);
    if (var1 == 0.0)
    {
        return 0; // avoid exception caused by division by zero
    }
    p = 1048576.0 - (double)adc_P;
    p = (p - (var2 / 4096.0)) * 6250.0 / var1;
    var1 = ((double)BME280DevicePointer->CompParams.dig_P9) * p * p / 2147483648.0;
    var2 = p * ((double)BME280DevicePointer->CompParams.dig_P8) / 32768.0;
    BME280DevicePointer->pressure =(float)( p + (var1 + var2 + ((double)BME280DevicePointer->CompParams.dig_P7)) / 16.0 );
    return BME280DevicePointer->pressure ;
}

float getRelativeHumidity_BME280(BME280* BME280DevicePointer, UARTDEVICE* UARTDevicePointer)
{
    /*
    int32_t v_x1_u32r;
    v_x1_u32r = (BME280DevicePointer->t_fine - ((int32_t)76800));
    v_x1_u32r = (((((adc_H << 14) - (((int32_t)BME280DevicePointer->CompParams.dig_H4) << 20) - (((int32_t)BME280DevicePointer->CompParams.dig_H5) * v_x1_u32r)) +
            ((int32_t)16384)) >> 15) * (((((((v_x1_u32r * ((int32_t)BME280DevicePointer->CompParams.dig_H6)) >> 10) * (((v_x1_u32r *((int32_t)BME280DevicePointer->CompParams.dig_H3)) >> 11) + ((int32_t)32768))) >> 10) + ((int32_t)2097152)) *
                    ((int32_t)BME280DevicePointer->CompParams.dig_H2) + 8192) >> 14));
    v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((int32_t)BME280DevicePointer->CompParams.dig_H1)) >> 4));
    v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
    v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
    BME280DevicePointer->relativeHumidity =  ( (uint32_t)(v_x1_u32r>>12) ) / 1024.0f;
     */
    double var_H;
    int32_t adc_H = (BME280DevicePointer->rawDataArray[BME280_H_MSB_POS] << 8)
                                                                    | (BME280DevicePointer->rawDataArray[BME280_H_LSB_POS] ) ;

    var_H = (((double)BME280DevicePointer->t_fine) - 76800.0);
    var_H = (adc_H - (((double)BME280DevicePointer->CompParams.dig_H4) * 64.0 + ((double)BME280DevicePointer->CompParams.dig_H5) / 16384.0 * var_H)) *
            (((double)BME280DevicePointer->CompParams.dig_H2) / 65536.0 * (1.0 + ((double)BME280DevicePointer->CompParams.dig_H6) / 67108864.0 * var_H *
                    (1.0 + ((double)BME280DevicePointer->CompParams.dig_H3) / 67108864.0 * var_H)));
    var_H = var_H * (1.0 - ((double)BME280DevicePointer->CompParams.dig_H1) * var_H / 524288.0);
    if (var_H > 100.0)
        var_H = 100.0;
    else if (var_H < 0.0)
        var_H = 0.0;
    BME280DevicePointer->relativeHumidity = (float)var_H;
    return BME280DevicePointer->relativeHumidity;
}


float getAltitude_BME280(BME280* BME280DevicePointer, UARTDEVICE* UARTDevicePointer)
{

    BME280DevicePointer->altitude = 44307.69396f * (1.0f - powf(BME280DevicePointer->pressure/101325.0f,0.190284f));

    return BME280DevicePointer->altitude ;


}

void getPHAT_BME280(BME280* BME280DevicePointer, UARTDEVICE* UARTDevicePointer)
{
    readRawData_BME280(BME280DevicePointer) ;
    getTemperature_BME280(BME280DevicePointer, UARTDevicePointer) ;
    getPressure_BME280(BME280DevicePointer, UARTDevicePointer) ;
    getAltitude_BME280(BME280DevicePointer, UARTDevicePointer) ;
    getRelativeHumidity_BME280(BME280DevicePointer, UARTDevicePointer) ;
}

static void readCompensationParameters(BME280* BME280DevicePointer,UARTDEVICE* UARTDevicePointer)
{
    uint8_t compensationParameterDatas[2] = {
                                             0
    };

    // readRegisters_BME280(BME280DevicePointer
    readRegisters_BME280(BME280DevicePointer,BME280_COMPENSATION_PARAMETER_DIG_T1_LSB,2,&compensationParameterDatas[0]) ;
    BME280DevicePointer->CompParams.dig_T1 = ( ((uint16_t)(compensationParameterDatas[1] << 8 )) | compensationParameterDatas[0] );
    UARTWrite(UARTDevicePointer, "%d (%b) %d(%b)\tdig_T1: %d\n",compensationParameterDatas[1],compensationParameterDatas[1],compensationParameterDatas[0],compensationParameterDatas[0],BME280DevicePointer->CompParams.dig_T1);


    readRegisters_BME280(BME280DevicePointer, BME280_COMPENSATION_PARAMETER_DIG_T2_LSB,2,&compensationParameterDatas[0]) ;
    BME280DevicePointer->CompParams.dig_T2 = ( (int16_t)(compensationParameterDatas[1] << 8 ) | compensationParameterDatas[0] );
    UARTWrite(UARTDevicePointer, "%d (%b) %d(%b)\tdig_T2: %d\n",compensationParameterDatas[1],compensationParameterDatas[1],compensationParameterDatas[0],compensationParameterDatas[0],BME280DevicePointer->CompParams.dig_T2);


    readRegisters_BME280(BME280DevicePointer, BME280_COMPENSATION_PARAMETER_DIG_T3_LSB,2,&compensationParameterDatas[0]) ;
    BME280DevicePointer->CompParams.dig_T3 = ( (int16_t)(compensationParameterDatas[1] << 8 ) | compensationParameterDatas[0] );
    UARTWrite(UARTDevicePointer, "%d (%b) %d(%b)\tdig_T3: %d\n",compensationParameterDatas[1],compensationParameterDatas[1],compensationParameterDatas[0],compensationParameterDatas[0],BME280DevicePointer->CompParams.dig_T3);

    readRegisters_BME280(BME280DevicePointer,BME280_COMPENSATION_PARAMETER_DIG_P1_LSB,2,&compensationParameterDatas[0]) ;
    BME280DevicePointer->CompParams.dig_P1 = ( ((uint16_t)(compensationParameterDatas[1] << 8 )) | compensationParameterDatas[0] );
    UARTWrite(UARTDevicePointer, "%d (%b) %d(%b)\tdig_P1: %d\n",compensationParameterDatas[1],compensationParameterDatas[1],compensationParameterDatas[0],compensationParameterDatas[0],BME280DevicePointer->CompParams.dig_P1);

    readRegisters_BME280(BME280DevicePointer, BME280_COMPENSATION_PARAMETER_DIG_P2_LSB,2,&compensationParameterDatas[0]) ;
    BME280DevicePointer->CompParams.dig_P2 = ( (int16_t)(compensationParameterDatas[1] << 8 ) | compensationParameterDatas[0] );
    UARTWrite(UARTDevicePointer, "%d (%b) %d(%b)\tdig_P2: %d\n",compensationParameterDatas[1],compensationParameterDatas[1],compensationParameterDatas[0],compensationParameterDatas[0],BME280DevicePointer->CompParams.dig_P2);

    readRegisters_BME280(BME280DevicePointer, BME280_COMPENSATION_PARAMETER_DIG_P3_LSB,2,&compensationParameterDatas[0]) ;
    BME280DevicePointer->CompParams.dig_P3 = ( (int16_t)(compensationParameterDatas[1] << 8 ) | compensationParameterDatas[0] );
    UARTWrite(UARTDevicePointer, "%d (%b) %d(%b)\tdig_P3: %d\n",compensationParameterDatas[1],compensationParameterDatas[1],compensationParameterDatas[0],compensationParameterDatas[0],BME280DevicePointer->CompParams.dig_P3);

    readRegisters_BME280(BME280DevicePointer, BME280_COMPENSATION_PARAMETER_DIG_P4_LSB,2,&compensationParameterDatas[0]) ;
    BME280DevicePointer->CompParams.dig_P4 = ( (int16_t)(compensationParameterDatas[1] << 8 ) | compensationParameterDatas[0] );
    UARTWrite(UARTDevicePointer, "%d (%b) %d(%b)\tdig_P4: %d\n",compensationParameterDatas[1],compensationParameterDatas[1],compensationParameterDatas[0],compensationParameterDatas[0],BME280DevicePointer->CompParams.dig_P4);

    readRegisters_BME280(BME280DevicePointer, BME280_COMPENSATION_PARAMETER_DIG_P5_LSB,2,&compensationParameterDatas[0]) ;
    BME280DevicePointer->CompParams.dig_P5 = ( (int16_t)(compensationParameterDatas[1] << 8 ) | compensationParameterDatas[0] );
    UARTWrite(UARTDevicePointer, "%d (%b) %d(%b)\tdig_P5: %d\n",compensationParameterDatas[1],compensationParameterDatas[1],compensationParameterDatas[0],compensationParameterDatas[0],BME280DevicePointer->CompParams.dig_P5);

    readRegisters_BME280(BME280DevicePointer, BME280_COMPENSATION_PARAMETER_DIG_P6_LSB,2,&compensationParameterDatas[0]) ;
    BME280DevicePointer->CompParams.dig_P6 = ( (int16_t)(compensationParameterDatas[1] << 8 ) | compensationParameterDatas[0] );
    UARTWrite(UARTDevicePointer, "%d (%b) %d(%b)\tdig_P6: %d\n",compensationParameterDatas[1],compensationParameterDatas[1],compensationParameterDatas[0],compensationParameterDatas[0],BME280DevicePointer->CompParams.dig_P6);

    readRegisters_BME280(BME280DevicePointer, BME280_COMPENSATION_PARAMETER_DIG_P7_LSB,2,&compensationParameterDatas[0]) ;
    BME280DevicePointer->CompParams.dig_P7 = ( (int16_t)(compensationParameterDatas[1] << 8 ) | compensationParameterDatas[0] );
    UARTWrite(UARTDevicePointer, "%d (%b) %d(%b)\tdig_P7: %d\n",compensationParameterDatas[1],compensationParameterDatas[1],compensationParameterDatas[0],compensationParameterDatas[0],BME280DevicePointer->CompParams.dig_P7);

    readRegisters_BME280(BME280DevicePointer, BME280_COMPENSATION_PARAMETER_DIG_P8_LSB,2,&compensationParameterDatas[0]) ;
    BME280DevicePointer->CompParams.dig_P8 = ( (int16_t)(compensationParameterDatas[1] << 8 ) | compensationParameterDatas[0] );
    UARTWrite(UARTDevicePointer, "%d (%b) %d(%b)\tdig_P8: %d\n",compensationParameterDatas[1],compensationParameterDatas[1],compensationParameterDatas[0],compensationParameterDatas[0],BME280DevicePointer->CompParams.dig_P8);

    readRegisters_BME280(BME280DevicePointer, BME280_COMPENSATION_PARAMETER_DIG_P9_LSB,2,&compensationParameterDatas[0]) ;
    BME280DevicePointer->CompParams.dig_P9 = ( (int16_t)(compensationParameterDatas[1] << 8 ) | compensationParameterDatas[0] );
    UARTWrite(UARTDevicePointer, "%d (%b) %d(%b)\tdig_P9: %d\n",compensationParameterDatas[1],compensationParameterDatas[1],compensationParameterDatas[0],compensationParameterDatas[0],BME280DevicePointer->CompParams.dig_P9);


    readRegisters_BME280(BME280DevicePointer,BME280_COMPENSATION_PARAMETER_DIG_H1,1,&compensationParameterDatas[0]);
    BME280DevicePointer->CompParams.dig_H1 = compensationParameterDatas[0] ;
    UARTWrite(UARTDevicePointer, "%d (%b)\tdig_H1: %d\n",compensationParameterDatas[0],compensationParameterDatas[0], BME280DevicePointer->CompParams.dig_H1);


    readRegisters_BME280(BME280DevicePointer, BME280_COMPENSATION_PARAMETER_DIG_H2_LSB,2,&compensationParameterDatas[0]) ;
    BME280DevicePointer->CompParams.dig_H2 = ( (int16_t)(compensationParameterDatas[1] << 8 ) | compensationParameterDatas[0] );
    UARTWrite(UARTDevicePointer, "%d (%b) %d(%b)\tdig_H2: %d\n",compensationParameterDatas[1],compensationParameterDatas[1],compensationParameterDatas[0],compensationParameterDatas[0],BME280DevicePointer->CompParams.dig_H2);

    readRegisters_BME280(BME280DevicePointer,BME280_COMPENSATION_PARAMETER_DIG_H3,1,&compensationParameterDatas[0]);
    BME280DevicePointer->CompParams.dig_H3 = compensationParameterDatas[0] ;
    UARTWrite(UARTDevicePointer, "%d (%b)\tdig_H3: %d\n",compensationParameterDatas[0],compensationParameterDatas[0], BME280DevicePointer->CompParams.dig_H3);

    compensationParameterDatas[0] = readRegister_BME280(BME280DevicePointer, BME280_COMPENSATION_PARAMETER_DIG_H4_MSB);
    compensationParameterDatas[1] = readRegister_BME280(BME280DevicePointer, BME280_COMPENSATION_PARAMETER_DIG_H4_LSB);
    BME280DevicePointer->CompParams.dig_H4 = ((int16_t)( compensationParameterDatas[0] << 4 )
            | (compensationParameterDatas[1] & 0b00001111) ) ;
    UARTWrite(UARTDevicePointer, "%d (%b) %d (%b)\tdig_H4: %d\n",compensationParameterDatas[0],compensationParameterDatas[0],compensationParameterDatas[1],compensationParameterDatas[1], BME280DevicePointer->CompParams.dig_H4);

    compensationParameterDatas[0] = readRegister_BME280(BME280DevicePointer, BME280_COMPENSATION_PARAMETER_DIG_H5_MSB);
    compensationParameterDatas[1] = readRegister_BME280(BME280DevicePointer, BME280_COMPENSATION_PARAMETER_DIG_H5_LSB);
    BME280DevicePointer->CompParams.dig_H5 = ((int16_t)( compensationParameterDatas[0] << 4 )
            | (compensationParameterDatas[1] & 0b11110000) ) ;
    UARTWrite(UARTDevicePointer, "%d (%b) %d (%b)\tdig_H5: %d\n",compensationParameterDatas[0],compensationParameterDatas[0],compensationParameterDatas[1],compensationParameterDatas[1], BME280DevicePointer->CompParams.dig_H5);

    readRegisters_BME280(BME280DevicePointer,BME280_COMPENSATION_PARAMETER_DIG_H6,1,&compensationParameterDatas[0]);
    BME280DevicePointer->CompParams.dig_H6 = compensationParameterDatas[0] ;
    UARTWrite(UARTDevicePointer, "%d (%b)\tdig_H3: %d\n",compensationParameterDatas[0],compensationParameterDatas[0], BME280DevicePointer->CompParams.dig_H6);

}

static uint8_t readRegister_BME280(BME280* BME280DevicePointer, uint8_t registerAddress)
{
    //    readRegister_BME280(BME280DevicePointer
    uint8_t registerValue = 0 ;
#ifdef BME280_USE_I2C
    registerValue = I2CReadByte(&BME280DevicePointer->BME280I2CDevice, registerAddress) ;
#endif
#ifdef BME280_USE_SSI
    setChipSelectPinState_BME280(BME280DevicePointer, BME280_CHIP_SELECT) ;
    SSIWriteValue(&BME280DevicePointer->BME280SSIDevice, registerAddress | BME280_SSI_READ_FROM_REGISTER) ;
    registerValue = SSIReadValue(&BME280DevicePointer->BME280SSIDevice) ;
    setChipSelectPinState_BME280(BME280DevicePointer, BME280_CHIP_UNSELECT) ;
#endif
    return registerValue ;
}
static uint8_t* readRegisters_BME280(BME280* BME280DevicePointer, uint8_t startRegisterAddress, uint8_t numOfRegisters, uint8_t * dataArray)
{
#ifdef BME280_USE_I2C
    I2CReadBytes(&BME280DevicePointer->BME280I2CDevice, startRegisterAddress, numOfRegisters, dataArray) ;
#endif
#ifdef BME280_USE_SSI
    setChipSelectPinState_BME280(BME280DevicePointer, BME280_CHIP_SELECT) ;
    SSIWriteValue(&BME280DevicePointer->BME280SSIDevice, startRegisterAddress | BME280_SSI_READ_FROM_REGISTER) ;
    //    registerValue = SSIReadValue(&BME280DevicePointer->BME280SSIDevice) ;
    uint8_t looper = 0;
    for(looper = 0; looper < numOfRegisters; ++looper)
    {
        dataArray[looper] = SSIReadValue(&BME280DevicePointer->BME280SSIDevice) ;
    }
    setChipSelectPinState_BME280(BME280DevicePointer, BME280_CHIP_UNSELECT) ;
#endif

    return dataArray;
}

static void writeRegister_BME280(BME280* BME280DevicePointer, uint8_t registerAddress, uint8_t registerValue)
{
#ifdef BME280_USE_I2C
    I2CWriteByte(&BME280DevicePointer->BME280I2CDevice, registerAddress, registerValue) ;
#endif
#ifdef BME280_USE_SSI
    setChipSelectPinState_BME280(BME280DevicePointer, BME280_CHIP_SELECT) ;
    SSIWriteValue(&BME280DevicePointer->BME280SSIDevice, ( registerAddress & (~(0x80)) ) | BME280_SSI_WRITE_TO_REGISTER) ;
    SSIWriteValue(&BME280DevicePointer->BME280SSIDevice, registerValue) ;
    setChipSelectPinState_BME280(BME280DevicePointer, BME280_CHIP_UNSELECT) ;
#endif
}


#ifdef BME280_USE_SSI
static void setChipSelectPinState_BME280(BME280* BME280DevicePointer, BME280_CHIP_SELECT_OR_UNSELECT chipSelectState)
{
    digitalWrite(&BME280DevicePointer->chipSelectPin, chipSelectState) ;
}
#endif
