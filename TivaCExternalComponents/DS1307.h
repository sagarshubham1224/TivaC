/*
 * DS1307.h
 *
 *  Created on: 01-Jun-2018
 *      Author: sagar
 */

#ifndef DS1307_H_
#define DS1307_H_


#include "stdint.h"
#include "stdbool.h"
#include "ss_TIME.h"
#include "ss_I2C.h"
#include "ss_UART.h"

typedef enum DS1307HOURMODE {
	DS1307_HOUR_MODE_23 = 0b00000000,
	DS1307_HOUR_MODE_12 = 0b01000000
}DS1307HOURMODE ;

typedef enum AM_OR_PM {
	TIME_AM	=	0b00000000,
	TIME_PM	=	0b00100000,
	TIME_23 =	0b11111111
}AM_OR_PM ;


typedef struct TIMESTAMP {
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	AM_OR_PM amOrpm;
	uint8_t day;
	uint8_t date;
	uint8_t month;
	int16_t year;
}TIMESTAMP;


typedef struct DS1307SENSOR {
	I2CDEVICE DS1307I2CDevice;
	DS1307HOURMODE hourMode;
	TIMESTAMP timeStamp;

}DS1307SENSOR ;


static char monthNames[12][10] = {
		"January",
		"February",
		"March",
		"April",
		"May",
		"June",
		"July",
		"August",
		"September",
		"October",
		"November",
		"December"
};

static char dayNames[7][10] = {
		"Sunday",
		"Monday",
		"Tuesday",
		"Wednesday",
		"Thursday",
		"Friday",
		"Saturday"
};


//#define TIME_DATA_CHECK_TRUE

#define DS1307_I2C_ADDRESS							0x68
#define DS1307_SECONDS_REGISTER						0x00
#define DS1307_MINUTES_REGISTER						0x01
#define DS1307_HOURS_REGISTER						0x02
#define DS1307_DAY_REGISTER							0x03
#define DS1307_DATE_REGISTER						0x04
#define DS1307_MONTH_REGISTER						0x05
#define DS1307_YEAR_REGISTER						0x06
#define DS1307_CONTROL_REGISTER						0x07
#define DS1307_MILLENNIUM_CENTURY_REGISTER			0x08   //Register to Store Millennium and Century Number.Range 00YY to 99YY


typedef enum DS1307_OUTPUT_PIN {
	DS1307_OUTPIN_STATE_HIGH	=					0b10000000,
	DS1307_OUTPIN_STATE_LOW		=					0b00000000,
	DS1307_ENABLE_SQUAREWAVE	=					0b00010000,
	DS1307_DISABLE_SQUAREWAVE	= 					0b00000000,
	DS1307_OUTPUT_FREQ_1_HZ		=					0b00000000,
	DS1307_OUTPUT_FREQ_4096_HZ	=					0b00000001,
	DS1307_OUTPUT_FREQ_8192_HZ	=					0b00000010,
	DS1307_OUTPUT_FREQ_32768_HZ	=					0b00000011
}DS1307_OUTPUT_PIN;

//#define DS1307_OUTPIN_STATE_HIGH					0b10000000
//#define DS1307_OUTPIN_STATE_LOW						0b00000000
//#define DS1307_ENABLE_SQUAREWAVE					0b00010000
//#define DS1307_DISABLE_SQUAREWAVE					0b00000000
//#define DS1307_OUTPUT_FREQ_1_HZ						0b00000000
//#define DS1307_OUTPUT_FREQ_4096_HZ					0b00000001
//#define DS1307_OUTPUT_FREQ_8192_HZ					0b00000010
//#define DS1307_OUTPUT_FREQ_32768_HZ					0b00000011


void disableSquareWaveOutput_DS1307(DS1307SENSOR* DS1307SensorPointer, uint8_t outpinState) ;

void enableSquareWaveOutput_DS1307(DS1307SENSOR* DS1307SensorPointer, uint8_t outputFrequency) ;

DS1307SENSOR* initialize_DS1307(DS1307SENSOR* DS1307SensorPointer,DS1307HOURMODE hourMode, UARTDEVICE* UARTDevicePointer) ;

char* getMonthName(uint8_t monthNumber);


char* getDayName(uint8_t dayNumber) ;

void getTimeStamp_DS1307(DS1307SENSOR* DS1307SensorPointer) ;



void setInitialTimeStamp(DS1307SENSOR* DS1307SensorPointer, TIMESTAMP* timeStampPointer) ;


#endif /* DS1307_H_ */
