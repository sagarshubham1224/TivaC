/*
 * PERIPHERALS.h
 * File Description :
 * File to hold internal peripheral details of Tiva C Boards.
 * Currently, only supports TIVA C TM4C123GH6PM.
 */

#ifndef PERIPHERALS_H_
#define PERIPHERALS_H_

#ifdef PART_TM4C123GH6PM
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
/*
 * GPIO PERIPHERAL ELEMENTS
 */
#define GPIO_PERIPHERAL_COUNT       6

/*
 * Array to hold GPIO Peripheral Address SYSCTL_PERIPH_GPIOX, where X can be Port A, B, C, D, E, or F.
 */
const static uint32_t ui32GPIOPeripheralAddressArray[GPIO_PERIPHERAL_COUNT] = {
        SYSCTL_PERIPH_GPIOA, SYSCTL_PERIPH_GPIOB, SYSCTL_PERIPH_GPIOC,
        SYSCTL_PERIPH_GPIOD, SYSCTL_PERIPH_GPIOE, SYSCTL_PERIPH_GPIOF
};

/*
 * Array to hold GPIO Base Address GPIO_PORTX_BASE, where X can be Port A, B, C, D, E, or F.
 */
const static uint32_t ui32GPIOBaseAddressArray[GPIO_PERIPHERAL_COUNT] = {
        GPIO_PORTA_BASE, GPIO_PORTB_BASE, GPIO_PORTC_BASE,
        GPIO_PORTD_BASE, GPIO_PORTE_BASE, GPIO_PORTF_BASE
};

/*
 * Enum to hold GPIO Pin Codes GPIOXn X is [A,F] , n is [0,7]
 */
typedef enum GPIO_PIN_CODE {
    GPIOA0 = 0,
    GPIOA1 = 1,
    GPIOA2 = 2,
    GPIOA3 = 3,
    GPIOA4 = 4,
    GPIOA5 = 5,
    GPIOA6 = 6,
    GPIOA7 = 7,

    GPIOB0 = 8,
    GPIOB1 = 9,
    GPIOB2 = 10,
    GPIOB3 = 11,
    GPIOB4 = 12,
    GPIOB5 = 13,
    GPIOB6 = 14,
    GPIOB7 = 15,

    GPIOC0 = 16,
    GPIOC1 = 17,
    GPIOC2 = 18,
    GPIOC3 = 19,
    GPIOC4 = 20,
    GPIOC5 = 21,
    GPIOC6 = 22,
    GPIOC7 = 23,

    GPIOD0 = 24,
    GPIOD1 = 25,
    GPIOD2 = 26,
    GPIOD3 = 27,
    GPIOD4 = 28,
    GPIOD5 = 29,
    GPIOD6 = 30,
    GPIOD7 = 31,

    GPIOE0 = 32,
    GPIOE1 = 33,
    GPIOE2 = 34,
    GPIOE3 = 35,
    GPIOE4 = 36,
    GPIOE5 = 37,
    GPIOE6 = 38,
    GPIOE7 = 39,

    GPIOF0 = 40,
    GPIOF1 = 41,
    GPIOF2 = 42,
    GPIOF3 = 43,
    GPIOF4 = 44,
    GPIOF5 = 45,
    GPIOF6 = 46,
    GPIOF7 = 47

}GPIO_PIN_CODE;





/*
 * UART PERIPHERAL ELEMENTS
 */
/*
 * UART Pin Description
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
#define UART_PERIPHERAL_COUNT       8

typedef enum UART_PERIPHERAL {
    UART0 = 0 ,
    UART1 = 1 ,
    UART2 = 2 ,
    UART3 = 3 ,
    UART4 = 4 ,
    UART5 = 5 ,
    UART6 = 6 ,
    UART7 = 7 ,
    UART_DEFAULT = 0
}UART_PERIPHERAL;
/*
 * Array to hold UART GPIO Pin address according to UART pins.
 */
static const uint32_t ui32UARTPinsAddressArray[8] = {
                                                     GPIO_PIN_0|GPIO_PIN_1, GPIO_PIN_0|GPIO_PIN_1,
                                                     GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_6|GPIO_PIN_7,
                                                     GPIO_PIN_4|GPIO_PIN_5, GPIO_PIN_4|GPIO_PIN_5,
                                                     GPIO_PIN_4|GPIO_PIN_5, GPIO_PIN_0|GPIO_PIN_1
};


/*
 * Array to hold UART Interrupt Assignments according to UART Number.
 */
static const uint32_t ui32UARTInterruptsAddressArray[8] = {
                                                           INT_UART0, INT_UART1,
                                                           INT_UART2, INT_UART3,
                                                           INT_UART4, INT_UART5,
                                                           INT_UART6, INT_UART7
};

/*
 * Array to hold UART Module Base Address UARTX_BASE, where X can be 0, 1, 2, 3, 4, 5, 6, or 7.
 */
static const uint32_t ui32UARTBaseAddressArray[8] = {
                                                     UART0_BASE, UART1_BASE, UART2_BASE, UART3_BASE,
                                                     UART4_BASE, UART5_BASE, UART6_BASE, UART7_BASE
};

/*
 * Array to hold UART Module Peripheral Address SYSCTL_PERIPH_UARTX, where X can be 0, 1, 2, 3, 4, 5, 6, or 7.
 */
static const uint32_t ui32UARTPeripheralAddressArray[8] = {
                                                           SYSCTL_PERIPH_UART0, SYSCTL_PERIPH_UART1, SYSCTL_PERIPH_UART2, SYSCTL_PERIPH_UART3,
                                                           SYSCTL_PERIPH_UART4, SYSCTL_PERIPH_UART5, SYSCTL_PERIPH_UART6, SYSCTL_PERIPH_UART7
};

/*
 * Array to hold GPIO Base Address GPIO_PORTX_BASE, where X can be Port A, B, D, C, or E.
 */
static const uint32_t ui32UARTGPIOBaseAddressArray[8] = {
                                                         GPIO_PORTA_BASE, GPIO_PORTB_BASE, GPIO_PORTD_BASE, GPIO_PORTC_BASE,
                                                         GPIO_PORTC_BASE, GPIO_PORTE_BASE, GPIO_PORTD_BASE, GPIO_PORTE_BASE
};

/*
 * Array to hold GPIO Peripheral Address SYSCTL_PERIPH_GPIOX, where X can be Port A, B, D, C, or E.
 */
static const uint32_t ui32UARTGPIOPeripheralAddressArray[8] = {
                                                               SYSCTL_PERIPH_GPIOA, SYSCTL_PERIPH_GPIOB, SYSCTL_PERIPH_GPIOD, SYSCTL_PERIPH_GPIOC,
                                                               SYSCTL_PERIPH_GPIOC, SYSCTL_PERIPH_GPIOE, SYSCTL_PERIPH_GPIOD, SYSCTL_PERIPH_GPIOE
};

/*
 * Array to hold Alternative Pin Function Address according to UART pins.
 */
static const uint32_t ui32UARTPinsAltAddressArray[8][2] = {
                                                           {GPIO_PA0_U0RX,GPIO_PA1_U0TX}, {GPIO_PB0_U1RX,GPIO_PB1_U1TX},
                                                           {GPIO_PD6_U2RX,GPIO_PD7_U2TX}, {GPIO_PC6_U3RX,GPIO_PC7_U3TX},
                                                           {GPIO_PC4_U4RX,GPIO_PC5_U4TX}, {GPIO_PE4_U5RX,GPIO_PE5_U5TX},
                                                           {GPIO_PD4_U6RX,GPIO_PD5_U6TX}, {GPIO_PE0_U7RX,GPIO_PE1_U7TX}
};
#endif
#endif
