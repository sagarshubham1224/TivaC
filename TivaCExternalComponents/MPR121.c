/*
 * MPR121.c
 *
 *  Created on: 04-Jun-2018
 *      Author: sagar
 */


#include "MPR121.h"




void dumpRegisters(MPR121SENSOR* MPR121SensorPointer, UARTDEVICE* UARTDevicePointer)
{
	uint8_t looper = 0;
	uint8_t vals[128] ;
	I2CReadBytes(&MPR121SensorPointer->MPR121I2CDevice,0x00, 128, &vals[0]) ;
	for(;looper<0x81; ++ looper)
	{
		UARTWrite(UARTDevicePointer,"Register: 0x%X\tValue: 0x%x\t %d\n",looper, vals[looper], vals[looper]);
	}
}
MPR121SENSOR* initialize_MPR121(MPR121SENSOR* MPR121SensorPointer,MPR121SENSOR_SLAVE_ADDRESS I2CSlaveAddress,
		char GPIOPortLetter, uint8_t pinNumber, void (*irqInterruptHandler)(void),
		UARTDEVICE* UARTDevicePointer)
{
	initI2CMaster(&MPR121SensorPointer->MPR121I2CDevice,0,I2CSlaveAddress, FAST_400k_I2C) ;
	if(irqInterruptHandler)
	{
		initGPIO(&MPR121SensorPointer->IRQPin,GPIOPortLetter,pinNumber,INPUT) ;
		initGPIOI(&MPR121SensorPointer->IRQPin,GPIO_FALLING_EDGE,irqInterruptHandler);
	}
	uint8_t registerValue = 0;

//	dumpRegisters(MPR121SensorPointer, UARTDevicePointer) ;
	/*
	 *
	 * "Registers write operation can only be done
	 * 	after MPR121 is set into Stop Mode,
	 * 	except the ECR and GPIO/LED related registers."
	 */

	/*
	 * Step 1. Set MPR121 into Stop Mode.
	 * "MPR121 can be set into Stop Mode
	 *  by writing 2bXX000000 to ECR at any time."
	 */
	I2CWriteByte(&MPR121SensorPointer->MPR121I2CDevice,MPR121_ELECTRODE_CONFIGURATION_REGISTER,0x00);
	milliSecondDelay(1);
	UARTWrite(UARTDevicePointer,"ECR: %d\n",I2CReadByte(&MPR121SensorPointer->MPR121I2CDevice,MPR121_ELECTRODE_CONFIGURATION_REGISTER)) ;


	/*
	 * Step 2. Set Config Register 1,
	 * FFI to default 6 samples of 1st filter iterations.
	 * CDC global to default 16 uA.
	 */
	registerValue = MPR121_FFI_06_SAMPLES | (uint8_t)16 ;
	I2CWriteByte(&MPR121SensorPointer->MPR121I2CDevice,MPR121_AFE_CONFIGURATION_1_REGISTER, registerValue) ;
	UARTWrite(UARTDevicePointer,"CFG1: %d\n",I2CReadByte(&MPR121SensorPointer->MPR121I2CDevice,MPR121_AFE_CONFIGURATION_1_REGISTER)) ;
	/*
	 * Step 3. Set Configuration Register 2,
	 * CDT global to default 0.5 us
	 * SFI to default 4 samples of 2nd filter iterations.
	 * ESI to default 16ms sample interval.
	 */
	registerValue = MPR121_CDT_0P5_US | MPR121_SFI_04_SAMPLES | MPR121_ESI_16_MS ;
	I2CWriteByte(&MPR121SensorPointer->MPR121I2CDevice,MPR121_AFE_CONFIGURATION_2_REGISTER, registerValue) ;
	UARTWrite(UARTDevicePointer,"CFG2: %d\n",I2CReadByte(&MPR121SensorPointer->MPR121I2CDevice,MPR121_AFE_CONFIGURATION_2_REGISTER)) ;

	/*
	 * Step 4.Set Release and touch thresholds for the electrodes.
	 * We are using an experimental value in the range 0x05 to 0x30.
	 */
	uint8_t looper = MPR121_ELE0_TOUCH_THRESHOLD_REGISTER ;
	for(; looper < MPR121_ELEPROX_RELEASE_THRESHOLD_REGISTER; looper += 2)
	{
		I2CWriteByte(&MPR121SensorPointer->MPR121I2CDevice,looper, MPR121_ELEX_TOUCH_THRESHOLD_VALUE);
		I2CWriteByte(&MPR121SensorPointer->MPR121I2CDevice,looper+1, MPR121_ELEX_RELEASE_THRESHOLD_VALUE);
	}

	/*
	 * Step 5. Set Baseline Filer Control for the electrodes.
	 * These are experimental values.
	 */
	I2CWriteByte(&MPR121SensorPointer->MPR121I2CDevice,MPR121_MHD_RISING_REGISTER,0x01);
	I2CWriteByte(&MPR121SensorPointer->MPR121I2CDevice,MPR121_NHD_RISING_REGISTER,0x01);
	I2CWriteByte(&MPR121SensorPointer->MPR121I2CDevice,MPR121_NCL_RISING_REGISTER,0x01);
	I2CWriteByte(&MPR121SensorPointer->MPR121I2CDevice,MPR121_FDL_RISING_REGISTER,0x01);
	I2CWriteByte(&MPR121SensorPointer->MPR121I2CDevice,MPR121_MHD_FALLING_REGISTER,0x01);
	I2CWriteByte(&MPR121SensorPointer->MPR121I2CDevice,MPR121_NHD_FALLING_REGISTER,0x01);
	I2CWriteByte(&MPR121SensorPointer->MPR121I2CDevice,MPR121_NCL_FALLING_REGISTER,0x01);
	I2CWriteByte(&MPR121SensorPointer->MPR121I2CDevice,MPR121_FDL_FALLING_REGISTER,0x01);


	/*
	 * Step 6. Set MPR121 into Run Mode.
	 * To start capacitance measurement and touch detection,
	 * set into Run Mode by selecting the sensing inputs to be enabled.
	 */
	I2CWriteByte(&MPR121SensorPointer->MPR121I2CDevice,MPR121_ELECTRODE_CONFIGURATION_REGISTER,0x0C);

//	dumpRegisters(MPR121SensorPointer,UARTDevicePointer) ;
	return MPR121SensorPointer ;
}
