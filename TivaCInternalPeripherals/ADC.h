/*
 * ADC.h
 * File Description:
 * File to hold ADC related functions.
 */

#ifndef ADC_H_
#define ADC_H_
/*
 * Included .h files.
 */
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/adc.h"
#include "PERIPHERALS.h"


/*
 * Macros Definition
 */

//List of ADC Pins



/*
 * ADC data block Struct Definition.
 */
typedef struct ADCDEVICE{
    uint32_t ADCBase;
    uint8_t ADCSequenceNumber;
    uint32_t *ADCDEVICEDataArrayPointer;
}ADCDEVICE;


//********** ADC Functions **********//
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
 *                                                 Must make sure to have them in order of priority.
 *                                                 Also, the sequencer should be deep enough to support all the pins.
 *  uint32_t *ADCDEVICEDataArrayPointer         :: Pointer to Data Array in which ADC Captured Data is to be stored.
 * Returns:
 *  none.
 */
void initADCDEVICE(  ADCDEVICE* ADCDEVICEPointer,       ADC_PERIPHERAL ADCPeripheralNumber,
                     ADC_SEQUENCER ADCSequencerNumber,  ADC_TRIGGER_TYPE ADCTriggerType,
                     uint8_t ADCPinCount,               ADC_PINS* ADCChannelList,
                     uint32_t* ADCDEVICEDataArrayPointer);


/*
 * Function to read ADC Block.
 * Arguments:
 *  ADCDEVICE* ADCDEVICEPointer                    :: Pointer to struct of ADC Block.
 *                                                    Data will be stored with help of the struct's
 *                                                    uint32_t *ADCDEVICEDataArrayPointer member.
 * Returns:
 * none.
 */
void analogRead(ADCDEVICE* ADCDEVICEPointer);

/*
 * Read a single pin in ADC Block.
 * Arguments:
 *  ADCDEVICE* ADCDEVICEPointer                    :: Pointer to struct of ADC Block.
 *  uint8_t pinNum                              :: Pin Number according to ADC Block's Data Array.
 * Returns:
 *  uint32_t analog Pin's Value                 :: Value corresponding to read ADC Value at particular pin.
 */
uint32_t analogReadPin(ADCDEVICE* ADCDEVICEPointer,uint8_t pinNum);

/*
 * Read a single Pin scaled to a range.
 * Arguments:
 *  ADCDEVICE* ADCDEVICEPointer                    :: Pointer to struct of ADC Block.
 *  uint8_t pinNum                              :: Pin Number according to ADC Block's Data Array.
 *  uint16_t minVal                             :: Lower Limit of range.
 *  uint32_t maxVal                             :: Upper Limit of range.
 * Returns:
 *  uint32_t scaled analog Pin's Value          :: Value corresponding to read ADC Value at particular pin.
 */
uint32_t analogReadPinScaled(ADCDEVICE* ADCDEVICEPointer, uint8_t pinNum, uint16_t minVal, uint32_t maxVal);




//private static non-extern Functions.


/*
 * Function to return ADC Base according to ADC Peripheral Number.
 * Arguments:
 *  ADC_PERIPHERAL ADCPeripheralNumber                 :: ADC Peripheral Number ADC0, or ADC1.
 * Returns:
 *  uint32_t ADCBaseAddress                     :: Based on ADC Peripheral Number, ADC0_BASE, or ADC1_BASE is returned.
 */
static uint32_t getADCBaseAddress(ADC_PERIPHERAL ADCPeripheralNumber);



/*
 * Function to return ADC Peripheral Address according to ADC Peripheral Number.
 * Arguments:
 *  ADC_PERIPHERAL ADCPeripheralNumber          :: ADC Peripheral Number ADC0, or ADC1.
 * Returns:
 *  uint32_t ADCPeripheralAddress               :: Based on ADC Peripheral Number, SYSCTL_PERIPH_ADC0, or SYSCTL_PERIPH_ADC0 is returned.
 */
static uint32_t getADCPeripheralAddress(ADC_PERIPHERAL ADCPeripheralNumber);


/*
 * Function to get ADC Pins Peripheral, Base and Pin values for enabling them.
 * Arguments:
 *  ADC_PINS ADCPinValue                        :: Value of ADC PIN in form AINx_Pxx
 *  uint32_t* ADCPeripheralVariablePointer              :: Pointer to variable to store ADC Peripheral address.
 *  uint32_t* ADCBaseVariablePointer                    :: Pointer to variable to store ADC Base address.
 *  uint8_t* ADCPinVariableAddress                      :: Pointer to variable to store ADC Pin address.
 */
static void getADCPinsData(ADC_PINS ADCPinValue, uint32_t* ADCPeripheralVariablePointer, uint32_t* ADCBaseVariablePointer, uint8_t* ADCPinVariableAddress ) ;

/*
 * Function to provide Sequencer Priority to ADC Blocks. FIFO service according to sequence of initialization.
 * Arguments:
 *  none.
 * Returns:
 *  uint8_t presentSequencerPriority            :: priority available for the present sequencer.
 */
static uint8_t dispatchADCSequencerPriority();

#endif
