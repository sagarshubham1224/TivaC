#ifndef LINE_SENSOR_ARRAY_H_
#define LINE_SENSOR_ARRAY_H_

#include "stdbool.h"
#include "stdint.h"
#include "TIMER.h"
#include "I2C.h"
#include "SX1509_REGISTERS.h"

typedef enum LINE_SENSOR_BAR_ON_STATUS {
    LINE_BAR_ON_ONLY_WHEN_READ      = true ,
    LINE_BAR_ALWAYS_ON              = false
}LINE_SENSOR_BAR_ON_STATUS;

typedef enum LINE_SENSOR_LINE_COLOUR {
    WHITE_LINE_ON_BLACK_BACKGROUND  = true ,
    BLACK_LINE_ON_WHITE_BACKGROUND  = false
}LINE_SENSOR_LINE_COLOUR;

typedef enum LINE_SENSOR_INTERRUPT_STATES {
    INT_CHANGE = 0 ,
    INT_RISING = 1,
    INT_FALLING = 2
}LINE_SENSOR_INTERRUPT_STATES;

typedef struct LINE_SENSOR_ARRAY_DEVICE {
    I2CDEVICE                   lineSensorI2CDevice ;
    int8_t                      lineSensorValue ;
    uint8_t                     lastBarRawValue ;
    int8_t                      lastBarPositionValue ;
    uint8_t oscSource ;
    uint8_t oscPinFunction ;
    uint8_t oscFreqOut;
    uint8_t oscDivider;
    LINE_SENSOR_BAR_ON_STATUS   barStrobe ;
    LINE_SENSOR_LINE_COLOUR     invertBits ;
}LINE_SENSOR_ARRAY_DEVICE;



extern uint8_t initialize_lineSensorArrayDevice(LINE_SENSOR_ARRAY_DEVICE *lineSensorArrayDevicePointer,
                                                I2C_PERIPHERAL I2CPeripheralCode,
                                                uint8_t I2CAddress) ;

extern void lineSensor_reset_sx1509(LINE_SENSOR_ARRAY_DEVICE *lineSensorArrayDevicePointer) ;


extern uint8_t lineSensor_getRaw(LINE_SENSOR_ARRAY_DEVICE *lineSensorArrayDevicePointer) ;



extern int8_t lineSensor_getPosition(LINE_SENSOR_ARRAY_DEVICE *lineSensorArrayDevicePointer) ;

extern uint8_t lineSensor_getDensity(LINE_SENSOR_ARRAY_DEVICE *lineSensorArrayDevicePointer) ;

extern void lineSensor_setBarStrobeStatus(LINE_SENSOR_ARRAY_DEVICE *lineSensorArrayDevicePointer,
                                          LINE_SENSOR_BAR_ON_STATUS barStatus) ;

extern void lineSensor_setLineColour(LINE_SENSOR_ARRAY_DEVICE *lineSensorArrayDevicePointer,
                                     LINE_SENSOR_LINE_COLOUR lineColour) ;

extern void lineSensor_debounceConfig(LINE_SENSOR_ARRAY_DEVICE *lineSensorArrayDevicePointer,
                                      uint8_t configValue);

extern void lineSensor_debounceEnable(LINE_SENSOR_ARRAY_DEVICE *lineSensorArrayDevicePointer,
                                      uint8_t pin);

extern void lineSensor_enableInterrupt(LINE_SENSOR_ARRAY_DEVICE *lineSensorArrayDevicePointer,
                                       uint8_t pin, uint8_t riseFall) ;

extern uint16_t lineSensor_interruptSource(LINE_SENSOR_ARRAY_DEVICE *lineSensorArrayDevicePointer) ;

extern void lineSensor_configClock(LINE_SENSOR_ARRAY_DEVICE *lineSensorArrayDevicePointer,
                                   uint8_t oscSource,
                                   uint8_t oscPinFunction,
                                   uint8_t oscFreqOut,
                                   uint8_t oscDivider) ;

static void lineSensor_scan(LINE_SENSOR_ARRAY_DEVICE *lineSensorArrayDevicePointer) ;


static uint8_t lineSensor_readSingleByte(LINE_SENSOR_ARRAY_DEVICE* lineSensorArrayDevicePointer,
                                         uint8_t registerAddress) ;


static uint16_t lineSensor_read_u16_Word(LINE_SENSOR_ARRAY_DEVICE *lineSensorArrayDevicePointer,
                                         uint8_t registerValue) ;


static void lineSensor_writeSingleByte(LINE_SENSOR_ARRAY_DEVICE* lineSensorArrayDevicePointer,
                                       uint8_t registerAddress, uint8_t registerValue) ;

static void lineSensor_write_u16_Word(LINE_SENSOR_ARRAY_DEVICE* lineSensorArrayDevicePointer,
                                      uint8_t startRegisterAddress, uint16_t registerValues) ;




#endif
