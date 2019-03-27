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








extern void initSSIDEVICE(    SSIDEVICE* SSIDevicePointer,
                                     SSI_PERIPHERAL SSIPeripheralCode ,
                                     uint32_t SSIClockFrequency,
                                     uint32_t SSIDataWidth, SSI_FRAME_FORMAT frameFormat,
                                     FSS_RX_PIN_USAGE FSSPinUse, FSS_RX_PIN_USAGE SSIRxPinUse)
{
    uint32_t SSIPeripheralAddress, SSIBaseAddress, SSIGPIOPeripheralAddress, SSIGPIOBaseAddress ;
    uint32_t SSIClockPinAlternateAddress, SSIFSSPinAlternateAddress, SSIRxPinAlternateAddress, SSITxPinAlternateAddress ;
    uint8_t SSIGPIOPinValues ;
    getSSISauce(SSIPeripheralCode,
                 &SSIPeripheralAddress,
                 &SSIBaseAddress,
                 &SSIGPIOPeripheralAddress,
                 &SSIGPIOBaseAddress,
                 &SSIClockPinAlternateAddress,
                 &SSIFSSPinAlternateAddress,
                 &SSIRxPinAlternateAddress,
                 &SSITxPinAlternateAddress,
                 &SSIGPIOPinValues,
                 FSSPinUse,
                 SSIRxPinUse);
    SSIDevicePointer->SSIBase = SSIBaseAddress ;
    SysCtlPeripheralEnable(SSIPeripheralAddress) ;
    while(!SysCtlPeripheralReady(SSIPeripheralAddress));
    if(!SysCtlPeripheralReady(SSIGPIOPeripheralAddress))
    {
        SysCtlPeripheralEnable(SSIGPIOPeripheralAddress) ;
        while(!SysCtlPeripheralReady(SSIGPIOPeripheralAddress));
    }
    GPIOPinConfigure(SSIClockPinAlternateAddress) ;
    if(FSSPinUse)   GPIOPinConfigure(SSIFSSPinAlternateAddress) ;
    if(SSIRxPinUse) GPIOPinConfigure(SSIRxPinAlternateAddress) ;
    GPIOPinConfigure(SSITxPinAlternateAddress) ;

    GPIOPinTypeSSI(SSIGPIOBaseAddress, SSIGPIOPinValues) ;

    SSIConfigSetExpClk(SSIDevicePointer->SSIBase,
                       SysCtlClockGet(),
                       frameFormat,
                       SSI_MODE_MASTER,
                       SSIClockFrequency,
                       SSIDataWidth) ;

    SSIEnable(SSIDevicePointer->SSIBase) ;


    while(SSIDataGetNonBlocking(SSIDevicePointer->SSIBase, &SSIDevicePointer->receiverData)) ;
    SSIDevicePointer->receiverData = 0 ;


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

extern uint8_t SSIReadDataWidth(SSIDEVICE* SSIDevicePointer)
{
    return (uint8_t)(HWREG(SSIDevicePointer->SSIBase + SSI_O_CR0) & 0xF) ; // return DSS Page 961 on Datasheet.
}

extern void SSISetDataWidth(SSIDEVICE* SSIDevicePointer, uint8_t dataWidth)
{
    if(dataWidth <4 || dataWidth >16)
        return ;
    else
    {
        // Clear DSS Bits.
        HWREG(SSIDevicePointer->SSIBase + SSI_O_CR0) &= ~(0xF) ;
        // Set New DSS Bits.
        HWREG(SSIDevicePointer->SSIBase + SSI_O_CR0) |= ((dataWidth-1) & 0xF) ;
    }
}





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
                         FSS_RX_PIN_USAGE SSIRxPinUse)
{
    switch (SSIPeripheralCode) {
    case SSI0: // Port A.
        *SSIPeripheralAddressVariablePointer =          ui32SSIPeripheralAddressArray[0] ;
        *SSIBaseAddressVariablePointer =                ui32SSIBaseAddressArray[0] ;
        *SSIGPIOPeripheralAddressVariablePointer =      ui32GPIOPeripheralAddressArray[0] ;
        *SSIGPIOBaseAddressVariablePointer =            ui32GPIOBaseAddressArray[0] ;
        *SSIClockPinAlternateAddressVariablePointer =   GPIO_PA2_SSI0CLK ;
        *SSIFSSPinAlternateAddressVariablePointer =     GPIO_PA3_SSI0FSS ;
        *SSIRxPinAlternateAddressVariablePointer =      GPIO_PA4_SSI0RX ;
        *SSITxPinAlternateAddressVariablePointer =      GPIO_PA5_SSI0TX ;
        *SSIGPIOPinValuesVariablePointer =              GPIO_PIN_2 | GPIO_PIN_5 ;
        if(SSIFSSPinUse == SET_FSS_PIN)
        {
            *SSIGPIOPinValuesVariablePointer |=         GPIO_PIN_3 ;
        }
        if(SSIRxPinUse == SET_SSIRX_PIN)
        {
            *SSIGPIOPinValuesVariablePointer |=         GPIO_PIN_4 ;
        }
        break;
    case SSI1: // Port F.
        *SSIPeripheralAddressVariablePointer =          ui32SSIPeripheralAddressArray[1] ;
        *SSIBaseAddressVariablePointer =                ui32SSIBaseAddressArray[1] ;
        *SSIGPIOPeripheralAddressVariablePointer =      ui32GPIOPeripheralAddressArray[5] ;
        *SSIGPIOBaseAddressVariablePointer =            ui32GPIOBaseAddressArray[5] ;
        *SSIClockPinAlternateAddressVariablePointer =   GPIO_PF2_SSI1CLK ;
        *SSIFSSPinAlternateAddressVariablePointer =     GPIO_PF3_SSI1FSS ;
        *SSIRxPinAlternateAddressVariablePointer =      GPIO_PF0_SSI1RX ;
        *SSITxPinAlternateAddressVariablePointer =      GPIO_PF1_SSI1TX ;
        *SSIGPIOPinValuesVariablePointer =              GPIO_PIN_2 | GPIO_PIN_1 ;
        if(SSIFSSPinUse == SET_FSS_PIN)
        {
            *SSIGPIOPinValuesVariablePointer |=         GPIO_PIN_3 ;
        }
        if(SSIRxPinUse == SET_SSIRX_PIN)
        {
            *SSIGPIOPinValuesVariablePointer |=         GPIO_PIN_0 ;
        }
    case SSI2: // Port B.
        *SSIPeripheralAddressVariablePointer =          ui32SSIPeripheralAddressArray[2] ;
        *SSIBaseAddressVariablePointer =                ui32SSIBaseAddressArray[2] ;
        *SSIGPIOPeripheralAddressVariablePointer =      ui32GPIOPeripheralAddressArray[1] ;
        *SSIGPIOBaseAddressVariablePointer =            ui32GPIOBaseAddressArray[1] ;
        *SSIClockPinAlternateAddressVariablePointer =   GPIO_PB4_SSI2CLK ;
        *SSIFSSPinAlternateAddressVariablePointer =     GPIO_PB5_SSI2FSS ;
        *SSIRxPinAlternateAddressVariablePointer =      GPIO_PB6_SSI2RX ;
        *SSITxPinAlternateAddressVariablePointer =      GPIO_PB7_SSI2TX ;
        *SSIGPIOPinValuesVariablePointer =              GPIO_PIN_4 | GPIO_PIN_7 ;
        if(SSIFSSPinUse == SET_FSS_PIN)
        {
            *SSIGPIOPinValuesVariablePointer |=         GPIO_PIN_5 ;
        }
        if(SSIRxPinUse == SET_SSIRX_PIN)
        {
            *SSIGPIOPinValuesVariablePointer |=         GPIO_PIN_6 ;
        }
        break;
    case SSI3: // Port F.
        *SSIPeripheralAddressVariablePointer =          ui32SSIPeripheralAddressArray[3] ;
        *SSIBaseAddressVariablePointer =                ui32SSIBaseAddressArray[3] ;
        *SSIGPIOPeripheralAddressVariablePointer =      ui32GPIOPeripheralAddressArray[3] ;
        *SSIGPIOBaseAddressVariablePointer =            ui32GPIOBaseAddressArray[3] ;
        *SSIClockPinAlternateAddressVariablePointer =   GPIO_PD0_SSI3CLK ;
        *SSIFSSPinAlternateAddressVariablePointer =     GPIO_PD1_SSI3FSS ;
        *SSIRxPinAlternateAddressVariablePointer =      GPIO_PD2_SSI3RX ;
        *SSITxPinAlternateAddressVariablePointer =      GPIO_PD3_SSI3TX ;
        *SSIGPIOPinValuesVariablePointer =              GPIO_PIN_0 | GPIO_PIN_3 ;
        if(SSIFSSPinUse == SET_FSS_PIN)
        {
            *SSIGPIOPinValuesVariablePointer |=         GPIO_PIN_1 ;
        }
        if(SSIRxPinUse == SET_SSIRX_PIN)
        {
            *SSIGPIOPinValuesVariablePointer |=         GPIO_PIN_2 ;
        }
        break;
    default:
        break;
    }

}









