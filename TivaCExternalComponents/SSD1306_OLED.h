/*
 * SSD1306_OLED.h
 *
 *  Created on: 04-Apr-2018
 *      Author: sagar
 */

#ifndef SSD1306_OLED_H_
#define SSD1306_OLED_H_


#include "stdint.h"
#include "stdbool.h"
#include "ss_TIME.h"
#include "ss_SSI.h"
#include "ss_UART.h"
#include "ss_GPIO.h"
#include "SSD1306_FONTS.h"


#define _swap_uint8_t(a,b) {uint8_t t = a; a = b; b = t;}


typedef enum SSD1306_DATA_OR_COMMAND {
	SSD1306_DATA			= 	HIGH,
	SSD1306_COMMAND			= 	LOW
}SSD1306_DATA_OR_COMMAND;


typedef enum SSD1306_SELECT_UNSELECT {
	SSD1306_UNSELECT_CHIP	=	HIGH,
	SSD1306_SELECT_CHIP		=	LOW
}SSD1306_SELECT_UNSELECT ;

#define SSD1306_128x64_WIDTH								128
#define SSD1306_128x64_HEIGHT								64

#define SSD1306_SPI_CLOCK_FREQUENCY							20000000
#define SSD1306_DATAWIDTH									8


#define SSD1306_DISPLAY_ON_OFF_COMMAND						0b10101110
#define SSD1306_DISPLAY_ON									0b00000001
#define SSD1306_DISPLAY_OFF									0b00000000

#define SSD1306_DIV_RATIO_N_OSC_FREQ_COMMAND				0b11010101
#define SSD1306_DIV_RATIO_N_OSC_FREQ_DEFAULT_VALUE			0b10000000

#define SSD1306_SET_MULTIPLEX_RATIO_COMMAND					0b10101000
#define SSD1306_MUX_RATIO_NUMBER							SSD1306_128x64_HEIGHT - 1


#define SSD1306_DISPLAY_OFFSET_COMMAND						0b11010011
#define SSD1306_DISPLAY_OFFSET_VALUE						0b00000000

#define SSD1306_DISPLAY_START_LINE_COMMAND					0b01000000
#define SSD1306_DISPLAY_START_LINE_VALUE					0b00000000


#define SSD1306_CHARGE_PUMP_SETTINGS_COMMAND				0b10001101
#define SSD1306_CHARGE_PUMP_ENABLE							0b00010100
#define SSD1306_CHARGE_PUMP_DISABLE							0b00010100


#define SSD1306_MEMORY_ADDRESSING_MODE_COMMAND				0b00100000
#define SSD1306_MEMORY_ADDRESSING_HORIZONTAL_MODE			0b00000000
#define SSD1306_MEMORY_ADDRESSING_VERTICAL_MODE				0b00000001
#define SSD1306_MEMORY_ADDRESSING_PAGE_MODE					0b00000010


#define SSD1306_SEGMENT_REMAP_COMMAND						0b10100000
#define SSD1306_SEGMENT_REMAP_COL0_SEG0						0b00000000
#define SSD1306_SEGMENT_REMAP_COL127_SEG0					0b00000001

#define SSD1306_COM_OUTPUT_SCAN_DIRECTION_COMMAND			0b11000000
#define SSD1306_COM_OUTPUT_SCAN_DIRECTION_COM0_COMN_1		0b00000000
#define SSD1306_COM_OUTPUT_SCAN_DIRECTION_COMN_1_COM0		0b00001000


#define SSD1306_COM_PINS_HARDWARE_CONFIG_COMMAND			0b11011010
#define SSD1306_COM_PINS_HARDWARE_CONFIG_SEQUENTIAL			0b00000010
#define SSD1306_COM_PINS_HARDWARE_CONFIG_ALTERNATIVE		0b00010010
#define SSD1306_COM_PINS_HARDWARE_CONFIG_DISABLE_REMAP		0b00000010
#define SSD1306_COM_PINS_HARDWARE_CONFIG_ENABLE_REMAP		0b00100010


#define SSD1306_CONTRAST_CONTROL_COMMAND					0b10000001
#define SSD1306_CONTRAST_CONTROL_DEFAULT					0b11001111


#define SSD1306_PRECHARGE_PERIOD_SET_COMMAND				0b11011001
#define SSD1306_PRECHARGE_PERIOD_SET_PHASE_1				0b00000001
#define SSD1306_PRECHARGE_PERIOD_SET_PHASE_2				0b11110000

#define SSD1306_VCOMH_DESELECT_LEVEL_SET_COMMAND			0b11011011
#define SSD1306_VCOMH_DESELECT_LEVEL_SET_POINT86_VCC		0b01000000


#define SSD1306_ENTIRE_DISPLAY_ON_COMMAND					0b10100100
#define SSD1306_ENTIRE_DISPLAY_ON_FOLLOW_RAM_CONTENT		0b00000000
#define SSD1306_ENTIRE_DISPLAY_ON_IGNORE_RAM_CONTENT		0b00000001

#define SSD1306_DISPLAY_MODE_COMMAND						0b10100110
#define SSD1306_DISPLAY_MODE_NORMAL							0b00000000
#define SSD1306_DISPLAY_MODE_INVERSE						0b00000001

#define SSD1306_DEACTIVATE_SCROLL_COMMAND					0b00101110
#define SSD1306_ACTIVATE_SCROLL_COMMAND						0b00101111


#define SSD1306_COLUMN_ADDRESS_SET_COMMAND					0b00100001
#define SSD1306_COLUMN_ADDRESS_SET_END						0b01111111


#define SSD1306_PAGE_ADDRESS_SET_COMMAND					0b00100010
#define SSD1306_PAGE_ADDRESS_SET_END						0b00000111


#define SSD1306_NUMBER_OF_BANKS								8
#define SSD1306_NUMBER_OF_COLUMNS							128

#define SSD1306_FLOAT_PRECISION								3
#define SSD1306_FLOAT_STRING_BUFFER_SIZE					12
#define SSD1306_BINARY_STRING_BUFFER_SIZE		9
#define SSD1306_BINARY_SIZE_08					8

/*
 * Array to hold float value as a String to be printed.
 */
static char floatStringBuffer_SSD1306[SSD1306_FLOAT_STRING_BUFFER_SIZE] = {
		'\0'
};


static char binaryStringBuffer_SSD1306[SSD1306_BINARY_STRING_BUFFER_SIZE] = {
		'\0'
};


static uint8_t GDDRAMArray[SSD1306_NUMBER_OF_BANKS][SSD1306_NUMBER_OF_COLUMNS] = {
		0x00
};
static uint8_t GDDRAMBankKeeper = 0 ;
static uint8_t GDDRAMBankColumnPositionKeeperArray[SSD1306_NUMBER_OF_BANKS][1] = {
		0x00
};

static const char * const g_pcHex = "0123456789ABCDEF";

typedef struct SSD1306 {
	SSIDEVICE SSD1306SSIDevice;
	GPIO chipSelectPin;
	GPIO dataCommandPin;
	GPIO resetPin;
}SSD1306 ;



SSD1306* initialize_SSD1306(SSD1306* SSD1306DevicePointer,
		char chipSelectPinPortLetter, uint8_t chipSelectPinNumber,
		char dataCommandPinPortLetter,uint8_t dataCommandPinNumber,
		char resetPinPortLetter, uint8_t resetPinNumber) ;



void setMemoryAddressingMode_SSD1306(SSD1306* SSD1306DevicePointer, uint8_t addressMode) ;
void pulsateResetPin_SSD1306(SSD1306* SSD1306DevicePointer) ;
void writeSingleValue_SSD1306(SSD1306* SSD1306DevicePointer, SSD1306_DATA_OR_COMMAND dataCommandValue, uint8_t dataInValue) ;

void invertDisplay_SSD1306(SSD1306* SSD1306DevicePointer) ;

void normalDisplay_SSD1306(SSD1306* SSD1306DevicePointer) ;


void setDisplayAddress_SSD1306(SSD1306* SSD1306DevicePointer, uint8_t columnXAddress, uint8_t bankYAddress) ;

void setPixel_SSD1306(SSD1306* SSD1306DevicePointer, uint8_t xPos, uint8_t yPos, UARTDEVICE* UARTDevicePointer) ;

void drawLine_SSD1306(SSD1306* SSD1306DevicePointer, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) ;
void showRAM_SSD1306(SSD1306* SSD1306DevicePointer, UARTDEVICE* UARTDevicePointer) ;



void setOperationMode_SSD1306(SSD1306* SSD1306DevicePointer, SSD1306_DATA_OR_COMMAND dataOrCommand) ;
void unselectChip_SSD1306(SSD1306* SSD1306DevicePointer);
void selectChip_SSD1306(SSD1306* SSD1306DevicePointer);



void clearGDDRAM_SSD1306(SSD1306* SSD1306DevicePointer);
void clearGDDRAMandARRAY_SSD1306(SSD1306* SSD1306DevicePointer) ;


void SSD1306vaListWrite(SSD1306* SSD1306DevicePointer, const char *printString, va_list vaList) ;
void SSD1306Write(SSD1306* SSD1306DevicePointer, const char *printString, ...) ;
void SSD1306vaListWrite(SSD1306* SSD1306DevicePointer, const char *printString, va_list vaList) ;



/*
 * Function to convert a float value to a string of characters.
 * Arguments:
 * 	float floatVal								:: Value in float type to be printed.
 *	uint8_t precision							:: Number of fraction places to be printed. SET as per FLOAT_PRECISION.
 *	uint8_t bufSize								:: Size of char buffer where string would be sored. SET as per FLOAT_STRING_BUFFER_SIZE.
 * Returns:
 * 	uint8_t StringSize							:: Number of characters written to char buffer, This is used in turn by UARTWriteCharString to write the char buffer.
 */
static uint8_t floatToString_SSD1306(float floatVal,uint8_t precision,uint8_t buffSize) ;


/*
 * Function to convert integer value to string of characters in binary.
 * Arguments:
 * 	uint32_t numVal 							:: unsigned integer value.
 * Returns:
 * 	No. of characters written.
 */
static uint8_t intToBinaryString_SSD1306(uint32_t numVal) ;

#endif /* SSD1306_OLED_H_ */
