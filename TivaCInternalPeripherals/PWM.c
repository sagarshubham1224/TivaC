#include "PWM.h"


/*
 * There are two PWM modules; Module 0  and Module 1
 * Each of them have 4 Generators. Generators are numbered from  Generator 0 to Generator 7.
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
 */



//******** PWM Sauce Arrays *********//
//**********************************//



/*
 * Array to hold GPIO Peripheral Address SYSCTL_PERIPH_GPIOX, where X can be Port A, B, C, D, E, or F.
 */
const static uint32_t ui32GPIOPeripheralAddressArray[6] = {
        SYSCTL_PERIPH_GPIOA, SYSCTL_PERIPH_GPIOB, SYSCTL_PERIPH_GPIOC,
        SYSCTL_PERIPH_GPIOD, SYSCTL_PERIPH_GPIOE, SYSCTL_PERIPH_GPIOF
};

/*
 * Array to hold GPIO Base Address GPIO_PORTX_BASE, where X can be Port A, B, C, D, E, or F.
 */
const static uint32_t ui32GPIOBaseAddressArray[6] = {
        GPIO_PORTA_BASE, GPIO_PORTB_BASE, GPIO_PORTC_BASE,
        GPIO_PORTD_BASE, GPIO_PORTE_BASE, GPIO_PORTF_BASE
};

/*
 * Array to hold GPIO Pin Address GPIO_PIN_X, where X can be 0, 1, 2, 3, 4, 5, 6, or 7.
 */
const static uint8_t ui8GPIOPinAddressArray[8] = {
        GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3,
        GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_7
};


/*
 * Array to hold PWM Module Peripheral Address SYSCTL_PERIPH_PWMX, where X can be 0, or 1.
 */
const static uint32_t ui32PWMModulePeripheralAddressArray[2] = {
        SYSCTL_PERIPH_PWM0,SYSCTL_PERIPH_PWM1
};

/*
 * Array to hold PWM Module Base Address PWMX_BASE, where X can be 0, or 1.
 */
const static uint32_t ui32PWMModuleBaseAddressArray[2] = {
        PWM0_BASE, PWM1_BASE
};




//********** PWM FUNCTIONS **********//
//**********************************//


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
                    uint32_t pwmFrequency )
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
     *  For getting values in Step 1., 2., 3., 5., 6., 7., and 8. appropriate functions are used to get correct values. Those have their own functional description.
     *  For getting values in Step 4., 11., and 12. getPWMSauce() is used and it returns value according to need.
     */
    pwmPinPointer->pwmModuleBase = getPWMModuleBaseAddress(PWMPeripheralNumber);
    pwmPinPointer->pwmGenenrator = getPWMSauce(GPIOPortLetter,PWMPeripheralNumber,GPIOPinNumber,GET_GEN);
    pwmPinPointer->pwmOut = getPWMSauce(GPIOPortLetter,PWMPeripheralNumber,GPIOPinNumber,GET_PWM);
    pwmPinPointer->pwmOutBits = getPWMSauce(GPIOPortLetter,PWMPeripheralNumber,GPIOPinNumber,GET_PBT);
    pwmPinPointer->pwmFrequency = pwmFrequency ;
    pwmPinPointer->pwmPeriod = SysCtlClockGet()/preScalarNumber/pwmFrequency- 1;
    pwmPinPointer->pwmOutputState = false;
    pwmPinPointer->pwmPrescalar = getPreScalar(preScalarNumber) ;
    /*
     * Step 1: Enable GPIO Module:-
     */
    SysCtlPeripheralEnable(getGPIOPeripheralAddress(GPIOPortLetter));
    SysCtlDelay(10);
    /*
     * Step 2: Enable PWM Module:-
     */
    SysCtlPeripheralEnable(getPWMModulePeripheralAddress(PWMPeripheralNumber));     //In this example, as all signals are from the same Module,we only initialize that one.
    SysCtlDelay(10);
    /*
     * Step 3: Enable Clock on PWM Module.
     * As SystemClock is set at 80Mhz, and Pre-scalar for PWM module is set at 64, 80M/64 = 1.25MHz is the Clock period for the PWM Module.
     */
    SysCtlPWMClockSet(getPreScalar(preScalarNumber));
    /*
     * Step 4: Configure alternate function, both on pin and on port base.
     */
    GPIOPinTypePWM(getGPIOBaseAddress(GPIOPortLetter),getGPIOPinAddress(GPIOPinNumber));
    GPIOPinConfigure(getPWMSauce(GPIOPortLetter,PWMPeripheralNumber,GPIOPinNumber,GET_ALT));
    /*
     * Step 5: Configure each of the the PWM Generator in use.
     */
    PWMGenConfigure(pwmPinPointer->pwmModuleBase,pwmPinPointer->pwmGenenrator,configMode);
    /*
     * Step 6: Set Period(Load) value of each generator.
     * For Desired Frequency F Hz, at System clock C Hz, and when using pre-scalar value P, Load value L is given by :-
     * L = (C/P)/F - 1
     */
    PWMGenPeriodSet(pwmPinPointer->pwmModuleBase,pwmPinPointer->pwmGenenrator,pwmPinPointer->pwmPeriod);
    /*
     * Step 7: Set initial Pulse width as Zero
     */
    pwmPinPointer->pwmOutputState = false;
    PWMOutputState(pwmPinPointer->pwmModuleBase,pwmPinPointer->pwmOutBits,false);
    /*
     * Step 8: Enable the timer on the generator.
     */
    PWMGenEnable(pwmPinPointer->pwmModuleBase,pwmPinPointer->pwmGenenrator);


    //  UARTprintf("GPIO Peripheral: Sent: %X Actual: %X\n",getGPIOPeripheral(GPIOPortLetter),SYSCTL_PERIPH_GPIOF);
    //  UARTprintf("PWM Module Peripheral: Sent: %X Actual: %X\n",getPWMModulePeripheralAddress(PWMPeripheralNumber),SYSCTL_PERIPH_PWM1);
    //  UARTprintf("PreScalar: Sent: %X Actual %X\n",getPreScalar(preScalarNumber), SYSCTL_PWMDIV_64);
    //  UARTprintf("PinAddress: Sent: %X Actual: %X\n",getGPIOPinAddress(GPIOPinNumber),GPIO_PIN_1);

    /*
     * Step 9: Return the PWMPin pointer.
     */
    return pwmPinPointer;
}

void setPWMFrequency(PWMPin* PWMPinPointer, uint32_t pwmFrequency)
{
    PWMPinPointer->pwmPeriod = SysCtlClockGet()/PWMPinPointer->pwmPrescalar / pwmFrequency - 1 ;
    PWMPinPointer->pwmFrequency = pwmFrequency ;
    PWMGenPeriodSet(PWMPinPointer->pwmModuleBase, PWMPinPointer->pwmGenenrator, PWMPinPointer->pwmPeriod) ;
    analogPWMWrite(PWMPinPointer, 30) ;
}

/*
 * Write PWM Width from %age Value of Duty Cycle.
 * Arguments:
 *  PWMPin* pwmPinPointer                       :: Pointer to PWMPin struct.
 *  uint8_t dutyCycle100                        :: duty Cycle value between 0 and 100 which is to be applied at the PWM Pin.
 * Returns:
 *  nothing.
 */
void analogPWMWrite(PWMPin* pwmPinPointer, uint8_t dutyCycle100)
{
    if(dutyCycle100)
    {
        PWMPulseWidthSet(pwmPinPointer->pwmModuleBase,pwmPinPointer->pwmOut,dutyCycle100*pwmPinPointer->pwmPeriod/100);
        if(!pwmPinPointer->pwmOutputState)
        {
            PWMOutputState(pwmPinPointer->pwmModuleBase,pwmPinPointer->pwmOutBits,true);
            pwmPinPointer->pwmOutputState = true;
        }
    }
    else
    {
        PWMOutputState(pwmPinPointer->pwmModuleBase,pwmPinPointer->pwmOutBits,false);
        pwmPinPointer->pwmOutputState = false;
    }
}

/*
 * Write PWM Width from %age Value of Duty Cycle.
 * Arguments:
 *  PWMPin* pwmPinPointer                       :: Pointer to PWMPin struct.
 *  uint32_t dutyCycle                          :: duty Cycle value between 0 Period(Load) Value which is to be applied at PWM Pin.
 * Returns:
 *  nothing.
 */
void analogPWMWriteRaw(PWMPin* pwmPin, uint32_t dutyCycle)
{
    if(dutyCycle)
    {
        PWMPulseWidthSet(pwmPin->pwmModuleBase,pwmPin->pwmOut,dutyCycle);
        if(!pwmPin->pwmOutputState)
        {
            PWMOutputState(pwmPin->pwmModuleBase,pwmPin->pwmOutBits,true);
            pwmPin->pwmOutputState = true;
        }
    }
    else
    {
        PWMOutputState(pwmPin->pwmModuleBase,pwmPin->pwmOutBits,false);
        pwmPin->pwmOutputState = false;
    }
}



//private static non-external Functions:


/*
 * Function to get the preScalar #defined value from preScalar number.
 * Arguments:
 *  uint8_t preScalarNumber                     :: preScalar value 1, 2, 4, 8, 16, 32, or 64. Any other value will be treated as 64.
 * Returns:
 *  uint32_t PWM Clock PreScalar                :: prescalar #defined values as SYSCTL_PWMDIV_X, where X can be 1, 2, 4, 8, 16, 32, or 64. *
 */
static uint32_t getPreScalar(uint8_t preScalarNumber)
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


/*
 * Function to get all the required sauce for making a PWM Pin tick.
 * Arguments:
 *  char GPIOPortLetter                         :: GPIO Port Letter 'A', 'B', 'C', 'D' , 'E', or 'F'.
 *  uint8_t PWMPeripheralNumber                 :: PWM Peripheral Number 0, or 1.
 *  uint8_t GPIOPinNumber                       :: GPIO Pin Number respective to Port, i.e. 0, 1, 2, 3, 4, 5, 6, or 7.
 *  uint8_t retValType                          :: value to decide which type of return value is required.
 * Returns:
 *  if retValType == GET_ALT                    :: returns pinAlternateFunctionAddress, GPIO_Pxy_MnPWMq, where x is Port Number, y is Pin Number, n is PWM Module Number and q is  PWM Pin.
 *  if retValType == GET_GEN                    :: returns pwmGenerator, PWM_GEN_X, where X is 0, 1, 2, or 3.
 *  if retValType == GEN_PWM                    :: returns pwmOut, PWM_OUT_X, where X is 0, 1, 2, 3, 4, 5, 6, or 7.
 *  if retValType == GEN_PBT                    :: returns pwmOutBit, PWM_OUT_X_BIT, where X is 0, 1, 2, 3, 4, 5, 6, or 7.
 */
static uint32_t getPWMSauce(char GPIOPortLetter, uint8_t PWMPeripheralNumber, uint8_t GPIOPinNumber, uint8_t retValType)
{

    uint32_t pinAlternateFunctionAddress = 0;
    uint32_t pwmGenerator = 0;
    uint32_t pwmOut = 0;
    uint32_t pwmOutBit = 0;
    uint32_t returnVal = 0;
    if( PWMPeripheralNumber == 0 )              // Implies PWM Peripheral 0 is being used.
    {
        switch ( GPIOPortLetter ) {

        case 'B':                               // For PORTB, PB6 (GEN 0), PB7 (GEN 0), PB4 (GEN 1), and PB5 (GEN 0), are PWM Pins for Generator 0 and 1 in PWM Module 0.
            if( GPIOPinNumber == 6 )
            {
                pinAlternateFunctionAddress = GPIO_PB6_M0PWM0;
                pwmGenerator = PWM_GEN_0;
                pwmOut = PWM_OUT_0;
                pwmOutBit = PWM_OUT_0_BIT;
            }

            else if( GPIOPinNumber == 7 )
            {
                pinAlternateFunctionAddress = GPIO_PB7_M0PWM1;
                pwmGenerator = PWM_GEN_0;
                pwmOut = PWM_OUT_1;
                pwmOutBit = PWM_OUT_1_BIT;
            }

            else if( GPIOPinNumber == 4 )
            {
                pinAlternateFunctionAddress = GPIO_PB4_M0PWM2;
                pwmGenerator = PWM_GEN_1;
                pwmOut = PWM_OUT_2;
                pwmOutBit = PWM_OUT_2_BIT;
            }

            else if( GPIOPinNumber == 5 )
            {
                pinAlternateFunctionAddress = GPIO_PB5_M0PWM3;
                pwmGenerator = PWM_GEN_1;
                pwmOut = PWM_OUT_3;
                pwmOutBit = PWM_OUT_3_BIT;
            }

            break;

        case 'E':                               // For PORTE, PE4, and PE5 are PWM Pins for Generator 2 in PWM Module 0.
            if( GPIOPinNumber == 4 )
            {
                pinAlternateFunctionAddress = GPIO_PE4_M0PWM4;
                pwmGenerator = PWM_GEN_2;
                pwmOut = PWM_OUT_4;
                pwmOutBit = PWM_OUT_4_BIT;
            }

            else if( GPIOPinNumber == 5 )
            {
                pinAlternateFunctionAddress = GPIO_PE5_M0PWM5;
                pwmGenerator = PWM_GEN_2;
                pwmOut = PWM_OUT_5;
                pwmOutBit = PWM_OUT_5_BIT;
            }

            break;

        case 'C':                               // For PORTC, PC4, and PC5 are PWM Pins for Generator 3 in PWM Module 0.

            if( GPIOPinNumber == 4 )
            {
                pinAlternateFunctionAddress = GPIO_PC4_M0PWM6;
                pwmGenerator = PWM_GEN_3;
                pwmOut = PWM_OUT_6;
                pwmOutBit = PWM_OUT_6_BIT;
            }

            else if( GPIOPinNumber == 5 )
            {
                pinAlternateFunctionAddress = GPIO_PC5_M0PWM7;
                pwmGenerator = PWM_GEN_3;
                pwmOut = PWM_OUT_7;
                pwmOutBit = PWM_OUT_7_BIT;
            }

            break;

        case 'D':                               // For PORTD, PD0, and PD1 are PWM Pins for Generator 3 in PWM Module 0.

            if( GPIOPinNumber == 0 )
            {
                pinAlternateFunctionAddress = GPIO_PD0_M0PWM6;
                pwmGenerator = PWM_GEN_3;
                pwmOut = PWM_OUT_6;
                pwmOutBit = PWM_OUT_6_BIT;
            }

            else if( GPIOPinNumber == 1 )
            {
                pinAlternateFunctionAddress = GPIO_PD1_M0PWM7;
                pwmGenerator = PWM_GEN_3;
                pwmOut = PWM_OUT_7;
                pwmOutBit = PWM_OUT_7_BIT;
            }

            break;
        default:
            break;
        }

    }
    else if( PWMPeripheralNumber == 1 )         // Implies PWM Peripheral 1 is being used.
    {
        switch (GPIOPortLetter) {
        case 'D':                               // For PORTD, PD0, and PD1 are PWM Pins for Generator 0 in PWM Module 1.

            if( GPIOPinNumber == 0)
            {
                pinAlternateFunctionAddress = GPIO_PD0_M1PWM0;
                pwmGenerator = PWM_GEN_0;
                pwmOut = PWM_OUT_0;
                pwmOutBit = PWM_OUT_0_BIT;
            }

            else if( GPIOPinNumber == 1 )
            {
                pinAlternateFunctionAddress = GPIO_PD1_M1PWM1;
                pwmGenerator = PWM_GEN_0;
                pwmOut = PWM_OUT_1;
                pwmOutBit = PWM_OUT_1_BIT;
            }

            break;
        case 'A':                               // For PORTA, PA6, and PA7 are PWM Pins for Generator 1 in PWM Module 1.

            if( GPIOPinNumber == 6)
            {
                pinAlternateFunctionAddress = GPIO_PA6_M1PWM2;
                pwmGenerator = PWM_GEN_1;
                pwmOut = PWM_OUT_2;
                pwmOutBit = PWM_OUT_2_BIT;
            }

            else if( GPIOPinNumber == 7 )
            {
                pinAlternateFunctionAddress = GPIO_PA7_M1PWM3;
                pwmGenerator = PWM_GEN_1;
                pwmOut = PWM_OUT_3;
                pwmOutBit = PWM_OUT_3_BIT;
            }

            break;

        case 'E':                               // For PORTE, PE4, and PE5 are PWM Pins for Generator 1 in PWM Module 1.

            if( GPIOPinNumber == 4 )
            {
                pinAlternateFunctionAddress = GPIO_PE4_M1PWM2;
                pwmGenerator = PWM_GEN_1;
                pwmOut = PWM_OUT_2;
                pwmOutBit = PWM_OUT_2_BIT;
            }

            else if( GPIOPinNumber == 5 )
            {
                pinAlternateFunctionAddress = GPIO_PE5_M1PWM3;
                pwmGenerator = PWM_GEN_1;
                pwmOut = PWM_OUT_3;
                pwmOutBit = PWM_OUT_3_BIT;
            }

            break;

        case 'F':                               // For PORTF, PF0 (GEN 2), PF1 (GEN 2), PF2 (GEN 3), and PF3 (GEN 2), are PWM Pins for Generator 2 and Generator 3 in PWM Module 1.

            if( GPIOPinNumber == 0 )
            {
                pinAlternateFunctionAddress = GPIO_PF0_M1PWM4;
                pwmGenerator = PWM_GEN_2;
                pwmOut = PWM_OUT_4;
                pwmOutBit = PWM_OUT_4_BIT;
            }

            else if( GPIOPinNumber == 1 )
            {
                pinAlternateFunctionAddress = GPIO_PF1_M1PWM5;
                pwmGenerator = PWM_GEN_2;
                pwmOut = PWM_OUT_5;
                pwmOutBit = PWM_OUT_5_BIT;
            }

            else if( GPIOPinNumber == 2 )
            {
                pinAlternateFunctionAddress = GPIO_PF2_M1PWM6;
                pwmGenerator = PWM_GEN_3;
                pwmOut = PWM_OUT_6;
                pwmOutBit = PWM_OUT_6_BIT;
            }

            else if( GPIOPinNumber == 3 )
            {
                pinAlternateFunctionAddress = GPIO_PF3_M1PWM7;
                pwmGenerator = PWM_GEN_3;
                pwmOut = PWM_OUT_7;
                pwmOutBit = PWM_OUT_7_BIT;
            }

            break;

        default:
            break;
        }
    }

    if( retValType == GET_ALT )     returnVal = pinAlternateFunctionAddress;
    else if( retValType == GET_GEN ) returnVal = pwmGenerator;
    else if( retValType == GET_PWM ) returnVal = pwmOut;
    else if( retValType == GET_PBT ) returnVal = pwmOutBit;

    return returnVal;
}









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

/*
 * Function to get PWM Peripheral Address depending upon PWM Module Peripheral Number.
 * Arguments:
 *  uint8_t PWMPeripheralNumber                 :: PWM Peripheral Number 0, or 1.
 * Returns:
 *  uint32_t address of PWM Peripheral          :: Depending upon passed argument, the appropriate value from array ui32PWMModulePeripheralAddressArray is returned.
 */
static uint32_t getPWMModulePeripheralAddress(uint8_t PWMPeripheralNumber)
{
    return ui32PWMModulePeripheralAddressArray[PWMPeripheralNumber];
}

/*
 * Function to get PWM Base Address depending upon PWM Module Peripheral Number.
 * Arguments:
 *  uint8_t PWMPeripheralNumber                 :: PWM Peripheral Number 0, or 1.
 * Returns:
 *  uint32_t address of PWM Base                :: Depending upon passed argument, the appropriate value from array ui32PWMModuleBaseAddressArray is returned.
 */
static uint32_t getPWMModuleBaseAddress(uint8_t PWMPeripheralNumber)
{
    return ui32PWMModuleBaseAddressArray[PWMPeripheralNumber];
}




/*
 * Function to get Pin Address depending upon GPIO Pin number respective to Port.
 * Arguments:
 *  uint8_t GPIOPinNumber                       :: GPIO Pin Number respective to Port, i.e. 0, 1, 2, 3, 4, 5, 6, or 7.
 * Returns:
 *  uint8_t GPIO Pin Address                    :: Depending upon passed argument, the appropriate value from array ui32GPIOPinAddressArray is returned.
 */
static uint8_t getGPIOPinAddress(uint8_t GPIOPinNumber)
{
    return ui8GPIOPinAddressArray[GPIOPinNumber];
}
