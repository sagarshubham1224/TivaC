/*
 * AK8963.h
 *
 *  Created on: 15-Mar-2018
 *      Author: sagar
 */

#ifndef AK8963_H_
#define AK8963_H_

#include "stdint.h"
#include "stdbool.h"
#include "math.h"

#include "ss_I2C.h"
#include "ss_TIME.h"
#include "ss_PeripheralNames.h"

//#define SELFTEST_NEEDED

//#define CALIBRATION_NEEDED

#define MPU9250_PRESENT


#if defined(SELFTEST_NEEDED) || defined(CALIBRATION_NEEDED)
#include "ss_UART.h"
#endif


#define DECLINATION_ANGLE   6.17f

#ifndef PI
#define PI 3.14159265f
#endif

#ifdef MPU9250_PRESENT
#define MPU9250_ADDRESS						0x68
#define MPU9250_PWR_MGMT_1_REGISTER			0x6B
#define MPU9250_SMPLRT_DIV_REGISTER			0x19
#define MPU9250_USER_CTRL_REGISTER			0x6A
#define MPU9250_INT_PIN_CFG_REGISTER		0x37
#define MPU9250_INT_ENABLE_REGISTER			0x38
#endif

#define AK8963_SLAVE_ADDRESS				0x0C
#define AK8963_WAI_RETURN_VALUE				0x48

#define AK8963_WHO_AM_I_REGISTER			0x00
#define AK8963_INFORMATION_REGISTER			0x01
#define AK8963_STATUS1_REGISTER				0x02
#define AK8963_XOUT_L_REGISTER				0x03
#define AK8963_XOUT_H_REGISTER				0x04
#define AK8963_YOUT_L_REGISTER				0x05
#define AK8963_YOUT_H_REGISTER				0x06
#define AK8963_ZOUT_L_REGISTER				0x07
#define AK8963_ZOUT_H_REGISTER				0x08
#define AK8963_STATUS2_REGISTER				0x09
#define AK8963_CONTROL_REGISTER1			0x0A
#define AK8963_CONTROL_REGISTER2			0x0B
#define AK8963_SELF_TEST_CONTROL_REGISTER	0x0C
#define AK8963_TEST1_REGISTER				0x0D
#define AK8963_TEST2_REGISTER				0x0E
#define AK8963_I2C_DISABLE_REGISTER			0x0F
#define AK8963_SENSE_ADJ_X_REGISTER			0x10
#define AK8963_SENSE_ADJ_Y_REGISTER			0x11
#define AK8963_SENSE_ADJ_Z_REGISTER			0x12



#define AK8963_SELF_TEST_CONTROL_REGISTER_SELF_BIT		6
#define AK8963_STATUS1_REGISTER_DRDY_BIT				0
#define AK8963_STATUS2_REGISTER_HOFL_BIT				3
#define AK8963_SOFT_RESET								0x01



#define AK8963_X_AXIS		0
#define AK8963_Y_AXIS		1
#define AK8963_Z_AXIS		2


#define AK8963_XH_REGISTER_POS		1
#define AK8963_XL_REGISTER_POS		0
#define AK8963_YH_REGISTER_POS		3
#define AK8963_YL_REGISTER_POS		2
#define AK8963_ZH_REGISTER_POS		5
#define AK8963_ZL_REGISTER_POS		4
#define AK8963_ST2_REGISTER_POS		6







typedef enum AK8963_OPMODE {
	POWER_DOWN_MODE 					= 0b0000,
	SINGLE_MEASUREMENT_MODE 			= 0b0001,
	CONTINOUS_MEASUREMENT_MODE_8Hz 		= 0b0010,
	CONTINOUS_MEASUREMENT_MODE_100Hz 	= 0b0110,
	EXTERNAL_TRIGGER_MEASUREMENT_MODE	= 0b0100,
	SELF_TEST_MODE						= 0b1000,
	FUSE_ROM_MODE						= 0b1111
}AK8963_OPMODES;


typedef enum AK8963_DATA_RESOLUTION {
	RESOLUTION_14_BIT					= 0b00000000,
	RESOLUTION_16_BIT					= 0b00010000
}AK8963_DATARES;


typedef struct AK8963_DEVICE {
    I2CDEVICE I2CDevice;
    AK8963_OPMODES operationMode;
    AK8963_DATARES dataResolution;
    uint8_t rawMagRegisterDatas[7] ;
    float sensitivityAdjustment_ResolutionValues[3];
    float magneticFluxDensityValues[3];
    float hardIronOffsetValues[3];

}AK8963_DEVICE;

extern AK8963_DEVICE* initialize_AK8963(AK8963_DEVICE* AK8963DevicePointer,
                                        I2C_PERIPHERAL I2CPeripheralCode,
        AK8963_OPMODES opModeVal,
        AK8963_DATARES dataResVal) ;

extern void setHardIronOffsetValues(AK8963_DEVICE* AK8963DevicePointer, float xOffsetValue, float yOffsetValue, float zOffsetValue);

#ifdef SELFTEST_NEEDED
extern bool isSelfTestPassed_AK8963(AK8963_DEVICE* AK8963DevicePointer, UARTDEVICE* UARTDevicePointer);
#endif


extern uint8_t wai_AK8963(AK8963_DEVICE* AK8963DevicePointer);

extern bool readRawMagFluxDataNB_AK8963(AK8963_DEVICE* AK8963DevicePointer);

extern bool readMagFluxDensities_AK8963(AK8963_DEVICE* AK8963DevicePointer);


extern float getHeading_AK8963(AK8963_DEVICE* AK8963DevicePointer) ;
#ifdef CALIBRATION_NEEDED
void calibrate_AK8963(AK8963_DEVICE* AK8963DevicePointer, UARTDEVICE* UARTDevicePointer);
#endif



static void setMode_AK8963(AK8963_DEVICE* AK8963DevicePointer, uint8_t modeValue);


#ifdef MPU9250_PRESENT
static void enableMPU9250Bypass(I2C_PERIPHERAL I2CPeripheralCode);
#endif

#endif /* AK8963_H_ */
