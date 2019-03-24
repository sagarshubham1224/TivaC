/*
 * PWM.h
 * File Description:
 * File to hold PWD related functions.
 */


#ifndef PWM_H_
#define PWM_H_


/*
 * Included .h files.
 */
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"

/*
 * Macro Definitions:
 */
#define PWM_PREDEFINED_CONFIG_MODE      PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC
#define PWM_PREDEFINED_FREQUENCY        2000
#define USE_FAST_PWM
#define GET_ALT     0
#define GET_GEN     1
#define GET_PWM     2
#define GET_PBT     3

/*
 * PWMPin Struct Definition.
 */
typedef struct PWMPin{
    bool pwmOutputState;
    uint32_t pwmModuleBase;
    uint32_t pwmGenenrator;
    uint32_t pwmFrequency ;
    uint32_t pwmPeriod;
    uint8_t  pwmPrescalar ;
    uint32_t pwmOut;
    uint32_t pwmOutBits;
}PWMPin;




/*
 * Pin Description
 * Module - Generator - PinName - Pin Assignment
 *
╔══════════╦══════════════╦═══════════╦═════════════════╗
║ Module   ║  Generator   ║  PinName  ║  PinAssignment  ║
╠══════════╬══════════════╬═══════════╬═════════════════╣
║          ║  Generator0  ║  M0PWM0   ║  PB6            ║
║          ║  Generator0  ║  M0PWM1   ║  PB7            ║
║          ║  Generator1  ║  M0PWM2   ║  PB4            ║
║          ║  Generator1  ║  M0PWM3   ║  PB5            ║
║ Module0  ║  Generator2  ║  M0PWM4   ║  PE4            ║
║          ║  Generator2  ║  M0PWM5   ║  PE5            ║
║          ║  Generator3  ║  M0PWM6   ║  PC4/PD0        ║
║          ║  Generator3  ║  M0PWM7   ║  PC5/PD1        ║
╚══════════╩══════════════╩═══════════╩═════════════════╝
╔══════════╦══════════════╦═══════════╦═════════════════╗
║  Module  ║  Generator   ║  PinName  ║  PinAssignment  ║
╠══════════╬══════════════╬═══════════╬═════════════════╣
║          ║  Generator0  ║  M1PWM0   ║  PD0            ║
║          ║  Generator0  ║  M1PWM1   ║  PD1            ║
║          ║  Generator1  ║  M1PWM2   ║  PA6/PE4        ║
║          ║  Generator1  ║  M1PWM3   ║  PA7/PE5        ║
║ Module1  ║  Generator2  ║  M1PWM4   ║  PF0            ║
║          ║  Generator2  ║  M1PWM5   ║  PF1            ║
║          ║  Generator3  ║  M1PWM6   ║  PF2            ║
║          ║  Generator3  ║  M1PWM7   ║  PF3            ║
╚══════════╩══════════════╩═══════════╩═════════════════╝
 *
 * Table created with: https://ozh.github.io/ascii-tables/
 */


/*
 * Functions:
 */


/*
 * PWM Functions:
 */




//public non-static external Functions:

/*
 * Create and return a Struct Pointer of type PWMPin to control PWM Signal on a Pin.
 * Arguments:
 *  PWMPin *pwmPinPointer                       :: Pointer to struct of PWMPin.
 *  uint8_t PWMPeripheralNumber                 :: PWM Peripheral Number 0, or 1.
 *  char GPIOPortLetter                         :: GPIO Port Letter 'A', 'B', 'C', 'D' , 'E', or 'F'.
 *  uint8_t GPIOPinNumber                       :: GPIO pin number respective to Port, 0, 1, 2, 3, 4, 5, 6, or 7.
 *  uint8_t preScalarNumber                     :: preScalar value 1, 2, 4, 8, 16, 32, or 64. Any other value will be treated as 64.
 *  uint32_t configMode                         :: Desired Combination of PWM configuration of PWM Generator.
 *  uint32_t pwmFrequency                       :: Desired PWM Frequency. MUST BE LESS THAN (System Clock)/(preScalarNumber).
 * Steps:
 *  Step 1: Enable GPIO Module.
 *  Step 2: Enable PWM Module.
 *  Step 3: Enable Clock of PWM Module using preScalar.
 *  Step 4: Enable Alternate Functionality, both for pin and pin-base combination.
 *  Step 5: Configure PWM Module's Generator to be used.
 *  Step 6: Set Period(Load) Value.
 *  Step 7: Set initial pulse width as 0. This is done by turning off the PWM pin.
 *  Step 8: Enable the generator.
 *  Step 9: return the pointer to PWMpin struct.
 * Returns:
 *  PWMPin* pwmPinPointer                       :: Pointer to PWMPin struct.
 */
PWMPin* initPWM(    PWMPin *pwmPinPointer,          uint8_t PWMPeripheralNumber,
                    char GPIOPortLetter,            uint8_t GPIOPinNumber,
                    uint8_t preScalarNumber,        uint32_t configMode,
                    uint32_t pwmFrequency );


void setPWMFrequency(PWMPin* PWMPinPointer, uint32_t pwmFrequency) ;


/*
 * Write PWM Width from %age Value of Duty Cycle.
 * Arguments:
 *  PWMPin* pwmPinPointer                       :: Pointer to PWMPin struct.
 *  uint32_t dutyCycle                          :: duty Cycle value between 0 Period(Load) Value which is to be applied at PWM Pin.
 * Returns:
 *  nothing.
 */
extern void analogPWMWrite(PWMPin* pwmPin, uint8_t dutyCycle100);

/*
 * Write PWM Width from %age Value of Duty Cycle.
 * Arguments:
 *  PWMPin* pwmPinPointer                       :: Pointer to PWMPin struct.
 *  uint8_t dutyCycle100                        :: duty Cycle value between 0 and 100 which is to be applied at the PWM Pin.
 * Returns:
 *  nothing.
 */
void analogPWMWriteRaw(PWMPin* pwmPin, uint32_t dutyCycle);



//private static non-external Functions:


/*
 * Function to get the preScalar #defined value from preScalar number.
 * Arguments:
 *  uint8_t preScalarNumber                     :: preScalar value 1, 2, 4, 8, 16, 32, or 64. Any other value will be treated as 64.
 * Returns:
 *  uint32_t PWM Clock PreScalar                :: prescalar #defined values as SYSCTL_PWMDIV_X, where X can be 1, 2, 4, 8, 16, 32, or 64. *
 */
static uint32_t getPreScalar(uint8_t preScalar);

/*
 * Function to get all the required sauce for making a PWM Pin tick.
 * Arguments:
 *  char GPIOPortLetter                         :: GPIO Peripheral Letter 'A', 'B', 'C', 'D' , 'E', or 'F'.
 *  uint8_t PWMPeripheralNumber                 :: PWM Peripheral Number 0, or 1.
 *  uint8_t GPIOPinNumber                       :: GPIO Pin Number respective to Port, i.e. 0, 1, 2, 3, 4, 5, 6, or 7.
 *  uint8_t retValType                          :: value to decide which type of return value is required.
 * Returns:
 *  if retValType == GET_ALT                    :: returns pinAlternateFunctionAddress, GPIO_Pxy_MnPWMq, where x is Port Number, y is Pin Number, n is PWM Module Number and q is  PWM Pin.
 *  if retValType == GET_GEN                    :: returns pwmGenerator, PWM_GEN_X, where X is 0, 1, 2, or 3.
 *  if retValType == GEN_PWM                    :: returns pwmOut, PWM_OUT_X, where X is 0, 1, 2, 3, 4, 5, 6, or 7.
 *  if retValType == GEN_PBT                    :: returns pwmOutBit, PWM_OUT_X_BIT, where X is 0, 1, 2, 3, 4, 5, 6, or 7.
 */
static uint32_t getPWMSauce(char GPIOPortLetter, uint8_t PWMPeripheralNumber, uint8_t GPIOPinNumber, uint8_t retValType);



/*
 * Function to get GPIO Peripheral address depending upon GPIO Peripheral Letter.
 * Arguments:
 *  char GPIOPortLetter                         :: GPIO Port Letter 'A', 'B', 'C', 'D' , 'E', or 'F'.
 * Returns:
 *  uint32_t address Of GPIO Peripheral         :: Depending upon passed argument, the appropriate value from array ui32GPIOPeripheralAddressArray is returned.
 */
static uint32_t getGPIOPeripheralAddress(char GPIOPortLetter);

/*
 * Function to get GPIO Base Address depending upon GPIO Peripheral Letter.
 * Arguments:
 *  uint8_t GPIOPortLetter                      :: GPIO Port Letter 'A', 'B', 'C', 'D' , 'E', or 'F'.
 * Returns:
 *  uint32_t address of GPIO Base               :: Depending upon passed argument, the appropriate value from array ui32PGPIOBaseAddressArray is returned.
 */
static uint32_t getGPIOBaseAddress(char GPIOPortLetter);

/*
 * Function to get PWM Peripheral Address depending upon PWM Module Peripheral Number.
 * Arguments:
 *  uint8_t PWMPeripheralNumber                 :: PWM Peripheral Number 0, or 1.
 * Returns:
 *  uint32_t address of PWM Peripheral          :: Depending upon passed argument, the appropriate value from array ui32PWMModulePeripheralAddressArray is returned.
 */
static uint32_t getPWMModulePeripheralAddress(uint8_t PWMPeripheralNumber);

/*
 * Function to get PWM Base Address depending upon PWM Module Peripheral Number.
 * Arguments:
 *  uint8_t PWMPeripheralNumber                 :: PWM Peripheral Number 0, or 1.
 * Returns:
 *  uint32_t address of PWM Base                :: Depending upon passed argument, the appropriate value from array ui32PWMModuleBaseAddressArray is returned.
 */
static uint32_t getPWMModuleBaseAddress(uint8_t PWMPeripheralNumber);




/*
 * Function to get Pin Address depending upon GPIO Pin number respective to Port.
 * Arguments:
 *  uint8_t GPIOPinNumber                       :: GPIO Pin Number respective to Port, i.e. 0, 1, 2, 3, 4, 5, 6, or 7.
 * Returns:
 *  uint8_t GPIO Pin Address                    :: Depending upon passed argument, the appropriate value from array ui32GPIOPinAddressArray is returned.
 */
static uint8_t getGPIOPinAddress(uint8_t GPIOPinNumber);

#endif
