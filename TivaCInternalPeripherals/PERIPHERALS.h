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
#endif
#endif
