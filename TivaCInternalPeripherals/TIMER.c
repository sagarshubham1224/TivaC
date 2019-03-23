
#include "TIMER.h"

volatile static uint64_t ticks = 0;

static TIMERDEVICE systemTimer ;









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
                                       void (*timerEventFunction)(void))
{
    SysCtlPeripheralEnable(getTimerPeripheralAddress(timerNumber)) ;
    while(! SysCtlPeripheralReady(getTimerPeripheralAddress(timerNumber))) ;
    TIMERDEVICEPointer->TIMERBase = getTimerBaseAddress(timerNumber) ;
    TIMERDEVICEPointer->timeEventFunction = timerEventFunction ;
    TIMERDEVICEPointer->timerEventRepeatFrequency = timerEventRepeatFrequency ;
    TimerConfigure(TIMERDEVICEPointer->TIMERBase, TIMER_CFG_PERIODIC) ;
    TimerLoadSet(TIMERDEVICEPointer->TIMERBase, TIMER_A, SysCtlClockGet()/timerEventRepeatFrequency-1) ;
    TimerIntRegister(TIMERDEVICEPointer->TIMERBase, TIMER_A, timerEventFunction) ;
    TimerIntEnable(TIMERDEVICEPointer->TIMERBase, TIMER_TIMA_TIMEOUT) ;
    TimerEnable(TIMERDEVICEPointer->TIMERBase, TIMER_A) ;

}

/*
 * Function to initiate TIMER 0
 * Arguments:
 *  none.
 * Returns:
 *  none.
 */
extern void initSystemTimer(void)
{
    initTimerFullWidthPeriodic(&systemTimer, TIMER0, SYSTEM_TIMER_FREQUENCY, systemTimerInterruptHandler) ;
}
/*
 * Function to get number of milliseconds since last reboot.
 * Arguments:
 *  none.
 * Returns:
 *  uint64_t milliSeconds                       :: Number of millisecond since last reboot.
 */
extern uint64_t millis(void)
{
    return (getSystemTicks()*1000)/SYSTEM_TIMER_FREQUENCY;
}

/*
 * Function to get number of ticks generated since last reboot.
 * 1 tick is equivalent to (TIMING_FACTOR) / (System Clock Frequency in Hz) seconds.
 * 1 tick is 250 microseconds by Default (4000 / 80000000) seconds.
 * Arguments:
 *  none.
 * Returns:
 *  uint64_t ticks                              :: Number of ticks since last reboot.
 */
extern uint64_t getSystemTicks(void)
{
    return ticks;
}


/*
 * Function to generate delay in milliseconds
 * Arguments:
 *  uint64_t delayTimeInMilliSeconds            :: amount of time for which delay is needed.
 * Returns:
 *  none.
 */
extern void milliSecondDelay(uint64_t delayTimeInMilliSeconds)
{
    volatile uint64_t startTicks = ticks;
    while ( (ticks-startTicks) < SYSTEM_TIMER_FREQUENCY*delayTimeInMilliSeconds/1000);
}

/*
 * Function to generate delay in ticks
 * Arguments:
 *  uint64_t delayTimeInTicks           :: amount of ticks for which delay is needed.
 * Returns:
 *  none.
 */
extern void systemTicksDelay(uint64_t delayTimeInTicks)
{
    volatile uint64_t startTicks = ticks;
    while ( (ticks-startTicks) < delayTimeInTicks);
}

//private static non-extern Functions:

/*
 * Function to handle Timer0 Interrupts and increase ticks.
 * Arguments:
 *  none.
 * Returns:
 *  none.
 */
static void systemTimerInterruptHandler(void)
{
    uint32_t status = TimerIntStatus(systemTimer.TIMERBase,true);
    TimerIntClear(systemTimer.TIMERBase,status);
    ticks++;
}

/*
 * Function to get Timer Peripheral address from ui32TIMERPeripheralAddressArray.
 * Arguments:
 *  TIMER_PERIPHERAL timerNumber        :: TIMERX where X is [0,TIMER_PERIPHERAL_COUNT-1].
 * Returns:
 *  uint32_t TIMER peripheral address.
 *  TODO : implement similar function for wide timers.
 */
static uint32_t getTimerPeripheralAddress(TIMER_PERIPHERAL timerNumber)
{
    return ui32TIMERPeripheralAddressArray[(uint8_t)(timerNumber)] ;
}

/*
 * Function to get Timer Base address from ui32TIMERBaseAddressArray.
 * Arguments:
 *  TIMER_PERIPHERAL timerNumber        :: TIMERX where X is [0,TIMER_PERIPHERAL_COUNT-1].
 * Returns:
 *  uint32_t TIMER base address.
 *  TODO : implement similar function for wide timers.
 */
static uint32_t getTimerBaseAddress(TIMER_PERIPHERAL timerNumber)
{
    return ui32TIMERBaseAddressArray[(uint8_t)(timerNumber)] ;
}
