/*
 * ST7735R.c
 *
 *  Created on: 19-Jun-2018
 *      Author: sagar
 */


#include "ST7735R.h"

/*
 * Array to hold float value as a String to be printed.
 */
static char floatStringBuffer[FLOAT_STRING_BUFFER_SIZE] = {
                                                           '\0'
};
static char binaryStringBuffer[BINARY_STRING_BUFFER_SIZE] = {
                                                             '\0'
};


//Initialization Method.
extern ST7735* initialize_ST7735(ST7735* ST7735DevicePointer,
                                 SSI_PERIPHERAL SSIPeripheralCode ,
                                 GPIO_PIN_CODE chipSelectPinCode,
                                 GPIO_PIN_CODE dataCommandPinCode,
                                 GPIO_PIN_CODE resetPinCode)
{
    milliSecondDelay(200) ;
    initGPIO(&ST7735DevicePointer->chipSelectPin,chipSelectPinCode,OUTPUT) ;
    setChipSelectState_ST7735(ST7735DevicePointer, ST7735_CHIP_UNSELECT) ;
    initGPIO(&ST7735DevicePointer->dataCommandPin, dataCommandPinCode, OUTPUT) ;
    initGPIO(&ST7735DevicePointer->resetPin, resetPinCode, OUTPUT) ;
    hardReset_ST7735(ST7735DevicePointer) ;

    initSSIDEVICE(&ST7735DevicePointer->ST7735SSIDevice, SSIPeripheralCode,ST7735_SSI_CLOCK_FREQUENCY,
                  ST7735_SSI_DATA_WIDTH,ST777735_SSI_CPOL_CPHA_SETINGS, LEAVE_FSS_PIN,LEAVE_SSIRX_PIN) ;


    /*
     * Step 1. Soft Reset the TFT LCD.
     */
    softReset_ST7735(ST7735DevicePointer) ;


    /*
     * Step 2. Bring TFT LCD to Sleep Out Mode from Sleep In Mode.
     * Reset (Hardware or Software), puts the LCD in Sleep In Mode.
     */
    sendChipValue_ST7735(ST7735DevicePointer, ST7735_COMMAND,ST7735_SLEEP_OUT_MODE_COMMAND) ;
    milliSecondDelay(200) ;

    /*
     * Step 3. Set Frame Rate Control in Normal Mode.
     */
    sendChipCommandParameters_ST7735(ST7735DevicePointer,ST7735_FRAME_RATE_CONTROL_NORMAL_MODE_COMMAND, 3,
                                     0x00,0x00,0x00) ;


    /*
     * Step 4. Set Inversion Control to No inversion.
     */
    sendChipCommandParameters_ST7735(ST7735DevicePointer,ST7735_DISPLAY_INVERSION_CONTROL_COMMAND, 1,
                                     0b00000111) ;

    /*
     * Step 5. Set Power Control 1 Command.
     *  0b10100010:0xA2 AVDD = 5, VRHP = 4.6
     *  0b00000010:0x02 VRHN = -4.6
     *  0b01000100:0x44 Mode = 3X
     */
    sendChipCommandParameters_ST7735(ST7735DevicePointer,ST7735_POWER_CONTROL1_COMMAND,3,
                                     0xA2, 0x02, 0x44) ;

    /*
     * Step 6. Set Power Control 2 Command.
     *  0b11000101:0xC5 V25 = 2.4, VGH = 3*AVDD = 3*5 = 15, VGL = -10
     */
    sendChipCommandParameters_ST7735(ST7735DevicePointer,ST7735_POWER_CONTROL2_COMMAND, 1, 0xC5) ;

    /*
     * Step 7. Set Power Control 3 Command.
     *  0b00001010:0x0A
     *  0b00000000:0x00
     *
     *  DCA[9:0] = 0000000000 Booster circuit Step-up cycle in Normal mode = Clock frequency for Booster circuit
     *  SAP[2:0] = 001 Amount of Current in Operational Amplifier = Small
     *  APA[2:0] = 010 Amount of Current in Operational Amplifier = Medium Low
     */
    sendChipCommandParameters_ST7735(ST7735DevicePointer,ST7735_POWER_CONTROL3_NORMAL_MODE_COMMAND,2, 0x0A,0x00) ;

    /*
     * Step 8. Set VCOM Command.
     */
    sendChipCommandParameters_ST7735(ST7735DevicePointer,ST7735_VCOM_CONTROL1_COMMAND,1, 0x04) ;

    /*
     * Step 9. Set Inversion Off.
     */
    sendChipValue_ST7735(ST7735DevicePointer,ST7735_COMMAND, ST7735_DISPLAY_INVERSION_OFF_COMMAND) ;

    /*
     * Step 10. Set Memory Data Access Control Command.
     * MY = 0
     * MX = 0
     * MV = 0
     * ML = 0
     * RGB = 0
     * MH = 0
     * D0 = X
     * D1 = X
     * 0b00000000:0x00
     */
    sendChipCommandParameters_ST7735(ST7735DevicePointer, ST7735_MEMORY_DATA_ACCESS_CONTROL_COMMAND, 1, 0x00) ;
    ST7735DevicePointer->width = ST7735_X_MAX_WIDTH ;
    ST7735DevicePointer->height = ST7735_Y_MAX_HEIGHT ;



    /*
     * Step 11. Set Color Mode Command to 16-bit.
     */
    sendChipCommandParameters_ST7735(ST7735DevicePointer, ST7735_RGB_COLOR_MODE_COMMAND, 1, ST7735_16_BIT_COLOR_MODE) ;


    /*
     * Step 12. Set CASET and RASET Commands.
     *
     */
    sendChipCommandParameters_ST7735(ST7735DevicePointer,ST7735_COLUMN_ADDRESS_SET_COMMAND,4,
                                     0x00,0x00,0x00,0x7F) ;
    sendChipCommandParameters_ST7735(ST7735DevicePointer,ST7735_ROW_ADDRESS_SET_COMMAND, 4,
                                     0x00,0x00,0x00,0x9F) ;


    /*
     * Step 13.Set Positive and Negative Gamma Adjustment Values.
     * Set According to Adafruit's Unicorn Dust.
     * Thanks Adafruit! :)
     */
    sendChipCommandParameters_ST7735(ST7735DevicePointer, ST7735_GAMMA_ADJUSTMENT_POSITIVE1_COMMAND, 16,
                                     0x02, 0x1c, 0x07, 0x12,
                                     0x37, 0x32, 0x29, 0x2d,
                                     0x29, 0x25, 0x2B, 0x39,
                                     0x00, 0x01, 0x03, 0x10) ;


    sendChipCommandParameters_ST7735(ST7735DevicePointer, ST7735_GAMMA_ADJUSTMENT_NEGATIVE1_COMMAND, 16,
                                     0x03, 0x1d, 0x07, 0x06,
                                     0x2E, 0x2C, 0x29, 0x2D,
                                     0x2E, 0x2E, 0x37, 0x3F,
                                     0x00, 0x00, 0x02, 0x10) ;

    /*
     * Step 14. Turn on Display and Normal Mode.
     */
    sendChipValue_ST7735(ST7735DevicePointer,ST7735_COMMAND,ST7735_NORMAL_MODE_ON_COMMAND) ;
    sendChipValue_ST7735(ST7735DevicePointer,ST7735_COMMAND,ST7735_DISPLAY_ON_COMMAND) ;


    /*
     * Step 15. Set Entire Screen BLACK.
     */
    fillTFT_ST7735(ST7735DevicePointer, BLACK_RGB565) ;


    /*
     * Step 16. Set Default Font to FONT8x8
     */
    setFont_ST7735(ST7735DevicePointer, ST7735_FONT8x8) ;


    return ST7735DevicePointer ;
}

extern void setFont_ST7735(ST7735* ST7735DevicePointer, ST7735_SUPPORTED_FONTS fontValue)
{
    switch (fontValue) {
    case ST7735_FONT8x8:
    default:
        ST7735DevicePointer->usedFont.fontArrayPointer = &FONT8x8[0][0] ;
        ST7735DevicePointer->usedFont.fontFirstCharacter = FONT8x8_FIRST_CHARACTER ;
        ST7735DevicePointer->usedFont.fontHeight = FONT8x8_HEIGHT ;
        ST7735DevicePointer->usedFont.fontWidth = FONT8x8_WIDTH ;
        ST7735DevicePointer->usedFont.fontNumberOfCharacters = FONT8x8_NUMBER_OF_CHARACTERS ;
        ST7735DevicePointer->usedFont.fontMaxCharactersOnScreen = ST7735_FONT8x8_MAX_CHARACTERS_ON_SCREEN ;
        break;

    }
}

extern void setOrientation_ST7735(ST7735* ST7735DevicePointer, ST7735_ORIENTATION orientation)
{
    sendChipCommandParameters_ST7735(ST7735DevicePointer, ST7735_MEMORY_DATA_ACCESS_CONTROL_COMMAND, 1, orientation) ;
    if(orientation & ST7735_ORIENT_XY_XCHNG)
    {
        ST7735DevicePointer->width = ST7735_Y_MAX_HEIGHT ;
        ST7735DevicePointer->height = ST7735_X_MAX_WIDTH ;
    }
}

extern void drawImage_ST7735(ST7735* ST7735DevicePointer,
                             uint8_t x0, uint8_t y0,
                             const uint16_t* imageArrayPointer,
                             uint8_t imageHeight,
                             uint8_t imageWidth)
{
    setAddressWindow_ST7735(ST7735DevicePointer, x0, y0, x0+imageWidth, y0+imageHeight) ;
    uint8_t heightScroller = 0, widthScroller = 0;
    for(heightScroller = 0; heightScroller <imageHeight; ++ heightScroller)
    {
        for(widthScroller = 0; widthScroller < imageWidth; ++ widthScroller)
        {
            //twoD[i][j] = oneD[i*w+j]; i over height, j over width.
            if(imageArrayPointer[heightScroller*imageWidth + widthScroller])
                drawPixel_ST7735(ST7735DevicePointer, widthScroller, heightScroller, imageArrayPointer[heightScroller*imageWidth + widthScroller]) ;
            else
                drawPixel_ST7735(ST7735DevicePointer, widthScroller, heightScroller, BLACK_RGB565) ;
        }
    }
}

extern void drawPixel_ST7735(ST7735* ST7735DevicePointer, uint8_t x0, uint8_t y0,uint16_t pixelColor)
{
    setAddressWindow_ST7735(ST7735DevicePointer, x0, y0, x0+1, y0+1) ;
    SSIWriteValue(&ST7735DevicePointer->ST7735SSIDevice, pixelColor >> 8) ;
    SSIWriteValue(&ST7735DevicePointer->ST7735SSIDevice, pixelColor & 0xFF) ;
    setChipSelectState_ST7735(ST7735DevicePointer, ST7735_CHIP_UNSELECT) ;
}
extern inline void drawVerticalLine_ST7735(ST7735* ST7735DevicePointer,
                                           uint8_t x0, uint8_t y0, uint8_t lineLength,uint16_t lineColor)
{
    uint8_t lineColor8LSB   = lineColor & 0xFF , lineColor8MSB  = lineColor >> 8 ;
    setAddressWindow_ST7735(ST7735DevicePointer, x0,y0,x0,y0+lineLength-1) ;
    while(lineLength--)
    {
        SSIWriteValue(&ST7735DevicePointer->ST7735SSIDevice,lineColor8MSB) ;
        SSIWriteValue(&ST7735DevicePointer->ST7735SSIDevice,lineColor8LSB) ;
        ++y0 ;
    }
    setChipSelectState_ST7735(ST7735DevicePointer,ST7735_CHIP_UNSELECT) ;

}
extern inline void drawHorizontalLine_ST7735(ST7735* ST7735DevicePointer,
                                             uint8_t x0, uint8_t y0, uint8_t lineLength,uint16_t lineColor)
{
    uint8_t lineColor8LSB   = lineColor & 0xFF , lineColor8MSB  = lineColor >> 8 ;
    setAddressWindow_ST7735(ST7735DevicePointer, x0,y0,x0+lineLength-1,y0) ;
    while(lineLength--)
    {
        SSIWriteValue(&ST7735DevicePointer->ST7735SSIDevice,lineColor8MSB) ;
        SSIWriteValue(&ST7735DevicePointer->ST7735SSIDevice,lineColor8LSB) ;
        ++x0 ;
    }
    setChipSelectState_ST7735(ST7735DevicePointer,ST7735_CHIP_UNSELECT) ;

}


extern void drawLine_ST7735(ST7735* ST7735DevicePointer,
                            uint8_t x_start, uint8_t y_start,
                            uint8_t x_end, uint8_t y_end,
                            uint16_t lineColor)
{
    if(x_end == x_start)
    {
        if(y_start <= y_end)
        {
            drawVerticalLine_ST7735(ST7735DevicePointer, x_start, y_start, (y_end-y_start), lineColor) ;
        }
        else
        {
            drawVerticalLine_ST7735(ST7735DevicePointer, x_start, y_end, (y_start-y_end), lineColor) ;
        }

        return ;
    }
    else if (y_end == y_start)
    {
        if(x_start <= x_end)
        {
            drawHorizontalLine_ST7735(ST7735DevicePointer, x_start, y_start, (x_end-x_start), lineColor) ;
        }
        else
        {
            drawHorizontalLine_ST7735(ST7735DevicePointer, x_end, y_start, (x_start - x_end), lineColor) ;
        }
        return ;
    }
    else
    {
        int16_t steep = abs(y_end - y_start) > abs(x_end - x_start) ;
        if (steep) {
            x_start ^= y_start ;
            y_start ^= x_start ;
            x_start ^= y_start ;
            x_end ^= y_end ;
            y_end ^= x_end ;
            x_end ^= y_end ;

        }

        if (x_start > x_end) {
            x_start ^= x_end ;
            x_end ^= x_start ;
            x_start ^= x_end ;

            y_start ^= y_end ;
            y_end ^= y_start ;
            y_start ^= y_end ;


        }

        int16_t dx, dy ;
        dx = x_end - x_start ;
        dy = abs(y_end - y_start) ;

        int16_t err = dx / 2 ;
        int16_t ystep ;

        if (y_start < y_end) {
            ystep = 1 ;
        } else {
            ystep = -1 ;
        }

        for ( ; x_start<=x_end ; x_start++) {
            if (steep) {
                drawPixel_ST7735(ST7735DevicePointer, y_start, x_start, lineColor) ;
            } else {
                drawPixel_ST7735(ST7735DevicePointer, x_start, y_start, lineColor) ;
            }
            err -= dy ;
            if (err < 0) {
                y_start += ystep ;
                err += dx ;
            }
        }
    }
}


extern void drawHollowRectangle_ST7735(ST7735* ST7735DevicePointer,
                                       uint8_t x0, uint8_t y0,
                                       uint8_t rectangleWidth, uint8_t rectangleHeight,
                                       uint16_t rectangleColor)
{
    drawHorizontalLine_ST7735(ST7735DevicePointer, x0,y0,rectangleWidth, rectangleColor) ;
    drawVerticalLine_ST7735(ST7735DevicePointer, x0,y0,rectangleHeight,  rectangleColor) ;
    drawHorizontalLine_ST7735(ST7735DevicePointer, x0,y0+rectangleHeight,rectangleWidth, rectangleColor) ;
    drawVerticalLine_ST7735(ST7735DevicePointer, x0+rectangleWidth,y0,rectangleHeight+1,  rectangleColor) ;
}

extern void drawSolidRectangle_ST7735(ST7735* ST7735DevicePointer,
                                      uint8_t x0, uint8_t y0,
                                      uint8_t rectangleWidth, uint8_t rectangleHeight,
                                      uint16_t rectangleColor)
{

    uint8_t rectangleColor8LSB = rectangleColor & 0xFF , rectangleColor8MSB = rectangleColor>>8 ;

    uint16_t  xPos = 0 , totalPixels = rectangleHeight * rectangleWidth ;
    setAddressWindow_ST7735(ST7735DevicePointer,x0,y0,x0+rectangleWidth-1,y0+rectangleHeight-1) ;

    for( ;xPos < totalPixels ;++xPos)
    {
        SSIWriteValue(&ST7735DevicePointer->ST7735SSIDevice,rectangleColor8MSB) ;
        SSIWriteValue(&ST7735DevicePointer->ST7735SSIDevice,rectangleColor8LSB) ;
    }
    setChipSelectState_ST7735(ST7735DevicePointer,ST7735_CHIP_UNSELECT) ;

}

extern void drawcircle_ST7735(ST7735* ST7735DevicePointer, uint8_t x0, uint8_t y0, uint8_t radius, uint16_t circleColor)
{
    if(!radius)
    {
        return ;
    }
    uint8_t x = radius-1 ;
    uint8_t y = 0 ;
    uint8_t dx = 1 ;
    uint8_t dy = 1 ;
    int8_t err = dx - (radius << 1) ;

    while (x >= y)
    {
        drawPixel_ST7735(ST7735DevicePointer,x0 + x, y0 + y,circleColor) ;
        drawPixel_ST7735(ST7735DevicePointer,x0 + y, y0 + x,circleColor) ;
        drawPixel_ST7735(ST7735DevicePointer,x0 - y, y0 + x,circleColor) ;
        drawPixel_ST7735(ST7735DevicePointer,x0 - x, y0 + y,circleColor) ;
        drawPixel_ST7735(ST7735DevicePointer,x0 - x, y0 - y,circleColor) ;
        drawPixel_ST7735(ST7735DevicePointer,x0 - y, y0 - x,circleColor) ;
        drawPixel_ST7735(ST7735DevicePointer,x0 + y, y0 - x,circleColor) ;
        drawPixel_ST7735(ST7735DevicePointer,x0 + x, y0 - y,circleColor) ;

        if (err <= 0)
        {
            y++ ;
            err += dy ;
            dy += 2 ;
        }

        if (err > 0)
        {
            x-- ;
            dx += 2 ;
            err += dx - (radius << 1) ;
        }
    }
}


extern writeSingleCharacter_ST7735(ST7735* ST7735DevicePointer,
                                   uint8_t xPos, uint8_t yPos,
                                   char character,
                                   uint16_t letterColor)
                                        {
    setAddressWindow_ST7735(ST7735DevicePointer, xPos, yPos,
                            xPos + ST7735DevicePointer->usedFont.fontWidth -1,
                            yPos + ST7735DevicePointer->usedFont.fontHeight - 1) ;
    uint8_t characterHeightLooper = 0 ;
    for (characterHeightLooper = 0 ; characterHeightLooper < ST7735DevicePointer->usedFont.fontHeight ; ++characterHeightLooper)
    {

        //        uint8_t characterWidthValue = FONT8x8[letter - FONT8x8_FIRST_CHARACTER][characterHeightLooper] ;
        uint8_t characterRow = (uint8_t)(character - ST7735DevicePointer->usedFont.fontFirstCharacter) ;
        uint8_t characterWidthValue = *(ST7735DevicePointer->usedFont.fontArrayPointer + characterRow*ST7735DevicePointer->usedFont.fontWidth + characterHeightLooper) ;
        uint8_t characterWidthLooper = 0 ;
        for (characterWidthLooper = 0 ; characterWidthLooper < ST7735DevicePointer->usedFont.fontWidth ; ++characterWidthLooper) {
            if(characterWidthValue & 0x80)
            {
                SSIWriteValue(&ST7735DevicePointer->ST7735SSIDevice, letterColor >> 8) ;
                SSIWriteValue(&ST7735DevicePointer->ST7735SSIDevice, letterColor & 0xFF) ;
            }
            else
            {
                SSIWriteValue(&ST7735DevicePointer->ST7735SSIDevice, 0x00) ;
                SSIWriteValue(&ST7735DevicePointer->ST7735SSIDevice, 0x00) ;
            }
            characterWidthValue = characterWidthValue << 1 ;
        }
    }
    setChipSelectState_ST7735(ST7735DevicePointer, ST7735_CHIP_UNSELECT) ;
                                        }

extern void writeSingleLineString_ST7735(ST7735* ST7735DevicePointer,
                                         uint8_t xPos, uint8_t yPos,
                                         uint8_t *nextXPosPointer, uint8_t *nextYPosPointer,
                                         uint8_t originalXPos,
                                         const char* stringBuffer, uint8_t stringLength,
                                         uint16_t stringColor,
                                         bool bAlignText)
{
    setAddressWindow_ST7735(ST7735DevicePointer,
                            xPos, yPos,
                            xPos + stringLength * ST7735DevicePointer->usedFont.fontWidth - 1,
                            yPos + ST7735DevicePointer->usedFont.fontHeight - 1) ;
    *nextXPosPointer = xPos + (stringLength) * ST7735DevicePointer->usedFont.fontWidth ;
    *nextYPosPointer = yPos ;

    if(*nextXPosPointer + ST7735DevicePointer->usedFont.fontWidth > ST7735DevicePointer->width)
    {
        *nextYPosPointer = yPos + ST7735DevicePointer->usedFont.fontHeight ;
        if(bAlignText)
            *nextXPosPointer = originalXPos ;
        else
            *nextXPosPointer = 0 ;
    }
    uint8_t characterHeightLooper = 0 ;
    uint8_t colorMSB = stringColor >> 8, colorLSB = stringColor & 0xFF ;
    for(characterHeightLooper = 0 ; characterHeightLooper < ST7735DevicePointer->usedFont.fontHeight ; ++characterHeightLooper)
    {
        uint8_t charLooper = 0 ;
        for(charLooper = 0 ; charLooper < stringLength; ++charLooper)
        {
            uint8_t characterRow = (uint8_t)(stringBuffer[charLooper] - ST7735DevicePointer->usedFont.fontFirstCharacter) ;
            uint8_t characterWidthValue = *(ST7735DevicePointer->usedFont.fontArrayPointer + characterRow*ST7735DevicePointer->usedFont.fontWidth + characterHeightLooper) ;
            uint8_t charWidthLooper = 0 ;
            for(charWidthLooper = 0 ; charWidthLooper < ST7735DevicePointer->usedFont.fontWidth ; ++charWidthLooper)
            {
                if(characterWidthValue & 0x80)
                {
                    SSIWriteValue(&ST7735DevicePointer->ST7735SSIDevice, colorMSB) ;
                    SSIWriteValue(&ST7735DevicePointer->ST7735SSIDevice, colorLSB) ;
                }
                else
                {
                    SSIWriteValue(&ST7735DevicePointer->ST7735SSIDevice, 0x00) ;
                    SSIWriteValue(&ST7735DevicePointer->ST7735SSIDevice, 0x00) ;
                }
                characterWidthValue = characterWidthValue << 1 ;
            }
        }
    }
    setChipSelectState_ST7735(ST7735DevicePointer, ST7735_CHIP_UNSELECT) ;

}
extern void writeString_ST7735(ST7735* ST7735DevicePointer,
                               uint8_t xPos, uint8_t yPos,
                               uint8_t *nextXPosPointer, uint8_t *nextYPosPointer,
                               uint8_t originalXPos,
                               const char* stringBuffer, uint16_t stringLength,
                               uint16_t stringColor, bool bAlignText)
{
    if(yPos + ST7735DevicePointer->usedFont.fontHeight > ST7735DevicePointer->height)
    {
        yPos = 0 ;
    }
    if(stringLength > ST7735DevicePointer->usedFont.fontMaxCharactersOnScreen)
    {
        stringLength = ST7735DevicePointer->usedFont.fontMaxCharactersOnScreen ;
    }
    int16_t len = ST7735DevicePointer->width - (xPos + stringLength * ST7735DevicePointer->usedFont.fontWidth) ;
    if(len < 0)
    {
        uint8_t firstLineLength = (stringLength*ST7735DevicePointer->usedFont.fontWidth + len)/ST7735DevicePointer->usedFont.fontWidth ;
        if(firstLineLength)
            writeSingleLineString_ST7735(ST7735DevicePointer,
                                         xPos, yPos,
                                         nextXPosPointer, nextYPosPointer,
                                         originalXPos,
                                         stringBuffer,
                                         firstLineLength,
                                         stringColor,
                                         bAlignText) ;
        xPos = *nextXPosPointer ;
        if(bAlignText)
        {
            writeString_ST7735(ST7735DevicePointer,
                               xPos, yPos+ST7735DevicePointer->usedFont.fontHeight,
                               nextXPosPointer, nextYPosPointer,
                               originalXPos,
                               &stringBuffer[firstLineLength],
                               stringLength - firstLineLength,
                               stringColor,
                               bAlignText) ;
            xPos = *nextXPosPointer ;
        }
        else
        {
            writeString_ST7735(ST7735DevicePointer,
                               0, yPos+ST7735DevicePointer->usedFont.fontHeight,
                               nextXPosPointer, nextYPosPointer,
                               originalXPos,
                               &stringBuffer[firstLineLength],
                               stringLength - firstLineLength,
                               stringColor,
                               bAlignText) ;
            xPos = *nextXPosPointer ;
        }
    }
    else
    {
        writeSingleLineString_ST7735(ST7735DevicePointer,
                                     xPos,yPos,
                                     nextXPosPointer, nextYPosPointer,
                                     originalXPos,
                                     stringBuffer,
                                     stringLength,
                                     stringColor,
                                     bAlignText) ;
        xPos = *nextXPosPointer ;
    }

}

static void incrementXYPos(uint8_t *xPosPointer, uint8_t *yPosPointer,
                           uint8_t *nextXPosPointer,uint8_t *nextYPosPointer)
{
    *yPosPointer = *nextYPosPointer ;
    *xPosPointer = *nextXPosPointer ;

}
static void writeText_ST7735vaList(ST7735* ST7735DevicePointer,
                                   uint16_t textColor,
                                   uint8_t xPos, uint8_t yPos,
                                   bool bAlignText ,
                                   const char *printString,
                                   va_list vaList)
{
    uint32_t ui32Idx, ui32Value, ui32Pos, ui32Count, ui32Base, ui32Neg;
    uint8_t nextXPos = xPos, nextYPos = yPos , originalXPos = xPos;
    char *pcStr, pcBuf[16], cFill;
    float floatVal;
    while(*printString)
    {
        for(ui32Idx = 0; (printString[ui32Idx] != '%') && (printString[ui32Idx] != '\0');ui32Idx++);    // Find the first non-% character, or the end of the string.
        writeString_ST7735(ST7735DevicePointer,
                           xPos, yPos,
                           &nextXPos, &nextYPos,
                           originalXPos,
                           printString,
                           ui32Idx,
                           textColor,
                           bAlignText) ;
        incrementXYPos(  &xPos, &yPos, &nextXPos, &nextYPos);
        printString += ui32Idx;         // Skip the portion of the string that was written.
        if(*printString == '%')         // See if the next character is a %.
        {
            printString++;              // Skip the %.
            ui32Count = 0;              //Set the digit count to zero, and the fill character to space (in other words, to the defaults).
            cFill = ' ';

            again:                      // It may be necessary to get back here to process more characters.
            switch(*printString++)      // Determine how to handle the next character.
            {

            case '0':                   // Handle the digit characters.
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            {
                if((printString[-1] == '0') && (ui32Count == 0)) //If this is a zero, and it is the first digit, then the fill character is a zero instead of a space.
                {
                    cFill = '0';
                }
                ui32Count *= 10;                    // Update the digit count.
                ui32Count += printString[-1] - '0';
                goto again;                         // Get the next character.
            }

            case 'c':                               // Handle the %c command.
            {
                ui32Value = va_arg(vaList, uint32_t);                       // Get the value from the varargs.
                writeString_ST7735(ST7735DevicePointer,
                                   xPos, yPos,
                                   &nextXPos, &nextYPos,
                                   originalXPos,
                                   (char*)(&ui32Value),
                                   1,
                                   textColor,
                                   bAlignText) ;
                incrementXYPos( &xPos,&yPos,&nextXPos, &nextYPos) ;
                break;                                                      // This command has been handled.
            }

            case 'd':           // Handle the %d and %i commands.
            case 'i':
            {
                ui32Value = va_arg(vaList, uint32_t);                       // Get the value from the varargs.
                ui32Pos = 0;                                                // Reset the buffer position.
                if((int32_t)ui32Value < 0)                                  // If the value is negative, make it positive and indicate that a minus sign is needed.
                {
                    ui32Value = -(int32_t)ui32Value;                        // Make the value positive.
                    ui32Neg = 1;                                            // Indicate that the value is negative.
                }
                else
                {
                    ui32Neg = 0;                                            // Indicate that the value is positive so that a minus sign isn't inserted.
                }
                ui32Base = 10;                                              // Set the base to 10.
                goto convert;                                               // Convert the value to ASCII.
            }

            case 's':                                                       // Handle the %s command.
            {
                pcStr = va_arg(vaList, char *);                             // Get the string pointer from the varargs.
                for(ui32Idx = 0; pcStr[ui32Idx] != '\0'; ui32Idx++);        // Determine the length of the string.
                writeString_ST7735(ST7735DevicePointer,
                                   xPos, yPos,
                                   &nextXPos, &nextYPos,
                                   originalXPos,
                                   pcStr,
                                   ui32Idx,
                                   textColor,
                                   bAlignText) ;
                incrementXYPos( &xPos, &yPos, &nextXPos, &nextYPos) ;
                if(ui32Count > ui32Idx)                                     // Write any required padding spaces
                {
                    ui32Count -= ui32Idx;
                    while(ui32Count--)
                    {
                        writeString_ST7735(ST7735DevicePointer,
                                           xPos, yPos,
                                           &nextXPos, &nextYPos,
                                           originalXPos,
                                           " " ,
                                           1,
                                           textColor,
                                           bAlignText) ;
                        incrementXYPos( &xPos, &yPos,  &nextXPos, &nextYPos) ;
                    }
                }
                break;                                                      // This command has been handled.
            }

            case 'u':                                                       // Handle the %u command.
            {
                ui32Value = va_arg(vaList, uint32_t);                       // Get the value from the varargs.
                ui32Pos = 0;                                                // Reset the buffer position.
                ui32Base = 10;                                              // Set the base to 10.
                ui32Neg = 0;                                                // Indicate that the value is positive so that a minus sign isn't inserted.
                goto convert;                                               // Convert the value to ASCII.
            }

            // Handle the %x and %X commands.  Note that they are treated
            // identically; in other words, %X will use lower case letters
            // for a-f instead of the upper case letters it should use.  We
            // also alias %p to %x.
            case 'x':
            case 'X':
            case 'p':
            {
                ui32Value = va_arg(vaList, uint32_t);                       // Get the value from the varargs.
                ui32Pos = 0;                                                // Reset the buffer position.
                ui32Base = 16;                                              // Set the base to 16.
                ui32Neg = 0;                                                // Indicate that the value is positive so that a minus sign isn't inserted.

                convert:
                for(ui32Idx = 1;
                        (((ui32Idx * ui32Base) <= ui32Value) &&
                                (((ui32Idx * ui32Base) / ui32Base) == ui32Idx));    // Determine the number of digits in the string version of the value.
                        ui32Idx *= ui32Base, ui32Count--)
                {
                }
                if(ui32Neg)                                                 // If the value is negative, reduce the count of padding characters needed.
                {
                    ui32Count--;
                }
                if(ui32Neg && (cFill == '0'))   // If the value is negative and the value is padded with zeros, then place the minus sign before the padding.
                {
                    pcBuf[ui32Pos++] = '-';                                 // Place the minus sign in the output buffer.
                    ui32Neg = 0;                                            // The minus sign has been placed, so turn off the negative flag.
                }

                if((ui32Count > 1) && (ui32Count < 16))                     // Provide additional padding at the beginning of the string conversion if needed.
                {
                    for(ui32Count--; ui32Count; ui32Count--)
                    {
                        pcBuf[ui32Pos++] = cFill;
                    }
                }

                if(ui32Neg)                                                 // If the value is negative, then place the minus sign before the number.
                {
                    pcBuf[ui32Pos++] = '-';                                 // Place the minus sign in the output buffer.
                }
                for(; ui32Idx; ui32Idx /= ui32Base)                         // Convert the value into a string.
                {
                    pcBuf[ui32Pos++] = g_pcHex[(ui32Value / ui32Idx) % ui32Base];
                }
                writeString_ST7735(ST7735DevicePointer,
                                   xPos, yPos,
                                   &nextXPos, &nextYPos,
                                   originalXPos,
                                   pcBuf,
                                   ui32Pos,
                                   textColor,
                                   bAlignText) ;
                incrementXYPos( &xPos, &yPos, &nextXPos, &nextYPos) ;

                break;                                                      // This command has been handled.
            }

            case '%':                                                       // Handle the %% command.
            {
                writeString_ST7735(ST7735DevicePointer,
                                   xPos, yPos,
                                   &nextXPos, &nextYPos,
                                   originalXPos,
                                   "%",
                                   1,
                                   textColor,
                                   bAlignText) ;
                incrementXYPos(&xPos, &yPos, &nextXPos, &nextYPos) ;
                break;                                                      // This command has been handled.
            }

            case 'f':                                                       // Handle %f float values.
            {
                floatVal = (float) (va_arg(vaList, double)) ;
                uint8_t floatStringSize = floatToString(floatVal, FLOAT_PRECISION, FLOAT_STRING_BUFFER_SIZE,floatStringBuffer);
                writeString_ST7735(ST7735DevicePointer,
                                   xPos, yPos,
                                   &nextXPos, &nextYPos,
                                   originalXPos,
                                   floatStringBuffer,
                                   floatStringSize,
                                   textColor,
                                   bAlignText) ;
                incrementXYPos( &xPos, &yPos, &nextXPos, &nextYPos) ;
                break;                                                      // This command has been handled.
            }
            case 'b':
            {
                ui32Value = (va_arg(vaList, uint32_t));
                uint8_t binaryStringSize = intToBinaryString(ui32Value,BINARY_SIZE_08,binaryStringBuffer);
                  writeString_ST7735(ST7735DevicePointer,
                                   xPos, yPos,
                                   &nextXPos, &nextYPos,
                                   originalXPos,
                                   binaryStringBuffer,
                                   binaryStringSize,
                                   textColor,
                                   bAlignText) ;
                incrementXYPos( &xPos, &yPos, &nextXPos, &nextYPos) ;

                break;
            }

            default:
            {
                //
                // Indicate an error.
                //
                break;                                                      // This command has been handled.
            }
            }
        }
    }
}

void writeText_ST7735(ST7735* ST7735DevicePointer,
                      uint16_t textColor,
                      uint8_t xPos, uint8_t yPos,
                      bool bAlignText ,
                      const char *printString, ...)
{
    va_list vaList;
    va_start(vaList, printString);
    writeText_ST7735vaList(ST7735DevicePointer,
                           textColor,
                           xPos, yPos,
                           bAlignText,
                           printString,
                           vaList);
    va_end(vaList);
}


extern void fillTFT_ST7735(ST7735* ST7735DevicePointer, uint16_t screenColor)
{
    uint8_t color8LSB = screenColor & 0xFF , color8MSB = screenColor >> 8 ;

    uint16_t  xPos = 0 ;
    setAddressWindow_ST7735(ST7735DevicePointer,0,0,ST7735DevicePointer->width-1,ST7735DevicePointer->height-1) ;

    for( ;xPos < ST7735_TOTAL_PIXELS ;++xPos)
    {
        SSIWriteValue(&ST7735DevicePointer->ST7735SSIDevice,color8MSB) ;
        SSIWriteValue(&ST7735DevicePointer->ST7735SSIDevice,color8LSB) ;
    }
    setChipSelectState_ST7735(ST7735DevicePointer,ST7735_CHIP_UNSELECT) ;

}

extern void clearScreen_ST7735(ST7735* ST7735DevicePointer)
{
    uint16_t  xPos = 0 ;
    setAddressWindow_ST7735(ST7735DevicePointer,0,0,ST7735DevicePointer->width-1,ST7735DevicePointer->height-1) ;

    for( ;xPos < ST7735_TOTAL_PIXELS ;++xPos)
    {
        SSIWriteValue(&ST7735DevicePointer->ST7735SSIDevice,0x00) ;
        SSIWriteValue(&ST7735DevicePointer->ST7735SSIDevice,0x00) ;
    }
    setChipSelectState_ST7735(ST7735DevicePointer,ST7735_CHIP_UNSELECT) ;
}

static void setAddressWindow_ST7735(ST7735* ST7735DevicePointer,
                                    uint8_t x_start, uint8_t y_start,
                                    uint8_t x_end, uint8_t y_end)
{


    // Write the Column Address Set Command.
    setChipDataCommandState_ST7735(ST7735DevicePointer,ST7735_COMMAND) ;
    setChipSelectState_ST7735(ST7735DevicePointer,ST7735_CHIP_SELECT) ;
    SSIWriteValue(&ST7735DevicePointer->ST7735SSIDevice,ST7735_COLUMN_ADDRESS_SET_COMMAND) ;
    // Write Parameters.
    setChipDataCommandState_ST7735(ST7735DevicePointer,ST7735_DATA) ;
    SSIWriteValue(&ST7735DevicePointer->ST7735SSIDevice,0x00) ;
    SSIWriteValue(&ST7735DevicePointer->ST7735SSIDevice,x_start) ;
    SSIWriteValue(&ST7735DevicePointer->ST7735SSIDevice,0x00) ;
    SSIWriteValue(&ST7735DevicePointer->ST7735SSIDevice,x_end) ;

    // Write the Row Address Set Command.
    setChipDataCommandState_ST7735(ST7735DevicePointer,ST7735_COMMAND) ;
    SSIWriteValue(&ST7735DevicePointer->ST7735SSIDevice,ST7735_ROW_ADDRESS_SET_COMMAND) ;
    // Write Parameters.
    setChipDataCommandState_ST7735(ST7735DevicePointer,ST7735_DATA) ;
    SSIWriteValue(&ST7735DevicePointer->ST7735SSIDevice,0x00) ;
    SSIWriteValue(&ST7735DevicePointer->ST7735SSIDevice,y_start) ;
    SSIWriteValue(&ST7735DevicePointer->ST7735SSIDevice,0x00) ;
    SSIWriteValue(&ST7735DevicePointer->ST7735SSIDevice,y_end) ;
    // Write Memory Write Command.
    setChipDataCommandState_ST7735(ST7735DevicePointer,ST7735_COMMAND) ;
    SSIWriteValue(&ST7735DevicePointer->ST7735SSIDevice, ST7735_MEMORY_WRITE_COMMAND) ;
    setChipDataCommandState_ST7735(ST7735DevicePointer,ST7735_DATA) ;

}




static void sendChipCommandParameters_ST7735 (ST7735* ST7735DevicePointer, uint8_t st7735Command, uint8_t numOfParameters, ...)
{
    va_list parameterList ;
    va_start(parameterList,numOfParameters) ;

    // Write the Command.
    setChipDataCommandState_ST7735(ST7735DevicePointer,ST7735_COMMAND) ;
    setChipSelectState_ST7735(ST7735DevicePointer,ST7735_CHIP_SELECT) ;
    SSIWriteValue(&ST7735DevicePointer->ST7735SSIDevice,st7735Command) ;

    //Write Parameters.
    setChipDataCommandState_ST7735(ST7735DevicePointer,ST7735_DATA) ;
    while(numOfParameters)
    {
        SSIWriteValue(&ST7735DevicePointer->ST7735SSIDevice, va_arg(parameterList,uint8_t)) ;
        numOfParameters -= 1 ;
    }
    setChipSelectState_ST7735(ST7735DevicePointer,ST7735_CHIP_UNSELECT) ;


}



static void sendChipValue_ST7735(ST7735* ST7735DevicePointer, ST7735_DATA_OR_COMMAND dataOrCommand, uint8_t value)
{
    setChipDataCommandState_ST7735(ST7735DevicePointer, dataOrCommand) ;
    setChipSelectState_ST7735(ST7735DevicePointer,ST7735_CHIP_SELECT) ;
    SSIWriteValue(&ST7735DevicePointer->ST7735SSIDevice, value) ;
    setChipSelectState_ST7735(ST7735DevicePointer,ST7735_CHIP_SELECT) ;
}


static inline void setChipDataCommandState_ST7735(ST7735* ST7735DevicePointer, ST7735_DATA_OR_COMMAND dataOrCommand)
{
    digitalWrite(&ST7735DevicePointer->dataCommandPin,dataOrCommand) ;
}
static inline void setChipSelectState_ST7735(ST7735* ST7735DevicePointer, ST7735_CHIP_SELECT_OR_UNSELECT chipState)
{
    digitalWrite(&ST7735DevicePointer->chipSelectPin, chipState) ;

}



static void hardReset_ST7735(ST7735* ST7735DevicePointer)
{
    digitalWrite(&ST7735DevicePointer->resetPin, LOW) ;
    systemTicksDelay(10);
    digitalWrite(&ST7735DevicePointer->resetPin, HIGH) ;
    milliSecondDelay(200) ;
}

static void softReset_ST7735(ST7735* ST7735DevicePointer)
{
    setChipDataCommandState_ST7735(ST7735DevicePointer, ST7735_COMMAND) ;
    setChipSelectState_ST7735(ST7735DevicePointer,ST7735_CHIP_SELECT) ;
    SSIWriteValue(&ST7735DevicePointer->ST7735SSIDevice,ST7735_SOFTWARE_RESET_COMMAND) ;
    setChipSelectState_ST7735(ST7735DevicePointer,ST7735_CHIP_UNSELECT) ;
    milliSecondDelay(200) ;
}




