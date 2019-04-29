/*
 * XBEE.c
 *
 *  Created on: 25-Apr-2019
 *      Author: sagarshubham
 */


#include "XBEE.h"

static uint32_t xbeeUARTBaseLocal ;
static XBEEDEVICE* xbeeDevicePointerLocal ;

void xbeeReceiverInterruptHandler(void)
{
    uint32_t IntStatus;
    IntStatus = UARTIntStatus(xbeeUARTBaseLocal, true);
    UARTIntClear(xbeeUARTBaseLocal, IntStatus);
    char receivedCharacter = UARTCharGet(xbeeUARTBaseLocal) ;
    if(receivedCharacter == XBEE_ACK_CHARACTER)
    {
        UARTCharPut(xbeeUARTBaseLocal, XBEE_ACK_CHARACTER) ;
        xbeeDevicePointerLocal->firstACKReceivedAndSent = true ;
        return ;
    }
    if(!xbeeDevicePointerLocal->isXbeeReceiveBufferFull)
    {
        xbeeDevicePointerLocal->xbeeReceiveBuffer[xbeeDevicePointerLocal->xbeeReceiveBufferCurrentPosition] =
                receivedCharacter ;
        ++xbeeDevicePointerLocal->xbeeReceiveBufferCurrentPosition ;
        if(xbeeDevicePointerLocal->xbeeReceiveBufferCurrentPosition == XBEE_RECEIVE_BUFFER_SIZE-1)
        {
            xbeeDevicePointerLocal->xbeeReceiveBuffer[XBEE_RECEIVE_BUFFER_SIZE-1] = '\0' ;
            xbeeDevicePointerLocal->isXbeeReceiveBufferFull = true ;
        }
    }
}

void initialize_XBEE(XBEEDEVICE* xbeeDevicePointer,
              UART_PERIPHERAL UARTPeripheralCode,
              uint32_t baudRate)
{

    xbeeDevicePointer->xbeeReceiveBufferCurrentPosition = 0 ;
    xbeeDevicePointer->xbeeSendBufferCurrentPosition = 0 ;
    xbeeDevicePointer->isXbeeReceiveBufferFull = false ;
    xbeeDevicePointer->isXbeeSendBufferFull = false ;
    xbeeDevicePointer->firstACKReceivedAndSent = false ;
    xbeeDevicePointer->sendPacketNumber = 0 ;

    initUART(&xbeeDevicePointer->xbeeUART,
             UARTPeripheralCode,
             baudRate,
             xbeeReceiverInterruptHandler) ;
    xbeeUARTBaseLocal = xbeeDevicePointer->xbeeUART.ui32Base ;
    xbeeDevicePointerLocal = xbeeDevicePointer ;
}


void createXBEEPacket(XBEEDEVICE* xbeeDevicePointer, void* dataMemoryLocation)
{
    xbeeDevicePointer->sendPacketNumber++ ;
    xbeeDevicePointer->xbeeSendBuffer[0] = (char)((xbeeDevicePointer->sendPacketNumber & 0xFF00) >> 8) ;
    xbeeDevicePointer->xbeeSendBuffer[1] = (char)(xbeeDevicePointer->sendPacketNumber & 0x00FF) ;
    xbeeDevicePointer->xbeeSendBuffer[2] = *(char*)(dataMemoryLocation) ;
    uint32_t checkSum = getCheckSum(xbeeDevicePointer, 2) ;
    xbeeDevicePointer->xbeeSendBuffer[3] = (checkSum >> 24) & 0xFF;
    xbeeDevicePointer->xbeeSendBuffer[4] = (checkSum >> 16) & 0xFF ;
    xbeeDevicePointer->xbeeSendBuffer[5] = (checkSum >> 8) & 0xFF ;
    xbeeDevicePointer->xbeeSendBuffer[6] = (checkSum     ) & 0xFF ;
}
uint32_t getCheckSum(XBEEDEVICE *xbeeDevicePointer, uint8_t checkTillPos)
{
    uint8_t looper = 0;
    uint32_t checkSumValue = 0 ;
    for(looper = 0; looper <= checkTillPos; ++looper)
    {
        checkSumValue += (uint32_t)xbeeDevicePointer->xbeeSendBuffer[looper] ;
    }
    return checkSumValue ;
}
//void sendPacket_XBEE(XBEEDEVICE* xbeeDevicePointer)
//{
//
//}
