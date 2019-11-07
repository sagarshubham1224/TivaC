/*
 * PCD8544_LCD.c
 *
 *  Created on: 02-Apr-2018
 *      Author: sagar
 */


#include "PCD8544_LCD.h"


uint32_t numVal = 0;


void initLCD_PCD8544(PCD8544* PCD8544DevicePointer, uint8_t SSIPortNumber, char SSIGPIOPortLetter,
						GPIO* chipSelectPinPointer,
						GPIO* resetPinPointer,
						GPIO* dataCommandPinPointer
						)
{
	PCD8544DevicePointer->chipSelectPin.base 		= chipSelectPinPointer->base;
	PCD8544DevicePointer->chipSelectPin.pin  		= chipSelectPinPointer->pin;
	PCD8544DevicePointer->chipSelectPin.pinNumber 	= chipSelectPinPointer->pinNumber;
	PCD8544DevicePointer->chipSelectPin.pinType 	= chipSelectPinPointer->pinType;
	unselectChip_PCD8544(PCD8544DevicePointer) ;

	PCD8544DevicePointer->dataCommandPin.base 		= dataCommandPinPointer->base;
	PCD8544DevicePointer->dataCommandPin.pin  		= dataCommandPinPointer->pin;
	PCD8544DevicePointer->dataCommandPin.pinNumber 	= dataCommandPinPointer->pinNumber;
	PCD8544DevicePointer->dataCommandPin.pinType 	= dataCommandPinPointer->pinType;

	PCD8544DevicePointer->resetPin.base 		= resetPinPointer->base;
	PCD8544DevicePointer->resetPin.pin  		= resetPinPointer->pin;
	PCD8544DevicePointer->resetPin.pinNumber 	= resetPinPointer->pinNumber;
	PCD8544DevicePointer->resetPin.pinType 	= resetPinPointer->pinType;

	PCD8544DevicePointer->statesArray[instructionTypeBit]	= PCD8544_H_BIT_BASIC_INSTRUCTIONS;
	PCD8544DevicePointer->statesArray[addressingModeBit]	= PCD8544_V_BIT_HORIZONTAL_ADDRESSING >> 1 ;
	PCD8544DevicePointer->statesArray[chipStateBit]			= PCD8544_PD_BIT_CHIP_ACTIVE >> 2;
	PCD8544DevicePointer->statesArray[displayControlBitD]	= 1;
	PCD8544DevicePointer->statesArray[displayControlBitE]	= 0;

	digitalWrite(&PCD8544DevicePointer->resetPin,HIGH) ;
	pulsateResetPin_PCD8544(PCD8544DevicePointer) ;
	initSSIDEVICE(&PCD8544DevicePointer->PCD8544_SSIDevice,SSIPortNumber,SSIGPIOPortLetter,PCD8544_CLOCK_FREQUENCY,PCD8544_BYTE_LENGTH) ;
	pulsateResetPin_PCD8544(PCD8544DevicePointer) ;


	/*
	 * Step 1. Set Contrast by setting V-OP
	 * We set V-OP to Experimental value.
	 */
	setContrast_PCD8544(PCD8544DevicePointer, PCD8544_VOP_CONTRAST_SETTINGS );


	/*
	 * Step 2. Set Temperature Coefficient.
	 * Write Coefficient 0 for now. Experimental Value.
	 */
	setTemperatureCoefficient(PCD8544DevicePointer, PCD8544_TEMP_COEFF_0);

	/*
	 * Step 3. Set Bias Configurations.
	 * Set for 1:48 mux.
	 */
	setBiasConfigurations(PCD8544DevicePointer, PCD8544_BIAS_SETTING_1_48);


	/*
	 * Step 4. Write Command Display Control Set.
	 * Set for basic display mode for now.
	 */
	setDisplayControlMode(PCD8544DevicePointer, PCD8544_DISPLAY_CONTROL_NORMAL_MODE);


	/*
	 * Step 5. Clear Display Data RAM.
	 */
	clearDDRAM(PCD8544DevicePointer);

}

void clearDDRAM(PCD8544* PCD8544DevicePointer)
{
	uint16_t pos = 0;
	setOperationMode_PCD8544(PCD8544DevicePointer, PCD8544_DATA);
	selectChip_PCD8544(PCD8544DevicePointer);
	for(pos = 0; pos < PCD8544_WIDTH * PCD8544_HEIGHT; ++pos)
	{
		SSIWriteValue(&PCD8544DevicePointer->PCD8544_SSIDevice, 0x00);
	}
	unselectChip_PCD8544(PCD8544DevicePointer);

}


void writeSingleChar_PCD8544(PCD8544* PCD8544DevicePointer, char charValue)
{
	uint8_t looper = 0;
	uint8_t charWidth = PCD8544_SMALL_FONT_WIDTH ;
	setOperationMode_PCD8544(PCD8544DevicePointer,PCD8544_DATA);
	selectChip_PCD8544(PCD8544DevicePointer);
	for(looper = 0; looper < charWidth; ++looper)
	{
		SSIWriteValue(&PCD8544DevicePointer->PCD8544_SSIDevice,PCD8544_SMALL_FONT[charValue - 0x20][looper]);
	}
	//	SSIWriteValue(&PCD8544DevicePointer->PCD8544_SSIDevice,0x00);
	unselectChip_PCD8544(PCD8544DevicePointer);

}

void writeMultipleDataValues_PCD8544(PCD8544* PCD8544DevicePointer, uint8_t* arrayBuffer, uint16_t arrayBufferSize )
{

	uint16_t looper = 0;
	setOperationMode_PCD8544(PCD8544DevicePointer,PCD8544_DATA);
	selectChip_PCD8544(PCD8544DevicePointer);
	for(; looper<arrayBufferSize; ++looper)
	{

		SSIWriteValue(&PCD8544DevicePointer->PCD8544_SSIDevice,arrayBuffer[looper]);
	}
	unselectChip_PCD8544(PCD8544DevicePointer);

}


void clearDisplay_PCD8544(PCD8544* PCD8544DevicePointer,bool doIReturnToOrigin)
{
	if(PCD8544DevicePointer->statesArray[instructionTypeBit] != PCD8544_H_BIT_BASIC_INSTRUCTIONS)
	{
		enterBasicCommandsMode_PCD8544(PCD8544DevicePointer);
	}
	uint8_t dataInValue = PCD8544_DISPLAY_CONTROL_COMMAND | PCD8544_DISPLAY_CONTROL_DISPLAY_BLANK;
	writeSingleValue_PCD8544(PCD8544DevicePointer,PCD8544_COMMAND,dataInValue);
	PCD8544DevicePointer->statesArray[displayControlBitD] = 0;
	PCD8544DevicePointer->statesArray[displayControlBitE] = 0;
	if(doIReturnToOrigin)
	{
		dataInValue = PCD8544_X_ADDRESS_SET_COMMAND | PCD8544_X_ORIGIN_ADDRESS ;
		writeSingleValue_PCD8544(PCD8544DevicePointer,PCD8544_COMMAND,dataInValue);

		dataInValue = PCD8544_Y_ADDRESS_SET_COMMAND | PCD8544_Y_ORIGIN_ADDRESS ;
		writeSingleValue_PCD8544(PCD8544DevicePointer,PCD8544_COMMAND,dataInValue);
	}
}


void setXYRAMAddress(PCD8544* PCD8544DevicePointer, uint8_t xPosition, uint8_t yPosition)
{
	if(PCD8544DevicePointer->statesArray[instructionTypeBit] != PCD8544_H_BIT_BASIC_INSTRUCTIONS)
	{
		enterBasicCommandsMode_PCD8544(PCD8544DevicePointer);
	}
	uint8_t dataInValue = PCD8544_X_ADDRESS_SET_COMMAND | xPosition;
	writeSingleValue_PCD8544(PCD8544DevicePointer,PCD8544_COMMAND,dataInValue);

	dataInValue = PCD8544_Y_ADDRESS_SET_COMMAND | yPosition  ;
	writeSingleValue_PCD8544(PCD8544DevicePointer,PCD8544_COMMAND,dataInValue);

}

void setDisplayControlMode(PCD8544* PCD8544DevicePointer, uint8_t displayControlMode)
{
	if(PCD8544DevicePointer->statesArray[instructionTypeBit] != PCD8544_H_BIT_BASIC_INSTRUCTIONS)
	{
		enterBasicCommandsMode_PCD8544(PCD8544DevicePointer);
	}
	uint8_t dataInValue = PCD8544_DISPLAY_CONTROL_COMMAND | displayControlMode;
	writeSingleValue_PCD8544(PCD8544DevicePointer,PCD8544_COMMAND,dataInValue);
}

void setBiasConfigurations(PCD8544* PCD8544DevicePointer, uint8_t biasSettings)
{
	if(PCD8544DevicePointer->statesArray[instructionTypeBit] != PCD8544_H_BIT_EXTENDED_INSTRUCTIONS)
	{
		enterExtendedCommandsMode_PCD8544(PCD8544DevicePointer);
	}
	uint8_t dataInValue = PCD8544_BIAS_SETTING_COMMAND | biasSettings ;
	writeSingleValue_PCD8544(PCD8544DevicePointer,PCD8544_COMMAND,dataInValue);

}


void setTemperatureCoefficient(PCD8544* PCD8544DevicePointer, uint8_t tempCoeff)
{
	if(PCD8544DevicePointer->statesArray[instructionTypeBit] != PCD8544_H_BIT_EXTENDED_INSTRUCTIONS)
	{
		enterExtendedCommandsMode_PCD8544(PCD8544DevicePointer);
	}
	uint8_t dataInValue = PCD8544_TEMP_COEFF_SETTING_COMMAND | tempCoeff ;
	writeSingleValue_PCD8544(PCD8544DevicePointer,PCD8544_COMMAND,dataInValue);

}



void setContrast_PCD8544(PCD8544* PCD8544DevicePointer, uint8_t contrastSettings)
{
	if(PCD8544DevicePointer->statesArray[instructionTypeBit] != PCD8544_H_BIT_EXTENDED_INSTRUCTIONS)
	{
		enterExtendedCommandsMode_PCD8544(PCD8544DevicePointer);
	}
	uint8_t dataInValue = PCD8544_VOP_CONTRAST_SET_COMMAND | contrastSettings ;
	writeSingleValue_PCD8544(PCD8544DevicePointer,PCD8544_COMMAND,dataInValue);
}

void enterExtendedCommandsMode_PCD8544(PCD8544* PCD8544DevicePointer)
{
	uint8_t dataInValue = 	 PCD8544_FUNCTION_SET_COMMAND 								|
							(PCD8544DevicePointer->statesArray[chipStateBit]) 			|
							(PCD8544DevicePointer->statesArray[addressingModeBit] << 1) |
							 PCD8544_H_BIT_EXTENDED_INSTRUCTIONS ;


	writeSingleValue_PCD8544(PCD8544DevicePointer,PCD8544_COMMAND,dataInValue);

	PCD8544DevicePointer->statesArray[instructionTypeBit] = PCD8544_H_BIT_EXTENDED_INSTRUCTIONS;
}

void enterBasicCommandsMode_PCD8544(PCD8544* PCD8544DevicePointer)
{
	uint8_t dataInValue = 	 PCD8544_FUNCTION_SET_COMMAND 								|
							(PCD8544DevicePointer->statesArray[chipStateBit]) 			|
							(PCD8544DevicePointer->statesArray[addressingModeBit] << 1) |
							 PCD8544_H_BIT_BASIC_INSTRUCTIONS ;
	writeSingleValue_PCD8544(PCD8544DevicePointer,PCD8544_COMMAND,dataInValue);

	PCD8544DevicePointer->statesArray[instructionTypeBit] = PCD8544_H_BIT_BASIC_INSTRUCTIONS;
}

void writeSingleValue_PCD8544(PCD8544* PCD8544DevicePointer,PCD8544_DATA_OR_COMMAND mode, uint8_t value)
{
	setOperationMode_PCD8544(PCD8544DevicePointer,mode);
	selectChip_PCD8544(PCD8544DevicePointer);
	SSIWriteValue(&PCD8544DevicePointer->PCD8544_SSIDevice,value);
	unselectChip_PCD8544(PCD8544DevicePointer);
}


void selectChip_PCD8544(PCD8544* PCD8544DevicePointer)
{
	digitalWrite(&PCD8544DevicePointer->chipSelectPin,PCD8544_SELECT) ;
}


void unselectChip_PCD8544(PCD8544* PCD8544DevicePointer)
{
	digitalWrite(&PCD8544DevicePointer->chipSelectPin,PCD8544_UNSELECT) ;
}

void setOperationMode_PCD8544(PCD8544* PCD8544DevicePointer, PCD8544_DATA_OR_COMMAND mode)
{
	digitalWrite(&PCD8544DevicePointer->dataCommandPin,mode) ;
}

void pulsateResetPin_PCD8544(PCD8544* PCD8544DevicePointer)
{
	digitalWrite(&PCD8544DevicePointer->resetPin,LOW) ;
	ticksDelay(10) ;   // 2500 us delay
	digitalWrite(&PCD8544DevicePointer->resetPin,HIGH) ;
}
