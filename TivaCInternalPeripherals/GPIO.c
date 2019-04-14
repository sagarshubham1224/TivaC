#include "GPIO.h"


// ********** DIGITAL IO FUNCTIONS **********//
// *****************************************//


//public non-static external Functions:



/*
 * Initialize the gpio struct. Returns a pointer to the same.
 * Arguments:
 *  GPIO *gpioPointer                       :: Pointer to GPIO struct to be initialized.
 *  GPIO_PERIPHERAL GPIOPinCode             :: GPIO ShortCut Name, GPIOXn, X is [A,F], x is [0,7] .
 *  GPIO_PIN_IO_TYPE pinType                :: INPUT or OUTPUT pin.
 * Steps:
 *  Step 1: Enable peripheral of gpio (i.e., supply Clock) if not already supplied.
 *  Step 2: Enable gpio according to pin Type
 *  Step 3: return gpio struct pointer.
 * Returns:
 *  GPIO *gpioPointer                       :: Pointer to GPIO struct now initialized.
 */
GPIO* initGPIO(GPIO *gpioPointer,GPIO_PIN_CODE GPIOPinCode,GPIO_PIN_IO_TYPE pinType)
{
    char GPIOPortLetter         = getGPIOPortLetter(GPIOPinCode) ;
    uint8_t pinNumber           = getPinNumber(GPIOPinCode) ;
    gpioPointer->GPIOBase       = getGPIOBaseAddress(GPIOPortLetter);
    gpioPointer->pinNumber      = pinNumber;
    gpioPointer->pin            = 1 << pinNumber;
    gpioPointer->pinType        = pinType;
#ifndef USE_FAST_GPIO
    gpioPointer->pinState       = LOW;
#endif
    if(!SysCtlPeripheralReady(getGPIOPeripheralAddress(GPIOPortLetter))) SysCtlPeripheralEnable(getGPIOPeripheralAddress(GPIOPortLetter));    // If Peripheral is not enabled, enable it.
    if(gpioPointer->pinType == INPUT)                   // ==INPUT, so set pin as Input
    {
        GPIOPinTypeGPIOInput(gpioPointer->GPIOBase,gpioPointer->pin);
        GPIOPadConfigSet(gpioPointer->GPIOBase,gpioPointer->pin, GPIO_STRENGTH, GPIO_PIN_TYPE); // change last two for next iteration.
    }
    else if(gpioPointer->pinType == OUTPUT)                             // ==OUTPUT, so set pin as Output
    {
        GPIOPinTypeGPIOOutput(gpioPointer->GPIOBase,gpioPointer->pin);
        GPIOPinWrite(gpioPointer->GPIOBase,gpioPointer->pin,gpioPointer->pin & LOW);
#ifndef USE_FAST_GPIO
        GPIOPinWrite(gpioPointer->GPIOBase,gpioPointer->pin,gpioPointer->pinState);             // Set pin to low for now, pinState set to LOW.
#endif
    }
    return gpioPointer;
}

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
GPIO* initGPIOI(GPIO * gpioPointer,uint32_t interruptType, uint32_t portInt,void (*intHandlerFunction)(void))
{
    GPIOIntClear(gpioPointer->GPIOBase, gpioPointer->pin);                                  // Clear any existing interrupts
    GPIOIntRegister(gpioPointer->GPIOBase, intHandlerFunction);                             // Register Interrupt for gpio->base with ISR intHandlerFunction.
    GPIOIntTypeSet(gpioPointer->GPIOBase, gpioPointer->pin, interruptType);                 // Setting Interrupt to trigger based on interruptType
    GPIOIntEnable(gpioPointer->GPIOBase, gpioPointer->pin);                                 // Enable the GPIO Interrupts on Port F
    IntEnable(portInt);                                                                 // Enable Interrupts on Port F
    IntMasterEnable();                                                                  // Enable Global interrupts
    return gpioPointer;
}

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
void digitalWrite(GPIO *gpioPointer,GPIO_PIN_STATE writePinState)
{
    GPIOPinWrite(gpioPointer->GPIOBase,gpioPointer->pin,gpioPointer->pin & writePinState);
#ifndef USE_FAST_GPIO
    gpioPointer->pinState = writePinState & gpioPointer->pin;
#endif
}

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
uint8_t digitalRead(GPIO *gpioPointer)
{

#ifdef USE_FAST_GPIO
    return GPIOPinRead(gpioPointer->GPIOBase,gpioPointer->pin) >> gpioPointer->pinNumber ;
#else
    gpioPointer->pinState = GPIOPinRead(gpioPointer->GPIOBase,gpioPointer->pin) >> gpioPointer->pinNumber ;
    return (gpioPointer->pinState);
#endif

}




//private static non-extern Functions:



/*
 * Function to get GPIO Peripheral address depending upon GPIO Peripheral Letter.
 * Arguments:
 *  char GPIOPortLetter                         :: GPIO Port Letter 'A', 'B', 'C', 'D' , 'E', or 'F'.
 * Returns:
 *  uint32_t address Of GPIO Peripheral         :: Depending upon passed argument, the appropriate value from array ui32GPIOPeripheralAddressArray is returned.
 */
static uint32_t getGPIOPeripheralAddress(char GPIOPortLetter)
{
    return ui32GPIOPeripheralAddressArray[(uint8_t)(GPIOPortLetter - 'A')];
}


/*
 * Function to get GPIO Base Address depending upon GPIO Peripheral Letter.
 * Arguments:
 *  uint8_t GPIOPortLetter                      :: GPIO Port Letter 'A', 'B', 'C', 'D' , 'E', or 'F'.
 * Returns:
 *  uint32_t address of GPIO Base               :: Depending upon passed argument, the appropriate value from array ui32PGPIOBaseAddressArray is returned.
 */
static uint32_t getGPIOBaseAddress(char GPIOPortLetter)
{
    return ui32GPIOBaseAddressArray[(uint8_t)(GPIOPortLetter - 'A')];
}

static uint8_t getPinNumber(GPIO_PIN_CODE GPIOPinCode)
{
    switch (GPIOPinCode) {
    case GPIOA0 :
    case GPIOB0 :
    case GPIOC0 :
    case GPIOD0 :
    case GPIOE0 :
    case GPIOF0 :
        return 0 ;
    case GPIOA1 :
    case GPIOB1 :
    case GPIOC1 :
    case GPIOD1 :
    case GPIOE1 :
    case GPIOF1 :
        return 1 ;
    case GPIOA2 :
    case GPIOB2 :
    case GPIOC2 :
    case GPIOD2 :
    case GPIOE2 :
    case GPIOF2 :
        return 2 ;
    case GPIOA3 :
    case GPIOB3 :
    case GPIOC3 :
    case GPIOD3 :
    case GPIOE3 :
    case GPIOF3 :
        return 3 ;
    case GPIOA4 :
    case GPIOB4 :
    case GPIOC4 :
    case GPIOD4 :
    case GPIOE4 :
    case GPIOF4 :
        return 4 ;
    case GPIOA5 :
    case GPIOB5 :
    case GPIOC5 :
    case GPIOD5 :
    case GPIOE5 :
    case GPIOF5 :
        return 5 ;
    case GPIOA6 :
    case GPIOB6 :
    case GPIOC6 :
    case GPIOD6 :
    case GPIOE6 :
    case GPIOF6 :
        return 6 ;
    case GPIOA7 :
    case GPIOB7 :
    case GPIOC7 :
    case GPIOD7 :
    case GPIOE7 :
    case GPIOF7 :
        return 7 ;

    default:
        break;
    }
    return 100 ;

}

static char getGPIOPortLetter(GPIO_PIN_CODE GPIOPinCode)
{
    switch (GPIOPinCode) {
    case GPIOA0 :
    case GPIOA1 :
    case GPIOA2 :
    case GPIOA3 :
    case GPIOA4 :
    case GPIOA5 :
    case GPIOA6 :
    case GPIOA7 :
        return 'A' ;
    case GPIOB0 :
    case GPIOB1 :
    case GPIOB2 :
    case GPIOB3 :
    case GPIOB4 :
    case GPIOB5 :
    case GPIOB6 :
    case GPIOB7 :
        return 'B' ;
    case GPIOC0 :
    case GPIOC1 :
    case GPIOC2 :
    case GPIOC3 :
    case GPIOC4 :
    case GPIOC5 :
    case GPIOC6 :
    case GPIOC7 :
        return 'C' ;
    case GPIOD0 :
    case GPIOD1 :
    case GPIOD2 :
    case GPIOD3 :
    case GPIOD4 :
    case GPIOD5 :
    case GPIOD6 :
    case GPIOD7 :
        return 'D' ;
    case GPIOE0 :
    case GPIOE1 :
    case GPIOE2 :
    case GPIOE3 :
    case GPIOE4 :
    case GPIOE5 :
    case GPIOE6 :
    case GPIOE7 :
        return 'E' ;
    case GPIOF0 :
    case GPIOF1 :
    case GPIOF2 :
    case GPIOF3 :
    case GPIOF4 :
    case GPIOF5 :
    case GPIOF6 :
    case GPIOF7 :
        return 'F' ;

    default:
        break;
    }
    return 100 ;
}
