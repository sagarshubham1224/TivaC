/*
 * DS1307.c
 *
 *  Created on: 01-Jun-2018
 *      Author: sagar
 */

#include "DS1307.h"


void disableSquareWaveOutput_DS1307(DS1307SENSOR* DS1307SensorPointer,  DS1307_OUTPUT_PIN outpinState)
{
	uint8_t registerValue = DS1307_DISABLE_SQUAREWAVE | outpinState ;
	I2CWriteByte(&DS1307SensorPointer->DS1307I2CDevice,DS1307_CONTROL_REGISTER,registerValue) ;
}

void enableSquareWaveOutput_DS1307(DS1307SENSOR* DS1307SensorPointer, uint8_t outputFrequency)
{
	uint8_t registerValue = DS1307_ENABLE_SQUAREWAVE | outputFrequency ;
	I2CWriteByte(&DS1307SensorPointer->DS1307I2CDevice,DS1307_CONTROL_REGISTER, registerValue) ;
}

DS1307SENSOR* initialize_DS1307(DS1307SENSOR* DS1307SensorPointer,DS1307HOURMODE hourMode, UARTDEVICE* UARTDevicePointer)
{
	initI2CMaster(&DS1307SensorPointer->DS1307I2CDevice,0,DS1307_I2C_ADDRESS, FAST_400k_I2C) ;
	uint8_t registerValue ;

	/*
	 * Step 1. Enable Oscillator by setting CH bit to 0.
	 */
	registerValue = I2CReadByte(&DS1307SensorPointer->DS1307I2CDevice,DS1307_SECONDS_REGISTER) ;
	registerValue &= 0b01111111;
	I2CWriteByte(&DS1307SensorPointer->DS1307I2CDevice,DS1307_SECONDS_REGISTER, registerValue);

	/*
	 * Step 2. Disable Square wave output.
	 */
	disableSquareWaveOutput_DS1307(DS1307SensorPointer,DS1307_OUTPIN_STATE_LOW);
	/*
	 * Step 3. Set Hour Mode.
	 */
	DS1307SensorPointer->hourMode = hourMode ;
	registerValue = I2CReadByte(&DS1307SensorPointer->DS1307I2CDevice,DS1307_HOURS_REGISTER);
	registerValue &= ~(0b01000000); // Clear Bit 6.
	registerValue |= DS1307SensorPointer->hourMode ; // Set it as 1 or 0 according to hour mode.
	I2CWriteByte(&DS1307SensorPointer->DS1307I2CDevice,DS1307_HOURS_REGISTER,registerValue) ;


	return DS1307SensorPointer ;
}

void setInitialTimeStamp(DS1307SENSOR* DS1307SensorPointer, TIMESTAMP* timeStampPointer)
{
#ifdef TIME_DATA_CHECK_TRUE
	if(timeStampPointer->seconds > 59)
	{
		timeStampPointer->seconds %= 60 ;
		timeStampPointer->minutes += 1;
	}
#endif
	timeStampPointer->seconds = ( (timeStampPointer->seconds / 10) << 4) | (timeStampPointer->seconds % 10);


#ifdef TIME_DATA_CHECK_TRUE
	if(timeStampPointer->minutes > 60)
	{
		timeStampPointer->minutes %= 60;
		timeStampPointer->hours += 1;
	}
#endif
	timeStampPointer->minutes = ( (timeStampPointer->minutes / 10) << 4) | (timeStampPointer->minutes % 10);

	switch (DS1307SensorPointer->hourMode) {
	case DS1307_HOUR_MODE_12:

#ifdef TIME_DATA_CHECK_TRUE
		if(timeStampPointer->hours > 12)
		{
			timeStampPointer->hours %= 12;
		}
#endif
		timeStampPointer->hours = (timeStampPointer->amOrpm)
											| ((timeStampPointer->hours / 10 ) << 4)
											| (timeStampPointer->hours % 10) ;
		break;
	case DS1307_HOUR_MODE_23:

#ifdef TIME_DATA_CHECK_TRUE
		if(timeStampPointer->hours > 23)
		{
			timeStampPointer->hours %= 24;
		}
#endif
		timeStampPointer->hours = (timeStampPointer->amOrpm)
											| ((timeStampPointer->hours / 10 ) << 4)
											| (timeStampPointer->hours % 10) ;

		break;
	}

#ifdef TIME_DATA_CHECK_TRUE
	if(timeStampPointer->day > 7)
	{
		timeStampPointer->day %= 7;
	}
#endif

	timeStampPointer->date = ((timeStampPointer->date / 10 ) << 4) | (timeStampPointer->date % 10) ;

#ifdef TIME_DATA_CHECK_TRUE
	if(timeStampPointer->month > 12)
	{
		timeStampPointer->month %= 12;
	}
#endif
	timeStampPointer->month = ((timeStampPointer->month / 10 ) << 4) | (timeStampPointer->month % 10) ;


	uint8_t decade_year = timeStampPointer->year % 100 ;

	timeStampPointer->year -=  decade_year;
	decade_year = ((decade_year/10)<<4) | (decade_year % 10) ;


	timeStampPointer->year /= 100;

#ifdef TIME_DATA_CHECK_TRUE
	if(timeStampPointer->year > 99)
	{
		timeStampPointer->year %= 100;
	}
#endif
	timeStampPointer->year  =((timeStampPointer->year / 10 ) << 4) | (timeStampPointer->year % 10) ;


	I2CWriteByte(&DS1307SensorPointer->DS1307I2CDevice,DS1307_SECONDS_REGISTER,(uint8_t)(timeStampPointer->seconds & 0xFF));
	I2CWriteByte(&DS1307SensorPointer->DS1307I2CDevice,DS1307_MINUTES_REGISTER,(uint8_t)(timeStampPointer->minutes & 0xFF));
	I2CWriteByte(&DS1307SensorPointer->DS1307I2CDevice,DS1307_HOURS_REGISTER,(uint8_t)(timeStampPointer->hours & 0xFF));
	I2CWriteByte(&DS1307SensorPointer->DS1307I2CDevice,DS1307_DAY_REGISTER,(uint8_t)(timeStampPointer->day & 0xFF));
	I2CWriteByte(&DS1307SensorPointer->DS1307I2CDevice,DS1307_DATE_REGISTER,(uint8_t)(timeStampPointer->date & 0xFF));
	I2CWriteByte(&DS1307SensorPointer->DS1307I2CDevice,DS1307_MONTH_REGISTER,(uint8_t)(timeStampPointer->month & 0xFF));
	I2CWriteByte(&DS1307SensorPointer->DS1307I2CDevice,DS1307_YEAR_REGISTER,(uint8_t)(decade_year & 0xFF));
	I2CWriteByte(&DS1307SensorPointer->DS1307I2CDevice,DS1307_MILLENNIUM_CENTURY_REGISTER, (uint8_t)(timeStampPointer->year & 0xFF)) ;

}



char* getMonthName(uint8_t monthNumber)
{
	return monthNames[monthNumber - 1] ;
}

char* getDayName(uint8_t dayNumber)
{
	return dayNames[dayNumber - 1] ;
}

void getTimeStamp_DS1307(DS1307SENSOR* DS1307SensorPointer)
{
	uint8_t timeStampValues[9];
	I2CReadBytes(&DS1307SensorPointer->DS1307I2CDevice,DS1307_SECONDS_REGISTER,9,&timeStampValues[0]) ;

	DS1307SensorPointer->timeStamp.seconds = ( timeStampValues[DS1307_SECONDS_REGISTER] >> 4 ) * 10
			+ (timeStampValues[DS1307_SECONDS_REGISTER] & 0x0F) ;

	DS1307SensorPointer->timeStamp.minutes = ( timeStampValues[DS1307_MINUTES_REGISTER] >> 4 ) * 10
			+ (timeStampValues[DS1307_MINUTES_REGISTER] & 0x0F) ;

	switch (DS1307SensorPointer->hourMode) {
	case DS1307_HOUR_MODE_12:
		if(timeStampValues[DS1307_HOURS_REGISTER] & (1<<5) )
		{
			DS1307SensorPointer->timeStamp.amOrpm  = TIME_PM ;
		}
		else
		{
			DS1307SensorPointer->timeStamp.amOrpm = TIME_AM ;
		}
		DS1307SensorPointer->timeStamp.hours = ( (timeStampValues[DS1307_HOURS_REGISTER] >> 4 ) & 0x01 ) *10
				+ (timeStampValues[DS1307_HOURS_REGISTER] & 0x0F)  ;

		break;
	case DS1307_HOUR_MODE_23:
		DS1307SensorPointer->timeStamp.amOrpm = TIME_23 ;
		DS1307SensorPointer->timeStamp.hours = ( (timeStampValues[DS1307_HOURS_REGISTER] >> 4 ) & 0x03 ) *10
				+ (timeStampValues[DS1307_HOURS_REGISTER] & 0x0F)  ;
		break;
	}

	DS1307SensorPointer->timeStamp.day = timeStampValues[DS1307_DAY_REGISTER] ;

	DS1307SensorPointer->timeStamp.date = ( timeStampValues[DS1307_DATE_REGISTER] >> 4 ) * 10
				+ (timeStampValues[DS1307_DATE_REGISTER] & 0x0F) ;


	DS1307SensorPointer->timeStamp.month = ( timeStampValues[DS1307_MONTH_REGISTER] >> 4 ) * 10
			+ (timeStampValues[DS1307_MONTH_REGISTER] & 0x0F) ;

	DS1307SensorPointer->timeStamp.year = ( timeStampValues[DS1307_YEAR_REGISTER] >> 4 ) * 10
			+ (timeStampValues[DS1307_YEAR_REGISTER] & 0x0F) ;

	DS1307SensorPointer->timeStamp.year += (timeStampValues[DS1307_MILLENNIUM_CENTURY_REGISTER] >> 4) * 1000
			+ (timeStampValues[DS1307_MILLENNIUM_CENTURY_REGISTER] & 0x0F) * 100 ;
}
