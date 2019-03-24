#include "ADC.h"


//***********ADC Sauce Data **********//


static uint8_t sequencerPriorityDistributor = 0;




/*
 * There are two ADC Modules in TM4C123GH6PM. ADC0 and ADC1.
 * There are 4 Sequencers.
 * These Sequencers have depth of 1 (SS3), 4 (SS2 and SS1), or 8 (SS0). Each of these can be given a priority with 0 being highest priority.
 * The Following Information is needed to get ADC data:
 *  1. The ADC Module's PERIPHERAL address.SYSCTL_PERIPH_ADCX As it is only needed one time, no need to store it in Struct.
 *  2. The Base Address of ADC Module. Will be used to trigger ADC so save in Struct.
 *  3. The Sequencer to be used. Will be used in ADC so save in Struct.
 *  4. The trigger type. Will not need to save in Struct.
 *  5. Priority of Sequencer for that module. Will not be needed to save in struct.
 *  6. Based on Sequencer used, ADC Channel to be used. Can be mapped via Array so need to store in struct.
 *  7. Array of Length according to Sequencer used. Will be used to get Data. So store in struct.
 */



//********** ADC Fucntions **********//
//**********************************//


//public non-static extern Functions.

/*
 * Setup ADCDEVICE struct to use as ADC Block.
 * Arguments:
 *  ADCDEVICE* ADCDEVICEPointer                 :: Pointer to ADCDEVICE struct.
 *  ADC_PERIPHERAL ADCPeripheralNumber          :: ADC Peripheral Number ADC0 or ADC1.
 *  ADC_SEQUENCER ADCSequencerNumber            :: ADC Sequencer Number, ADC_SEQX, X is [0,3].
 *  ADC_TRIGGER_TYPE ADCTriggerType             :: ADC Trigger Type.
 *                                                 initADCDEVICE sets up the device for ADC_TRIGGER_PROCESSOR
 *  uint8_t ADCPinCount                         :: Number of pins in ADC Block, must be lower than or equal to the
 *                                                 maximum depth of selected ADCSequencer.
 *  ADC_PINS *ADCChannelList                    :: Pointer (Array Decayed) to Pins to be used as ADC Pins.
 *                                                 The length of this array should be equal to ADCPinCount.
 *                                                 Must make sure to have them in order of priority.
 *                                                 Also, the sequencer should be deep enough to support all the pins.
 *  uint32_t *ADCDEVICEDataArrayPointer         :: Pointer to Data Array in which ADC Captured Data is to be stored.
 *                                                 The length of this array should be equal to ADCPinCount.
 * Returns:
 *  none.
 */
void initADCDEVICE(  ADCDEVICE* ADCDEVICEPointer,       ADC_PERIPHERAL ADCPeripheralNumber,
                     ADC_SEQUENCER ADCSequencerNumber,  ADC_TRIGGER_TYPE ADCTriggerType,
                     uint8_t ADCPinCount,               ADC_PINS* ADCChannelList,
                     uint32_t* ADCDEVICEDataArrayPointer)
{
    ADCDEVICEPointer->ADCBase = getADCBaseAddress(ADCPeripheralNumber);
    ADCDEVICEPointer->ADCSequenceNumber = (uint8_t)ADCSequencerNumber;
    ADCDEVICEPointer->ADCDEVICEDataArrayPointer = ADCDEVICEDataArrayPointer;
    uint8_t looper = 0;
    uint32_t ADCPinsPeripheralAddress = 0, ADCPinsBaseAddress = 0 ;
    uint8_t ADCPINNumber = 0 ;


    SysCtlPeripheralEnable(getADCPeripheralAddress(ADCPeripheralNumber));
    while(!SysCtlPeripheralReady(getADCPeripheralAddress(ADCPeripheralNumber)));
    for (looper = 0; looper < ADCPinCount; ++ looper)
    {
        getADCPinsData(ADCChannelList[looper], &ADCPinsPeripheralAddress, &ADCPinsBaseAddress, &ADCPINNumber) ;
        if(!SysCtlPeripheralReady(ADCPinsPeripheralAddress))
        {
            SysCtlPeripheralEnable(ADCPinsPeripheralAddress) ;
            while(!SysCtlPeripheralReady(ADCPinsPeripheralAddress));
        }
        GPIOPinTypeADC(ADCPinsBaseAddress, ADCPINNumber) ;
    }

    ADCSequenceConfigure(ADCDEVICEPointer->ADCBase, ADCDEVICEPointer->ADCSequenceNumber, ADCTriggerType, dispatchADCSequencerPriority());

    ADCIntClear(ADCDEVICEPointer->ADCBase,ADCDEVICEPointer->ADCSequenceNumber);

    for(looper=0; looper<ADCPinCount-1; ++looper)
    {
        ADCSequenceStepConfigure(ADCDEVICEPointer->ADCBase, ADCDEVICEPointer->ADCSequenceNumber, looper, ADCChannelList[looper]);
    }

    ADCSequenceStepConfigure(ADCDEVICEPointer->ADCBase, ADCDEVICEPointer->ADCSequenceNumber, looper, (ADC_CTL_IE | ADC_CTL_END | ADCChannelList[looper]) );

    ADCSequenceEnable(ADCDEVICEPointer->ADCBase,ADCDEVICEPointer->ADCSequenceNumber);

    ADCIntClear(ADCDEVICEPointer->ADCBase, ADCDEVICEPointer->ADCSequenceNumber);

}

/*
 * Function to read ADC Block.
 * Arguments:
 *  ADCDEVICE* ADCBlockPointer                    :: Pointer to struct of ADC Block. Data will be stored with help of the struct's uint32_t *ADCDEVICEDataArrayPointer member.
 * Returns:
 * none.
 */
void analogRead(ADCDEVICE* ADCBlockPointer)
{
    ADCProcessorTrigger(ADCBlockPointer->ADCBase, ADCBlockPointer->ADCSequenceNumber);
    while(!ADCIntStatus(ADCBlockPointer->ADCBase, ADCBlockPointer->ADCSequenceNumber, false));
    ADCSequenceDataGet(ADCBlockPointer->ADCBase, ADCBlockPointer->ADCSequenceNumber, ADCBlockPointer->ADCDEVICEDataArrayPointer);
}

/*
 * Read a single pin in ADC Block.
 * Arguments:
 *  ADCDEVICE* ADCBlockPointer                    :: Pointer to struct of ADC Block.
 *  uint8_t pinNum                              :: Pin Number according to ADC Block's Data Array.
 * Returns:
 *  uint32_t analog Pin's Value                 :: Value corresponding to read ADC Value at particular pin.
 */
uint32_t analogReadPin(ADCDEVICE* ADCBlockPointer,uint8_t pinNum)
{
    ADCProcessorTrigger(ADCBlockPointer->ADCBase,ADCBlockPointer->ADCSequenceNumber);
    while(!ADCIntStatus(ADCBlockPointer->ADCBase,ADCBlockPointer->ADCSequenceNumber,false));
    ADCSequenceDataGet(ADCBlockPointer->ADCBase,ADCBlockPointer->ADCSequenceNumber,ADCBlockPointer->ADCDEVICEDataArrayPointer);
    return ADCBlockPointer->ADCDEVICEDataArrayPointer[pinNum];
}


/*
 * Read a single Pin scaled to a range.
 * Arguments:
 *  ADCDEVICE* ADCBlockPointer                    :: Pointer to struct of ADC Block.
 *  uint8_t pinNum                              :: Pin Number according to ADC Block's Data Array.
 *  uint16_t minVal                             :: Lower Limit of range.
 *  uint32_t maxVal                             :: Upper Limit of range.
 * Returns:
 *  uint32_t scaled analog Pin's Value          :: Value corresponding to read ADC Value at particular pin.
 */
uint32_t analogReadPinScaled(ADCDEVICE* ADCBlockPointer, uint8_t pinNum, uint16_t minVal, uint32_t maxVal)
{
    ADCProcessorTrigger(ADCBlockPointer->ADCBase,ADCBlockPointer->ADCSequenceNumber);
    while(!ADCIntStatus(ADCBlockPointer->ADCBase,ADCBlockPointer->ADCSequenceNumber,false));
    ADCSequenceDataGet(ADCBlockPointer->ADCBase,ADCBlockPointer->ADCSequenceNumber,ADCBlockPointer->ADCDEVICEDataArrayPointer);
    return ((ADCBlockPointer->ADCDEVICEDataArrayPointer[pinNum]-minVal)*(maxVal-minVal))/4096 + minVal;
}




/*
 * Function to return ADC Base according to ADC Peripheral Number.
 * Arguments:
 *  ADC_PERIPHERAL ADCPeripheralNumber          :: ADC Peripheral Number ADC0, or ADC1.
 * Returns:
 *  uint32_t ADCBaseAddress                     :: Based on ADC Peripheral Number, ADC0_BASE, or ADC1_BASE is returned.
 */
static uint32_t getADCBaseAddress(ADC_PERIPHERAL ADCPeripheralNumber)
{
    return ui32ADCBaseAddressArray[(uint8_t)ADCPeripheralNumber];
}


/*
 * Function to return ADC Peripheral Address according to ADC Peripheral Number.
 * Arguments:
 *  ADC_PERIPHERAL ADCPeripheralNumber          :: ADC Peripheral Number ADC0, or ADC1.
 * Returns:
 *  uint32_t ADCPeripheralAddress               :: Based on ADC Peripheral Number, SYSCTL_PERIPH_ADC0, or SYSCTL_PERIPH_ADC0 is returned.
 */
static uint32_t getADCPeripheralAddress(ADC_PERIPHERAL ADCPeripheralNumber)
{
    return ui32ADCPeripheralAddressArray[(uint8_t)ADCPeripheralNumber];
}

/*
 * Function to get ADC Pins Peripheral, Base and Pin values for enabling them.
 * Arguments:
 *  ADC_PINS ADCPinValue                        :: Value of ADC PIN in form AINx_Pxx
 *  uint32_t* ADCPeripheralVariablePointer              :: Pointer to variable to store ADC Peripheral address.
 *  uint32_t* ADCBaseVariablePointer                    :: Pointer to variable to store ADC Base address.
 *  uint8_t* ADCPinVariableAddress                      :: Pointer to variable to store ADC Pin address.
 */
static void getADCPinsData(ADC_PINS ADCPinValue, uint32_t* ADCPeripheralVariablePointer, uint32_t* ADCBaseVariablePointer, uint8_t* ADCPinVariableAddress )
{
    switch (ADCPinValue) {
        case AIN0_PE3:
            *ADCPeripheralVariablePointer = SYSCTL_PERIPH_GPIOE ;
            *ADCBaseVariablePointer = GPIO_PORTE_BASE ;
            *ADCPinVariableAddress = GPIO_PIN_3 ;
            break;
        case AIN1_PE2:
            *ADCPeripheralVariablePointer = SYSCTL_PERIPH_GPIOE ;
            *ADCBaseVariablePointer = GPIO_PORTE_BASE ;
            *ADCPinVariableAddress = GPIO_PIN_2 ;
            break;
        case AIN2_PE1:
            *ADCPeripheralVariablePointer = SYSCTL_PERIPH_GPIOE ;
            *ADCBaseVariablePointer = GPIO_PORTE_BASE ;
            *ADCPinVariableAddress = GPIO_PIN_1 ;
            break;
        case AIN3_PE0:
            *ADCPeripheralVariablePointer = SYSCTL_PERIPH_GPIOE ;
            *ADCBaseVariablePointer = GPIO_PORTE_BASE ;
            *ADCPinVariableAddress = GPIO_PIN_0 ;
            break;
        case AIN4_PD3:
            *ADCPeripheralVariablePointer = SYSCTL_PERIPH_GPIOD ;
            *ADCBaseVariablePointer = GPIO_PORTD_BASE ;
            *ADCPinVariableAddress = GPIO_PIN_3 ;
            break;
        case AIN5_PD2:
            *ADCPeripheralVariablePointer = SYSCTL_PERIPH_GPIOD ;
            *ADCBaseVariablePointer = GPIO_PORTD_BASE ;
            *ADCPinVariableAddress = GPIO_PIN_2 ;
            break;
        case AIN6_PD1:
            *ADCPeripheralVariablePointer = SYSCTL_PERIPH_GPIOD ;
            *ADCBaseVariablePointer = GPIO_PORTD_BASE ;
            *ADCPinVariableAddress = GPIO_PIN_1 ;
            break;
        case AIN7_PD0:
            *ADCPeripheralVariablePointer = SYSCTL_PERIPH_GPIOD ;
            *ADCBaseVariablePointer = GPIO_PORTD_BASE ;
            *ADCPinVariableAddress = GPIO_PIN_0 ;
            break;
        case AIN8_PE5:
            *ADCPeripheralVariablePointer = SYSCTL_PERIPH_GPIOE ;
            *ADCBaseVariablePointer = GPIO_PORTE_BASE ;
            *ADCPinVariableAddress = GPIO_PIN_5 ;
            break;
        case AIN9_PE4:
            *ADCPeripheralVariablePointer = SYSCTL_PERIPH_GPIOE ;
            *ADCBaseVariablePointer = GPIO_PORTE_BASE ;
            *ADCPinVariableAddress = GPIO_PIN_4 ;
            break;
        case AIN10_PB4:
            *ADCPeripheralVariablePointer = SYSCTL_PERIPH_GPIOB ;
            *ADCBaseVariablePointer = GPIO_PORTB_BASE ;
            *ADCPinVariableAddress = GPIO_PIN_4 ;
            break;
        case AIN11_PB5:
            *ADCPeripheralVariablePointer = SYSCTL_PERIPH_GPIOB ;
            *ADCBaseVariablePointer = GPIO_PORTB_BASE ;
            *ADCPinVariableAddress = GPIO_PIN_5 ;
            break;
        default:
            break;
    }
}

/*
 * Function to provide Sequencer Priority to ADC Blocks. FIFO service according to sequence of initialization.
 * Arguments:
 *  none.
 * Returns:
 *  uint8_t presentSequencerPriority            :: priority available for the present sequencer.
 */
static uint8_t dispatchADCSequencerPriority()
{
    uint8_t presentSequencerPriority = 0;
    presentSequencerPriority = sequencerPriorityDistributor;
    sequencerPriorityDistributor = sequencerPriorityDistributor + 1;
    return presentSequencerPriority;
}
