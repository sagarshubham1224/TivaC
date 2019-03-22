#include "stringUtil.h"


/*
 * Function to convert a float value to a string of characters.
 * Arguments:
 *  float floatVal                              :: Value in float type to be printed.
 *  uint8_t precision                           :: Number of fraction places to be printed.
 *  uint8_t bufSize                             :: Size of char buffer where string would be stored
 *  char* stringBuffer                          :: Pointer to Array for storing the string representation of the float value.
 * Returns:
 *  uint8_t StringSize                          :: Number of characters written to char buffer,
 *                                                 This is used in turn by the calling Peripheral(or Component) to write the char output.
 */
extern  uint8_t floatToString(float floatVal,uint8_t precision,uint8_t buffSize, char* stringBuffer)
{
    int8_t sign = (floatVal>0)?1:-1;                        // Check if float value to be printed is positive or negative.
    uint8_t presentPosition = 0;
    int8_t intPos = 0;
    uint32_t intPart = (uint32_t)(floatVal*sign);           // Find the Integer portion of the float value.
    uint8_t numOfIntDigits = 0;

    //Find number of digits in integer part of float value.
    if (intPart < 10)               numOfIntDigits = 1;
    else if (intPart <100)          numOfIntDigits = 2;
    else if (intPart <1000)         numOfIntDigits = 3;
    else if (intPart <10000)        numOfIntDigits = 4;
    else if (intPart <100000)       numOfIntDigits = 5;
    else if (intPart <1000000)      numOfIntDigits = 6;
    else if (intPart <10000000)     numOfIntDigits = 7;
    else if (intPart <100000000)    numOfIntDigits = 8;
    else if (intPart <1000000000)   numOfIntDigits = 9;
    else if (intPart <10000000000)  numOfIntDigits = 10;

    {
        memset(stringBuffer,'\0',buffSize);

        if(sign == -1)              //Insert SIGN if needed.
        {
            *(stringBuffer+presentPosition) = '-';
            presentPosition += 1;
        }

        // ADD decimal part to buffer.
        if(intPart == 0)
        {
            *(stringBuffer+presentPosition)    = '0';
            presentPosition             += 1 ;
        }
        else    // Get the digit at units' place in the integer part of the float value, and add it to the string at correct position.
            //Do this till all digits are put in there.
        {
            intPos = numOfIntDigits+presentPosition-1;
            while(intPart > 0)
            {
                uint8_t unitDigit   = intPart%10;
                *(stringBuffer+intPos)     = unitDigit + '0';
                intPart             /= 10;
                presentPosition                 += 1;
                intPos              -= 1;
            }
        }

        //ADD fractional part to buffer.
        if(precision != 0)
        {
            *(stringBuffer+presentPosition) = '.';
            presentPosition += 1;
            floatVal = sign*(floatVal - (uint32_t)(floatVal))*10.0f;
            while(precision > 0)
            {
                uint32_t unitDigit = (uint32_t) (floatVal );
                *(stringBuffer + presentPosition) = unitDigit%10 +'0';
                presentPosition             += 1;
                floatVal        = floatVal*10.0f;
                precision       -= 1;
            }
        }
        *(stringBuffer+presentPosition) = '\0';
    }
    return presentPosition;
}


/*
 * Function to convert integer value to string of characters in binary.
 * Arguments:
 *  uint32_t numVal                             :: unsigned integer value.
 *  uint8_t bitSize                             :: maximum bits in the representation. Must be less than equal to 32. The stringBuffer should also be able to support this.
 *  char* stringBuffer                          :: Pointer to Array for storing the string representation of the unsigned integer in binary.
 * Returns:
 *  No. of characters written.
 */
extern uint8_t intToBinaryString(uint32_t numVal, uint8_t bitSize, char* stringBuffer)
{
    uint8_t remainder = 0;
    int8_t bitPos = bitSize;
    uint8_t returnBitLen = bitPos;


    //      //  Check if max length needs update and do so.
    //      if(numVal >= (1 << BINARY_SIZE_16)) bitPos = BINARY_SIZE_32;
    //      else if (numVal >= (1 << BINARY_SIZE_08)) bitPos = BINARY_SIZE_16;
    //      else if (numVal >= (1 << BINARY_SIZE_04)) bitPos = BINARY_SIZE_08;
    //      else if (numVal >= (1 << BINARY_SIZE_02)) bitPos = BINARY_SIZE_04;

    //  Set last position of array to NUL.
    stringBuffer[bitPos] = '\0';
    while(numVal)
    {
        remainder = numVal % 2;
        numVal = numVal / 2;
        bitPos -= 1;
        stringBuffer[bitPos] = (char)(remainder + '0');
    }
    bitPos -= 1;
    while(bitPos >= 0)
    {
        stringBuffer[bitPos] = '0';
        bitPos -= 1;
    }
    return returnBitLen;
}
