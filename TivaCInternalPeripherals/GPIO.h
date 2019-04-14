/*
 * GPIO.h
 * File Description : Contains functions for GPIO peripherals for Tiva C boards.
 */

#ifndef GPIO_H_
#define GPIO_H_
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "PERIPHERALS.h"

#include "driverlib/sysctl.h"
#include "inc/hw_memmap.h"

/*
 * Macro Definitions:
 */
//#define INPUT 1                                 // Pin type is set as INPUT.
//#define OUTPUT 2                                // Pin type is set as OUTPUT.
//#define HIGH 255                                // If Pin type is Output, setting Pin to HIGH, puts 3.3V on the pin.
//#define LOW 0                                   // If Pin type is Output, setting Pin to LOW, puts 0V on the pin.
#define GPIO_STRENGTH GPIO_STRENGTH_2MA         // Decides gpio strength.
#define GPIO_PIN_TYPE GPIO_PIN_TYPE_STD_WPU     // Decides pull-up type.
#define USE_FAST_GPIO                           // Attempt faster code by not updating pin state.
// /End Macro Definitions.

typedef enum GPIO_PIN_IO_TYPE {
    INPUT = false ,
    OUTPUT = true
}GPIO_PIN_IO_TYPE;

typedef enum GPIO_PIN_STATE {
    HIGH = 255 ,
    LOW  = 0
}GPIO_PIN_STATE;

/*
 * GPIO: Struct Definition:
 * pinType:     Type of Pin, INPUT or OUTPUT
 * pinNumber:   Pin Number relative to Port. GPIO_PIN_3 will be pinNumber 3
 * pin:         Actual Pin Address. If pinNumber is 3, then pin=1<<3, i.e. = 8
 * pinState:    Whether pin is set to HIGH or LOW, either in INPUT OR OUTPUT type.
 * peripheral:  GPIO Peripheral Address to which port belongs. Needs one time activation to feed clock.
 * base:        Port Base Address of the Pin.
*/
typedef struct GPIO{
    GPIO_PIN_IO_TYPE pinType;                   // Input or Output Pin.
    uint8_t pin;                                // Pin value, GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6, or GPIO_PIN_7.
    uint8_t pinNumber;                          // Pin Number according to Port, based on this, (uint8_t pin) value is decided.
                                                // This is Done in the following way:
                                                //  Suppose, entered pinNumber is 3.
                                                //  Then, pin = 1 << pinNumber = 1 << 3 = 8, which is bitpacked value GPIO_PIN_3.
#ifndef USE_FAST_GPIO
    uint8_t pinState;                           // What is the present state of pin, does it get HIGH or LOW?Is it set to HIGH or LOW?
#endif
    uint32_t GPIOBase;                              // Base Address of Pin.
}GPIO;

/*
 * Functions:
 */



// ********** DIGITAL IO FUNCTIONS **********//
// *****************************************//

//public non-static external Functions:



/*
 * Initialize the gpio struct. Returns a pointer to the same.
 * Arguments:
 *  GPIO *gpioPointer                       :: Pointer to GPIO struct to be initialized.
 *  GPIO_PIN_CODE GPIOPinCode               :: GPIO_PIN_CODE enum value, GPIOXn, X is [A,F], n is [0,7] .
 *  uint8_t pinType                         :: INPUT or OUTPUT pin.
 * Steps:
 *  Step 1: Enable peripheral of gpio (i.e., supply Clock) if not already supplied.
 *  Step 2: Enable gpio according to pin Type
 *  Step 3: return gpio struct pointer.
 * Returns:
 *  GPIO *gpioPointer                       :: Pointer to GPIO struct now initialized.
 */
GPIO* initGPIO(GPIO *gpioPointer,
               GPIO_PIN_CODE GPIOPinCode,
               GPIO_PIN_IO_TYPE pinType) ;


/*
 * initialize Interrupt controlled GPIO.
 * MUST CALL initGPIO(...) BEFORE CALLING THIS ONE!!!
 * Arguments:
 *  GPIO* gpioPointer                               :: pointer to GPIO pin Set as input.
 *  uint32_t interruptType                          :: type of interrupt to be set.
 *  uint32_t portInt                                :: the interrupt of the particular port. Eg. INT_GPIOF
 *  void (*intHandlerFunction)(void)                :: Pointer to Interrupt Handler.
 * Steps:
 *  Step 1: Clear all interrupts on this base.
 *  Step 2: Register ISR.
 *  Step 3: Set Interrupt type.
 *  Step 4: Enable Interrupt on GPIO Level.
 *  Step 5: Enable Interrupt on NVIC Leve.
 *  Step 6: Enable Master Interrupt.
 * Returns:
 *  GPIO* gpioPointer                               :: pointer to GPIO pin Set as input. *
 */
extern GPIO* initGPIOI(GPIO * gpioPointer,uint32_t interruptType, uint32_t portInt,void (*intHandlerFunction)(void));

/*
 * Write HIGH or LOW Value on OUTPUT Set gpio pin.
 * Arguments:
 *  GPIO *gpioPointer                           :: Pointer to the GPIO whose state needs writing.
 *  uint8_t writePinState                       :: State to be written on the GPIO Pin(s). Must be in range of uint8_t.
 * Steps:
 *  Step 1: Write New State by Logical AND of gpio->pin & (uint8_t pinState).
 *  Step 2: If USE_FAST_GPIO is not defined, update gpio->pinState as well.
 * Returns:
 *  void.
 */
extern void digitalWrite(GPIO *gpio,uint8_t writePinState);

/*
 * Read and Return Input Set gpio pin.
 * Arguments:
 *  GPIO *gpio                              :: pointer to GPIO which needs to be read.
 * Steps:
 *  Step 1: Read pinState.
 *  Step 2: Shift it right (divide by 2) pinNumber times.
 *  Step 3: If USE_FAST_GPIO is defined, return the above value immediately.
 *  Step 4: If USE_FAST_GPIO is not defined, assign Step 1,2 value to gpio->pinState.
 *  Step 4: If USE_FAST_GPIO Return gpio->pinState.
 * Returns:
 *  uint8_t pinState                        :: If USE_FAST_GPIO is  defined, directly returns the read state.
 *  uint8_t gpioPointer->pinState                   :: pinState of GPIO pointer *gpioPointer.
 */
extern uint8_t digitalRead(GPIO *gpio);



//private static non-extern Functions:


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
 * Function to get GPIO Pin Number depending upon GPIO_PIN_CODE
 * Arguments:
 *  GPIO_PIN_CODE                               :: GPIO_PIN_CODE enum value.
 * Returns:
 *  uint8_t pinNumber                           :: Pin Number value.
 */
static uint8_t getPinNumber(GPIO_PIN_CODE GPIOPinCode) ;

/*
 * Function to get GPIO Port Letter depending upin GPIO_PIN_CODE
 * Arguments:
 *  GPIO_PIN_CODE GPIOPinCode                   :: GPIO_PIN_CODE enum value.
 * Returns:
 *  char GPIOPortLetter                         :: Port Letter value.
 */
static char getGPIOPortLetter(GPIO_PIN_CODE GPIOPinCode) ;

#endif
