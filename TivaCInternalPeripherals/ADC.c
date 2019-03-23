#include "ADC.h"


//***********ADC Sauce Data **********//


static uint8_t sequencerPriorityDistributor = 0;


static const uint32_t ui32ADCBaseAddressArray[2] = {
        ADC0_BASE, ADC1_BASE
};

static const uint32_t ui32ADCPeripheralAddressArray[2] = {
        SYSCTL_PERIPH_ADC0, SYSCTL_PERIPH_ADC1
};

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
 * Create and return a Struct Pointer to ADCDATA struct to use as ADC Block.
 * Arguments:
 *  ADCData
 *  uint8_t ADCPeripheralNumber                 :: ADC Peripheral Number 0 or 1.
 *  uint32_t ADCSequencerNumber                 :: ADC Sequencer Number, either 0, 1, 2, or 3.
 *  uint32_t ADCTriggerType                     :: ADC Trigger Type. By default macro it is Processor Trigger.
 *  uint32_t *ADCChannelList                    :: Pointer (Array Decayed) to Pins to be used as ADC Pins. Must make sure to have them in order of priority as well as make
 *                                                  sure the length of passed array corresponds to depth of Sequencer selected.
 *  uint32_t *ADCDataArrayPointer               :: Pointer to Data Array in which ADC Captured Data is to be stored.
 * Returns:
 *  ADCDATA* ADCDataBlockPointer                :: Pointer to ADCDATA struct
 *
 */
ADCData* initADCBlock(  ADCData* ADCBlockPointer,       uint32_t ADCPeripheralNumber,
                        uint32_t ADCSequencerNumber,    uint32_t ADCTriggerType,
                        uint32_t* ADCChannelList,       uint32_t* ADCDatatArray)
{
    ADCBlockPointer->ADCBase = getADCBaseAddress(ADCPeripheralNumber);
    ADCBlockPointer->ADCSequenceNumber = ADCSequencerNumber;
    ADCBlockPointer->ADCDataArrayPointer = ADCDatatArray;
    uint8_t looper = 0;
    uint8_t ADCDataArraySize = 0;
    ADCDataArraySize = getADCDataArraySize(ADCSequencerNumber);


    SysCtlPeripheralEnable(getADCPeripheralAddress(ADCPeripheralNumber));
    SysCtlDelay(10);

    ADCSequenceConfigure(ADCBlockPointer->ADCBase, ADCBlockPointer->ADCSequenceNumber, ADCTriggerType, dispatchADCSequencerPriority());

    ADCIntClear(ADCBlockPointer->ADCBase,ADCBlockPointer->ADCSequenceNumber);

    for(looper=0; looper<ADCDataArraySize-1; ++looper)
    {
        ADCSequenceStepConfigure(ADCBlockPointer->ADCBase, ADCBlockPointer->ADCSequenceNumber, looper, ADCChannelList[looper]);
    }

    ADCSequenceStepConfigure(ADCBlockPointer->ADCBase, ADCBlockPointer->ADCSequenceNumber, looper, (ADC_CTL_IE | ADC_CTL_END | ADCChannelList[looper]) );

    ADCSequenceEnable(ADCBlockPointer->ADCBase,ADCBlockPointer->ADCSequenceNumber);

    ADCIntClear(ADCBlockPointer->ADCBase, ADCBlockPointer->ADCSequenceNumber);

    return ADCBlockPointer;
}

/*
 * Function to read ADC Block.
 * Arguments:
 *  ADCData* ADCBlockPointer                    :: Pointer to struct of ADC Block. Data will be stored with help of the struct's uint32_t *ADCDataArrayPointer member.
 * Returns:
 * none.
 */
void analogRead(ADCData* ADCBlockPointer)
{
    ADCProcessorTrigger(ADCBlockPointer->ADCBase, ADCBlockPointer->ADCSequenceNumber);
    while(!ADCIntStatus(ADCBlockPointer->ADCBase, ADCBlockPointer->ADCSequenceNumber, false));
    ADCSequenceDataGet(ADCBlockPointer->ADCBase, ADCBlockPointer->ADCSequenceNumber, ADCBlockPointer->ADCDataArrayPointer);
}

/*
 * Read a single pin in ADC Block.
 * Arguments:
 *  ADCData* ADCBlockPointer                    :: Pointer to struct of ADC Block.
 *  uint8_t pinNum                              :: Pin Number according to ADC Block's Data Array.
 * Returns:
 *  uint32_t analog Pin's Value                 :: Value corresponding to read ADC Value at particular pin.
 */
uint32_t analogReadPin(ADCData* ADCBlockPointer,uint8_t pinNum)
{
    ADCProcessorTrigger(ADCBlockPointer->ADCBase,ADCBlockPointer->ADCSequenceNumber);
    while(!ADCIntStatus(ADCBlockPointer->ADCBase,ADCBlockPointer->ADCSequenceNumber,false));
    ADCSequenceDataGet(ADCBlockPointer->ADCBase,ADCBlockPointer->ADCSequenceNumber,ADCBlockPointer->ADCDataArrayPointer);
    return ADCBlockPointer->ADCDataArrayPointer[pinNum];
}


/*
 * Read a single Pin scaled to a range.
 * Arguments:
 *  ADCData* ADCBlockPointer                    :: Pointer to struct of ADC Block.
 *  uint8_t pinNum                              :: Pin Number according to ADC Block's Data Array.
 *  uint16_t minVal                             :: Lower Limit of range.
 *  uint32_t maxVal                             :: Upper Limit of range.
 * Returns:
 *  uint32_t scaled analog Pin's Value          :: Value corresponding to read ADC Value at particular pin.
 */
uint32_t analogReadPinScaled(ADCData* ADCBlockPointer, uint8_t pinNum, uint16_t minVal, uint32_t maxVal)
{
    ADCProcessorTrigger(ADCBlockPointer->ADCBase,ADCBlockPointer->ADCSequenceNumber);
    while(!ADCIntStatus(ADCBlockPointer->ADCBase,ADCBlockPointer->ADCSequenceNumber,false));
    ADCSequenceDataGet(ADCBlockPointer->ADCBase,ADCBlockPointer->ADCSequenceNumber,ADCBlockPointer->ADCDataArrayPointer);
    return ((ADCBlockPointer->ADCDataArrayPointer[pinNum]-minVal)*(maxVal-minVal))/4096 + minVal;
}




/*
 * Function to return ADC Base according to ADC Peripheral Number.
 * Arguments:
 *  uint8_t ADCPeripheralNumber                 :: ADC Peripheral Number 0, or 1.
 * Returns:
 *  uint32_t ADCBaseAddress                     :: Based on ADC Peripheral Number, ADC0_BASE, or ADC1_BASE is returned.
 */
static uint32_t getADCBaseAddress(uint8_t ADCPeripheralNumber)
{
    return ui32ADCBaseAddressArray[ADCPeripheralNumber];
}


/*
 * Function to return ADC Peripheral Address according to ADC Peripheral Number.
 * Arguments:
 *  uint8_t ADCPeripheralNumber                 :: ADC Peripheral Number 0, or 1.
 * Returns:
 *  uint32_t ADCPeripheralAddress               :: Based on ADC Peripheral Number, SYSCTL_PERIPH_ADC0, or SYSCTL_PERIPH_ADC0 is returned.
 */
static uint32_t getADCPeripheralAddress(uint8_t ADCPeripheralNumber)
{
    return ui32ADCPeripheralAddressArray[ADCPeripheralNumber];
}


/*
 * Function to return ADCBlock's data array size according to ADC Sequence Number.
 * Arguments:
 *  uint32_t ADCSequencerNumber                 :: Sequencer Number Selected, 0, 1, 2, or 3.
 * Returns:
 *  uint8_t ADC Data Array Size                 :: Size of ADC Data Array Size for ADC Sequencer.
 */
static uint8_t getADCDataArraySize(uint32_t ADCSequencerNumber)
{
    uint8_t ADCDataArraySize = 0;
    switch(ADCSequencerNumber)
    {
    case 0:
        ADCDataArraySize = 8;
        break;
    case 1:
    case 2:
        ADCDataArraySize = 4;
        break;
    case 3:
    default:
        ADCDataArraySize = 1;
        break;
    }

    return ADCDataArraySize;

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
