/*
 * MPU9250.h
 *
 *  Created on: 27-Jul-2018
 *      Author: sagarshubham
 */

#ifndef MPU9250_H_
#define MPU9250_H_

//#define MPU9250_USE_DEGREES
//#define MPU9250_USE_RADIANS

//#define MPU9250_SELFTEST_NEEDED

#define MPU9250_CALIBRATION_NEEDED

//#define TEMP_DATA_NEEDED

//#define MPU9250_USE_SSI


#define MPU9250_USE_I2C

/*
 * Pin Connections.
 *
╔═════════════╦═══════════════╦═════════════════════╗
║ SERIAL TYPE ║  MPU9250 PIN  ║      DEVICE PIN     ║
╠═════════════╬═══════════════╬═════════════════════╣
║             ║               ║                     ║
║             ║   SCL/SCLK    ║   SSI CLOCK PIN     ║
║     SSI     ║    SDA/SDI    ║    SSI TX PIN       ║
║             ║    AD0/SDO    ║    SSI RX PIN       ║
║             ║      nCS      ║ SSI CHIP SELECT PIN ║
║             ║               ║                     ║
╚═════════════╩═══════════════╩═════════════════════╝

╔═════════════╦═══════════════╦════════════════╗
║ SERIAL TYPE ║  MPU9250 PIN  ║   DEVICE PIN   ║
╠═════════════╬═══════════════╬════════════════╣
║             ║               ║                ║
║             ║   SCL/SCLK    ║  I2C CLOCK PIN ║
║    I2C      ║    SDA/SDI    ║   I2C DATA PIN ║
╚═════════════╩═══════════════╩════════════════╝
 *
 */

#ifdef MPU9250_USE_SSI
#undef MPU9250_USE_I2C
#endif


#ifdef MPU9250_USE_I2C
#undef MPU9250_USE_SSI
#endif


#include "stdbool.h"
#include "stdint.h"
#include "math.h"
#include "TIMER.h"
#include "UART.h"
#ifdef MPU9250_USE_SSI
#include "ss_SSI.h"
#include "ss_GPIO.h"
#endif
#ifdef MPU9250_USE_I2C
#include "I2C.h"
#endif
#include "MPU9250_REGISTERS.h"


#ifdef MPU9250_USE_SSI

#define MPU9250_SSI_CLOCK_FREQUENCY                     1000000
#define MPU9250_SSI_DATA_WIDTH                          8
#define MPU9250_SSI_READ_REGISTER                       0x80
#define MPU9250_SSI_WRITE_REGISTER                      0x00


typedef enum MPU9250_CHIP_SELECT_OR_UNSELECT {
    MPU9250_CHIP_UNSELECT = HIGH,
    MPU9250_CHIP_SELECT   = LOW
}MPU9250_CHIP_SELECT_OR_UNSELECT ;

#endif



#ifdef MPU9250_USE_I2C
#define MPU9250_SLAVE_ADDRESS_ADO_0_0x68                     0x68
#define MPU9250_SLAVE_ADDRESS_ADO_1_0x69                     0x69
#endif
#ifndef PI
#define PI                                              3.14159265358979f
#endif

#define RADIANS_TO_DEGREES                              (180.0f/PI)
#define DEGREES_TO_RADIANS                              (PI/180.0f)

#define MPU9250_AX_AXIS                                 0
#define MPU9250_AY_AXIS                                 1
#define MPU9250_AZ_AXIS                                 2
#define MPU9250_GX_AXIS                                 0
#define MPU9250_GY_AXIS                                 1
#define MPU9250_GZ_AXIS                                 2
#define MPU9250_AXH_REGISTER_ARRAY_POS                  0
#define MPU9250_AXL_REGISTER_ARRAY_POS                  1
#define MPU9250_AYH_REGISTER_ARRAY_POS                  2
#define MPU9250_AYL_REGISTER_ARRAY_POS                  3
#define MPU9250_AZH_REGISTER_ARRAY_POS                  4
#define MPU9250_AZL_REGISTER_ARRAY_POS                  5
#define MPU9250_TH_REGISTER_ARRAY_POS                   6
#define MPU9250_TL_REGISTER_ARRAY_POS                   7
#define MPU9250_GXH_REGISTER_ARRAY_POS                  8
#define MPU9250_GXL_REGISTER_ARRAY_POS                  9
#define MPU9250_GYH_REGISTER_ARRAY_POS                  10
#define MPU9250_GYL_REGISTER_ARRAY_POS                  11
#define MPU9250_GZH_REGISTER_ARRAY_POS                  12
#define MPU9250_GZL_REGISTER_ARRAY_POS                  13



typedef enum GYRO_FULLSCALE_RANGES {
    G_FS_SEL_250_DPS    = 0b00000000,       // resolution = 2.0f / 262.0f
    G_FS_SEL_500_DPS    = 0b00001000,       // resolution = 4.0f / 262.0f
    G_FS_SEL_1000_DPS   = 0b00010000,       // resolution = 8.0f / 262.0f
    G_FS_SEL_2000_DPS   = 0b00011000        // resolution = 16.0f / 262.0f

}GYRO_FULLSCALE_RANGES ;

typedef enum ACCEL_FULLSCALE_RANGES {
    A_FS_SEL_2_G    = 0b00000000,           // resolution = 2.0f / 32768.0f
    A_FS_SEL_4_G    = 0b00001000,           // resolution = 4.0f / 32768.0f
    A_FS_SEL_8_G    = 0b00010000,           // resolution = 8.0f / 32768.0f
    A_FS_SEL_16_G   = 0b00011000            // resolution = 16.0f / 32768.0f

}ACCEL_FULLSCALE_RANGES ;


typedef struct MPU9250_DEVICE {
#ifdef MPU9250_USE_SSI
    SSIDEVICE MPU9250SSIDevice ;
    GPIO chipSelectPin ;
#endif
#ifdef MPU9250_USE_I2C
    I2CDEVICE MPU9250I2CDevice ;
#endif
    GYRO_FULLSCALE_RANGES gyroFSSEL ;
    ACCEL_FULLSCALE_RANGES accelFSSEL ;

    float accelerometerBiasValues[3] ;
    float gyroscopeBiasValues[3] ;

    float accelerometerResolution ;
    float gyroscopeResolution ;

    float accelerometerAccelerationValues[3] ;
    float gyroscopeAngularVelocityValues[3] ;
}MPU9250_DEVICE ;

extern MPU9250_DEVICE* initialize_MPU9250(MPU9250_DEVICE* MPU9250DevicePointer,
                                          I2C_PERIPHERAL I2CPeripheralCode,
                                          uint8_t MPU9250SlaveAddress,
                                          GYRO_FULLSCALE_RANGES gyroFullScaleRangeValue,
                                          ACCEL_FULLSCALE_RANGES accelFullScaleRangeValue) ;

extern uint8_t whoAmi_MPU9250(MPU9250_DEVICE* MPU9250DevicePointer) ;

#ifdef MPU9250_CALIBRATION_NEEDED
static void calibrate_MPU9250(MPU9250_DEVICE* MPU9250DevicePointer) ;
#endif

extern float readAllValues_MPU9250(MPU9250_DEVICE* MPU9250DevicePointer) ;

extern void readAccelerationValues_MPU9250(MPU9250_DEVICE* MPU9250DevicePointer);

#ifdef TEMP_DATA_NEEDED
extern float readTemperatureValues_MPU9250(MPU9250_DEVICE* MPU9250DevicePointer) ;
#endif

extern void readGyrationValues_MPU9250(MPU9250_DEVICE* MPU9250DevicePointer) ;

#ifdef MPU9250_SELFTEST_NEEDED

extern bool checkSelfTestResults_MPU9250(MPU9250_DEVICE* MPU9250DevicePointer, UARTDEVICE* UARTDevicePointer) ;
static bool isSelfTestPassed_MPU9250(   MPU9250_DEVICE* MPU9250DevicePointer ,
                                        float* accelerometerSelfTestResults,
                                        float* gyroscopeSelfTestResults
/*, UARTDEVICE* UARTDevicePointer*/);
#endif

static inline void readRawAccelerationData_MPU9250(MPU9250_DEVICE* MPU9250DevicePointer);

#ifdef TEMP_DATA_NEEDED
static inline void readRawTemeratureData_MPU9250(MPU9250_DEVICE* MPU9250DevicePointer);
#endif

static inline void readRawGyrationData_MPU9250(MPU9250_DEVICE* MPU9250DevicePointer);

static inline void readRawAllRegisterData_MPU9250(MPU9250_DEVICE* MPU9250DevicePointer);

static uint8_t readByte_mpu9250(MPU9250_DEVICE* MPU9250DevicePointer, uint8_t registerAddress) ;

static uint8_t* readBytes_mpu9250(MPU9250_DEVICE* MPU9250DevicePointer, uint8_t startRegisterAddress,
                                  uint8_t numOfRegisters, uint8_t* dataArray ) ;

static void writeByte_mpu9250(MPU9250_DEVICE* MPU9250DevicePointer, uint8_t registerAddress, uint8_t registerValue) ;


#endif /* MPU9250_H_ */
