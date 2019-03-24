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
#include "driverlib/PWM.h"
#include "driverlib/pin_map.h"
#include "PERIPHERALS.h"


/*
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
 *
 * We will bind PE4 and PE5 only to Module0 to reduce initialization complexities.
 * Similarly, we will bind PD0 and PD1 only to Module1.
 */


/*
 * Macro Definitions:
 */
#define PWM_PREDEFINED_CONFIG_MODE      PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC
#define PWM_PREDEFINED_FREQUENCY        2000

typedef enum PWM_ENABLE_STATES {
    PWM_ENABLE = true ,
    PWM_DISABLE = false
}PWM_ENABLE_STATES;
/*
 * PWMPin Struct Definition.
 */
typedef struct PWMPin{
    PWM_ENABLE_STATES PWMState;
    uint32_t PWMModuleBase;
    uint32_t PWMGenenrator;
    uint32_t PWMFrequency ;
    uint32_t PWMPeriod;
    uint8_t  PWMPrescalar ;
    uint32_t PWMOut;
    uint32_t PWMOutBits;
}PWMPin;






/*
 * Functions:
 */


//public non-static external Functions:

/*
 * Create a Struct Pointer of type PWMPin to control PWM Signal on a Pin.
 * Arguments:
 *  PWMPin *PWMPinPointer                       :: Pointer to struct of PWMPin.
 *  GPIO_PIN_CODE GPIOPinCode                   :: GPIO_PIN_CODE enum value, GPIOXn, X is [A,F], n is [0,7] .
 *  PWM_PRESCALAR preScalarNumber               :: preScalar enum value. All other values will be treated as prescalar of 64.
 *  uint32_t PWMFrequency                       :: Desired PWM Frequency. MUST BE LESS THAN (System Clock)/(preScalarNumber).
 *                                              :: If not, it would be set to that.
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
 *  none.
 */
void initPWM(    PWMPin *PWMPinPointer,          GPIO_PIN_CODE GPIOPinCode,
                    PWM_PRESCALAR preScalarNumber,  uint32_t PWMFrequency );


void setPWMFrequency(PWMPin* PWMPinPointer, uint32_t PWMFrequency) ;


/*
 * Write PWM Width from %age Value of Duty Cycle.
 * Arguments:
 *  PWMPin* PWMPinPointer                       :: Pointer to PWMPin struct.
 *  uint32_t dutyCycle                          :: duty Cycle value between 0 and Period(Load) Value which is to be applied at PWM Pin.
 * Returns:
 *  nothing.
 */
extern void analogPWMWrite(PWMPin* PWMPin, uint8_t dutyCycle100);

/*
 * Write PWM Width from %age Value of Duty Cycle.
 * Arguments:
 *  PWMPin* PWMPinPointer                       :: Pointer to PWMPin struct.
 *  uint8_t dutyCycle100                        :: duty Cycle value between 0 and 100 which is to be applied at the PWM Pin.
 * Returns:
 *  nothing.
 */
void analogPWMWriteRaw(PWMPin* PWMPin, uint32_t dutyCycle);



//private static non-external Functions:





/*
 * Function to get PWM Module Sauce.
 * Arguments:
 *  GPIO_PIN_CODE GPIOPinCode                                   :: GPIO_PIN_CODE enum value, GPIOXn, X is [A,F], n is [0,7] .
 *  uint32_t* PWMPinAlternateFunctionAddressVariablePointer     :: Pointer to variable to store PWMPin Alternate Function Address
 *  uint32_t* PWMGeneratorAddressVariablePointer                :: Pointer to variable to store PWM Generator Address
 *  uint32_t* PWMOutAddressVariablePointer                      :: Pointer to variable to store PWM Out Address
 *  uint32_t* PWMOutBitAddressVariablePointer                   :: Pointer to variable to store PWM Out Bit Address
 * Returns:
 *  none.
 */
static void getPWMSauce(GPIO_PIN_CODE GPIOPinCode,
                         uint32_t* PWMPinAlternateFunctionAddressVariablePointer,
                         uint32_t* PWMGeneratorAddressVariablePointer,
                         uint32_t* PWMOutAddressVariablePointer,
                         uint32_t* PWMOutBitAddressVariablePointer) ;

/*
 * Function to get GPIO Peripheral and Base Address depending upon GPIO_PIN_CODE.
 * Arguments:
 *  uint8_t GPIOPortLetter                              :: GPIO Port Letter 'A', 'B', 'C', 'D' , 'E', or 'F'.
 *  uint32_t* PWMGPIOPeripheralAddressVariablePointer   :: Pointer to variable to store GPIO Peripheral Address.
 *  uint32__t* PWMGPIOBaseAddressVariablePointer        :: Pointer to variable to store GPIO Base Address.
 * Returns:
 *  none.
 */
static void getPWMGPIOPeripheralAndBaseAddress(GPIO_PIN_CODE GPIOPinCode,
                                               uint32_t* PWMGPIOPeripheralAddressVariablePointer,
                                               uint32_t* PWMGPIOBaseAddressVariablePointer) ;


/*
 * Function to get PWM Peripheral and Base Address depending GPIO_PIN_CODE.
 * Arguments:
 *  GPIO_PIN_CODE GPIOPinCode                       :: GPIO_PIN_CODE enum value, GPIOXn, X is [A,F], n is [0,7] .
 *  uint32_t* PWMPeripheralAddressVariablePointer   :: Pointer to variable to store PWM Peripheral Address.
 *  uint32_t* PWMBaseAddressVariablePointer         :: Pointer to variable to store PWM Base Address.
 * Returns:
 *  none.
 */
static void getPWMPeripheralAndBaseAddress(GPIO_PIN_CODE GPIOPinCode,
                                               uint32_t* PWMPeripheralAddressVariablePointer,
                                               uint32_t* PWMBaseAddressVariablePointer) ;


/*
 * Function to get Pin Address depending upon GPIO_PIN_CODE.
 * Arguments:
 *  uint8_t GPIOPinNumber                       :: GPIO Pin Number respective to Port, i.e. 0, 1, 2, 3, 4, 5, 6, or 7.
 * Returns:
 *  uint8_t GPIO Pin Address                    :: Depending upon passed argument, the appropriate value from array ui32GPIOPinAddressArray is returned.
 */
static uint8_t getPWMGPIOPinAddress(uint8_t GPIOPinNumber);

/*
 * Function to get the preScalar #defined value from preScalar number.
 * Arguments:
 *  uint8_t preScalarNumber                     :: preScalar value 1, 2, 4, 8, 16, 32, or 64. Any other value will be treated as 64.
 * Returns:
 *  uint32_t PWM Clock PreScalar                :: prescalar #defined values as SYSCTL_PWMDIV_X, where X can be 1, 2, 4, 8, 16, 32, or 64. *
 */
static uint32_t getPreScalar(PWM_PRESCALAR preScalarNumber);

#endif
