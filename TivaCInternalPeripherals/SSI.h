/*
 * SSI.h
 * File Description:
 * File to hold SSI Peripheral related functions.
 */


#ifndef SSI_H_
#define SSI_H_

#include <stdint.h>
#include <stdbool.h>


#include "inc/hw_memmap.h"

#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "inc/hw_types.h"
#include "inc/hw_ssi.h"
#include "driverlib/ssi.h"
//#include "ss_PeripheralNames.h"
#include "PERIPHERALS.h"
/*
 * Pin Values
 *
╔══════════════╦═════════════╦═══════════╦══════════╦═════════╗
║  SSI Module  ║  CLOCK PIN  ║  FSS PIN  ║  RX PIN  ║  TX PIN ║
╠══════════════╬═════════════╬═══════════╬══════════╬═════════╣
║    SSI0      ║     PA2     ║    PA3    ║    PA4   ║    PA5  ║
║    SSI1(F)   ║     PF2     ║    PF3    ║    PF0   ║    PF1  ║
║    SSI1(D)   ║     PD0     ║    PD1    ║    PD2   ║    PD3  ║
║    SSI2      ║     PB4     ║    PB5    ║    PB6   ║    PB7  ║
║    SSI3      ║     PD0     ║    PD1    ║    PD2   ║    PD3  ║
╚══════════════╩═════════════╩═══════════╩══════════╩═════════╝
 */



typedef enum SSI_FRAM_FORMAT {
    SSI_FRAME_FORMAT_CPOL0_CPHA0 = SSI_FRF_MOTO_MODE_0,
    SSI_FRAME_FORMAT_CPOL0_CPHA1 = SSI_FRF_MOTO_MODE_1,
    SSI_FRAME_FORMAT_CPOL1_CPHA0 = SSI_FRF_MOTO_MODE_2,
    SSI_FRAME_FORMAT_CPOL1_CPHA1 = SSI_FRF_MOTO_MODE_3
}SSI_FRAME_FORMAT;
typedef enum FSS_RX_PIN_USAGE {
    SET_FSS_PIN     = true,
    LEAVE_FSS_PIN   = false,
    SET_SSIRX_PIN   = true,
    LEAVE_SSIRX_PIN = false
}FSS_RX_PIN_USAGE;
typedef struct SSIDEVICE{
    uint32_t SSIBase;
    uint32_t receiverData;
}SSIDEVICE;

//public non-static extern Functions:



extern void initSSIDEVICE(    SSIDEVICE* SSIDevicePointer,
                                     SSI_PERIPHERAL SSIPeripheralCode ,
                                     uint32_t SSIClockFrequency,
                                     uint32_t SSIDataWidth, SSI_FRAME_FORMAT frameFormat,
                                     FSS_RX_PIN_USAGE FSSPinUse, FSS_RX_PIN_USAGE SSIRxPinUse) ;

extern void SSIWriteValue(SSIDEVICE* SSIDevicePointer, uint32_t value);


extern uint32_t SSIReadValue(SSIDEVICE* SSIDevicePointer);


extern void SSISetDataWidth(SSIDEVICE* SSIDevicePointer, uint8_t dataWidth) ;



extern uint8_t SSIReadDataWidth(SSIDEVICE* SSIDevicePointer) ;
//private static non-extern Functions:

static void getSSISauce(SSI_PERIPHERAL SSIPeripheralCode,
                             uint32_t* SSIPeripheralAddressVariablePointer,
                             uint32_t* SSIBaseAddressVariablePointer,
                             uint32_t* SSIGPIOPeripheralAddressVariablePointer,
                             uint32_t* SSIGPIOBaseAddressVariablePointer,
                             uint32_t* SSIClockPinAlternateAddressVariablePointer,
                             uint32_t* SSIFSSPinAlternateAddressVariablePointer,
                             uint32_t* SSIRxPinAlternateAddressVariablePointer,
                             uint32_t* SSITxPinAlternateAddressVariablePointer,
                             uint8_t* SSIGPIOPinValuesVariablePointer,
                             FSS_RX_PIN_USAGE SSIFSSPinUse,
                             FSS_RX_PIN_USAGE SSIRxPinUse) ;



#endif
