/*
 * UART.h
 * File Description:
 * File to hold UART Peripheral Functions.
 */

#ifndef UART_H_
#define UART_H_
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>

#include "inc/hw_memmap.h"
#include "inc/hw_uart.h"
#include "inc/hw_ints.h"
#include "inc/hw_types.h"

#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/uart.h"
#include "string.h"
#include "stringUtil.h"
#include "PERIPHERALS.h"

#define UART_8N1                        UART_CONFIG_WLEN_8 | UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_ONE
#define UART_SIMPLE_RX_INTERRUPT        UART_INT_RX|UART_INT_RT
#define NO_UART_RX                      NULL

#define SIGN_SIZE                       1
#define DECIMAL_SIZE                    1
#define END_SEQUENCE_SIZE               1
#define FLOAT_BUFFER_TOO_SMALL_ERROR    0

#define FLOAT_STRING_BUFFER_SIZE        15
#define FLOAT_PRECISION                 3

#define BINARY_STRING_BUFFER_SIZE       33
#define BINARY_SIZE_32                  32
#define BINARY_SIZE_16                  16
#define BINARY_SIZE_08                  8
#define BINARY_SIZE_04                  4
#define BINARY_SIZE_02                  2
#define BINARY_SIZE_01                  1

typedef enum UART_INTERRUPT_STATE_ENABLE_DISABLE {
    UART_INTERRUPT_ENABLE   = 1,
    UART_INTERRUPT_DISABLE  = 0,
    UART_NO_INTERRUPT       = 2
}UART_INTERRUPT_STATE_ENABLE_DISABLE;

typedef struct UARTDEVICE{
    uint32_t    ui32Base;
    uint32_t    interruptType ;
    UART_INTERRUPT_STATE_ENABLE_DISABLE currentInterruptState ;
    void (*UARTReceiveInterruptHandler)(void) ;
}UARTDEVICE;


//Function Declarations.

//public non-static extern Functions.


/*
 * Function to initialize UART Module.
 * Arguments:
 *  UARTDEVICE* UARTDevicePointer               :: Pointer to struct of UARTDEVICE.
 *  uint8_t ui32UARTNumber                      :: UART Number to be used. Value can be between 0 to 7.
 *  uint32_t ui32Baud                           :: Baud Rate to be used.
 *  void(*pfnHandler)(void)                     :: Interrupt Handler for Receiver Data. Pass NULL if not needed.
 * Returns:
 *  UARTDEVICE* UARTDevicePointer               :: Pointer to struct of UARTDEVICE.
 */
extern UARTDEVICE* initUART(UARTDEVICE *UARTDevicePointer,  UART_PERIPHERAL UARTPeripheralCode,uint32_t ui32Baud,void(*pfnHandler)(void));

/*
 * Function to enable or disable UART Interrupt.
 * Arguments:
 *  UARTDEVICE *UARTDevicePointer                       :: Pointer to struct of UARTDEVICE.
 *  UART_INTERRUPT_STATE_ENABLE_DISABLE interruptState  :: Interrupt State to be Set.
 * Returns:
 *  none.
 */
extern void UARTSetInterruptState(UARTDEVICE *UARTDevicePointer, UART_INTERRUPT_STATE_ENABLE_DISABLE interruptState) ;

/*
 * Function to read a single Character from UART and return it.
 * Arguments:
 *  UARTDEVICE* UARTDevicePointer               :: Pointer to struct of UARTDEVICE.
 * Returns:
 *  char charValueInUART                        :: Value read (blocking) from the UART Receiver.
 */
extern char UARTGetCharSingle(UARTDEVICE* UARTDevicePointer);

/*
 * Function to write a string of characters into UART Transmitter.
 * Arguments:
 *  UARTDEVICE* UARTDevicePointer               :: Pointer to struct of UARTDEVICE.
 *  const char *charBuffer                      :: Pointer to location where string to be sent is stored.
 *  uint32_t charBufferLength                   :: Number of characters to be printed.
 * The Function writes either "charBufferLength" number of characters, or till it finds a null character '\0'.
 * Returns:
 *  unsigned int numberOfWrittenCharacters      :: NUmber of characters written to UART.
 */
extern unsigned int UARTWriteCharString(UARTDEVICE* UARTDevicePointer, const char *charBuffer, uint32_t charBufferLength);

/*
 * Function to write Data to UART Transmitter. Based on Code from uartstdio.c with added capability
 * of writing float values.
 * Till now, this can print %c, %d, %i, %u, %x, %p, %s, %f
 * Arguments:
 *  UARTDEVICE* UARTDevicePointer               :: Pointer to struct of UARTDEVICE.
 *  const char *printString                     :: Pointer to string to be printed.
 * Returns:
 *  none.
 */
extern void UARTWrite(UARTDEVICE* UARTDevicePointer, const char *printString, ...);

/*
 * Function to flush UART Receiver.
 * Arguments:
 *  UARTDEVICE* UARTDevicePointer               :: Pointer to struct of UARTDEVICE.
 * Returns:
 *  none.
 */
extern void UARTFlushReceiver(UARTDEVICE* UARTDevicePointer);

/*
 * Function to check if there is data in UART Receiver Buffer.
 * Arguments:
 *  UARTDEVICE* UARTDevicePointer               :: Pointer to struct of UARTDEVICE.
 * Returns:
 *  bool IsCharacterAvailable                   :: true if characters are availabe in Rx Buffer. false otherwise.
 */
extern inline bool UARTCheckCharAvailable(UARTDEVICE* UARTDevicePointer) ;
//private static non-extern Functions.

/*
 * Function to read UART Receive Buffer.
 * Arguments:
 *  UARTDEVICE* UARTDevicePointer               :: Pointer to struct of UARTDEVICE.
 *  uint16_t len                                :: Maximum of of characters to be read into the pointerToMemory.
 *                                                 Memory should at least have this length +1 (for '\0') for no error.
 *  char* pointerToMemory                       :: Pointer to char memory for putting Receiver Buffer Data.
 *  chart startCharacter                        :: (Inclusive) Character to start reading from receiver buffer into the memory.
 *                                                  All characters before would be discarded.
 *                                                  If not required, pass '\0'.
 *                                                  This will add all characters from start into the memory.
 *  char endCharacter                           :: (Inclusive) Character to end reading from receiver buffer to into the memory.
 * Returns:
 *  int16_t numberOfCharacters                  :: Number of characters filled and read into the memory.
 *                                                  Returns 0 in case UART Receiver Buffer was empty.
 */
extern uint16_t UARTReadBuffer(UARTDEVICE* UARTDevicePointer,
                               uint16_t len,
                               char* pointerToMemory,
                               char startCharacter,
                               char endCharacter) ;
extern uint16_t UARTReadBuffer2(UARTDEVICE* UARTDevicePointer,
                               int32_t len,
                               char* pointerToMemory,
                               char startCharacter,
                               char endCharacter) ;
extern uint16_t UARTReadBuffer3(UARTDEVICE* UARTDevicePointer,
                               int32_t len,
                               char* pointerToMemory,
                               char startCharacter,
                               char endCharacter,
                               uint32_t waitTimeSeconds) ;
/*
 * Function to receive a string from UART Receiver.
 * Function made static as of now as we are only using non buffered UART mode.
 * Arguments:
 *  UARTDEVICE* UARTDevicePointer               :: Pointer to struct of UARTDEVICE.
 * Returns:
 *  uint8_t charCount                           :: Number of characters read from UART Receiver.
static  uint8_t UARTGetCharString(UARTDEVICE * UARTDevicePointer);
 */

/*
 * Function where actual magic of writing to UART Transmitter happens.
 * Arguments:
 *  UARTDEVICE* UARTDevicePointer               :: Pointer to struct of UARTDEVICE.
 *  const char* printString                     :: String to be printed and used for printing different types of values.
 *  va_list vaList                              :: list of values to be printed.
 * Returns:
 *  none.
 */
static void UARTvaListWrite(UARTDEVICE* UARTDevicePointer, const char *printString, va_list vaList);








static uint8_t getUARTNumber(UART_PERIPHERAL UARTPeripheralCode) ;


static uint32_t getUARTGPIOPeripheralAddress(UART_PERIPHERAL UARTPeripheralCode) ;


static uint32_t getUARTGPIOBaseAddress(UART_PERIPHERAL UARTPeripheralCode) ;

#endif
