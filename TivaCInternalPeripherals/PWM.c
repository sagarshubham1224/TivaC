#include "PWM.h"


/*
 * There are two PWM modules; Module 0  and Module 1
 * Each of them have 4 Generators. Generators are numbered from  Generator 0 to Generator 4.
 * Each Generator has 2 signal outputs. These are named according to Modules0 and 1 and not Generator names.
 * Each Generator's output signal's frequency is controlled by 16 bit load value.
 * 2x4x2 = 16 maximum output PWM signals at one time.
 * We will use PF1,PF2,PF3 for PWM signal generation. First only for PF1.
 * PF1:- M1PWM5, Module 1, Generator 2
 * PF2:- M1PWM6, Module 1, Generator 3
 * PF3:- M1PWM7, Module 1, Generator 3
 */

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










//********** PWM FUNCTIONS **********//
//**********************************//


//public non-static external Functions:


/*
 * Create  Struct Pointer of type PWMPin to control PWM Signal on a Pin.
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
 * Returns:
 *  none.
 */
void initPWM(    PWMPin *PWMPinPointer,          GPIO_PIN_CODE GPIOPinCode,
                 PWM_PRESCALAR preScalarNumber,  uint32_t PWMFrequency )
{
    /*
     *
     * How this mess works :
     *  We need following info to get PWM signal on a pin:
     *      1.  The GPIO Peripheral the Pin belongs to. SYSCTL_PERIPH_GPIOX. Passed only at initialization, no need to keep in Struct.
     *      2.  The PWM Module Peripheral from which the PWM Signal will be provided. SYSCTL_PERIPH_PWMX. Passed only at initialization, no need to keep in Struct.
     *      3.  The Pre Scalar Value. SYSCTL_PWMDIV_X. Passed only at initialization, no need to keep in Struct.
     *      4.  The Alternate PWM function Address of the Pin. GPIO_PXX_MXPWMX. Passed only at initialization, no need to keep in Struct.
     *      5.  The SYSCTL_PERIPH_GPIOX's base address. GPIO_PORTX_BASE. Passed only at initialization, no need to keep in Struct.
     *      6.  The GPIO Pin Address. GPIO_PIN_X. Passed only at initialization, no need to keep in Struct.
     *      7.  The SYSCTL_PERIPH_PWMX's base address. PWMX_BASE, Need for Setting Pulse width at run time, so include in Struct.
     *      8.  The Generator Address of the PWM Modules. PWM_GEN_X May need to change period(load) at run, so include.
     *      9.  The Mode of Generator. Can be Combination. Passed only at initialization, no need to keep in Struct.
     *      10. The Frequency at which the PWM Signal is to be generated. From it, the Load Value will be calculated. May need to change at run,so include in Struct.
     *      11. The Offset address of PWM Pin. PWM_OUT_X. Will need to set Pulse width at run time, so include in Struct.
     *      12. The  Bit ID for setting PWM Output On or Off. PWM_OUT_X_BIT.Needed at runtime to shutdown PWM signal, so include in Struct.
     */
    uint32_t pinAlternateFunctionAddress = 0, PWMGeneratorAddress = 0, PWMOutAddress = 0, PWMOutBitAddress = 0;
    uint32_t PWMPeripheralAddress = 0, PWMBaseAddress = 0 ;
    uint32_t PWMGPIOPeripheralAddress = 0, PWMGPIOBaseAddress = 0 ;

    getPWMSauce(GPIOPinCode, &pinAlternateFunctionAddress, & PWMGeneratorAddress, &PWMOutAddress, &PWMOutBitAddress);
    getPWMPeripheralAndBaseAddress(GPIOPinCode, &PWMPeripheralAddress, &PWMBaseAddress);
    getPWMGPIOPeripheralAndBaseAddress(GPIOPinCode, &PWMGPIOPeripheralAddress, &PWMGPIOBaseAddress) ;

    PWMPinPointer->PWMModuleBase = PWMBaseAddress;
    PWMPinPointer->PWMGenenrator = PWMGeneratorAddress ;
    PWMPinPointer->PWMOut = PWMOutAddress ;
    PWMPinPointer->PWMOutBits = PWMOutBitAddress ;
    if(PWMFrequency > SysCtlClockGet()/preScalarNumber) PWMFrequency = SysCtlClockGet()/preScalarNumber ;
    PWMPinPointer->PWMFrequency = PWMFrequency ;
    PWMPinPointer->PWMPeriod = SysCtlClockGet()/preScalarNumber/PWMFrequency- 1;
    PWMPinPointer->PWMState = PWM_DISABLE;
    PWMPinPointer->PWMPrescalar = getPreScalar(preScalarNumber) ;
    /*
     * Step 1: Enable GPIO Module:-
     */
    SysCtlPeripheralEnable(PWMGPIOPeripheralAddress);
    SysCtlDelay(10);
    /*
     * Step 2: Enable PWM Module:-
     */
    SysCtlPeripheralEnable(PWMPeripheralAddress);     //In this example, as all signals are from the same Module,we only initialize that one.
    SysCtlDelay(10);
    /*
     * Step 3: Enable Clock on PWM Module.
     * As SystemClock is set at 80Mhz, and Pre-scalar for PWM module is set at 64, 80M/64 = 1.25MHz is the Clock period for the PWM Module.
     */
    SysCtlPWMClockSet(getPreScalar(preScalarNumber));
    /*
     * Step 4: Configure alternate function, both on pin and on port base.
     */
    GPIOPinTypePWM(PWMGPIOBaseAddress,getPWMGPIOPinAddress(GPIOPinCode));
    GPIOPinConfigure(pinAlternateFunctionAddress);
    /*
     * Step 5: Configure each of the the PWM Generator in use.
     */
    PWMGenConfigure(PWMPinPointer->PWMModuleBase,PWMPinPointer->PWMGenenrator,PWM_PREDEFINED_CONFIG_MODE);
    /*
     * Step 6: Set Period(Load) value of each generator.
     * For Desired Frequency F Hz, at System clock C Hz, and when using pre-scalar value P, Load value L is given by :-
     * L = (C/P)/F - 1
     */
    PWMGenPeriodSet(PWMPinPointer->PWMModuleBase,PWMPinPointer->PWMGenenrator,PWMPinPointer->PWMPeriod);
    /*
     * Step 7: Set initial Pulse width as Zero
     */
    PWMPinPointer->PWMState = PWM_DISABLE;
    PWMOutputState(PWMPinPointer->PWMModuleBase,PWMPinPointer->PWMOutBits,false);
    /*
     * Step 8: Enable the timer on the generator.
     */
    PWMGenEnable(PWMPinPointer->PWMModuleBase,PWMPinPointer->PWMGenenrator);


    //  UARTprintf("GPIO Peripheral: Sent: %X Actual: %X\n",getGPIOPeripheral(GPIOPortLetter),SYSCTL_PERIPH_GPIOF);
    //  UARTprintf("PWM Module Peripheral: Sent: %X Actual: %X\n",getPWMModulePeripheralAddress(PWMPeripheralNumber),SYSCTL_PERIPH_PWM1);
    //  UARTprintf("PreScalar: Sent: %X Actual %X\n",getPreScalar(preScalarNumber), SYSCTL_PWMDIV_64);
    //  UARTprintf("PinAddress: Sent: %X Actual: %X\n",getGPIOPinAddress(GPIOPinNumber),GPIO_PIN_1);

}

void setPWMFrequency(PWMPin* PWMPinPointer, uint32_t PWMFrequency)
{
    PWMPinPointer->PWMPeriod = SysCtlClockGet()/PWMPinPointer->PWMPrescalar / PWMFrequency - 1 ;
    PWMPinPointer->PWMFrequency = PWMFrequency ;
    PWMGenPeriodSet(PWMPinPointer->PWMModuleBase, PWMPinPointer->PWMGenenrator, PWMPinPointer->PWMPeriod) ;
    analogPWMWrite(PWMPinPointer, 30) ;
}

/*
 * Write PWM Width from %age Value of Duty Cycle.
 * Arguments:
 *  PWMPin* PWMPinPointer                       :: Pointer to PWMPin struct.
 *  uint8_t dutyCycle100                        :: duty Cycle value between 0 and 100 which is to be applied at the PWM Pin.
 * Returns:
 *  nothing.
 */
void analogPWMWrite(PWMPin* PWMPinPointer, uint8_t dutyCycle100)
{
    if(dutyCycle100)
    {
        PWMPulseWidthSet(PWMPinPointer->PWMModuleBase,PWMPinPointer->PWMOut,dutyCycle100*PWMPinPointer->PWMPeriod/100);
        if(PWMPinPointer->PWMState == PWM_DISABLE)
        {
            PWMOutputState(PWMPinPointer->PWMModuleBase,PWMPinPointer->PWMOutBits,true);
            PWMPinPointer->PWMState = PWM_ENABLE;
        }
    }
    else
    {
        PWMOutputState(PWMPinPointer->PWMModuleBase,PWMPinPointer->PWMOutBits,false);
        PWMPinPointer->PWMState = PWM_DISABLE;
    }
}

/*
 * Write PWM Width from %age Value of Duty Cycle.
 * Arguments:
 *  PWMPin* PWMPinPointer                       :: Pointer to PWMPin struct.
 *  uint32_t dutyCycle                          :: duty Cycle value between 0 Period(Load) Value which is to be applied at PWM Pin.
 * Returns:
 *  nothing.
 */
void analogPWMWriteRaw(PWMPin* PWMPin, uint32_t dutyCycle)
{
    if(dutyCycle)
    {
        PWMPulseWidthSet(PWMPin->PWMModuleBase,PWMPin->PWMOut,dutyCycle);
        if(PWMPin->PWMState == PWM_DISABLE)
        {
            PWMOutputState(PWMPin->PWMModuleBase,PWMPin->PWMOutBits,true);
            PWMPin->PWMState = PWM_ENABLE;
        }
    }
    else
    {
        PWMOutputState(PWMPin->PWMModuleBase,PWMPin->PWMOutBits,false);
        PWMPin->PWMState = PWM_DISABLE;
    }
}



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
                         uint32_t* PWMOutBitAddressVariablePointer)
{
    switch (GPIOPinCode) {
    case GPIOB6:
        *PWMPinAlternateFunctionAddressVariablePointer = GPIO_PB6_M0PWM0 ;
        *PWMGeneratorAddressVariablePointer = PWM_GEN_0 ;
        *PWMOutAddressVariablePointer = PWM_OUT_0 ;
        *PWMOutBitAddressVariablePointer = PWM_OUT_0_BIT ;
        break;
    case GPIOB7:
        *PWMPinAlternateFunctionAddressVariablePointer = GPIO_PB7_M0PWM1 ;
        *PWMGeneratorAddressVariablePointer = PWM_GEN_0 ;
        *PWMOutAddressVariablePointer = PWM_OUT_1 ;
        *PWMOutBitAddressVariablePointer = PWM_OUT_1_BIT ;
        break;
    case GPIOB4:
        *PWMPinAlternateFunctionAddressVariablePointer = GPIO_PB4_M0PWM2 ;
        *PWMGeneratorAddressVariablePointer = PWM_GEN_1 ;
        *PWMOutAddressVariablePointer = PWM_OUT_2 ;
        *PWMOutBitAddressVariablePointer = PWM_OUT_2_BIT ;
        break;
    case GPIOB5:
        *PWMPinAlternateFunctionAddressVariablePointer = GPIO_PB5_M0PWM3 ;
        *PWMGeneratorAddressVariablePointer = PWM_GEN_1 ;
        *PWMOutAddressVariablePointer = PWM_OUT_3 ;
        *PWMOutBitAddressVariablePointer = PWM_OUT_3_BIT ;
        break;
    case GPIOE4:
        *PWMPinAlternateFunctionAddressVariablePointer = GPIO_PE4_M0PWM4 ;
        *PWMGeneratorAddressVariablePointer = PWM_GEN_2 ;
        *PWMOutAddressVariablePointer = PWM_OUT_4 ;
        *PWMOutBitAddressVariablePointer = PWM_OUT_4_BIT ;
        break;
    case GPIOE5:
        *PWMPinAlternateFunctionAddressVariablePointer = GPIO_PE5_M0PWM5 ;
        *PWMGeneratorAddressVariablePointer = PWM_GEN_2 ;
        *PWMOutAddressVariablePointer = PWM_OUT_5 ;
        *PWMOutBitAddressVariablePointer = PWM_OUT_5_BIT ;
        break;
    case GPIOC4:
        *PWMPinAlternateFunctionAddressVariablePointer = GPIO_PC4_M0PWM6 ;
        *PWMGeneratorAddressVariablePointer = PWM_GEN_3 ;
        *PWMOutAddressVariablePointer = PWM_OUT_6 ;
        *PWMOutBitAddressVariablePointer = PWM_OUT_6_BIT ;
        break;
    case GPIOC5:
        *PWMPinAlternateFunctionAddressVariablePointer = GPIO_PC5_M0PWM7 ;
        *PWMGeneratorAddressVariablePointer = PWM_GEN_3 ;
        *PWMOutAddressVariablePointer = PWM_OUT_7 ;
        *PWMOutBitAddressVariablePointer = PWM_OUT_7_BIT ;
        break;
    case GPIOD0:
        *PWMPinAlternateFunctionAddressVariablePointer = GPIO_PD0_M1PWM0 ;
        *PWMGeneratorAddressVariablePointer = PWM_GEN_0 ;
        *PWMOutAddressVariablePointer = PWM_OUT_0 ;
        *PWMOutBitAddressVariablePointer = PWM_OUT_0_BIT ;
        break;
    case GPIOD1:
        *PWMPinAlternateFunctionAddressVariablePointer = GPIO_PD1_M1PWM1 ;
        *PWMGeneratorAddressVariablePointer = PWM_GEN_0 ;
        *PWMOutAddressVariablePointer = PWM_OUT_1 ;
        *PWMOutBitAddressVariablePointer = PWM_OUT_1_BIT ;
        break;
    case GPIOA6:
        *PWMPinAlternateFunctionAddressVariablePointer = GPIO_PA6_M1PWM2 ;
        *PWMGeneratorAddressVariablePointer = PWM_GEN_1 ;
        *PWMOutAddressVariablePointer = PWM_OUT_2 ;
        *PWMOutBitAddressVariablePointer = PWM_OUT_2_BIT ;
        break;
    case GPIOA7:
        *PWMPinAlternateFunctionAddressVariablePointer = GPIO_PA7_M1PWM3 ;
        *PWMGeneratorAddressVariablePointer = PWM_GEN_1 ;
        *PWMOutAddressVariablePointer = PWM_OUT_3 ;
        *PWMOutBitAddressVariablePointer = PWM_OUT_3_BIT ;
        break;
    case GPIOF0:
        *PWMPinAlternateFunctionAddressVariablePointer = GPIO_PF0_M1PWM4 ;
        *PWMGeneratorAddressVariablePointer = PWM_GEN_2 ;
        *PWMOutAddressVariablePointer = PWM_OUT_4 ;
        *PWMOutBitAddressVariablePointer = PWM_OUT_4_BIT ;
        break;
    case GPIOF1:
        *PWMPinAlternateFunctionAddressVariablePointer = GPIO_PF1_M1PWM5 ;
        *PWMGeneratorAddressVariablePointer = PWM_GEN_2 ;
        *PWMOutAddressVariablePointer = PWM_OUT_5 ;
        *PWMOutBitAddressVariablePointer = PWM_OUT_5_BIT ;
        break;
    case GPIOF2:
        *PWMPinAlternateFunctionAddressVariablePointer = GPIO_PF2_M1PWM6 ;
        *PWMGeneratorAddressVariablePointer = PWM_GEN_3 ;
        *PWMOutAddressVariablePointer = PWM_OUT_6 ;
        *PWMOutBitAddressVariablePointer = PWM_OUT_6_BIT ;
        break;
    case GPIOF3:
        *PWMPinAlternateFunctionAddressVariablePointer = GPIO_PF3_M1PWM7 ;
        *PWMGeneratorAddressVariablePointer = PWM_GEN_3 ;
        *PWMOutAddressVariablePointer = PWM_OUT_7 ;
        *PWMOutBitAddressVariablePointer = PWM_OUT_7_BIT ;
        break;
    default:
        break;
    }
}


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
                                               uint32_t* PWMGPIOBaseAddressVariablePointer)
{
    switch (GPIOPinCode) {
    case GPIOA6:
    case GPIOA7:
        *PWMGPIOPeripheralAddressVariablePointer = ui32GPIOPeripheralAddressArray[0] ;
        *PWMGPIOBaseAddressVariablePointer = ui32GPIOBaseAddressArray[0] ;
        break;
    case GPIOB6:
    case GPIOB7:
    case GPIOB4:
    case GPIOB5:
        *PWMGPIOPeripheralAddressVariablePointer = ui32GPIOPeripheralAddressArray[1] ;
        *PWMGPIOBaseAddressVariablePointer = ui32GPIOBaseAddressArray[1] ;
        break;
    case GPIOC4:
    case GPIOC5:
        *PWMGPIOPeripheralAddressVariablePointer = ui32GPIOPeripheralAddressArray[2] ;
        *PWMGPIOBaseAddressVariablePointer = ui32GPIOBaseAddressArray[2] ;
        break;
    case GPIOD0:
    case GPIOD1:
        *PWMGPIOPeripheralAddressVariablePointer = ui32GPIOPeripheralAddressArray[3] ;
        *PWMGPIOBaseAddressVariablePointer = ui32GPIOBaseAddressArray[3] ;
        break;
    case GPIOE4:
    case GPIOE5:
        *PWMGPIOPeripheralAddressVariablePointer = ui32GPIOPeripheralAddressArray[4] ;
        *PWMGPIOBaseAddressVariablePointer = ui32GPIOBaseAddressArray[4] ;
        break;
    case GPIOF0:
    case GPIOF1:
    case GPIOF2:
    case GPIOF3:
        *PWMGPIOPeripheralAddressVariablePointer = ui32GPIOPeripheralAddressArray[5] ;
        *PWMGPIOBaseAddressVariablePointer = ui32GPIOBaseAddressArray[5] ;
        break;
    default:
        break;
    }
}


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
                                               uint32_t* PWMBaseAddressVariablePointer)
{
    switch (GPIOPinCode) {
    case GPIOB6:
    case GPIOB7:
    case GPIOB4:
    case GPIOB5:
    case GPIOE4:
    case GPIOE5:
    case GPIOC4:
    case GPIOC5:
        *PWMPeripheralAddressVariablePointer    = ui32PWMModulePeripheralAddressArray[0] ;
        *PWMBaseAddressVariablePointer          = ui32PWMModuleBaseAddressArray[0] ;
    case GPIOD0:
    case GPIOD1:
    case GPIOA6:
    case GPIOA7:
    case GPIOF0:
    case GPIOF1:
    case GPIOF2:
    case GPIOF3:
        *PWMPeripheralAddressVariablePointer    = ui32PWMModulePeripheralAddressArray[1] ;
        *PWMBaseAddressVariablePointer          = ui32PWMModuleBaseAddressArray[1] ;
    default:
        break;
    }
}


/*
 * Function to get Pin Address depending upon GPIO_PIN_CODE.
 * Arguments:
 *  uint8_t GPIOPinNumber                       :: GPIO Pin Number respective to Port, i.e. 0, 1, 2, 3, 4, 5, 6, or 7.
 * Returns:
 *  uint8_t GPIO Pin Address                    :: Depending upon passed argument, the appropriate value from array ui32GPIOPinAddressArray is returned.
 */
static uint8_t getPWMGPIOPinAddress(GPIO_PIN_CODE GPIOPinCode)
{
    uint8_t GPIOPinAddress = 0 ;
    switch (GPIOPinCode) {
        case GPIOD0:
        case GPIOF0:
            GPIOPinAddress = ui8GPIOPinAddressArray[0] ;
            break;
        case GPIOD1:
        case GPIOF1:
            GPIOPinAddress = ui8GPIOPinAddressArray[1] ;
            break;
        case GPIOF2:
            GPIOPinAddress = ui8GPIOPinAddressArray[2] ;
            break;
        case GPIOF3:
            GPIOPinAddress = ui8GPIOPinAddressArray[3] ;
            break;
        case GPIOB4:
        case GPIOE4:
        case GPIOC4:
            GPIOPinAddress = ui8GPIOPinAddressArray[4] ;
            break;
        case GPIOB5:
        case GPIOE5:
        case GPIOC5:
            GPIOPinAddress = ui8GPIOPinAddressArray[5] ;
            break;
        case GPIOA6:
        case GPIOB6:
            GPIOPinAddress = ui8GPIOPinAddressArray[6] ;
            break;
        case GPIOA7:
        case GPIOB7:
            GPIOPinAddress = ui8GPIOPinAddressArray[7] ;
            break;
        default:
            break;
    }
    return GPIOPinAddress ;
}

/*
 * Function to get the preScalar #defined value from preScalar number.
 * Arguments:
 *  PWM_PRESCALAR preScalarNumber               :: preScalar value 1, 2, 4, 8, 16, 32, or 64. Any other value will be treated as 64.
 * Returns:
 *  uint32_t PWM Clock PreScalar                :: prescalar #defined values as SYSCTL_PWMDIV_X, where X can be 1, 2, 4, 8, 16, 32, or 64. *
 */
static uint32_t getPreScalar(PWM_PRESCALAR preScalarNumber)
{
    switch (preScalarNumber)
    {
    case 1 :
        return SYSCTL_PWMDIV_1;
    case 2 :
        return SYSCTL_PWMDIV_2;
    case 4 :
        return SYSCTL_PWMDIV_4;
    case 8 :
        return SYSCTL_PWMDIV_8;
    case 16 :
        return SYSCTL_PWMDIV_16;
    case 32 :
        return SYSCTL_PWMDIV_32;
    case 64 :
        return SYSCTL_PWMDIV_64;
    default:
        return 64;
    }
}
