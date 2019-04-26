#include "UART.h"



/*
 * Pin Description
 *
╔═══════════════╦═════════╦═════════╦═════════╗
║  UART-MODULE  ║  PORT   ║  RxPin  ║  TxPin  ║
╠═══════════════╬═════════╬═════════╬═════════╣
║            0  ║  GPIOA  ║  PA0    ║  PA1    ║
║            1  ║  GPIOB  ║  PB0    ║  PB1    ║
║            2  ║  GPIOD  ║  PD6    ║  PD7    ║
║            3  ║  GPIOC  ║  PC6    ║  PC7    ║
║            4  ║  GPIOC  ║  PC4    ║  PC5    ║
║            5  ║  GPIOE  ║  PE4    ║  PE5    ║
║            6  ║  GPIOD  ║  PD4    ║  PD5    ║
║            7  ║  GPIOE  ║  PE0    ║  PE1    ║
╚═══════════════╩═════════╩═════════╩═════════╝
 *
 * Table created with: https://ozh.github.io/ascii-tables/
 */










/*
 * Array to hold float value as a String to be printed.
 */
static char floatStringBuffer[FLOAT_STRING_BUFFER_SIZE] = {
                                                           '\0'
};

static char binaryStringBuffer[BINARY_STRING_BUFFER_SIZE] = {
                                                             '\0'
};

volatile extern bool defaultUARTReseter = false;

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
extern UARTDEVICE* initUART(UARTDEVICE* UARTDevicePointer, UART_PERIPHERAL UARTPeripheralCode,uint32_t ui32Baud,void(*pfnHandler)(void))
{
    uint8_t ui32UARTNumber = getUARTNumber(UARTPeripheralCode) ;
    UARTDevicePointer->ui32Base = ui32UARTBaseAddressArray[ui32UARTNumber];

    SysCtlPeripheralEnable(getUARTGPIOPeripheralAddress(UARTPeripheralCode));

    SysCtlPeripheralEnable(ui32UARTPeripheralAddressArray[ui32UARTNumber]);

    GPIOPinConfigure(ui32UARTPinsAltAddressArray[ui32UARTNumber][0]);
    GPIOPinConfigure(ui32UARTPinsAltAddressArray[ui32UARTNumber][1]);

    GPIOPinTypeUART(getUARTGPIOBaseAddress(UARTPeripheralCode),
                    ui32UARTPinsAddressArray[ui32UARTNumber]);

    UARTConfigSetExpClk(ui32UARTBaseAddressArray[ui32UARTNumber],
                        SysCtlClockGet(),
                        ui32Baud,
                        UART_8N1 );

    UARTFlushReceiver(UARTDevicePointer);

    if(pfnHandler){
        UARTIntRegister(ui32UARTBaseAddressArray[ui32UARTNumber], pfnHandler);
        IntEnable(ui32UARTInterruptsAddressArray[ui32UARTNumber]);
        UARTIntEnable(ui32UARTBaseAddressArray[ui32UARTNumber], UART_SIMPLE_RX_INTERRUPT);
        IntMasterEnable();
    }

    return UARTDevicePointer;
}



/*
 * Function to read a single Character from UART Receiver and return it.
 * Arguments:
 *  UARTDEVICE* UARTDevicePointer               :: Pointer to struct of UARTDEVICE.
 * Returns:
 *  char charValueInUART                        :: Value read (blocking) from the UART Receiver.
 */
extern char UARTGetCharSingle(UARTDEVICE* UARTDevicePointer)
{
    return UARTCharGet(UARTDevicePointer->ui32Base);
}

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
extern unsigned int UARTWriteCharString(UARTDEVICE* UARTDevicePointer, const char *charBuffer, uint32_t charBufferLength)
{
    unsigned int uIdx;


    for(uIdx = 0; uIdx < charBufferLength && charBuffer[uIdx] != '\0'; uIdx++)
    {
        //
        // If the character to the UART is \n, then add a \r before it so that
        // \n is translated to \n\r in the output.
        //
        if(charBuffer[uIdx] == '\n')
        {
            UARTCharPut(UARTDevicePointer->ui32Base, '\r');
        }
        UARTCharPut(UARTDevicePointer->ui32Base, charBuffer[uIdx]);                 // Send the character to the UART output.
    }
    return(uIdx);                   // Return the number of characters written.
}

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
extern void UARTWrite(UARTDEVICE* UARTDevicePointer, const char *printString, ...)
{
    va_list vaList;
    va_start(vaList, printString);
    UARTvaListWrite(UARTDevicePointer, printString, vaList);
    va_end(vaList);
}

/*
 * Function to flush UART Receiver.
 * Arguments:
 *  UARTDEVICE* UARTDevicePointer               :: Pointer to struct of UARTDEVICE.
 * Returns:
 *  none.
 */
extern void UARTFlushReceiver(UARTDEVICE* UARTDevicePointer)
{
    while(UARTCharsAvail(UARTDevicePointer->ui32Base))
    {
        UARTCharGet(UARTDevicePointer->ui32Base);
    }
}
/*
 * Function to check if there is data in UART Receiver Buffer.
 * Arguments:
 *  UARTDEVICE* UARTDevicePointer               :: Pointer to struct of UARTDEVICE.
 * Returns:
 *  bool IsCharacterAvailable                   :: true if characters are availabe in Rx Buffer. false otherwise.
 */
extern inline bool UARTCheckCharAvailable(UARTDEVICE* UARTDevicePointer)
{
    return ((HWREG(UARTDevicePointer->ui32Base + UART_O_FR) & UART_FR_RXFE) ? false : true) ;
}

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
                               char endCharacter)
{
    if(UARTCharsAvail(UARTDevicePointer->ui32Base))               // Step 1. Check if Buffer has data.
    {
        uint16_t numberOfCharacters = 0 ;                        // Variable to store return value.
        char readCharacter = ' ';                                // temp variable to store read data.
        readCharacter = UARTCharGet(UARTDevicePointer->ui32Base) ; // read first available character.
        if(startCharacter != '\0' && readCharacter != startCharacter)  // Step 2.  Check if startCharacter is not null
            // && readCharacter is not equal to startCharacter.
        {
            do      // Now keep reading data until either end of buffer is reached, or
            {       // startCharacter is read.
                if(UARTCharsAvail(UARTDevicePointer->ui32Base))
                {
                    readCharacter = UARTCharGet(UARTDevicePointer->ui32Base) ;
                }
                else
                {
                    return 0 ;   // Means Buffer was empty now, and we didn't find startCharacter. So we return empty-handed.
                }
            }while(readCharacter!= startCharacter) ;

        }
        // do-while loop exit(and subsequent if exit) and control at this point means
        // startCharacter was found. fill it in. check if
        // numberOfCharacters is equal to len, if so, then exit.
        pointerToMemory[numberOfCharacters++] = readCharacter ;
        if(numberOfCharacters == len-1)
        {
            pointerToMemory[numberOfCharacters+1] = '\0' ;
            return numberOfCharacters ;
        }
        while(UARTCheckCharAvailable(UARTDevicePointer))    // Step 3. read the next character if available.
        {
            readCharacter = UARTCharGet(UARTDevicePointer->ui32Base);
            pointerToMemory[numberOfCharacters++] = readCharacter ;
            if(numberOfCharacters == len)
            {
                pointerToMemory[numberOfCharacters+1] = '\0' ;
                return numberOfCharacters ;
            }
            if(readCharacter == endCharacter)
                break ;
        }
        pointerToMemory[numberOfCharacters+1] = '\0' ;
        return numberOfCharacters ;
    }
    else
    {
        return 0 ;
    }
}

/*
 * EXPERIMENTAL
 */
extern uint16_t UARTReadBuffer2(UARTDEVICE* UARTDevicePointer,
                                int32_t len,
                                char* pointerToMemory,
                                char startCharacter,
                                char endCharacter)
{
    uint16_t numberOfCharacters = 0 ;
    uint8_t state = 0 ;
    bool loopCondition = true ;
    uint8_t startCountTimer = 0;
    char readCharacter = '\0' ;
#define START_RECHECK_THRESHOLD     50
    if(len <= 0)
        return 0 ;
    while(loopCondition)
    {
        switch (state) {
        case 0:
        {// Just Started. Check if characters are available, if so, read it and advance to next state. else return.
            if(UARTCharsAvail(UARTDevicePointer->ui32Base))
            {
                readCharacter = UARTCharGetNonBlocking(UARTDevicePointer->ui32Base) ;
                state = 1;
            }
            else // No Character was available. Return with a 0 (initial value of numberOfCharacters)..
            {
                return 0 ;
            }
            break;
        }
        case 1:
        {// First Character has been read, that means more will follow, so now read blockingly.
            // If startCharacter is null, fill it in the memory,
            // update numberOfCharacters,check numberofCharacters < len-1 and advance to next state=2.
            // Else If startCharacter is not null, check if readCharacter is startCharacter, if so, fill it in the
            // memory, update numberOfCharacters,check numberofCharacters < len-1 and advance to next state=2.
            // If startCharacter is not null and readCharacter is not startCharacter, read next character blockingly,
            // and stay at same state. Keep reading START_THRESHOLD=50 times till readCharacter is equal to startCharacter.
            // if START_THRESHOLD is crossed, and yet startCharacter was not found, return with a 0 (initial value of numberOfCharacters).
            if(startCharacter == '\0')
            {
                pointerToMemory[0] = readCharacter ;
                numberOfCharacters++ ;
                if(numberOfCharacters == len  )
                {
                    pointerToMemory[numberOfCharacters+1] = '\0' ;
                    return numberOfCharacters ;
                }
                state = 2 ;
            }
            else if(readCharacter == startCharacter)
            {
                pointerToMemory[0] = readCharacter ;
                numberOfCharacters++ ;
                if(numberOfCharacters == len  )
                {
                    pointerToMemory[numberOfCharacters+1] = '\0' ;
                    return numberOfCharacters ;
                }
                state = 2 ;
            }
            else
            {
                startCountTimer += 1 ;
                if(startCountTimer <=len/2)
                {
                    readCharacter = UARTCharGet(UARTDevicePointer->ui32Base) ;
                }
                else
                {
                    return 0 ;
                }
            }
            break ;
        }
        case 2:
        {   // startCharacter has been filled. Now keep reading till last character is found, or
            // numberOfCharacters = len-2, then pad last '\0' and return numberOfCharacters.
            readCharacter = UARTCharGet(UARTDevicePointer->ui32Base) ;
            pointerToMemory[numberOfCharacters++] = readCharacter ;
            if(numberOfCharacters == len  )
            {
                pointerToMemory[numberOfCharacters+1] = '\0' ;
                return numberOfCharacters ;
            }
            if(readCharacter == endCharacter)
            {
                pointerToMemory[numberOfCharacters] = '\0' ;
                return numberOfCharacters ;
            }
        }
        default:
            break;
        }
    }
    return 0 ;
}

/*
 * EXPERIMENTAL
 */
extern uint16_t UARTReadBuffer3(UARTDEVICE* UARTDevicePointer,
                                int32_t len,
                                char* pointerToMemory,
                                char startCharacter,
                                char endCharacter,
                                uint32_t waitTimeSeconds)
{
    uint16_t numberOfCharacters = 0 ;
    uint8_t state = 0 ;
    bool loopCondition = true ;
    uint8_t startCountTimer = 0;
    char readCharacter = '\0' ;
#define START_RECHECK_THRESHOLD     50
    if(len <= 0)
        return 0 ;
    while(loopCondition)
    {
        switch (state) {
        case 0:
        {// Just Started. Check if characters are available, if so, read it and advance to next state. else return.
            if(UARTCharsAvail(UARTDevicePointer->ui32Base))
            {
                readCharacter = UARTCharGetNonBlocking(UARTDevicePointer->ui32Base) ;
                UARTCharPut(UARTDevicePointer->ui32Base, readCharacter) ;
                state = 1;
            }
            else // No Character was available. Return with a 0 (initial value of numberOfCharacters)..
            {
                bool noCharFound = true ;
                SysCtlDelay(waitTimeSeconds);
                if(UARTCharsAvail(UARTDevicePointer->ui32Base))
                {
                    readCharacter = UARTCharGetNonBlocking(UARTDevicePointer->ui32Base) ;
                    UARTCharPut(UARTDevicePointer->ui32Base, readCharacter) ;
                    state = 1;
                    noCharFound = false;
                }
                if (noCharFound) return 0 ;
                else break ;
            }
            break;
        }
        case 1:
        {// First Character has been read, that means more will follow, so now read blockingly.
            // If startCharacter is null, fill it in the memory,
            // update numberOfCharacters,check numberofCharacters < len-1 and advance to next state=2.
            // Else If startCharacter is not null, check if readCharacter is startCharacter, if so, fill it in the
            // memory, update numberOfCharacters,check numberofCharacters < len-1 and advance to next state=2.
            // If startCharacter is not null and readCharacter is not startCharacter, read next character blockingly,
            // and stay at same state. Keep reading START_THRESHOLD=50 times till readCharacter is equal to startCharacter.
            // if START_THRESHOLD is crossed, and yet startCharacter was not found, return with a 0 (initial value of numberOfCharacters).
            if(startCharacter == '\0')
            {
                pointerToMemory[0] = readCharacter ;
                numberOfCharacters++ ;
                if(numberOfCharacters == len  )
                {
                    pointerToMemory[numberOfCharacters+1] = '\0' ;
                    return numberOfCharacters ;
                }
                state = 2 ;
            }
            else if(readCharacter == startCharacter)
            {
                pointerToMemory[0] = readCharacter ;
                numberOfCharacters++ ;
                if(numberOfCharacters == len  )
                {
                    pointerToMemory[numberOfCharacters+1] = '\0' ;
                    return numberOfCharacters ;
                }
                state = 2 ;
            }
            else
            {
                startCountTimer += 1 ;
                if(startCountTimer <=len/2)
                {
                    readCharacter = UARTCharGet(UARTDevicePointer->ui32Base) ;
                    UARTCharPut(UARTDevicePointer->ui32Base, readCharacter) ;
                }
                else
                {
                    return 0 ;
                }
            }
            break ;
        }
        case 2:
        {   // startCharacter has been filled. Now keep reading till last character is found, or
            // numberOfCharacters = len-2, then pad last '\0' and return numberOfCharacters.
            readCharacter = UARTCharGet(UARTDevicePointer->ui32Base) ;
            UARTCharPut(UARTDevicePointer->ui32Base, readCharacter) ;
            pointerToMemory[numberOfCharacters++] = readCharacter ;
            if(numberOfCharacters == len  )
            {
                pointerToMemory[numberOfCharacters+1] = '\0' ;
                return numberOfCharacters ;
            }
            if(readCharacter == endCharacter)
            {
                pointerToMemory[numberOfCharacters] = '\0' ;
                return numberOfCharacters ;
            }
        }
        default:
            break;
        }
    }
    return 0 ;
}

/*
 * Function to receive a string from UART Receiver.
 * Function made static as of now as we are only using non buffered UART mode.
 * Arguments:
 *  UARTDEVICE* UARTDevicePointer               :: Pointer to struct of UARTDEVICE.
 * Returns:
 *  uint8_t charCount                           :: Number of characters read from UART Receiver.

static uint8_t UARTGetCharString(UARTDEVICE * UARTDevicePointer)
{
    uint8_t ui8Count = 0;
    int8_t cChar;
    uint8_t StringLen = UARTDevicePointer->ui8MaxStringLen - 1;
    while(1)
    {
        if(UARTCharsAvail(UARTDevicePointer->ui32Base))
        {
            cChar = UARTCharGet(UARTDevicePointer->ui32Base);
            if(ui8Count < StringLen)
            {
                UARTDevicePointer->charBuffer[ui8Count] = cChar;
                ui8Count++;
            }
        }
        else
            break;
    }

    UARTDevicePointer->charBuffer[ui8Count] = 0;
    return(ui8Count);
}


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
static void UARTvaListWrite(UARTDEVICE* UARTDevicePointer, const char *printString, va_list vaList)
{
    uint32_t ui32Idx, ui32Value, ui32Pos, ui32Count, ui32Base, ui32Neg;
    char *pcStr, pcBuf[16], cFill;
    float floatVal;
    while(*printString)
    {
        for(ui32Idx = 0; (printString[ui32Idx] != '%') && (printString[ui32Idx] != '\0');ui32Idx++);    // Find the first non-% character, or the end of the string.
        UARTWriteCharString(UARTDevicePointer, printString, ui32Idx);           // Write this portion of the string.
        printString += ui32Idx;         // Skip the portion of the string that was written.
        if(*printString == '%')         // See if the next character is a %.
        {
            printString++;              // Skip the %.
            ui32Count = 0;              //Set the digit count to zero, and the fill character to space (in other words, to the defaults).
            cFill = ' ';

            again:                      // It may be necessary to get back here to process more characters.
            switch(*printString++)      // Determine how to handle the next character.
            {

            case '0':                   // Handle the digit characters.
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
            {
                if((printString[-1] == '0') && (ui32Count == 0)) //If this is a zero, and it is the first digit, then the fill character is a zero instead of a space.
                {
                    cFill = '0';
                }
                ui32Count *= 10;                    // Update the digit count.
                ui32Count += printString[-1] - '0';
                goto again;                         // Get the next character.
            }

            case 'c':                               // Handle the %c command.
            {
                ui32Value = va_arg(vaList, uint32_t);                       // Get the value from the varargs.
                UARTWriteCharString(UARTDevicePointer, (char *)&ui32Value, 1);  // Print out the character.
                break;                                                      // This command has been handled.
            }

            case 'd':           // Handle the %d and %i commands.
            case 'i':
            {
                ui32Value = va_arg(vaList, uint32_t);                       // Get the value from the varargs.
                ui32Pos = 0;                                                // Reset the buffer position.
                if((int32_t)ui32Value < 0)                                  // If the value is negative, make it positive and indicate that a minus sign is needed.
                {
                    ui32Value = -(int32_t)ui32Value;                        // Make the value positive.
                    ui32Neg = 1;                                            // Indicate that the value is negative.
                }
                else
                {
                    ui32Neg = 0;                                            // Indicate that the value is positive so that a minus sign isn't inserted.
                }
                ui32Base = 10;                                              // Set the base to 10.
                goto convert;                                               // Convert the value to ASCII.
            }

            case 's':                                                       // Handle the %s command.
            {
                pcStr = va_arg(vaList, char *);                             // Get the string pointer from the varargs.
                for(ui32Idx = 0; pcStr[ui32Idx] != '\0'; ui32Idx++);        // Determine the length of the string.
                UARTWriteCharString(UARTDevicePointer, pcStr, ui32Idx);         // Write the string.
                if(ui32Count > ui32Idx)                                     // Write any required padding spaces
                {
                    ui32Count -= ui32Idx;
                    while(ui32Count--)
                    {
                        UARTWriteCharString(UARTDevicePointer, " ", 1);
                    }
                }
                break;                                                      // This command has been handled.
            }

            case 'u':                                                       // Handle the %u command.
            {
                ui32Value = va_arg(vaList, uint32_t);                       // Get the value from the varargs.
                ui32Pos = 0;                                                // Reset the buffer position.
                ui32Base = 10;                                              // Set the base to 10.
                ui32Neg = 0;                                                // Indicate that the value is positive so that a minus sign isn't inserted.
                goto convert;                                               // Convert the value to ASCII.
            }

            // Handle the %x and %X commands.  Note that they are treated
            // identically; in other words, %X will use lower case letters
            // for a-f instead of the upper case letters it should use.  We
            // also alias %p to %x.
            case 'x':
            case 'X':
            case 'p':
            {
                ui32Value = va_arg(vaList, uint32_t);                       // Get the value from the varargs.
                ui32Pos = 0;                                                // Reset the buffer position.
                ui32Base = 16;                                              // Set the base to 16.
                ui32Neg = 0;                                                // Indicate that the value is positive so that a minus sign isn't inserted.

                convert:
                for(ui32Idx = 1;
                        (((ui32Idx * ui32Base) <= ui32Value) &&
                                (((ui32Idx * ui32Base) / ui32Base) == ui32Idx));    // Determine the number of digits in the string version of the value.
                        ui32Idx *= ui32Base, ui32Count--)
                {
                }
                if(ui32Neg)                                                 // If the value is negative, reduce the count of padding characters needed.
                {
                    ui32Count--;
                }
                if(ui32Neg && (cFill == '0'))   // If the value is negative and the value is padded with zeros, then place the minus sign before the padding.
                {
                    pcBuf[ui32Pos++] = '-';                                 // Place the minus sign in the output buffer.
                    ui32Neg = 0;                                            // The minus sign has been placed, so turn off the negative flag.
                }

                if((ui32Count > 1) && (ui32Count < 16))                     // Provide additional padding at the beginning of the string conversion if needed.
                {
                    for(ui32Count--; ui32Count; ui32Count--)
                    {
                        pcBuf[ui32Pos++] = cFill;
                    }
                }

                if(ui32Neg)                                                 // If the value is negative, then place the minus sign before the number.
                {
                    pcBuf[ui32Pos++] = '-';                                 // Place the minus sign in the output buffer.
                }
                for(; ui32Idx; ui32Idx /= ui32Base)                         // Convert the value into a string.
                {
                    pcBuf[ui32Pos++] = g_pcHex[(ui32Value / ui32Idx) % ui32Base];
                }
                UARTWriteCharString(UARTDevicePointer, pcBuf, ui32Pos);         // Write the string.
                break;                                                      // This command has been handled.
            }

            case '%':                                                       // Handle the %% command.
            {
                UARTWriteCharString(UARTDevicePointer, printString - 1, 1);     // Simply write a single %.
                break;                                                      // This command has been handled.
            }

            case 'f':                                                       // Handle %f float values.
            {
                floatVal = (float) (va_arg(vaList, double)) ;
                uint8_t floatStringSize = floatToString(floatVal, FLOAT_PRECISION, FLOAT_STRING_BUFFER_SIZE,floatStringBuffer);
                UARTWriteCharString(UARTDevicePointer, floatStringBuffer, floatStringSize);
                break;                                                      // This command has been handled.
            }
            case 'b':
            {
                ui32Value = (va_arg(vaList, uint32_t));
                uint8_t binaryStringSize = intToBinaryString(ui32Value, BINARY_SIZE_08,binaryStringBuffer);
                UARTWriteCharString(UARTDevicePointer,binaryStringBuffer,binaryStringSize );
                break;
            }

            default:
            {
                //
                // Indicate an error.
                //
                UARTWriteCharString(UARTDevicePointer, "ERROR", 5);
                break;                                                      // This command has been handled.
            }
            }
        }
    }
}











static uint8_t getUARTNumber(UART_PERIPHERAL UARTPeripheralCode)
{
    switch (UARTPeripheralCode) {
    case UART0 :
        return 0 ;
    case UART1 :
        return 1 ;
    case UART2 :
        return 2 ;
    case UART3 :
        return 3 ;
    case UART4 :
        return 4 ;
    case UART5 :
        return 5 ;
    case UART6 :
        return 6 ;
    case UART7 :
        return 7 ;
    }
    return 100 ;
}

static uint32_t getUARTGPIOPeripheralAddress(UART_PERIPHERAL UARTPeripheralCode)
{
    switch (UARTPeripheralCode) {
        case UART0:
            return ui32GPIOPeripheralAddressArray[0] ; // PORT A
        case UART1:
            return ui32GPIOPeripheralAddressArray[1] ; // PORT B
        case UART2:
        case UART6:
            return ui32GPIOPeripheralAddressArray[3] ; // PORT D
        case UART3:
        case UART4:
            return ui32GPIOPeripheralAddressArray[2] ; // PORT C
        case UART5:
        case UART7:
            return ui32GPIOPeripheralAddressArray[4] ; // PORT E
        default:
            break ;
    }
    return 100 ;
}
static uint32_t getUARTGPIOBaseAddress(UART_PERIPHERAL UARTPeripheralCode)
{
    switch (UARTPeripheralCode) {
        case UART0:
            return ui32GPIOBaseAddressArray[0] ; // PORT A
        case UART1:
            return ui32GPIOBaseAddressArray[1] ; // PORT B
        case UART2:
        case UART6:
            return ui32GPIOBaseAddressArray[3] ; // PORT D
        case UART3:
        case UART4:
            return ui32GPIOBaseAddressArray[2] ; // PORT C
        case UART5:
        case UART7:
            return ui32GPIOBaseAddressArray[4] ; // PORT E
        default:
            break ;
    }
    return 100 ;
}


