#include "SSI.h"


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

static const uint32_t ui32SSIPeripheralAddressArray[4] = {
                                                          SYSCTL_PERIPH_SSI0, SYSCTL_PERIPH_SSI1,
                                                          SYSCTL_PERIPH_SSI2, SYSCTL_PERIPH_SSI3
} ;

static const uint32_t ui32SSIBaseAddressArray[4] = {
                                                    SSI0_BASE, SSI1_BASE,
                                                    SSI2_BASE, SSI3_BASE
} ;




extern SSIDEVICE* initSSIDEVICE(    SSIDEVICE* SSIDevicePointer,
                                    SSI_PERIPHERAL SSIPeripheralCode ,
                                    uint32_t SSIClockFrequency,
                                    uint32_t SSIDataWidth, SSI_FRAME_FORMAT frameFormat,
                                    FSS_RX_PIN_USAGE FSSPinUse, FSS_RX_PIN_USAGE SSIRXPinUse)
{
    uint8_t SSIPeripheralNumber = getSSIPeripheralNumber(SSIPeripheralCode) ;
    char SSIGPIOPortLetter = getSSIGPIOPortLetter(SSIPeripheralCode) ;
    SSIDevicePointer->SSIBase = getSSIBaseAddress(SSIPeripheralNumber) ;
    SysCtlPeripheralEnable(getSSIPeripheralAddress(SSIPeripheralNumber)) ;
    SysCtlPeripheralEnable(getSSIGPIOPeripheralAddress(SSIGPIOPortLetter)) ;

    GPIOPinConfigure(getSSISauce(SSIPeripheralNumber, SSIGPIOPortLetter,FSSPinUse, SSIRXPinUse, GET_CLK)) ;
    if(FSSPinUse)   GPIOPinConfigure(getSSISauce(SSIPeripheralNumber,SSIGPIOPortLetter,FSSPinUse, SSIRXPinUse, GET_FSS)) ;
    if(SSIRXPinUse) GPIOPinConfigure(getSSISauce(SSIPeripheralNumber,SSIGPIOPortLetter,FSSPinUse, SSIRXPinUse, GET_SRX)) ;
    GPIOPinConfigure(getSSISauce(SSIPeripheralNumber,SSIGPIOPortLetter,FSSPinUse, SSIRXPinUse, GET_STX)) ;

    GPIOPinTypeSSI(getSSIGPIOBaseAddress(SSIGPIOPortLetter), (uint8_t)getSSISauce(SSIPeripheralNumber,SSIGPIOPortLetter,FSSPinUse, SSIRXPinUse, GET_PTS)) ;

    SSIConfigSetExpClk(SSIDevicePointer->SSIBase,SysCtlClockGet(),frameFormat,SSI_MODE_MASTER,SSIClockFrequency,SSIDataWidth) ;

    SSIEnable(SSIDevicePointer->SSIBase) ;


    while(SSIDataGetNonBlocking(SSIDevicePointer->SSIBase, &SSIDevicePointer->receiverData)) ;
    SSIDevicePointer->receiverData = 0 ;

    return SSIDevicePointer ;

}

static uint32_t getSSIPeripheralNumber(SSI_PERIPHERAL SSIPeripheralCode)
{
    switch (SSIPeripheralCode) {
        case SSI0:
            return 0 ;
        case SSI1_D :
        case SSI1_F :
            return 1 ;
        case SSI2 :
            return 2 ;
        case SSI3 :
            return 3 ;
        default :
            break ;
    }
    return 100 ;
}

static char getSSIGPIOPortLetter(SSI_PERIPHERAL SSIPeripheralCode)
{

    switch (SSIPeripheralCode) {
        case SSI0:
            return 'A' ;
        case SSI1_F :
            return 'F' ;
        case SSI2 :
            return 'B' ;
        case SSI3 :
        case SSI1_D :
            return 'D' ;
        default :
            break ;
    }
    return 100 ;
}
extern void SSIWriteValue(SSIDEVICE* SSIDevicePointer, uint32_t value)
{
    //    SSIDataPut(SSIDevicePointer->SSIBase,value) ;
    //    while(SSIBusy(SSIDevicePointer->SSIBase)) ;

    SSIDataPutNonBlocking(SSIDevicePointer->SSIBase,value) ;
    //    if( !(SSIDataPutNonBlocking(SSIDevicePointer->SSIBase,value)) )
    //    {
    //        while(SSIBusy(SSIDevicePointer->SSIBase)) ;
    //        SSIDataPut(SSIDevicePointer->SSIBase,value) ;
    //    }



}


extern uint32_t SSIReadValue(SSIDEVICE* SSIDevicePointer)
{
    SSIDataGet(SSIDevicePointer->SSIBase, &SSIDevicePointer->receiverData) ;
    //    SSIDataGetNonBlocking(SSIDevicePointer->SSIBase, &SSIDevicePointer->receiverData) ;

    return SSIDevicePointer->receiverData ;
}


static uint32_t getSSISauce(uint8_t SSIPeripheralNumber, char SSIGPIOPortLetter,
                            FSS_RX_PIN_USAGE FSSPinUse, FSS_RX_PIN_USAGE SSIRXPinUse, uint8_t returnType)
{
    uint32_t SSIClockVal    = 0 ;
    uint32_t SSIFSSVal      = 0 ;
    uint32_t SSIRXVal       = 0 ;
    uint32_t SSITXVal       = 0 ;
    uint32_t GPIOPins       = 0 ;
    uint32_t returnVal      = 0 ;

    switch ( SSIPeripheralNumber )
    {
    case 0:
        SSIClockVal = GPIO_PA2_SSI0CLK ;
        SSIFSSVal   = GPIO_PA3_SSI0FSS ;
        SSIRXVal    = GPIO_PA4_SSI0RX ;
        SSITXVal    = GPIO_PA5_SSI0TX ;
        GPIOPins    = GPIO_PIN_2 | GPIO_PIN_5 ;
        if(FSSPinUse)   GPIOPins |= GPIO_PIN_3 ;
        if(SSIRXPinUse) GPIOPins |= GPIO_PIN_4 ;
        break ;
    case 1:
        if( SSIGPIOPortLetter == 'F' )
        {
            SSIClockVal = GPIO_PF2_SSI1CLK ;
            SSIFSSVal   = GPIO_PF3_SSI1FSS ;
            SSIRXVal    = GPIO_PF0_SSI1RX ;
            SSITXVal    = GPIO_PF1_SSI1TX ;
            GPIOPins    = GPIO_PIN_1 | GPIO_PIN_2  ;
            if(FSSPinUse)   GPIOPins |= GPIO_PIN_3 ;
            if(SSIRXPinUse) GPIOPins |= GPIO_PIN_0 ;
        }
        else if( SSIGPIOPortLetter == 'D' )
        {
            SSIClockVal = GPIO_PD0_SSI1CLK ;
            SSIFSSVal   = GPIO_PD1_SSI1FSS ;
            SSIRXVal    = GPIO_PD2_SSI1RX ;
            SSITXVal    = GPIO_PD3_SSI1TX ;
            GPIOPins    = GPIO_PIN_0 | GPIO_PIN_3  ;
            if(FSSPinUse)   GPIOPins |= GPIO_PIN_1 ;
            if(SSIRXPinUse) GPIOPins |= GPIO_PIN_2 ;
        }
        break ;
    case 2:
        SSIClockVal = GPIO_PB4_SSI2CLK ;
        SSIFSSVal   = GPIO_PB5_SSI2FSS ;
        SSIRXVal    = GPIO_PB6_SSI2RX ;
        SSITXVal    = GPIO_PB7_SSI2TX ;
        GPIOPins    = GPIO_PIN_4 | GPIO_PIN_7  ;
        if(FSSPinUse)   GPIOPins |= GPIO_PIN_5 ;
        if(SSIRXPinUse) GPIOPins |= GPIO_PIN_6 ;
        break ;
    case 3:
        SSIClockVal = GPIO_PD0_SSI3CLK ;
        SSIFSSVal   = GPIO_PD1_SSI3FSS ;
        SSIRXVal    = GPIO_PD2_SSI3RX ;
        SSITXVal    = GPIO_PD3_SSI3TX ;
        GPIOPins    = GPIO_PIN_0 | GPIO_PIN_3  ;
        if(FSSPinUse)   GPIOPins |= GPIO_PIN_1 ;
        if(SSIRXPinUse) GPIOPins |= GPIO_PIN_2 ;
        break ;
    default:
        break ;
    }

    switch (returnType)
    {
    case GET_CLK:
        returnVal = SSIClockVal ;
        break ;
    case GET_FSS:
        returnVal = SSIFSSVal ;
        break ;
    case GET_SRX:
        returnVal = SSIRXVal ;
        break ;
    case GET_STX:
        returnVal = SSITXVal ;
        break ;
    case GET_PTS:
        returnVal = GPIOPins ;
        break ;
    default:
        break ;
    }
    return returnVal ;

}



static uint32_t getSSIBaseAddress(uint8_t SSIPeripheralNumber)
{
    return ui32SSIBaseAddressArray[SSIPeripheralNumber] ;
}

static uint32_t getSSIPeripheralAddress(uint8_t SSIPeripheralNumber)
{
    return ui32SSIPeripheralAddressArray[SSIPeripheralNumber] ;
}

/*
 * Function to get GPIO Peripheral address depending upon GPIO Peripheral Letter.
 * Arguments:
 *  char SSIGPIOPortLetter                          :: GPIO Port Letter 'A', 'B', 'C', 'D' , 'E', or 'F'.
 * Returns:
 *  uint32_t address Of GPIO Peripheral         :: Depending upon passed argument, the appropriate value from array ui32GPIOPeripheralAddressArray is returned.
 */
static uint32_t getSSIGPIOPeripheralAddress(char SSIGPIOPortLetter)
{
    return ui32GPIOPeripheralAddressArray[(uint8_t)(SSIGPIOPortLetter - 'A')] ;
}


/*
 * Function to get GPIO Base Address depending upon GPIO Peripheral Letter.
 * Arguments:
 *  uint8_t SSIGPIOPortLetter                       :: GPIO Port Letter 'A', 'B', 'C', 'D' , 'E', or 'F'.
 * Returns:
 *  uint32_t address of GPIO Base               :: Depending upon passed argument, the appropriate value from array ui32PGPIOBaseAddressArray is returned.
 */
static uint32_t getSSIGPIOBaseAddress(char SSIGPIOPortLetter)
{
    return ui32GPIOBaseAddressArray[(uint8_t)(SSIGPIOPortLetter - 'A')] ;
}
