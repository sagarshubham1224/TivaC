#include "I2C.h"

/*
 * Pin Description
╔══════════════╦═════════╦═══════════╦══════════╗
║  I2C-MODULE  ║  PORT   ║  SCL_PIN  ║  SDA_PIN ║
╠══════════════╬═════════╬═══════════╬══════════╣
║  I2C0        ║  GPIOB  ║  PB2      ║  PB3     ║
║  I2C1        ║  GPIOA  ║  PA6      ║  PA7     ║
║  I2C2        ║  GPIOE  ║  PE4      ║  PE5     ║
║  I2C3        ║  GPIOD  ║  PD0      ║  PD1     ║
╚══════════════╩═════════╩═══════════╩══════════╝
 */






/*
 * Create and return I2CDEVICE pointer.
 * Arguments:
 *  I2CDEVICE* I2CDevicePointer                     :: Pointer to I2CDEVICE struct.
 *  uint8_t I2CBaseNumber                           :: base number of the I2C peripheral. Must be a value between 0-3.
 *  int8_t deviceAddress                            :: Address of I2C Device. Must be between 0-127.
 *  I2C_COMMUNICATION_SPEED I2CCommunicationSpeed   :: Speed at which I2C speed communication is to take place, 100kHz or 400kHz.
 * Returns:
 * I2CDEVICE* I2CDevicePointer              :: Pointer to I2CDEVICE struct.
 */
void initI2CMaster(I2CDEVICE* I2CDevicePointer,I2C_PERIPHERAL I2CPeripheralCode,int8_t deviceAddress, I2C_COMMUNICATION_SPEED I2CCommunicationSpeed)
{
    uint32_t I2CPeripheralAddressVariable;
    uint32_t I2CBaseAddressVariable;
    uint32_t I2CGPIOPeripheralAddressVariable;
    uint32_t I2CGPIOBaseAddressVariable;
    uint32_t I2CSCLPinAltAddressVariable;
    uint32_t I2CSDAPinAltAddressVariable;
    uint8_t I2CSCLPinAddressVariable;
    uint8_t I2CSDAPinAddressVariable;
    getI2CPeripheralDetails(I2CPeripheralCode,
                            &I2CPeripheralAddressVariable,
                            &I2CBaseAddressVariable,
                            &I2CGPIOPeripheralAddressVariable,
                            &I2CGPIOBaseAddressVariable,
                            &I2CSCLPinAltAddressVariable,
                            &I2CSDAPinAltAddressVariable,
                            &I2CSCLPinAddressVariable,
                            &I2CSDAPinAddressVariable) ;
    if(!SysCtlPeripheralReady(I2CPeripheralAddressVariable))
    {
        SysCtlPeripheralEnable(I2CPeripheralAddressVariable) ;
        while(!SysCtlPeripheralReady(I2CPeripheralAddressVariable)) ;
    }
    if ( !SysCtlPeripheralReady(I2CGPIOPeripheralAddressVariable) )
    {
        SysCtlPeripheralEnable(I2CGPIOPeripheralAddressVariable);
        while(! SysCtlPeripheralReady(I2CGPIOPeripheralAddressVariable)) ;
    }

    GPIOPinConfigure(I2CSCLPinAltAddressVariable);
    GPIOPinConfigure(I2CSDAPinAltAddressVariable);


    GPIOPinTypeI2CSCL(I2CGPIOBaseAddressVariable,
                      I2CSCLPinAddressVariable);
    GPIOPinTypeI2C(I2CGPIOBaseAddressVariable,
                   I2CSDAPinAddressVariable);

    I2CMasterInitExpClk(I2CBaseAddressVariable,
                        SysCtlClockGet(),FAST_400k_I2C);

    I2CMasterTimeoutSet(I2CBaseAddressVariable,I2C_MASTER_TIMEOUT_HEX_VAL);

    I2CMasterEnable(I2CBaseAddressVariable);

    while( I2CMasterBusy( I2CBaseAddressVariable ) );

    I2CDevicePointer->I2CBase = I2CBaseAddressVariable;
    I2CDevicePointer->deviceAddress = deviceAddress;

}



/*
 * Returns of WHO_AM_I register of the device.
 * Arguments:
 *  I2CDEVICE* I2CDevicePointer             :: Pointer to I2CDEVICE struct.
 *  uint8_t whoAmIRegisterValue             :: WHO_AM_I Regsiter value.
 * Returns:
 *  uint32_t whoAmIRegisterValue            :: Value as read from WHO_AM_I Register Address.
 */
extern uint32_t I2CWhoAmI(I2CDEVICE* I2CDevicePointer,uint8_t whoAmIRegisterAddress)
{
    uint8_t whoAmIRegisterValue;
    whoAmIRegisterValue = I2CReadByte(I2CDevicePointer,whoAmIRegisterAddress);
    return whoAmIRegisterValue;
}


/*
 * Reads single register from I2CDevice at registerAddress and returns the read value.
 * Arguments:
 *  I2CDEVICE* I2CDevicePointer             :: Pointer to I2CDevice struct.
 *  uint8_t registerAddress                 :: Register Address to be read from the device.
 * Returns:
 *  uint32_t receivedValue                  :: Value as read from Register Address.
 */
extern uint8_t I2CReadByte(I2CDEVICE* I2CDevicePointer,uint8_t registerAddress)
{
    uint8_t receivedValue = 0;
    I2CMasterSlaveAddrSet(I2CDevicePointer->I2CBase,I2CDevicePointer->deviceAddress,false);
    //  UARTprintf("register = %x \n",registerAddress);
    I2CMasterDataPut(I2CDevicePointer->I2CBase,registerAddress);
    I2CMasterControl(I2CDevicePointer->I2CBase,I2C_MASTER_CMD_SINGLE_SEND);
    while(I2CMasterBusy((I2CDevicePointer->I2CBase)));
    if(!I2CMasterErr(I2CDevicePointer->I2CBase))
    {
        I2CMasterSlaveAddrSet(I2CDevicePointer->I2CBase,I2CDevicePointer->deviceAddress,true);
        I2CMasterControl(I2CDevicePointer->I2CBase,I2C_MASTER_CMD_SINGLE_RECEIVE);
        while(I2CMasterBusy((I2CDevicePointer->I2CBase)));
        if(!I2CMasterErr(I2CDevicePointer->I2CBase))
        {
            receivedValue = (uint8_t)I2CMasterDataGet(I2CDevicePointer->I2CBase);

        }
        else receivedValue = (uint8_t)I2CMasterErr(I2CDevicePointer->I2CBase);
    }
    else receivedValue = (uint8_t)I2CMasterErr(I2CDevicePointer->I2CBase);
    return receivedValue;
}


/*
 * Reads multiple bytes from continuous registers I2CDEVICE and returns array of the read values.
 * Arguments:
 *  I2CDEVICE* I2CDevicePointer             :: Pointer to I2CDEVICE struct.
 *  uint8_t startRegisterAddress            :: register address of first register to be read.
 *  int8_t numOfRegisters                   :: Number of registers to be read.
 *  uint32_t* dataArray                     :: array in which read values are stored.
 * Returns:
 * uint32_t* dataArray                      :: array in which read values are stored. *
 */
extern uint8_t* I2CReadBytes(I2CDEVICE* I2CDevicePointer,
                             uint8_t startRegisterAddress,
                             uint8_t numOfRegisters, uint8_t* dataArray)
{
    uint8_t loopVal = 0;
    I2CMasterSlaveAddrSet(I2CDevicePointer->I2CBase,I2CDevicePointer->deviceAddress,false);
    I2CMasterDataPut(I2CDevicePointer->I2CBase,startRegisterAddress);
    I2CMasterControl(I2CDevicePointer->I2CBase,I2C_MASTER_CMD_BURST_SEND_START);
    while(I2CMasterBusy((I2CDevicePointer->I2CBase)));

    I2CMasterSlaveAddrSet(I2CDevicePointer->I2CBase,I2CDevicePointer->deviceAddress,true);
    I2CMasterControl(I2CDevicePointer->I2CBase,I2C_MASTER_CMD_BURST_RECEIVE_START);
    while(I2CMasterBusy((I2CDevicePointer->I2CBase)));
    dataArray[loopVal] = (uint8_t)I2CMasterDataGet(I2CDevicePointer->I2CBase);

    for(loopVal=1;loopVal<numOfRegisters-1;++loopVal)
    {
        I2CMasterControl(I2CDevicePointer->I2CBase,I2C_MASTER_CMD_BURST_RECEIVE_CONT);
        while(I2CMasterBusy((I2CDevicePointer->I2CBase)));
        dataArray[loopVal] = (uint8_t)I2CMasterDataGet(I2CDevicePointer->I2CBase);
    }

    I2CMasterControl(I2CDevicePointer->I2CBase,I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
    while(I2CMasterBusy((I2CDevicePointer->I2CBase)));
    dataArray[loopVal] = (uint8_t)I2CMasterDataGet(I2CDevicePointer->I2CBase);

    return dataArray;
}



/*
 * Write a single byte to I2CDEVICE on particular register.
 * Arguments:
 *  I2CDEVICE* I2CDevicePointer             :: Pointer to I2CDEVICE struct.
 *  uint8_t registerAddress                 :: Register address to be written.
 *  uint32_t registerValue                  :: Value to be written on register.
 * Returns:
 *  int8_t writeStatus                      :: If write operation was successful, returns 1, else returns 0.
 */
extern int8_t I2CWriteByte(I2CDEVICE* I2CDevicePointer,uint8_t registerAddress,uint32_t registerValue)
{
    int8_t writeStatus = 0;
    I2CMasterSlaveAddrSet(I2CDevicePointer->I2CBase,I2CDevicePointer->deviceAddress,false);
    I2CMasterDataPut(I2CDevicePointer->I2CBase,registerAddress);
    I2CMasterControl(I2CDevicePointer->I2CBase,I2C_MASTER_CMD_BURST_SEND_START);
    while(I2CMasterBusy((I2CDevicePointer->I2CBase)));
    if(!I2CMasterErr(I2CDevicePointer->I2CBase))
    {
        I2CMasterDataPut(I2CDevicePointer->I2CBase,registerValue);
        I2CMasterControl(I2CDevicePointer->I2CBase,I2C_MASTER_CMD_BURST_SEND_FINISH);
        while(I2CMasterBusy((I2CDevicePointer->I2CBase)));
        if(!I2CMasterErr(I2CDevicePointer->I2CBase))
        {
            writeStatus=1;
        }
    }
    return writeStatus;
}





static void getI2CPeripheralDetails(I2C_PERIPHERAL I2CPeripheralCode,
                                    uint32_t* I2CPeripheralAddressVariablePointer,
                                    uint32_t* I2CBaseAddressVariablePointer,
                                    uint32_t* I2CGPIOPeripheralAddressVariablePointer,
                                    uint32_t* I2CGPIOBaseAddressVariablePointer,
                                    uint32_t* I2CSCLPinAltAddressVariablePointer,
                                    uint32_t* I2CSDAPinAltAddressVariablePointer,
                                    uint8_t* I2CSCLPinAddressVariablePointer,
                                    uint8_t* I2CSDAPinAddressVariablePointer)
{
    switch (I2CPeripheralCode) {
    case I2C0: // SCL: PB2 SDA: PB3
    *I2CPeripheralAddressVariablePointer        = ui32I2CPeripheralAddressArray[0] ;
    *I2CBaseAddressVariablePointer              = ui32I2CBaseAddressArray[0] ;
    *I2CGPIOPeripheralAddressVariablePointer    = ui32GPIOPeripheralAddressArray[1] ; // I2C0 is at Port B.
    *I2CGPIOBaseAddressVariablePointer          = ui32GPIOBaseAddressArray[1] ;
    *I2CSCLPinAltAddressVariablePointer         = ui32I2CPinsAltAddressArray[0][0] ;
    *I2CSDAPinAltAddressVariablePointer         = ui32I2CPinsAltAddressArray[0][1] ;
    *I2CSCLPinAddressVariablePointer            = ui8GPIOPinAddressArray[2] ;
    *I2CSDAPinAddressVariablePointer            = ui8GPIOPinAddressArray[3] ;
    break;
    case I2C1: // SCL: PA6 SDA: PA7
        *I2CPeripheralAddressVariablePointer        = ui32I2CPeripheralAddressArray[1] ;
        *I2CBaseAddressVariablePointer              = ui32I2CBaseAddressArray[1] ;
        *I2CGPIOPeripheralAddressVariablePointer    = ui32GPIOPeripheralAddressArray[0] ; // I2C1 is at Port A.
        *I2CGPIOBaseAddressVariablePointer          = ui32GPIOBaseAddressArray[0] ;
        *I2CSCLPinAltAddressVariablePointer         = ui32I2CPinsAltAddressArray[1][0] ;
        *I2CSDAPinAltAddressVariablePointer         = ui32I2CPinsAltAddressArray[1][1] ;
        *I2CSCLPinAddressVariablePointer            = ui8GPIOPinAddressArray[6] ;
        *I2CSDAPinAddressVariablePointer            = ui8GPIOPinAddressArray[7] ;
        break;
    case I2C2: // SCL: PE4 SDA: PE5
        *I2CPeripheralAddressVariablePointer        = ui32I2CPeripheralAddressArray[2] ;
        *I2CBaseAddressVariablePointer              = ui32I2CBaseAddressArray[2] ;
        *I2CGPIOPeripheralAddressVariablePointer    = ui32GPIOPeripheralAddressArray[4] ; // I2C2 is at Port E.
        *I2CGPIOBaseAddressVariablePointer          = ui32GPIOBaseAddressArray[4] ;
        *I2CSCLPinAltAddressVariablePointer         = ui32I2CPinsAltAddressArray[2][0] ;
        *I2CSDAPinAltAddressVariablePointer         = ui32I2CPinsAltAddressArray[2][1] ;
        *I2CSCLPinAddressVariablePointer            = ui8GPIOPinAddressArray[4] ;
        *I2CSDAPinAddressVariablePointer            = ui8GPIOPinAddressArray[5] ;
        break;
    case I2C3: // SCL: PD0 SDA: PD1
        *I2CPeripheralAddressVariablePointer        = ui32I2CPeripheralAddressArray[3] ;
        *I2CBaseAddressVariablePointer              = ui32I2CBaseAddressArray[3] ;
        *I2CGPIOPeripheralAddressVariablePointer    = ui32GPIOPeripheralAddressArray[3] ; // I2C3 is at Port D.
        *I2CGPIOBaseAddressVariablePointer          = ui32GPIOBaseAddressArray[3] ;
        *I2CSCLPinAltAddressVariablePointer         = ui32I2CPinsAltAddressArray[3][0] ;
        *I2CSDAPinAltAddressVariablePointer         = ui32I2CPinsAltAddressArray[3][1] ;
        *I2CSCLPinAddressVariablePointer            = ui8GPIOPinAddressArray[0] ;
        *I2CSDAPinAddressVariablePointer            = ui8GPIOPinAddressArray[1] ;
        break;
    default:
        break;
    }
}
