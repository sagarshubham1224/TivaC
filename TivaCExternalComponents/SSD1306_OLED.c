/*
 * SSD1306_OLED.c
 *
 *  Created on: 04-Apr-2018
 *      Author: sagar
 */


#include "SSD1306_OLED.h"

SSD1306* initialize_SSD1306(SSD1306* SSD1306DevicePointer,
		char chipSelectPinPortLetter, uint8_t chipSelectPinNumber,
		char dataCommandPinPortLetter,uint8_t dataCommandPinNumber,
		char resetPinPortLetter, uint8_t resetPinNumber)
{
	initGPIO(&SSD1306DevicePointer->chipSelectPin,chipSelectPinPortLetter, chipSelectPinNumber,OUTPUT);
	unselectChip_SSD1306(SSD1306DevicePointer) ;
	initGPIO(&SSD1306DevicePointer->dataCommandPin,dataCommandPinPortLetter, dataCommandPinNumber, OUTPUT);
	initGPIO(&SSD1306DevicePointer->resetPin,resetPinPortLetter, resetPinNumber, OUTPUT);


	digitalWrite(&SSD1306DevicePointer->resetPin, HIGH);
	pulsateResetPin_SSD1306(SSD1306DevicePointer);
	ticksDelay(1);

	initSSIDEVICE(&SSD1306DevicePointer->SSD1306SSIDevice, 1, 'D', SSD1306_SPI_CLOCK_FREQUENCY, SSD1306_DATAWIDTH);

	/*
	 * Step 1. Set Display Off.
	 */
	writeSingleValue_SSD1306(SSD1306DevicePointer, SSD1306_COMMAND, SSD1306_DISPLAY_ON_OFF_COMMAND | SSD1306_DISPLAY_OFF);


	/*
	 * Step 2. Set Display Clock Divide Ratio and Oscillator Frequency.
	 * Use Frequency divider 1 and defualt Fosc.
	 */
	writeSingleValue_SSD1306(SSD1306DevicePointer, SSD1306_COMMAND, SSD1306_DIV_RATIO_N_OSC_FREQ_COMMAND);
	writeSingleValue_SSD1306(SSD1306DevicePointer, SSD1306_COMMAND, SSD1306_DIV_RATIO_N_OSC_FREQ_DEFAULT_VALUE);



	/*
	 * Step 3. Set Multiplex ratio to OLED HEIGHT
	 */
	writeSingleValue_SSD1306(SSD1306DevicePointer, SSD1306_COMMAND, SSD1306_SET_MULTIPLEX_RATIO_COMMAND);
	writeSingleValue_SSD1306(SSD1306DevicePointer, SSD1306_COMMAND, SSD1306_MUX_RATIO_NUMBER);

	/*
	 * Step 4. Set Display Offset to none or 0.
	 */
	writeSingleValue_SSD1306(SSD1306DevicePointer, SSD1306_COMMAND, SSD1306_DISPLAY_OFFSET_COMMAND) ;
	writeSingleValue_SSD1306(SSD1306DevicePointer, SSD1306_COMMAND, SSD1306_DISPLAY_OFFSET_VALUE) ;

	/*
	 * Step 5. Set Display start line.
	 */
	writeSingleValue_SSD1306(SSD1306DevicePointer, SSD1306_COMMAND, SSD1306_DISPLAY_START_LINE_COMMAND | SSD1306_DISPLAY_START_LINE_VALUE) ;

	/*
	 * Step 6. Set Charge Pump Settings. Turn Internal Charge Pump On.
	 */
	writeSingleValue_SSD1306(SSD1306DevicePointer, SSD1306_COMMAND, SSD1306_CHARGE_PUMP_SETTINGS_COMMAND);
	writeSingleValue_SSD1306(SSD1306DevicePointer, SSD1306_COMMAND, SSD1306_CHARGE_PUMP_ENABLE);

	/*
	 * Step 7. Set Memory Addressing Mode.
	 */
	writeSingleValue_SSD1306(SSD1306DevicePointer, SSD1306_COMMAND, SSD1306_MEMORY_ADDRESSING_MODE_COMMAND) ;
	writeSingleValue_SSD1306(SSD1306DevicePointer, SSD1306_COMMAND, SSD1306_MEMORY_ADDRESSING_HORIZONTAL_MODE);

	/*
	 * Step 8. Set Segment Remap.
	 */
	writeSingleValue_SSD1306(SSD1306DevicePointer, SSD1306_COMMAND, SSD1306_SEGMENT_REMAP_COMMAND | SSD1306_SEGMENT_REMAP_COL127_SEG0) ;

	/*
	 * Step 9. Set COM Output Scan Direction.
	 */
	writeSingleValue_SSD1306(SSD1306DevicePointer, SSD1306_COMMAND, SSD1306_COM_OUTPUT_SCAN_DIRECTION_COMMAND | SSD1306_COM_OUTPUT_SCAN_DIRECTION_COMN_1_COM0) ;

	/*
	 * Step 10. Set COM Pins Hardware Configurations.
	 */
	writeSingleValue_SSD1306(SSD1306DevicePointer, SSD1306_COMMAND, SSD1306_COM_PINS_HARDWARE_CONFIG_COMMAND) ;
	writeSingleValue_SSD1306(SSD1306DevicePointer, SSD1306_COMMAND, SSD1306_COM_PINS_HARDWARE_CONFIG_ALTERNATIVE | SSD1306_COM_PINS_HARDWARE_CONFIG_DISABLE_REMAP) ;

	/*
	 * Step 11. Set Contrast Control.
	 */
	writeSingleValue_SSD1306(SSD1306DevicePointer, SSD1306_COMMAND, SSD1306_CONTRAST_CONTROL_COMMAND);
	writeSingleValue_SSD1306(SSD1306DevicePointer, SSD1306_COMMAND, SSD1306_CONTRAST_CONTROL_DEFAULT);

	/*
	 * Step 12. Set Pre Charge Period.
	 */
	writeSingleValue_SSD1306(SSD1306DevicePointer, SSD1306_COMMAND, SSD1306_PRECHARGE_PERIOD_SET_COMMAND);
	writeSingleValue_SSD1306(SSD1306DevicePointer, SSD1306_COMMAND, SSD1306_PRECHARGE_PERIOD_SET_PHASE_1 | SSD1306_PRECHARGE_PERIOD_SET_PHASE_2);


	/*
	 * Step 13. Set VCOMH deselect Level.
	 */
	writeSingleValue_SSD1306(SSD1306DevicePointer, SSD1306_COMMAND, SSD1306_VCOMH_DESELECT_LEVEL_SET_COMMAND) ;
	writeSingleValue_SSD1306(SSD1306DevicePointer, SSD1306_COMMAND, SSD1306_VCOMH_DESELECT_LEVEL_SET_POINT86_VCC) ;

	/*
	 * Step 14. Set entire Display On Command
	 */
	writeSingleValue_SSD1306(SSD1306DevicePointer, SSD1306_COMMAND, SSD1306_ENTIRE_DISPLAY_ON_COMMAND | SSD1306_ENTIRE_DISPLAY_ON_FOLLOW_RAM_CONTENT) ;


	/*
	 * Step 15. Set Diplay Mode Normal.
	 */
	writeSingleValue_SSD1306(SSD1306DevicePointer, SSD1306_COMMAND, SSD1306_DISPLAY_MODE_COMMAND | SSD1306_DISPLAY_MODE_NORMAL) ;

	/*
	 * Step 16. Disable Scroll.
	 */
	writeSingleValue_SSD1306(SSD1306DevicePointer, SSD1306_COMMAND, SSD1306_DEACTIVATE_SCROLL_COMMAND);

	/*
	 * Step 17. Clear GDDRAM.
	 */
	//	clearGDDRAM_SSD1306(SSD1306DevicePointer) ;

	/*
	 * Step 17. Turn On the Display.
	 */

	writeSingleValue_SSD1306(SSD1306DevicePointer, SSD1306_COMMAND, SSD1306_DISPLAY_ON_OFF_COMMAND | SSD1306_DISPLAY_ON);
	//	writeSingleValue_SSD1306(SSD1306DevicePointer, SSD1306_COMMAND, SSD1306_DISPLAY_MODE_COMMAND | SSD1306_DISPLAY_MODE_INVERSE) ;

	return SSD1306DevicePointer ;
}




void setMemoryAddressingMode_SSD1306(SSD1306* SSD1306DevicePointer, uint8_t addressMode)
{
	writeSingleValue_SSD1306(SSD1306DevicePointer, SSD1306_COMMAND, SSD1306_MEMORY_ADDRESSING_MODE_COMMAND) ;
	writeSingleValue_SSD1306(SSD1306DevicePointer, SSD1306_COMMAND, addressMode);
}

void setOperationMode_SSD1306(SSD1306* SSD1306DevicePointer, SSD1306_DATA_OR_COMMAND dataOrCommand)
{
	digitalWrite(&SSD1306DevicePointer->dataCommandPin, dataOrCommand);
}



void setDisplayAddress_SSD1306(SSD1306* SSD1306DevicePointer, uint8_t columnXAddress, uint8_t bankYAddress)
{
	columnXAddress = columnXAddress % SSD1306_128x64_WIDTH ;
	bankYAddress = (bankYAddress/8 )%8;

	setOperationMode_SSD1306(SSD1306DevicePointer, SSD1306_COMMAND) ;

	selectChip_SSD1306(SSD1306DevicePointer);

	SSIWriteValue(&SSD1306DevicePointer->SSD1306SSIDevice, SSD1306_PAGE_ADDRESS_SET_COMMAND );
	SSIWriteValue(&SSD1306DevicePointer->SSD1306SSIDevice, bankYAddress);
	SSIWriteValue(&SSD1306DevicePointer->SSD1306SSIDevice, SSD1306_PAGE_ADDRESS_SET_END);

	SSIWriteValue(&SSD1306DevicePointer->SSD1306SSIDevice, SSD1306_COLUMN_ADDRESS_SET_COMMAND);
	SSIWriteValue(&SSD1306DevicePointer->SSD1306SSIDevice, columnXAddress);
	SSIWriteValue(&SSD1306DevicePointer->SSD1306SSIDevice, SSD1306_COLUMN_ADDRESS_SET_END);


	unselectChip_SSD1306(SSD1306DevicePointer) ;


}

void setPixel_SSD1306(SSD1306* SSD1306DevicePointer, uint8_t xPos, uint8_t yPos, UARTDEVICE* UARTDevicePointer)
{

	uint8_t bankNumber = (xPos/SSD1306_NUMBER_OF_BANKS)%SSD1306_NUMBER_OF_BANKS;
	uint8_t columnNumber = (yPos % SSD1306_NUMBER_OF_COLUMNS) ;

	GDDRAMArray[bankNumber][columnNumber] = GDDRAMArray[bankNumber][columnNumber] | (1 << (xPos % SSD1306_NUMBER_OF_BANKS));
	GDDRAMBankKeeper |= 1 << bankNumber ;
	GDDRAMBankColumnPositionKeeperArray[bankNumber][0] |= 1 << (columnNumber >> 4 ) ;
	//	UARTWrite(UARTDevicePointer, "xPos: %d\tyPos: %d\t",xPos,yPos) ;
	//	UARTWrite(UARTDevicePointer, "BankNumber %d\tBank: 0b%b\t Column: 0b%b\n",bankNumber, GDDRAMBankKeeper, GDDRAMBankColumnPositionKeeperArray[bankNumber][0]);

	//	setOperationMode_SSD1306(SSD1306DevicePointer, SSD1306_COMMAND) ;
	//	selectChip_SSD1306(SSD1306DevicePointer);
	//	SSIWriteValue(&SSD1306DevicePointer->SSD1306SSIDevice, SSD1306_PAGE_ADDRESS_SET_COMMAND );
	//	SSIWriteValue(&SSD1306DevicePointer->SSD1306SSIDevice, bankNumber);
	//	SSIWriteValue(&SSD1306DevicePointer->SSD1306SSIDevice, SSD1306_PAGE_ADDRESS_SET_END);
	//
	//	SSIWriteValue(&SSD1306DevicePointer->SSD1306SSIDevice, SSD1306_COLUMN_ADDRESS_SET_COMMAND);
	//	SSIWriteValue(&SSD1306DevicePointer->SSD1306SSIDevice, columnNumber);
	//	SSIWriteValue(&SSD1306DevicePointer->SSD1306SSIDevice, SSD1306_COLUMN_ADDRESS_SET_END);
	//	unselectChip_SSD1306(SSD1306DevicePointer) ;
	//
	//
	//
	//	setOperationMode_SSD1306(SSD1306DevicePointer, SSD1306_DATA) ;
	//	selectChip_SSD1306(SSD1306DevicePointer);
	//	SSIWriteValue(&SSD1306DevicePointer->SSD1306SSIDevice, GDDRAMArray[bankNumber][columnNumber]);
	//	unselectChip_SSD1306(SSD1306DevicePointer) ;
}

void drawLine_SSD1306(SSD1306* SSD1306DevicePointer, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
	uint8_t steep = abs(y1 - y0) > abs(x1 - x0);
	if (steep) {
		_swap_uint8_t(x0, y0);
		_swap_uint8_t(x1, y1);
	}

	if (x0 > x1) {
		_swap_uint8_t(x0, x1);
		_swap_uint8_t(y0, y1);
	}

	uint8_t dx, dy;
	dx = x1 - x0;
	dy = abs(y1 - y0);

	int8_t err = dx / 2;
	int8_t ystep;

	if (y0 < y1) {
		ystep = 1;
	} else {
		ystep = -1;
	}

	for (; x0<=x1; x0++) {
		if (steep) {
			//			setPixel_SSD1306(SSD1306DevicePointer,y0, x0);
		} else {
			//			setPixel_SSD1306(SSD1306DevicePointer,x0, y0);
		}
		err -= dy;
		if (err < 0) {
			y0 += ystep;
			err += dx;
		}
	}
}



void showRAM_SSD1306(SSD1306* SSD1306DevicePointer, UARTDEVICE* UARTDevicePointer)
{

	uint8_t columnPos = 0, bankPos = 0;
	for(bankPos = 0; bankPos < SSD1306_NUMBER_OF_BANKS; ++bankPos)
	{
		if(GDDRAMBankKeeper & (1<<bankPos))
		{
			//			UARTWrite(UARTDevicePointer, "Bank %d needs update. Columns: ", bankPos);
			for(columnPos = 0 ; columnPos < 8; ++columnPos)
			{
				if(GDDRAMBankColumnPositionKeeperArray[bankPos][0] & (1<<columnPos))
				{
					uint8_t columnStart = (columnPos<<4) ;
					uint8_t columnStop = columnStart + 16 ;
					//					UARTWrite(UARTDevicePointer, "bank: %d\t ColumnStart: %d\n",bankPos,columnStart);

					setOperationMode_SSD1306(SSD1306DevicePointer, SSD1306_COMMAND) ;

					selectChip_SSD1306(SSD1306DevicePointer);

					SSIWriteValue(&SSD1306DevicePointer->SSD1306SSIDevice, SSD1306_PAGE_ADDRESS_SET_COMMAND );
					SSIWriteValue(&SSD1306DevicePointer->SSD1306SSIDevice, bankPos);
					SSIWriteValue(&SSD1306DevicePointer->SSD1306SSIDevice, SSD1306_PAGE_ADDRESS_SET_END);

					SSIWriteValue(&SSD1306DevicePointer->SSD1306SSIDevice, SSD1306_COLUMN_ADDRESS_SET_COMMAND);
					SSIWriteValue(&SSD1306DevicePointer->SSD1306SSIDevice, columnStart);
					SSIWriteValue(&SSD1306DevicePointer->SSD1306SSIDevice, SSD1306_COLUMN_ADDRESS_SET_END);

					unselectChip_SSD1306(SSD1306DevicePointer) ;

					setOperationMode_SSD1306(SSD1306DevicePointer, SSD1306_DATA);
					selectChip_SSD1306(SSD1306DevicePointer) ;

					for(;columnStart < columnStop; ++columnStart)
					{
						//						if(GDDRAMArray[bankPos][columnStart])
						//						{
						//							UARTWrite(UARTDevicePointer, "GDDRAMArray[%d][%d] = 0b%b\n", bankPos, columnStart, GDDRAMArray[bankPos][columnStart]);
						//
						//						}
						SSIWriteValue(&SSD1306DevicePointer->SSD1306SSIDevice, GDDRAMArray[bankPos][columnStart]);
						milliSecondDelay(64);
					}
					unselectChip_SSD1306(SSD1306DevicePointer);
				}
			}
			//			UARTWrite(UARTDevicePointer, "\n") ;
		}
	}
	//	uint8_t columnPos = 0;
	//	uint8_t bankPos = 0;
	//	setOperationMode_SSD1306(SSD1306DevicePointer, SSD1306_DATA);
	//	selectChip_SSD1306(SSD1306DevicePointer) ;
	//	for(bankPos = 0; (bankPos < SSD1306_NUMBER_OF_BANKS) && ( GDDRAMPositionKeeperArray[0] & (1<<bankPos)); ++bankPos)
	//	{
	//		for(columnPos = 0; columnPos < SSD1306_NUMBER_OF_COLUMNS; ++columnPos)
	//		{
	//			SSIWriteValue(&SSD1306DevicePointer->SSD1306SSIDevice, GDDRAMArray[bankPos][columnPos]);
	//		}
	//	}
	//	unselectChip_SSD1306(SSD1306DevicePointer);


}




unsigned int SSD1306WriteCharString(SSD1306* SSD1306DevicePointer, const char *charBuffer, uint32_t charBufferLength)
{
	unsigned int uIdx;
	char charValue;
	uint8_t looper = 0;
	uint8_t charWidth = SSD1306_SMALL_FONT_WIDTH ;
	setOperationMode_SSD1306(SSD1306DevicePointer, SSD1306_DATA) ;
	selectChip_SSD1306(SSD1306DevicePointer) ;

	for(uIdx = 0; uIdx < charBufferLength && charBuffer[uIdx] != '\0'; uIdx++)
	{
		charValue = charBuffer[uIdx] ;
		for(looper = 0 ; looper < charWidth ; ++looper) SSIWriteValue(&SSD1306DevicePointer->SSD1306SSIDevice, SSD1306_SMALL_FONT[charValue - 0x20][looper]);
		SSIWriteValue(&SSD1306DevicePointer->SSD1306SSIDevice,0x00);
		SSIWriteValue(&SSD1306DevicePointer->SSD1306SSIDevice,0x00);
	}
	unselectChip_SSD1306(SSD1306DevicePointer) ;
	return(uIdx);					// Return the number of characters written.
}


void invertDisplay_SSD1306(SSD1306* SSD1306DevicePointer)
{
	writeSingleValue_SSD1306(SSD1306DevicePointer,SSD1306_COMMAND, SSD1306_DISPLAY_MODE_COMMAND | SSD1306_DISPLAY_MODE_INVERSE) ;
}

void normalDisplay_SSD1306(SSD1306* SSD1306DevicePointer)
{
	writeSingleValue_SSD1306(SSD1306DevicePointer,SSD1306_COMMAND, SSD1306_DISPLAY_MODE_COMMAND | SSD1306_DISPLAY_MODE_NORMAL) ;
}




void clearGDDRAM_SSD1306(SSD1306* SSD1306DevicePointer)
{
	uint8_t columnPos = 0;
	uint8_t bankPos = 0;
	setDisplayAddress_SSD1306(SSD1306DevicePointer,0,0) ;
	setOperationMode_SSD1306(SSD1306DevicePointer, SSD1306_DATA);
	selectChip_SSD1306(SSD1306DevicePointer) ;
	for(bankPos = 0; bankPos < SSD1306_NUMBER_OF_BANKS ; ++bankPos)
	{
		for(columnPos = 0; columnPos < SSD1306_NUMBER_OF_COLUMNS; ++columnPos)
		{
			SSIWriteValue(&SSD1306DevicePointer->SSD1306SSIDevice, 0xFF) ;

			milliSecondDelay(10);
		}
	}
	unselectChip_SSD1306(SSD1306DevicePointer) ;
	setDisplayAddress_SSD1306(SSD1306DevicePointer, 0, 0) ;
}

void clearGDDRAMandARRAY_SSD1306(SSD1306* SSD1306DevicePointer)
{
	uint8_t columnPos = 0;
	uint8_t bankPos = 0;
	setDisplayAddress_SSD1306(SSD1306DevicePointer,0,0) ;
	setOperationMode_SSD1306(SSD1306DevicePointer, SSD1306_DATA);
	selectChip_SSD1306(SSD1306DevicePointer) ;
	for(bankPos = 0; bankPos < SSD1306_NUMBER_OF_BANKS ; ++bankPos)
	{
		GDDRAMBankColumnPositionKeeperArray[bankPos][0] = 0 ;
		for(columnPos = 0; columnPos < SSD1306_NUMBER_OF_COLUMNS; ++columnPos)
		{
			SSIWriteValue(&SSD1306DevicePointer->SSD1306SSIDevice, 0x00) ;
			GDDRAMArray[bankPos][columnPos] = 0x00;
			GDDRAMBankKeeper &= ~(1<<bankPos) ;
		}
	}
	unselectChip_SSD1306(SSD1306DevicePointer) ;
	setDisplayAddress_SSD1306(SSD1306DevicePointer, 0, 0) ;
}



void pulsateResetPin_SSD1306(SSD1306* SSD1306DevicePointer)
{
	digitalWrite(&SSD1306DevicePointer->resetPin, LOW);
	ticksDelay(1);		// 250us Delay.
	digitalWrite(&SSD1306DevicePointer->resetPin, HIGH);
}

void unselectChip_SSD1306(SSD1306* SSD1306DevicePointer)
{
	digitalWrite(&SSD1306DevicePointer->chipSelectPin, SSD1306_UNSELECT_CHIP);
}


void selectChip_SSD1306(SSD1306* SSD1306DevicePointer)
{
	digitalWrite(&SSD1306DevicePointer->chipSelectPin, SSD1306_SELECT_CHIP);
}


void writeSingleValue_SSD1306(SSD1306* SSD1306DevicePointer, SSD1306_DATA_OR_COMMAND dataCommandValue, uint8_t dataInValue)
{
	setOperationMode_SSD1306(SSD1306DevicePointer, dataCommandValue) ;
	selectChip_SSD1306(SSD1306DevicePointer);
	SSIWriteValue(&SSD1306DevicePointer->SSD1306SSIDevice, dataInValue );
	unselectChip_SSD1306(SSD1306DevicePointer) ;
}





void SSD1306vaListWrite(SSD1306* SSD1306DevicePointer, const char *printString, va_list vaList)
{
	uint32_t ui32Idx, ui32Value, ui32Pos, ui32Count, ui32Base, ui32Neg;
	char *pcStr, pcBuf[16], cFill;
	float floatVal;
	while(*printString)
	{
		for(ui32Idx = 0; (printString[ui32Idx] != '%') && (printString[ui32Idx] != '\0');ui32Idx++);	// Find the first non-% character, or the end of the string.
		SSD1306WriteCharString(SSD1306DevicePointer, printString, ui32Idx);			// Write this portion of the string.
		printString += ui32Idx;			// Skip the portion of the string that was written.


		if(*printString == '%')			// See if the next character is a %.
		{
			printString++;				// Skip the %.
			ui32Count = 0;				//Set the digit count to zero, and the fill character to space (in other words, to the defaults).
			cFill = ' ';

			again:						// It may be necessary to get back here to process more characters.
			switch(*printString++)		// Determine how to handle the next character.
			{

			case '0':					// Handle the digit characters.
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
				ui32Count *= 10;					// Update the digit count.
				ui32Count += printString[-1] - '0';
				goto again;							// Get the next character.
			}

			case 'c':								// Handle the %c command.
			{
				ui32Value = va_arg(vaList, uint32_t); 						// Get the value from the varargs.
				SSD1306WriteCharString(SSD1306DevicePointer, (char *)&ui32Value, 1);	// Print out the character.
				break;														// This command has been handled.
			}

			case 'd':			// Handle the %d and %i commands.
			case 'i':
			{
				ui32Value = va_arg(vaList, uint32_t);						// Get the value from the varargs.
				ui32Pos = 0;												// Reset the buffer position.
				if((int32_t)ui32Value < 0)									// If the value is negative, make it positive and indicate that a minus sign is needed.
				{
					ui32Value = -(int32_t)ui32Value;						// Make the value positive.
					ui32Neg = 1;											// Indicate that the value is negative.
				}
				else
				{
					ui32Neg = 0;											// Indicate that the value is positive so that a minus sign isn't inserted.
				}
				ui32Base = 10;												// Set the base to 10.
				goto convert;												// Convert the value to ASCII.
			}


			case 's':														// Handle the %s command.
			{
				pcStr = va_arg(vaList, char *);								// Get the string pointer from the varargs.
				for(ui32Idx = 0; pcStr[ui32Idx] != '\0'; ui32Idx++);		// Determine the length of the string.
				SSD1306WriteCharString(SSD1306DevicePointer, pcStr, ui32Idx);			// Write the string.
				if(ui32Count > ui32Idx)										// Write any required padding spaces
				{
					ui32Count -= ui32Idx;
					while(ui32Count--)
					{
						SSD1306WriteCharString(SSD1306DevicePointer, " ", 1);
					}
				}
				break;														// This command has been handled.
			}

			case 'u':														// Handle the %u command.
			{
				ui32Value = va_arg(vaList, uint32_t);						// Get the value from the varargs.
				ui32Pos = 0;												// Reset the buffer position.
				ui32Base = 10;												// Set the base to 10.
				ui32Neg = 0;												// Indicate that the value is positive so that a minus sign isn't inserted.
				goto convert;												// Convert the value to ASCII.
			}

			// Handle the %x and %X commands.  Note that they are treated
			// identically; in other words, %X will use lower case letters
			// for a-f instead of the upper case letters it should use.  We
			// also alias %p to %x.
			case 'x':
			case 'X':
			case 'p':
			{
				ui32Value = va_arg(vaList, uint32_t); 						// Get the value from the varargs.
				ui32Pos = 0;												// Reset the buffer position.
				ui32Base = 16;												// Set the base to 16.
				ui32Neg = 0;												// Indicate that the value is positive so that a minus sign isn't inserted.

				convert:
				for(ui32Idx = 1;
						(((ui32Idx * ui32Base) <= ui32Value) &&
								(((ui32Idx * ui32Base) / ui32Base) == ui32Idx));	// Determine the number of digits in the string version of the value.
						ui32Idx *= ui32Base, ui32Count--)
				{
				}
				if(ui32Neg)													// If the value is negative, reduce the count of padding characters needed.
				{
					ui32Count--;
				}
				if(ui32Neg && (cFill == '0'))	// If the value is negative and the value is padded with zeros, then place the minus sign before the padding.
				{
					pcBuf[ui32Pos++] = '-';									// Place the minus sign in the output buffer.
					ui32Neg = 0;											// The minus sign has been placed, so turn off the negative flag.
				}

				if((ui32Count > 1) && (ui32Count < 16))						// Provide additional padding at the beginning of the string conversion if needed.
				{
					for(ui32Count--; ui32Count; ui32Count--)
					{
						pcBuf[ui32Pos++] = cFill;
					}
				}

				if(ui32Neg)													// If the value is negative, then place the minus sign before the number.
				{
					pcBuf[ui32Pos++] = '-';									// Place the minus sign in the output buffer.
				}
				for(; ui32Idx; ui32Idx /= ui32Base)							// Convert the value into a string.
				{
					pcBuf[ui32Pos++] = g_pcHex[(ui32Value / ui32Idx) % ui32Base];
				}
				SSD1306WriteCharString(SSD1306DevicePointer, pcBuf, ui32Pos);			// Write the string.
				break;														// This command has been handled.
			}
			case '%':														// Handle the %% command.
			{
				SSD1306WriteCharString(SSD1306DevicePointer, printString - 1, 1);		// Simply write a single %.
				break;														// This command has been handled.
			}
			case 'f':														// Handle %f float values.
			{
				floatVal = (float) (va_arg(vaList, double)) ;
				uint8_t floatStringSize = floatToString_SSD1306(floatVal, SSD1306_FLOAT_PRECISION, SSD1306_FLOAT_STRING_BUFFER_SIZE);
				SSD1306WriteCharString(SSD1306DevicePointer, floatStringBuffer_SSD1306, floatStringSize);
				break;														// This command has been handled.
			}

			case 'b':
			{
				ui32Value = (va_arg(vaList, uint32_t));
				uint8_t binaryStringSize = intToBinaryString_SSD1306(ui32Value);
				SSD1306WriteCharString(SSD1306DevicePointer,binaryStringBuffer_SSD1306,binaryStringSize );
				break;
			}
			/*
			default:
			{
				//
				// Indicate an error.
				//
				SSD1306WriteCharString(SSD1306DevicePointer, "", 5);
				break;														// This command has been handled.
			}*/
			}
		}
	}

}

void SSD1306Write(SSD1306* SSD1306DevicePointer, const char *printString, ...)
{
	va_list vaList;
	va_start(vaList, printString);
	SSD1306vaListWrite(SSD1306DevicePointer, printString, vaList);
	va_end(vaList);
}


/*
 * Function to convert a float value to a string of characters.
 * Arguments:
 * 	float floatVal								:: Value in float type to be printed.
 *	uint8_t precision							:: Number of fraction places to be printed. SET as per FLOAT_PRECISION.
 *	uint8_t bufSize								:: Size of char buffer where string would be sored. SET as per FLOAT_STRING_BUFFER_SIZE.
 * Returns:
 * 	uint8_t StringSize							:: Number of characters written to char buffer, This is used in turn by UARTWriteCharString to write the char buffer.
 */
static uint8_t floatToString_SSD1306(float floatVal,uint8_t precision,uint8_t buffSize)
{
	int8_t sign	= (floatVal>0)?1:-1;						// Check if float value to be printed is positive or negative.
	uint8_t presentPosition	= 0;
	int8_t intPos = 0;
	uint32_t intPart = (uint32_t)(floatVal*sign);			// Find the Integer portion of the float value.
	uint8_t numOfIntDigits = 0;

	//Find number of digits in integer part of float value.
	if (intPart < 10)				numOfIntDigits = 1;
	else if (intPart <100)			numOfIntDigits = 2;
	else if (intPart <1000)			numOfIntDigits = 3;
	else if (intPart <10000)		numOfIntDigits = 4;
	else if (intPart <100000)		numOfIntDigits = 5;
	else if (intPart <1000000)		numOfIntDigits = 6;
	else if (intPart <10000000)		numOfIntDigits = 7;
	else if (intPart <100000000)	numOfIntDigits = 8;
	else if (intPart <1000000000)	numOfIntDigits = 9;
	else if (intPart <10000000000)	numOfIntDigits = 10;

	{
		memset(floatStringBuffer_SSD1306,'\0',buffSize);

		if(sign == -1)				//Insert SIGN if needed.
		{
			*(floatStringBuffer_SSD1306+presentPosition) = '-';
			presentPosition += 1;
		}

		// ADD decimal part to buffer.
		if(intPart == 0)
		{
			*(floatStringBuffer_SSD1306+presentPosition) 	= '0';
			presentPosition				+= 1 ;
		}
		else   	// Get the digit at units' place in the integer part of the float value, and add it to the string at correct position.
			//Do this till all digits are put in there.
		{
			intPos = numOfIntDigits+presentPosition-1;
			while(intPart > 0)
			{
				uint8_t unitDigit 	= intPart%10;
				*(floatStringBuffer_SSD1306+intPos) 	= unitDigit + '0';
				intPart 			/= 10;
				presentPosition					+= 1;
				intPos				-= 1;
			}
		}

		//ADD fractional part to buffer.
		if(precision != 0)
		{
			*(floatStringBuffer_SSD1306+presentPosition) = '.';
			presentPosition += 1;
			floatVal = sign*(floatVal - (uint32_t)(floatVal))*10.0f;
			while(precision > 0)
			{
				uint32_t unitDigit = (uint32_t) (floatVal );
				*(floatStringBuffer_SSD1306 + presentPosition) = unitDigit%10 +'0';
				presentPosition 			+= 1;
				floatVal		= floatVal*10.0f;
				precision		-= 1;
			}
		}
		*(floatStringBuffer_SSD1306+presentPosition) = '\0';
	}
	return presentPosition;
}


/*
 * Function to convert integer value to string of characters in binary.
 * Arguments:
 * 	uint32_t numVal 							:: unsigned integer value.
 * Returns:
 * 	No. of characters written.
 */
static uint8_t intToBinaryString_SSD1306(uint32_t numVal)
{
	uint8_t remainder = 0;
	int8_t bitPos = SSD1306_BINARY_SIZE_08;
	uint8_t returnBitLen = bitPos;


	//		//	Check if max length needs update and do so.
	//		if(numVal >= (1 << BINARY_SIZE_16)) bitPos = BINARY_SIZE_32;
	//		else if (numVal >= (1 << BINARY_SIZE_08)) bitPos = BINARY_SIZE_16;
	//		else if (numVal >= (1 << BINARY_SIZE_04)) bitPos = BINARY_SIZE_08;
	//		else if (numVal >= (1 << BINARY_SIZE_02)) bitPos = BINARY_SIZE_04;

	//	Set last position of array to NUL.
	binaryStringBuffer_SSD1306[bitPos] = '\0';
	while(numVal)
	{
		remainder = numVal % 2;
		numVal = numVal / 2;
		bitPos -= 1;
		binaryStringBuffer_SSD1306[bitPos] = (char)(remainder + '0');
	}
	bitPos -= 1;
	while(bitPos >= 0)
	{
		binaryStringBuffer_SSD1306[bitPos] = '0';
		bitPos -= 1;
	}
	return returnBitLen;
}
