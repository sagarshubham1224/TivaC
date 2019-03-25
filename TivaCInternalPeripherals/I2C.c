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


static const uint32_t ui32I2CPeripheralAddressArray[4] = {
        SYSCTL_PERIPH_I2C0,
        SYSCTL_PERIPH_I2C1,
        SYSCTL_PERIPH_I2C2,
        SYSCTL_PERIPH_I2C3
};


static const uint32_t ui32I2CBaseAddressArray[4] = {
        I2C0_BASE,
        I2C1_BASE,
        I2C2_BASE,
        I2C3_BASE
};


static const uint32_t ui32I2CGPIOPeripheralAddressArray[4] = {
        SYSCTL_PERIPH_GPIOB,
        SYSCTL_PERIPH_GPIOA,
        SYSCTL_PERIPH_GPIOE,
        SYSCTL_PERIPH_GPIOD
};


static const uint32_t ui32I2CGPIOBaseAddressArray[4] = {
        GPIO_PORTB_BASE,
        GPIO_PORTA_BASE,
        GPIO_PORTE_BASE,
        GPIO_PORTD_BASE
};

static const uint32_t ui32I2CPinsAltAddressArray[4][2] = {
        {GPIO_PB2_I2C0SCL, GPIO_PB3_I2C0SDA},
        {GPIO_PA6_I2C1SCL, GPIO_PA7_I2C1SDA},
        {GPIO_PE4_I2C2SCL, GPIO_PE5_I2C2SDA},
        {GPIO_PD0_I2C3SCL, GPIO_PD1_I2C3SDA}
};


static const uint8_t ui8I2CClockDataPins[4][2] = {
        {GPIO_PIN_2, GPIO_PIN_3},
        {GPIO_PIN_6, GPIO_PIN_7},
        {GPIO_PIN_4, GPIO_PIN_5},
        {GPIO_PIN_0, GPIO_PIN_1}
};


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
I2CDEVICE* initI2CMaster(I2CDEVICE* I2CDevicePointer,I2C_PERIPHERAL I2CPeripheralCode,int8_t deviceAddress, I2C_COMMUNICATION_SPEED I2CCommunicationSpeed)
{
    uint8_t I2CBaseNumber = getI2CBaseNumber(I2CPeripheralCode) ;
    SysCtlPeripheralDisable(ui32I2CPeripheralAddressArray[I2CBaseNumber]);
    SysCtlPeripheralReset(ui32I2CPeripheralAddressArray[I2CBaseNumber]);
    SysCtlPeripheralEnable(ui32I2CPeripheralAddressArray[I2CBaseNumber]);

    if( ! (SysCtlPeripheralReady(ui32I2CGPIOPeripheralAddressArray[I2CBaseNumber]) ) ){
        SysCtlPeripheralEnable(ui32I2CGPIOPeripheralAddressArray[I2CBaseNumber]);
    }
    while(  ! (SysCtlPeripheralReady(ui32I2CGPIOPeripheralAddressArray[I2CBaseNumber]) ) ||
            ! (SysCtlPeripheralReady(ui32I2CPeripheralAddressArray[I2CBaseNumber]) ) );

    GPIOPinConfigure(ui32I2CPinsAltAddressArray[I2CBaseNumber][0]);
    GPIOPinConfigure(ui32I2CPinsAltAddressArray[I2CBaseNumber][1]);


    GPIOPinTypeI2CSCL(ui32I2CGPIOBaseAddressArray[I2CBaseNumber],
                        ui8I2CClockDataPins[I2CBaseNumber][0]);
    GPIOPinTypeI2C(ui32I2CGPIOBaseAddressArray[I2CBaseNumber],
                        ui8I2CClockDataPins[I2CBaseNumber][1]);

    I2CMasterInitExpClk(ui32I2CBaseAddressArray[I2CBaseNumber],
                        SysCtlClockGet(),FAST_400k_I2C);

    I2CMasterTimeoutSet(ui32I2CBaseAddressArray[I2CBaseNumber],I2C_MASTER_TIMEOUT_HEX_VAL);

    I2CMasterEnable(ui32I2CBaseAddressArray[I2CBaseNumber]);

    while( I2CMasterBusy( ui32I2CBaseAddressArray[I2CBaseNumber] ) );

    I2CDevicePointer->I2CBase = ui32I2CBaseAddressArray[I2CBaseNumber];
    I2CDevicePointer->deviceAddress = deviceAddress;

    return I2CDevicePointer;
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



static uint8_t getI2CBaseNumber(I2C_PERIPHERAL I2CPeripheralCode)
{
    switch (I2CPeripheralCode) {
        case I2C0:
            return 0;
        case I2C1:
            return 1;
        case I2C2:
            return 2;
        case I2C3:
            return 3;
    }
    return 100 ;
}
