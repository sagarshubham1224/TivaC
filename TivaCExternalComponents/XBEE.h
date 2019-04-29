/*
 * XBEE.h
 *
 *  Created on: 25-Apr-2019
 *      Author: sagarshubham
 */

#ifndef XBEE_H_
#define XBEE_H_

#include "stdint.h"
#include "stdbool.h"
#include "UART.h"
#define XBEE_PACKET_NUMBER_SIZE     2

// TODO Change Data size when adding/removing data.
#define XBEE_DATA_SIZE              1

#define XBEE_PACKET_CHECKSUM_SIZE   4           // Hopefully, all data fits in 32 bits.

#define XBEE_SEND_BUFFER_SIZE       ( XBEE_PACKET_NUMBER_SIZE+ XBEE_DATA_SIZE + XBEE_PACKET_CHECKSUM_SIZE )
#define XBEE_RECEIVE_BUFFER_SIZE    ( XBEE_PACKET_NUMBER_SIZE+ XBEE_DATA_SIZE + XBEE_PACKET_CHECKSUM_SIZE )

#define XBEE_ACK_CHARACTER          0x06


typedef struct XBEEDEVICE {
    UARTDEVICE xbeeUART ;
    char xbeeSendBuffer[XBEE_SEND_BUFFER_SIZE] ;
    char xbeeReceiveBuffer[XBEE_RECEIVE_BUFFER_SIZE] ;
    uint8_t xbeeSendBufferCurrentPosition  ;
    uint8_t xbeeReceiveBufferCurrentPosition  ;
    bool firstACKReceivedAndSent ;
    bool isXbeeSendBufferFull ;
    bool isXbeeReceiveBufferFull  ;
    uint16_t sendPacketNumber ;
}XBEEDEVICE;


void xbeeReceiverInterruptHandler(void) ;

void initialize_XBEE(XBEEDEVICE* xbeeDevicePointer,
                 UART_PERIPHERAL UARTPeripheralCode,
                 uint32_t baudRate) ;
void createXBEEPacket(XBEEDEVICE* xbeeDevicePointer, void* dataMemoryLocation)  ;



uint32_t getCheckSum(XBEEDEVICE *xbeeDevicePointer, uint8_t checkTillPos) ;
#endif /* XBEE_H_ */
