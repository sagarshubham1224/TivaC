/*
 * I2C.h
 * File Description:
 * File to hold I2C related functions.
 */

#ifndef I2C_H_
#define I2C_H_
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
//#include "ss_PeripheralNames.h"


/*
 * Macro Definitions.
 */

#define I2C_MASTER_TIMEOUT_HEX_VAL      0xFF

/*
 * I2C Pin Description
╔══════════════╦═════════╦═══════════╦══════════╗
║  I2C-MODULE  ║  PORT   ║  SCL_PIN  ║  SDA_PIN ║
╠══════════════╬═════════╬═══════════╬══════════╣
║  I2C0        ║  GPIOB  ║  PB2      ║  PB3     ║
║  I2C1        ║  GPIOA  ║  PA6      ║  PA7     ║
║  I2C2        ║  GPIOE  ║  PE4      ║  PE5     ║
║  I2C3        ║  GPIOD  ║  PD0      ║  PD1     ║
╚══════════════╩═════════╩═══════════╩══════════╝
 */
typedef enum I2C_PERIPHERAL {
    I2C0 = 0 ,
    I2C1 = 1 ,
    I2C2 = 2 ,
    I2C3 = 3
}I2C_PERIPHERAL;


typedef enum I2C_COMMUNICATION_SPEED {
    FAST_400k_I2C =     true,
    SLOW_100k_I2C =     false
}I2C_COMMUNICATION_SPEED;
typedef struct I2CDEVICE{
    uint32_t I2CBase;
    int8_t deviceAddress;
}I2CDEVICE;


//Function Definitions.

//public non-static extern Functions.

/*
 * Create and return I2CDEVICE pointer.
 * Arguments:
 *  I2CDEVICE* I2CDevicePointer                     :: Pointer to I2CDEVICE struct.
 *  I2C_PERIPHERAL I2CPeripheralCode                :: base number code of the I2C peripheral as I2CX, where X can be in [0,3]
 *  int8_t deviceAddress                            :: Address of I2C Device. Must be between 0-127.
 *  I2C_COMMUNICATION_SPEED I2CCommunicationSpeed   :: Speed at which I2C speed communication is to take place, 100kHz or 400kHz.
 * Returns:
 * I2CDEVICE* I2CDevicePointer              :: Pointer to I2CDEVICE struct.
 */
I2CDEVICE* initI2CMaster(I2CDEVICE* I2CDevicePointer,I2C_PERIPHERAL I2CPeripheralCode,int8_t deviceAddress, I2C_COMMUNICATION_SPEED I2CCommunicationSpeed) ;

/*
 * Returns of WHO_AM_I register of the device.
 * Arguments:
 *  I2CDEVICE* I2CDevicePointer             :: Pointer to I2CDEVICE struct.
 *  uint8_t whoAmIRegisterValue             :: WHO_AM_I Regsiter value.
 * Returns:
 *  uint32_t whoAmIRegisterValue            :: Value as read from WHO_AM_I Register Address.
 */
extern uint32_t I2CWhoAmI(I2CDEVICE* I2CDevicePointer,uint8_t whoAmIRegisterAddress);




/*
 * Reads single register from I2CDevice at registerAddress and returns the read value.
 * Arguments:
 *  I2CDEVICE* I2CDevicePointer             :: Pointer to I2CDevice struct.
 *  uint8_t registerAddress                 :: Register Address to be read from the device.
 * Returns:
 *  uint32_t receivedValue                  :: Value as read from Register Address.
 */
extern uint8_t I2CReadByte(I2CDEVICE* I2CDevicePointer,uint8_t registerAddress);


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
                            uint8_t numOfRegisters, uint8_t* dataArray);


/*
 * Write a single byte to I2CDEVICE on particular register.
 * Arguments:
 *  I2CDEVICE* I2CDevicePointer             :: Pointer to I2CDEVICE struct.
 *  uint8_t registerAddress                 :: Register address to be written.
 *  uint32_t registerValue                  :: Value to be written on register.
 * Returns:
 *  int8_t writeStatus                      :: If write operation was successful, returns 1, else returns 0.
 */
extern int8_t I2CWriteByte(I2CDEVICE* I2CDevicePointer,uint8_t registerAddress,uint32_t registerValue);







static uint8_t getI2CBaseNumber(I2C_PERIPHERAL I2CPeripheralCode) ;

#endif
