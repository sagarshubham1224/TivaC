/*
 * ST7735R.h
 *
 *  Created on: 19-Jun-2018
 *      Author: sagar
 */

#ifndef ST7735R_H_
#define ST7735R_H_


#include "stdint.h"
#include "stdbool.h"
#include "stdarg.h"
#include "string.h"
#include "inc/hw_gpio.h"
#include "inc/hw_types.h"
//#include "math.h"
#include "TIMER.h"
#include "SSI.h"
#include "GPIO.h"
#include "RGB565.h"
#include "ss_FONTS.h"
#include "stringUtil.h"
#include "PERIPHERALS.h"
//#include "ss_PeripheralNames.h"


/*
 * Pin Connections for ST7735R.
 * Pinout:
 * 	LED			:: Pin for backlight LED.
 * 	SCK			:: Clock Pin for 4-Wire SPI.
 * 	SDA			:: MOSI Pin for 4-Wire SPI.
 * 	A0			:: Data or Command Select Pin.
 * 	RESET		:: Reset Pin for the TFT.
 * 	CS			:: Chip Select Pin for SPI
 * 	GND			:: Ground Pin.
 * 	VCC			:: 3.3V Input Pin.
 */

typedef struct ST7735 {
    SSIDEVICE ST7735SSIDevice ;
    GPIO chipSelectPin ;
    GPIO dataCommandPin ;
    GPIO resetPin ;
    uint8_t width;
    uint8_t height;
    FONT usedFont;
}ST7735 ;

typedef enum ST7735_CHIP_SELECT_OR_UNSELECT {
    ST7735_CHIP_UNSELECT = HIGH,
    ST7735_CHIP_SELECT	 = LOW
}ST7735_CHIP_SELECT_OR_UNSELECT ;

typedef enum ST7735_DATA_OR_COMMAND {
    ST7735_DATA = HIGH,
    ST7735_COMMAND = LOW

}ST7735_DATA_OR_COMMAND ;

typedef enum ST7735_SLEEP_MODE {
    ST7735_SLEEP_IN_MODE_COMMAND = 0x10,
    ST7735_SLEEP_OUT_MODE_COMMAND = 0x11
}ST7735_SLEEP_MODE ;

typedef enum ST7735_COLOR_MODE {
    ST7735_12_BIT_COLOR_MODE = 0x03,
    ST7735_16_BIT_COLOR_MODE = 0x05,
    ST7735_18_BIT_COLOR_MODE = 0x06
}ST7735_COLOR_MODE;

typedef enum ST7735_ORIENTATION {
    ST7735_ORIENT_NORMAL		= 0b00000000,
    ST7735_ORIENT_X_MIRRORED	= 0b01000000,
    ST7735_ORIENT_Y_MIRRORED	= 0b10000000,
    ST7735_ORIENT_XY_MIRRORED	= 0b11000000,
    ST7735_ORIENT_XY_XCHNG		= 0b00100000,
    ST7735_ORIENT_RGB			= 0b00001000,
    ST7735_ORIENT_ML			= 0b00010000,
    ST7735_ORIENT_MH			= 0b00000100,

    ST7735_POTRAIT				= ST7735_ORIENT_NORMAL,
    ST7735_POTRAIT_INVERTED		= ST7735_ORIENT_XY_MIRRORED,
    ST7735_LANDSCAPE			= 0b01100000,
    ST7735_LANDSCAPE_INVERTED	= 0b10100000
}ST7735_ORIENTATION ;

typedef enum ST7735_SUPPORTED_FONTS {
    ST7735_FONT8x8 = 1
}ST7735_SUPPORTED_FONTS;

#define ST7735_SSI_CLOCK_FREQUENCY						25000000
#define ST7735_SSI_DATA_WIDTH							8
#define ST7735_SSI_CPOL_CPHA_SETINGS                    SSI_FRAME_FORMAT_CPOL0_CPHA0
#define ST7735_USE_INTERNAL_CS                          true            // Comment this line to not use internal CS or FSS.

#define  USE_HW_HACK_16_BITS   HWREG(ST7735DevicePointer->ST7735SSIDevice.SSIBase + SSI_O_CR0) |= 0xF
#define  USE_HW_HACK_8_BITS   HWREG(ST7735DevicePointer->ST7735SSIDevice.SSIBase + SSI_O_CR0) &= ~(0xB)

#define ST7735_NO_OPERATION								0x00
#define ST7735_SOFTWARE_RESET_COMMAND					0x01
#define ST7735_SLEEP_IN_MODE_COMMAND					0x10
#define ST7735_SLEEP_OUT_MODE_COMMAND					0x11
#define ST7735_PARTIAL_MODE_ON_COMMAND					0x12
#define ST7735_NORMAL_MODE_ON_COMMAND					0x13
#define ST7735_DISPLAY_INVERSION_OFF_COMMAND			0x20
#define ST7735_DISPLAY_INVERSION_ON_COMMAND				0x21
#define ST7735_GAMMA_CURVE_SELECT_COMMAND				0x26
#define ST7735_DISPLAY_OFF_COMMAND						0x28
#define ST7735_DISPLAY_ON_COMMAND						0x29
#define ST7735_COLUMN_ADDRESS_SET_COMMAND				0x2A
#define ST7735_ROW_ADDRESS_SET_COMMAND					0x2B
#define ST7735_MEMORY_WRITE_COMMAND						0x2C
#define ST7735_RGBSET_LUT_COMMAND						0x2D
#define ST7735_PARTIAL_MODE_START_END_SET_COMMAND		0x30
#define ST7735_TEARING_EFFECT_LINE_OFF_COMMAND			0x34
#define ST7735_TEARING_EFFECT_LINE_ON_COMMAND			0x35
#define ST7735_MEMORY_DATA_ACCESS_CONTROL_COMMAND		0x36
#define ST7735_IDLE_MODE_OFF_COMMAND					0x38
#define ST7735_IDLE_MODE_ON_COMMAND						0x39
#define ST7735_RGB_COLOR_MODE_COMMAND					0x3A

#define ST7735_FRAME_RATE_CONTROL_NORMAL_MODE_COMMAND	0xB1
#define ST7735_FRAME_RATE_CONTROL_IDLE_MODE_COMMAND		0xB2
#define ST7735_FRAME_RATE_CONTROL_PARTIAL_MODE_COMMAND	0xB3
#define ST7735_DISPLAY_INVERSION_CONTROL_COMMAND		0xB4
#define ST7735_POWER_CONTROL1_COMMAND					0xC0
#define ST7735_POWER_CONTROL2_COMMAND					0xC1
#define ST7735_POWER_CONTROL3_NORMAL_MODE_COMMAND		0xC2
#define ST7735_POWER_CONTROL4_IDLE_MODE_COMMAND			0xC3
#define ST7735_POWER_CONTROL4_PARTIAL_MODE_COMMAND		0xC4
#define ST7735_VCOM_CONTROL1_COMMAND					0xC5
#define ST7735_VCOM_OFFSET_CONTROL_COMMAND				0xC7

#define ST7735_GAMMA_ADJUSTMENT_POSITIVE1_COMMAND		0xE0
#define ST7735_GAMMA_ADJUSTMENT_NEGATIVE1_COMMAND		0xE1


#define ST7735_X_MAX_WIDTH								128
#define ST7735_Y_MAX_HEIGHT								160
#define ST7735_TOTAL_PIXELS								(ST7735_X_MAX_WIDTH * ST7735_Y_MAX_HEIGHT)
#ifdef USE_FONT8x8
#define ST7735_FONT8x8_MAX_CHARACTERS_ON_SCREEN         (ST7735_TOTAL_PIXELS / (FONT8x8_HEIGHT * FONT8x8_WIDTH) )
#endif


#define FLOAT_STRING_BUFFER_SIZE        15
#define FLOAT_PRECISION                 3

#define BINARY_STRING_BUFFER_SIZE       33
#define BINARY_SIZE_32                  32
#define BINARY_SIZE_16                  16
#define BINARY_SIZE_08                  8
#define BINARY_SIZE_04                  4
#define BINARY_SIZE_02                  2
#define BINARY_SIZE_01                  1



extern ST7735* initialize_ST7735(ST7735* ST7735DevicePointer,
                                 SSI_PERIPHERAL SSIPeripheralCode ,
#ifndef ST7735_USE_INTERNAL_CS
                                 GPIO_PIN_CODE chipSelectPinCode,
#endif
                                 GPIO_PIN_CODE dataCommandPinCode,
                                 GPIO_PIN_CODE resetPinCode) ;
//                                 char chipSelectPinPortLetter, uint8_t chipSelectPinNumber,
//                               char dataCommandPinPortLetter,uint8_t dataCommandPinNumber,
//                             char resetPinPortLetter, uint8_t resetPinNumber) ;

extern void setFont_ST7735(ST7735* ST7735DevicePointer, ST7735_SUPPORTED_FONTS fontValue) ;

extern void setOrientation_ST7735(ST7735* ST7735DevicePointer, ST7735_ORIENTATION orientation) ;


extern void drawImage_ST7735(ST7735* ST7735DevicePointer,
                             uint8_t x0, uint8_t y0,
                             const uint16_t* imageArrayPointer,
                             uint8_t imageHeight,
                             uint8_t imageWidth) ;

extern void drawPixel_ST7735(ST7735* ST7735DevicePointer,
                             uint8_t x0, uint8_t y0,
                             uint16_t pixelColor) ;

extern void drawVerticalLine_ST7735(ST7735* ST7735DevicePointer,
                                    uint8_t x0, uint8_t y0,
                                    uint8_t lineLength,
                                    uint16_t lineColor) ;

extern void drawHorizontalLine_ST7735(ST7735* ST7735DevicePointer,
                                      uint8_t x0, uint8_t y0,
                                      uint8_t lineLength,
                                      uint16_t lineColor) ;

extern void drawLine_ST7735(ST7735* ST7735DevicePointer,
                            uint8_t x_start, uint8_t y_start,
                            uint8_t x_end, uint8_t y_end,
                            uint16_t lineColor) ;

extern void drawHollowRectangle_ST7735(ST7735* ST7735DevicePointer,
                                       uint8_t x0, uint8_t y0,
                                       uint8_t rectangleWidth, uint8_t rectangleHeight,
                                       uint16_t rectangleColor);

extern void drawSolidRectangle_ST7735(ST7735* ST7735DevicePointer,
                                      uint8_t x0, uint8_t y0,
                                      uint8_t rectangleWidth, uint8_t rectangleHeight,
                                      uint16_t rectangleColor) ;

extern void drawcircle_ST7735(ST7735* ST7735DevicePointer,
                              uint8_t x0, uint8_t y0,
                              uint8_t radius,
                              uint16_t circleColor) ;


extern writeSingleCharacter_ST7735(ST7735* ST7735DevicePointer,
                                   uint8_t xPos, uint8_t yPos,
                                   char letter,
                                   uint16_t letterColor) ;

extern void writeSingleLineString_ST7735(ST7735* ST7735DevicePointer,
                                         uint8_t xPos, uint8_t yPos,
                                         uint8_t *nextXPosPointer, uint8_t *nextYPosPointer,
                                         uint8_t originalXPos,
                                         const char* stringBuffer, uint8_t stringLength,
                                         uint16_t stringColor,
                                         bool bAlignText) ;

extern void writeString_ST7735(ST7735* ST7735DevicePointer,
                               uint8_t xPos, uint8_t yPos,
                               uint8_t *nextXPosPointer, uint8_t *nextYPosPointer,
                               uint8_t originalXPos,
                               const char* stringBuffer, uint16_t stringLength,
                               uint16_t stringColor, bool bAlignText) ;

static void writeText_ST7735vaList(ST7735* ST7735DevicePointer,
                                   uint16_t textColor,
                                   uint8_t xPos, uint8_t yPos,
                                   bool bAlignText ,
                                   const char *printString,
                                   va_list vaList) ;

void writeText_ST7735(ST7735* ST7735DevicePointer,
                      uint16_t textColor,
                      uint8_t xPos, uint8_t yPos,
                      bool bAlignText ,
                      const char *printString, ...) ;


extern void fillTFT_ST7735(ST7735* ST7735DevicePointer, uint16_t screenColor) ;

extern void clearScreen_ST7735(ST7735* ST7735DevicePointer) ;






static void setAddressWindow_ST7735(ST7735* ST7735DevicePointer, uint8_t x_start, uint8_t y_start, uint8_t x_end, uint8_t y_end) ;
static void sendChipCommandParameters_ST7735 (ST7735* ST7735DevicePointer, uint8_t st7735Command, uint8_t numOfParameters, ...) ;
static void sendChipValue_ST7735(ST7735* ST7735DevicePointer, ST7735_DATA_OR_COMMAND dataOrCommand, uint8_t value) ;



static inline void setChipDataCommandState_ST7735(ST7735* ST7735DevicePointer, ST7735_DATA_OR_COMMAND dataOrCommand) ;
#ifndef ST7735_USE_INTERNAL_CS
static inline void setChipSelectState_ST7735(ST7735* ST7735DevicePointer, ST7735_CHIP_SELECT_OR_UNSELECT chipState) ;
#endif

static void hardReset_ST7735(ST7735* ST7735DevicePointer);
static void softReset_ST7735(ST7735* ST7735DevicePointer) ;



#endif /* ST7735R_H_ */
