#include "LINE_SENSOR_ARRAY.h"

/*
 * MAP
 * void SensorBar::writeByte(uint8_t registerAddress, uint8_t writeValue) = static void lineSensor_writeSingleByte(LINE_SENSOR_ARRAY_DEVICE* lineSensorArrayDevicePointer,
                                       uint8_t registerAddress, uint8_t registerValue)

   void SensorBar::writeWord(uint8_t registerAddress, unsigned int writeValue) = static void lineSensor_write_u16_Word(LINE_SENSOR_ARRAY_DEVICE* lineSensorArrayDevicePointer,
                                       uint8_t startRegisterAddress, uint16_t registerValues)


   unsigned int SensorBar::readWord(uint8_t registerAddress) = static uint16_t lineSensor_read_u16_Word(LINE_SENSOR_ARRAY_DEVICE *lineSensorArrayDevicePointer,
                             uint8_t registerValue)

   uint8_t SensorBar::readByte(uint8_t registerAddress) = static uint8_t lineSensor_readSingleByte(LINE_SENSOR_ARRAY_DEVICE* lineSensorArrayDevicePointer,
                                         uint8_t registerAddress)
 */

extern bool initialize_lineSensorArrayDevice(LINE_SENSOR_ARRAY_DEVICE *lineSensorArrayDevicePointer,
                                                I2C_PERIPHERAL I2CPeripheralCode,
                                                LINE_SENSOR_I2C_ADDRESSES I2CAddress)
{
    bool successfulInitialization = false ;
    initI2CMaster(&lineSensorArrayDevicePointer->lineSensorI2CDevice, I2CPeripheralCode, I2CAddress, FAST_400k_I2C) ;
    lineSensor_reset_sx1509(lineSensorArrayDevicePointer) ;
    uint16_t testRegister = 0 ;

    testRegister = lineSensor_read_u16_Word(lineSensorArrayDevicePointer, REG_INTERRUPT_MASK_A);

    if (testRegister == 0xFF00)
    {
        lineSensor_writeSingleByte(lineSensorArrayDevicePointer, REG_DIR_A, 0xFF) ;
        lineSensor_writeSingleByte(lineSensorArrayDevicePointer, REG_DIR_B, 0xFC) ;
        lineSensor_writeSingleByte(lineSensorArrayDevicePointer, REG_DATA_B, 0x01) ;

        lineSensorArrayDevicePointer->barStrobe = LINE_BAR_ON_ONLY_WHEN_READ ;
        lineSensorArrayDevicePointer->invertBits = BLACK_LINE_ON_WHITE_BACKGROUND ;
        lineSensorArrayDevicePointer->oscSource = 2 ;
        lineSensorArrayDevicePointer->oscPinFunction = 0 ;
        lineSensorArrayDevicePointer->oscFreqOut = 0 ;
        lineSensorArrayDevicePointer->oscDivider = 1 ;
        successfulInitialization = true ;
    }
    else
    {
        successfulInitialization = false;
    }
    return successfulInitialization ;
}




extern void lineSensor_reset_sx1509(LINE_SENSOR_ARRAY_DEVICE *lineSensorArrayDevicePointer)
{
    lineSensor_writeSingleByte(lineSensorArrayDevicePointer, REG_RESET, 0x12) ;
    lineSensor_writeSingleByte(lineSensorArrayDevicePointer, REG_RESET, 0x34) ;
}

extern uint8_t lineSensor_getRaw(LINE_SENSOR_ARRAY_DEVICE *lineSensorArrayDevicePointer)
{
    lineSensor_scan(lineSensorArrayDevicePointer) ;

    return lineSensorArrayDevicePointer->lastBarRawValue ;
}



extern int8_t lineSensor_getPosition(LINE_SENSOR_ARRAY_DEVICE *lineSensorArrayDevicePointer)
{
    int16_t accumulator = 0 ;
    uint8_t bitsCounted = 0 ;
    int16_t i ;

    lineSensor_scan(lineSensorArrayDevicePointer) ;

    for(i = 0; i < 8 ; ++i)
    {
        if( ((lineSensorArrayDevicePointer->lastBarRawValue >> i) & 0x01) == 1)
        {
            ++bitsCounted ;
        }
    }
    for ( i = 7; i > 3; i-- )
    {
        if ( ((lineSensorArrayDevicePointer->lastBarRawValue >> i) & 0x01) == 1 )
        {
            accumulator += ((-32 * (i - 3)) + 1);
        }
    }

    for ( i = 0; i < 4; i++ ) //iterate positive side bits
    {
        if ( ((lineSensorArrayDevicePointer->lastBarRawValue >> i) & 0x01) == 1 )
        {
            accumulator += ((32 * (4 - i)) - 1);
        }
    }

    if(bitsCounted > 0)
    {
        lineSensorArrayDevicePointer->lastBarPositionValue = accumulator/bitsCounted ;
    }
    else
    {
        lineSensorArrayDevicePointer->lastBarPositionValue = 0;
    }

    return lineSensorArrayDevicePointer->lastBarPositionValue  ;
}

extern uint8_t lineSensor_getDensity(LINE_SENSOR_ARRAY_DEVICE *lineSensorArrayDevicePointer)
{
    uint8_t bitsCounted = 0;
    uint8_t i = 0;

    lineSensor_scan(lineSensorArrayDevicePointer) ;

    for(i = 0; i < 8; ++i)
    {
        if( ((lineSensorArrayDevicePointer->lastBarRawValue >> i) & 0x01) == 1)
        {
            ++bitsCounted ;
        }
    }
    return bitsCounted ;
}


extern void lineSensor_setBarStrobeStatus(LINE_SENSOR_ARRAY_DEVICE *lineSensorArrayDevicePointer,
                                          LINE_SENSOR_BAR_ON_STATUS barStatus)
{
    lineSensorArrayDevicePointer->barStrobe = barStatus ;
}


extern void lineSensor_setLineColour(LINE_SENSOR_ARRAY_DEVICE *lineSensorArrayDevicePointer,
                                     LINE_SENSOR_LINE_COLOUR lineColour)
{
    lineSensorArrayDevicePointer->invertBits = lineColour ;
}


extern void lineSensor_debounceConfig(LINE_SENSOR_ARRAY_DEVICE *lineSensorArrayDevicePointer,
                                      uint8_t configValue)
{
    uint8_t tempu8 = lineSensor_readSingleByte(lineSensorArrayDevicePointer, REG_MISC) ;
    if((tempu8 & 0x70) == 0)
    {
        tempu8 |= (1 << 4) ;
        lineSensor_writeSingleByte(lineSensorArrayDevicePointer, REG_MISC, tempu8) ;
    }
    tempu8 = lineSensor_readSingleByte(lineSensorArrayDevicePointer, REG_CLOCK) ;
    if((tempu8 & 0x60) == 0)
    {
        tempu8 |= (1<<6) ;
        lineSensor_writeSingleByte(lineSensorArrayDevicePointer, REG_CLOCK, tempu8) ;
    }

    configValue &= 0b00000111 ;
    lineSensor_writeSingleByte(lineSensorArrayDevicePointer, REG_DEBOUNCE_CONFIG, configValue) ;
}

extern void lineSensor_debounceEnable(LINE_SENSOR_ARRAY_DEVICE *lineSensorArrayDevicePointer,
                                      uint8_t pin)
{
    uint16_t debounceEnable = lineSensor_read_u16_Word(lineSensorArrayDevicePointer, REG_DEBOUNCE_ENABLE_B) ;
    debounceEnable |= (1 << pin) ;
    lineSensor_write_u16_Word(lineSensorArrayDevicePointer, debounceEnable, REG_DEBOUNCE_ENABLE_B) ;
}

extern void lineSensor_enableInterrupt(LINE_SENSOR_ARRAY_DEVICE *lineSensorArrayDevicePointer,
                                       uint8_t pin, LINE_SENSOR_INTERRUPT_STATES riseFall)
{

    uint16_t tempWord = lineSensor_read_u16_Word(lineSensorArrayDevicePointer, REG_INTERRUPT_MASK_B) ;
    tempWord &= ~(1 << pin);  // 0 = event on IO will trigger interrupt
    lineSensor_write_u16_Word(lineSensorArrayDevicePointer, REG_INTERRUPT_MASK_B, tempWord) ;

    uint8_t sensitivity = 0;
    switch (riseFall)
    {
    case INT_CHANGE:
        sensitivity = 0b11;
        break;
    case INT_FALLING:
        sensitivity = 0b10;
        break;
    case INT_RISING:
        sensitivity = 0b01;
        break;
    }

    // Set REG_SENSE_XXX
    // Sensitivity is set as follows:
    // 00: None
    // 01: Rising
    // 10: Falling
    // 11: Both
    uint8_t pinMask = (pin & 0x07) * 2;
    uint8_t senseRegister;

    // Need to select between two words. One for bank A, one for B.
    if (pin >= 8) senseRegister = REG_SENSE_HIGH_B;
    else          senseRegister = REG_SENSE_HIGH_A;

    tempWord = lineSensor_read_u16_Word(lineSensorArrayDevicePointer, senseRegister) ;
    tempWord &= ~(0b11 << pinMask);   // Mask out the bits we want to write
    tempWord |= (sensitivity << pinMask); // Add our new bits
    lineSensor_write_u16_Word(lineSensorArrayDevicePointer, senseRegister, tempWord) ;
}

extern uint16_t lineSensor_interruptSource(LINE_SENSOR_ARRAY_DEVICE *lineSensorArrayDevicePointer)
{
    uint16_t intSource = lineSensor_read_u16_Word(lineSensorArrayDevicePointer, REG_INTERRUPT_SOURCE_B) ;
    lineSensor_write_u16_Word(lineSensorArrayDevicePointer, REG_INTERRUPT_SOURCE_B, 0xFFFF) ;
    return intSource ;
}

extern void lineSensor_configClock(LINE_SENSOR_ARRAY_DEVICE *lineSensorArrayDevicePointer,
                                   uint8_t oscSource,
                                   uint8_t oscPinFunction,
                                   uint8_t oscFreqOut,
                                   uint8_t oscDivider)
{
    // RegClock constructed as follows:
    //    6:5 - Oscillator frequency souce
    //        00: off, 01: external input, 10: internal 2MHz, 1: reserved
    //    4 - OSCIO pin function
    //        0: input, 1 ouptut
    //    3:0 - Frequency of oscout pin
    //        0: LOW, 0xF: high, else fOSCOUT = FoSC/(2^(RegClock[3:0]-1))
    oscSource = (oscSource & 0b11) << 5;      // 2-bit value, bits 6:5
    oscPinFunction = (oscPinFunction & 1) << 4;   // 1-bit value bit 4
    oscFreqOut = (oscFreqOut & 0b1111);   // 4-bit value, bits 3:0
    uint8_t regClock = oscSource | oscPinFunction | oscFreqOut;
    lineSensor_writeSingleByte(lineSensorArrayDevicePointer, REG_CLOCK, regClock) ;

    // Config RegMisc[6:4] with oscDivider
    // 0: off, else ClkX = fOSC / (2^(RegMisc[6:4] -1))
    oscDivider = (oscDivider & 0b111) << 4;   // 3-bit value, bits 6:4
    uint8_t regMisc = lineSensor_readSingleByte(lineSensorArrayDevicePointer, REG_MISC) ;
    regMisc &= ~(0b111 << 4);
    regMisc |= oscDivider;
    lineSensor_writeSingleByte(lineSensorArrayDevicePointer, REG_MISC, regMisc) ;
//    writeByte(REG_MISC, regMisc);
}

static void lineSensor_scan(LINE_SENSOR_ARRAY_DEVICE *lineSensorArrayDevicePointer)
{
    if(lineSensorArrayDevicePointer->barStrobe == LINE_BAR_ON_ONLY_WHEN_READ)
    {
        lineSensor_writeSingleByte(lineSensorArrayDevicePointer, REG_DATA_B, 0x02) ;
        milliSecondDelay(2) ;
        lineSensor_writeSingleByte(lineSensorArrayDevicePointer, REG_DATA_B, 0x00) ;
    }
    else
    {
        lineSensor_writeSingleByte(lineSensorArrayDevicePointer, REG_DATA_B, 0x00) ;
    }
    lineSensorArrayDevicePointer->lastBarRawValue = lineSensor_readSingleByte(lineSensorArrayDevicePointer, REG_DATA_A) ;

    if(lineSensorArrayDevicePointer->invertBits == WHITE_LINE_ON_BLACK_BACKGROUND)
    {
        lineSensorArrayDevicePointer->lastBarRawValue ^= 0xFF ;
    }

    if(lineSensorArrayDevicePointer->barStrobe == LINE_BAR_ON_ONLY_WHEN_READ)
    {
        lineSensor_writeSingleByte(lineSensorArrayDevicePointer, REG_DATA_B, 0x03) ;
    }
}

static uint8_t lineSensor_readSingleByte(LINE_SENSOR_ARRAY_DEVICE* lineSensorArrayDevicePointer,
                                         uint8_t registerAddress)
{
    return I2CReadByte(&lineSensorArrayDevicePointer->lineSensorI2CDevice, registerAddress) ;
}

static uint16_t lineSensor_read_u16_Word(LINE_SENSOR_ARRAY_DEVICE *lineSensorArrayDevicePointer,
                                         uint8_t registerValue)
{
    uint8_t byteArray[2] = {0x00,0x00} ;
    uint16_t returnValue = 0 ;
    I2CReadBytes(&lineSensorArrayDevicePointer->lineSensorI2CDevice, registerValue, 2, &byteArray[0]) ;

    returnValue = (((uint16_t)byteArray[0]) << 8) | byteArray[1] ;

    return returnValue ;
}



static void lineSensor_writeSingleByte(LINE_SENSOR_ARRAY_DEVICE* lineSensorArrayDevicePointer,
                                       uint8_t registerAddress, uint8_t registerValue)
{
    I2CWriteByte(&lineSensorArrayDevicePointer->lineSensorI2CDevice, registerAddress, registerValue) ;
}

static void lineSensor_write_u16_Word(LINE_SENSOR_ARRAY_DEVICE* lineSensorArrayDevicePointer,
                                      uint8_t startRegisterAddress, uint16_t registerValues)
{
    uint8_t msbValue = (uint8_t)((registerValues & 0xFF00) >> 8) ;
    uint8_t lsbValue = (uint8_t)(registerValues & 0x00FF) ;

    I2CWriteByte(&lineSensorArrayDevicePointer->lineSensorI2CDevice, startRegisterAddress, msbValue) ;
    I2CWriteByte(&lineSensorArrayDevicePointer->lineSensorI2CDevice, startRegisterAddress+1, lsbValue) ;
}


