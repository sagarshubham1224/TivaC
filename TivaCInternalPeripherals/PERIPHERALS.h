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
#include "inc/hw_memmap.h"

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
#endif
#endif
