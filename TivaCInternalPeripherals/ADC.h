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



/*
 * Macros Definition
 */

//List of ADC Pins

#define PE3 ADC_CTL_CH0
#define PE2 ADC_CTL_CH1
#define PE1 ADC_CTL_CH2
#define PE0 ADC_CTL_CH3
#define PD3 ADC_CTL_CH4
#define PD2 ADC_CTL_CH5
#define PD1 ADC_CTL_CH6
#define PD0 ADC_CTL_CH7
#define PE5 ADC_CTL_CH8
#define PE4 ADC_CTL_CH9
#define PB4 ADC_CTL_CH10
#define PB5 ADC_CTL_CH11


/*
 * ADC data block Struct Definition.
 */
typedef struct ADCData{
    uint32_t ADCBase;
    uint32_t ADCSequenceNumber;
    uint32_t *ADCDataArrayPointer;
}ADCData;


//********** ADC Functions **********//
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
                        uint32_t* ADCChannelList,       uint32_t* ADCDatatArray);


/*
 * Function to read ADC Block.
 * Arguments:
 *  ADCData* ADCBlockPointer                    :: Pointer to struct of ADC Block. Data will be stored with help of the struct's uint32_t *ADCDataArrayPointer member.
 * Returns:
 * none.
 */
void analogRead(ADCData* ADCBlockPointer);

/*
 * Read a single pin in ADC Block.
 * Arguments:
 *  ADCData* ADCBlockPointer                    :: Pointer to struct of ADC Block.
 *  uint8_t pinNum                              :: Pin Number according to ADC Block's Data Array.
 * Returns:
 *  uint32_t analog Pin's Value                 :: Value corresponding to read ADC Value at particular pin.
 */
uint32_t analogReadPin(ADCData* ADCBlockPointer,uint8_t pinNum);

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
uint32_t analogReadPinScaled(ADCData* ADCBlockPointer, uint8_t pinNum, uint16_t minVal, uint32_t maxVal);




//private static non-extern Functions.


/*
 * Function to return ADC Base according to ADC Peripheral Number.
 * Arguments:
 *  uint8_t ADCPeripheralNumber                 :: ADC Peripheral Number 0, or 1.
 * Returns:
 *  uint32_t ADCBaseAddress                     :: Based on ADC Peripheral Number, ADC0_BASE, or ADC1_BASE is returned.
 */
static uint32_t getADCBaseAddress(uint8_t ADCPeripheralNumber);



/*
 * Function to return ADC Peripheral Address according to ADC Peripheral Number.
 * Arguments:
 *  uint8_t ADCPeripheralNumber                 :: ADC Peripheral Number 0, or 1.
 * Returns:
 *  uint32_t ADCPeripheralAddress               :: Based on ADC Peripheral Number, SYSCTL_PERIPH_ADC0, or SYSCTL_PERIPH_ADC0 is returned.
 */
static uint32_t getADCPeripheralAddress(uint8_t ADCPeripheralNumber);


/*
 * Function to return ADCBlock's data array size according to ADC Sequence Number.
 * Arguments:
 *  uint32_t ADCSequencerNumber                 :: Sequencer Number Selected, 0, 1, 2, or 3.
 * Returns:
 *  uint8_t ADC Data Array Size                 :: Size of ADC Data Array Size for ADC Sequencer.
 */
static uint8_t getADCDataArraySize(uint32_t ADCSequencerNumber);


/*
 * Function to provide Sequencer Priority to ADC Blocks. FIFO service according to sequence of initialization.
 * Arguments:
 *  none.
 * Returns:
 *  uint8_t presentSequencerPriority            :: priority available for the present sequencer.
 */
static uint8_t dispatchADCSequencerPriority();

#endif
