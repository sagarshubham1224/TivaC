/*
 * TIMER.h
 * File Description:
 * File to hold TIMER related functions.
 */

#ifndef TIMER_H_
#define TIMER_H_
#include <stdint.h>
#include <stdbool.h>


#include "inc/hw_memmap.h"

#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "inc/hw_timer.h"
#include "PERIPHERALS.h"



/*
 * Macro Definitions
 */
//#define TIMING_FACTOR   4
#define SYSTEM_TIMER_FREQUENCY        4000 //250 useconds. Good enough for most purposes.

#define USE_SYSTICK_FOR_SYSTEM          // Comment this line and uncomment below line for using Timer 0 for system clock.

//#define USE_TIMER0_FOR_SYSTEM        // Uncomment this line and comment above line for using Timer 0 for system clock.

typedef struct TIMERDEVICE {
    uint32_t TIMERBase ;
    TIMER_AB TimerHalfWidthPart ;
    uint16_t timerEventRepeatFrequency ;
    uint32_t timerTimeOutFlag ;
    void (*timeEventFunction)(void) ;
}TIMERDEVICE;



/*
 * Functions:
 */

//public non-static extern Functions:


extern void setTimerEnableDisable(TIMERDEVICE *TIMERDEVICEPointer, TIMER_EN_DIS timerState) ;

/*
 * Function to initiate TIMER 0
 * Arguments:
 *  none.
 * Returns:
 *  none.
 */
extern void initSystemTimer(void);

/*
 * Function to initialize a 32 bit timer as full width periodic timer.
 * Arguments:
 *  TIMERDEVICE *TIMERDEVICEPointer             :: Pointer to TIMERDEVICE struct.
 *  TIMER_PERIPHERAL timerNumber                :: Timer Number to be used.
 *  uint16_t timerEvenRepeatFrequency           :: Frequency of timer event in Hz.
 *  void (*timerEventFunction)(void)            :: Pointer to timerEvent Function of specified type.
 * Returns:
 *  none.
 */
extern void initTimerFullWidthPeriodic(TIMERDEVICE *TIMERDEVICEPointer ,
                                       TIMER_PERIPHERAL timerNumber,
                                       uint16_t timerEventRepeatFrequency,
                                       void (*timerEventFunction)(void)) ;

extern void initTimerHalfWidthPeriodicInterrupt(TIMERDEVICE *TIMERDEVICEPointer,
                                               TIMER_PERIPHERAL timerNumber,
                                               TIMER_AB timerHalfWidthPart,
                                               uint16_t timerEventRepeatFrequency,
                                               void(*timerEventFunction)(void)) ;

/*
 * Function to get number of milliseconds since last reboot.
 * Arguments:
 *  none.
 * Returns:
 *  uint64_t milliSeconds                       :: Number of millisecond since last reboot.
 */
extern uint64_t millis(void);

/*
 * Function to get number of ticks generated since last reboot.
 * 1 tick is equivalent to (TIMING_FACTOR) / (System Clock Frequency in Hz) seconds.
 * 1 tick is 250 microseconds by Default (4000 / 80000000) seconds.
 * Arguments:
 *  none.
 * Returns:
 *  uint64_t ticks                              :: Number of ticks since last reboot.
 */
extern uint64_t getSystemTicks(void);

/*
 * Function to generate delay in ticks
 * Arguments:
 *  uint64_t delayTimeInTicks           :: amount of ticks for which delay is needed.
 * Returns:
 *  none.
 */
extern void systemTicksDelay(uint64_t delayTimeInTicks);

/*
 * Function to generate delay in milliseconds
 * Arguments:
 *  uint64_t delayTimeInMilliSeconds            :: amount of time for which delay is needed.
 * Returns:
 *  none.
 */
extern void milliSecondDelay(uint64_t delayTimeInMilliSeconds);


//private static non-extern Functions:

/*
 * Function to handle Timer0 Interrupts and increase ticks.
 * Arguments:
 *  none.
 * Returns:
 *  none.
 */
static void systemTimerInterruptHandler(void);

/*
 * Function to get Timer Peripheral address from ui32TIMERPeripheralAddressArray.
 * Arguments:
 *  TIMER_PERIPHERAL timerNumber        :: TIMERX where X is [0,TIMER_PERIPHERAL_COUNT-1].
 * Returns:
 *  uint32_t TIMER peripheral address.
 *  TODO : implement similar function for wide timers.
 */
static uint32_t getTimerPeripheralAddress(TIMER_PERIPHERAL timerNumber) ;

/*
 * Function to get Timer Base address from ui32TIMERBaseAddressArray.
 * Arguments:
 *  TIMER_PERIPHERAL timerNumber        :: TIMERX where X is [0,TIMER_PERIPHERAL_COUNT-1].
 * Returns:
 *  uint32_t TIMER base address.
 *  TODO : implement similar function for wide timers.
 */
static uint32_t getTimerBaseAddress(TIMER_PERIPHERAL timerNumber) ;

static uint32_t getTimerHalfTimerPart(TIMER_AB timerHalfWidthPart) ;

static uint32_t getTimerPart(TIMER_AB timerHalfWidthPart) ;


static uint32_t getTimerHalfTimeoutPart(TIMER_AB timerHalfWidthPart) ;


#endif
