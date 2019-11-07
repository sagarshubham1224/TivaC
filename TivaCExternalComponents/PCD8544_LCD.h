/*
 * PCD8544_LCD.h
 *
 *  Created on: 02-Apr-2018
 *      Author: sagar
 */

#ifndef PCD8544_LCD_H_
#define PCD8544_LCD_H_


#include "stdint.h"
#include "stdbool.h"
#include "ss_TIME.h"
#include "ss_SSI.h"
#include "ss_UART.h"
#include "PCD8544_FONTS.h"

#define PCD8544_CLOCK_FREQUENCY					4000000
#define PCD8544_BYTE_LENGTH						8

#define PCD8544_FUNCTION_SET_COMMAND			0b00100000
#define PCD8544_PD_BIT_CHIP_ACTIVE				0b00000000
#define PCD8544_PD_BIT_CHIP_POWER_DOWN			0b00000100
#define PCD8544_V_BIT_VERTICAL_ADDRESSING		0b00000010
#define PCD8544_V_BIT_HORIZONTAL_ADDRESSING		0b00000000
#define PCD8544_H_BIT_BASIC_INSTRUCTIONS		0b00000000
#define PCD8544_H_BIT_EXTENDED_INSTRUCTIONS		0b00000001


#define PCD8544_VOP_CONTRAST_SET_COMMAND		0b10000000
#define PCD8544_VOP_CONTRAST_SETTINGS			0b00111000				// Experimental.


#define PCD8544_TEMP_COEFF_SETTING_COMMAND		0b00000100
#define PCD8544_TEMP_COEFF_0					0b00000000				// Used this. Experimental.
#define PCD8544_TEMP_COEFF_1					0b00000001
#define PCD8544_TEMP_COEFF_2					0b00000010
#define PCD8544_TEMP_COEFF_3					0b00000011

#define PCD8544_BIAS_SETTING_COMMAND			0b00010000
#define PCD8544_BIAS_SETTING_1_100				0b00000000
#define PCD8544_BIAS_SETTING_1_80				0b00000001
#define PCD8544_BIAS_SETTING_1_65				0b00000010
#define PCD8544_BIAS_SETTING_1_48				0b00000011				// Use this. Experimental.
#define PCD8544_BIAS_SETTING_1_40				0b00000100
#define PCD8544_BIAS_SETTING_1_34				0b00000100
#define PCD8544_BIAS_SETTING_1_24				0b00000101
#define PCD8544_BIAS_SETTING_1_18				0b00000110
#define PCD8544_BIAS_SETTING_1_16				0b00000110
#define PCD8544_BIAS_SETTING_1_10				0b00000111
#define PCD8544_BIAS_SETTING_1_09				0b00000111
#define PCD8544_BIAS_SETTING_1_08				0b00000111



#define PCD8544_DISPLAY_CONTROL_COMMAND			0b00001000
#define PCD8544_DISPLAY_CONTROL_DISPLAY_BLANK	0b00000000
#define PCD8544_DISPLAY_CONTROL_NORMAL_MODE		0b00000100
#define PCD8544_DISPLAY_CONTROL_ALL_DS_ON		0b00000001
#define PCD8544_DISPLAY_CONTROL_INV_VIDEO_MODE	0b00000101



#define PCD8544_X_ADDRESS_SET_COMMAND			0b10000000
#define PCD8544_X_ORIGIN_ADDRESS				0b00000000

#define PCD8544_Y_ADDRESS_SET_COMMAND			0b01000000
#define PCD8544_Y_ORIGIN_ADDRESS				0b00000000


#define PCD8544_WIDTH							84
#define PCD8544_HEIGHT							48



#define instructionTypeBit						0
#define addressingModeBit						1
#define chipStateBit							2
#define displayControlBitD						3
#define displayControlBitE						4




typedef enum PCD8544_DATA_OR_COMMAND {
	PCD8544_DATA		= HIGH,
	PCD8544_COMMAND		= LOW
}PCD8544_DATA_OR_COMMAND;

typedef enum PCD8544_SELECT_UNSELECT {
	PCD8544_SELECT		= LOW,
	PCD8544_UNSELECT	= HIGH
}PCD8544_SELECT_UNSELECT;



typedef struct PCD8544 {
	SSIDEVICE PCD8544_SSIDevice;
	uint8_t statesArray[5];
	GPIO chipSelectPin;
	GPIO resetPin;
	GPIO dataCommandPin;
}PCD8544;



void initLCD_PCD8544(PCD8544* PCD8544DevicePointer, uint8_t SSIPortNumber, char SSIGPIOPortLetter,
						GPIO* chipSelectPinPointer,
						GPIO* resetPinPointer,
						GPIO* dataCommandPinPointer);


void enterExtendedCommandsMode_PCD8544(PCD8544* PCD8544DevicePointer);
void enterBasicCommandsMode_PCD8544(PCD8544* PCD8544DevicePointer);


void clearDDRAM(PCD8544* PCD8544DevicePointer);
void clearDisplay_PCD8544(PCD8544* PCD8544DevicePointer,bool doIReturnToOrigin) ;
void setDisplayControlMode(PCD8544* PCD8544DevicePointer, uint8_t displayControlMode) ;
void setContrast_PCD8544(PCD8544* PCD8544DevicePointer, uint8_t contrastSettings) ;
void setTemperatureCoefficient(PCD8544* PCD8544DevicePointer, uint8_t tempCoeff) ;
void setBiasConfigurations(PCD8544* PCD8544DevicePointer, uint8_t biasSettings) ;
void setXYRAMAddress(PCD8544* PCD8544DevicePointer, uint8_t xPosition, uint8_t yPosition);

void writeSingleValue_PCD8544(PCD8544* PCD8544DevicePointer,PCD8544_DATA_OR_COMMAND mode, uint8_t value);
void writeMultipleDataValues_PCD8544(PCD8544* PCD8544DevicePointer, uint8_t* arrayBuffer, uint16_t arrayBufferSize );

void selectChip_PCD8544(PCD8544* PCD8544DevicePointer) ;
void unselectChip_PCD8544(PCD8544* PCD8544DevicePointer) ;
void setOperationMode_PCD8544(PCD8544* PCD8544DevicePointer, PCD8544_DATA_OR_COMMAND mode) ;
void pulsateResetPin_PCD8544(PCD8544* PCD8544DevicePointer)	;

void writeSingleChar_PCD8544(PCD8544* PCD8544DevicePointer, char charValue) ;


#endif /* PCD8544_LCD_H_ */
