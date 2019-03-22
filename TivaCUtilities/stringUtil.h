/*
 * stringUtils.h
 * File Description:
 * File to hold functions for string manipulation.
 * From previous experiences, printing floats and binary values has been pain.
 * I created a few functions to help with these issues, but it was observed
 * that I was making copies of the same function for different print devices. There was one for
 * UART Console, another for ST7735 TFT, another for PCD8544, OLED SSD1306 and many others.
 * The aim of this file is
 * 1. Provide Functions which can be used by all these print devices to
 *      a. Reduce overall Code size.
 *      b. Bring uniformity
 * 2. Reduce the overall size of code when using multiple print devices.
 * 3. Make these functions independent of calling bodies, so that they can be later utilized for
 *      RTOS applications as well (minimum dependencies on global variables/ISRs).
 */

#ifndef STRINGUTILS_H_
#define STRINGUTILS_H_

#include "stdint.h"
#include "string.h"


//*****************************************************************************
//
// A mapping from an integer between 0 and 15 to its ASCII character
// equivalent.
//
//*****************************************************************************
static const char * const g_pcHex = "0123456789abcdef";

/*
 * Function to convert a float value to a string of characters.
 * Arguments:
 *  float floatVal                              :: Value in float type to be printed.
 *  uint8_t precision                           :: Number of fraction places to be printed.
 *  uint8_t bufSize                             :: Size of char buffer where string would be stored
 *  char* stringBuffer                          :: Pointer to Array for storing the string representation of the float value.
 * Returns:
 *  uint8_t StringSize                          :: Number of characters written to char buffer,
 *                                                 This is used in turn by the calling Peripheral(or Component) to write the char output.
 */
extern  uint8_t floatToString(float floatVal,uint8_t precision,uint8_t buffSize, char* stringBuffer);

/*
 * Function to convert integer value to string of characters in binary.
 * Arguments:
 *  uint32_t numVal                             :: unsigned integer value.
 *  uint8_t bitSize                             :: maximum bits in the representation. Must be less than equal to 32. The stringBuffer should also be able to support this.
 *  char* stringBuffer                          :: Pointer to Array for storing the string representation of the unsigned integer in binary.
 * Returns:
 *  No. of characters written.
 */
extern uint8_t intToBinaryString(uint32_t numVal, uint8_t bitSize, char* stringBuffer);


#endif
