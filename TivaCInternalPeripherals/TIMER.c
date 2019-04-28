
#include "TIMER.h"

volatile static uint64_t ticks = 0;

#ifdef USE_TIMER0_FOR_SYSTEM
static TIMERDEVICE systemTimer ;
#endif








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
    TIMERDEVICEPointer->TimerHalfWidthPart = TIMER_FULL ;
    TimerConfigure(TIMERDEVICEPointer->TIMERBase, TIMER_CFG_PERIODIC) ;
    TimerLoadSet(TIMERDEVICEPointer->TIMERBase, TIMER_A, SysCtlClockGet()/timerEventRepeatFrequency-1) ;
    TimerIntRegister(TIMERDEVICEPointer->TIMERBase, TIMER_A, timerEventFunction) ;
    TimerIntEnable(TIMERDEVICEPointer->TIMERBase, TIMER_TIMA_TIMEOUT) ;
}

extern void initTimerHalfWidthPeriodicInterrupt(TIMERDEVICE *TIMERDEVICEPointer,
                                                TIMER_PERIPHERAL timerNumber,
                                                TIMER_AB timerHalfWidthPart,
                                                uint16_t timerEventRepeatFrequency,
                                                void(*timerEventFunction)(void))
{
    if(timerHalfWidthPart == TIMER_FULL)
    {
        initTimerFullWidthPeriodic(TIMERDEVICEPointer, timerNumber, timerEventRepeatFrequency, timerEventFunction) ;
        return ;
    }
    if(!SysCtlPeripheralReady(getTimerPeripheralAddress(timerNumber)))
    {
        SysCtlPeripheralEnable(getTimerPeripheralAddress(timerNumber)) ;
        while(!SysCtlPeripheralReady(getTimerPeripheralAddress(timerNumber))) ;
    }
    TIMERDEVICEPointer->TIMERBase = getTimerBaseAddress(timerNumber) ;
    TIMERDEVICEPointer->timeEventFunction = timerEventFunction ;
    TIMERDEVICEPointer->timerEventRepeatFrequency = timerEventRepeatFrequency ;
    TIMERDEVICEPointer->TimerHalfWidthPart = timerHalfWidthPart ;
    uint32_t loadValue = SysCtlClockGet()/256/timerEventRepeatFrequency - 1 ;

    TimerConfigure(TIMERDEVICEPointer->TIMERBase, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PERIODIC | TIMER_CFG_B_PERIODIC ) ;
    TimerPrescaleSet(TIMERDEVICEPointer->TIMERBase, TIMERDEVICEPointer->TimerHalfWidthPart, 0xFF) ;
    TimerLoadSet(TIMERDEVICEPointer->TIMERBase, TIMERDEVICEPointer->TimerHalfWidthPart,loadValue)  ;
    TimerIntRegister(TIMERDEVICEPointer->TIMERBase, TIMERDEVICEPointer->TimerHalfWidthPart, timerEventFunction) ;
    TimerIntEnable(TIMERDEVICEPointer->TIMERBase, getTimerHalfTimeoutPart(timerHalfWidthPart)) ;
}

extern void setTimerEnableDisable(TIMERDEVICE *TIMERDEVICEPointer, TIMER_EN_DIS timerState)
{
    uint32_t timerPart = TIMERDEVICEPointer->TimerHalfWidthPart ;
    if(timerPart == TIMER_FULL)
    {
        timerPart = TIMER_HALF_A ;
    }
    if(timerState == TIMER_ENABLE)
    {
        TimerEnable(TIMERDEVICEPointer->TIMERBase, timerPart) ;
    }
    else if (timerState == TIMER_DISABLE)
    {
        TimerDisable(TIMERDEVICEPointer->TIMERBase, timerPart) ;
    }
}
/*
 * Function to initiate System Timer
 * Arguments:
 *  none.
 * Returns:
 *  none.
 */
extern void initSystemTimer(void)
{
#ifdef USE_TIMER0_FOR_SYSTEM
    initTimerFullWidthPeriodic(&systemTimer, TIMER0, SYSTEM_TIMER_FREQUENCY, systemTimerInterruptHandler) ;
    setTimerEnableDisable(&systemTimer, TIMER_ENABLE) ;
#endif
#ifdef USE_SYSTICK_FOR_SYSTEM
    uint32_t systemPeriod = SysCtlClockGet()/SYSTEM_TIMER_FREQUENCY  ;
    SysTickPeriodSet(systemPeriod);
    SysTickIntRegister(systemTimerInterruptHandler) ;
    SysTickIntEnable() ;
    SysTickEnable() ;
#endif
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
#ifdef USE_TIMER0_FOR_SYSTEM
    uint32_t status = TimerIntStatus(systemTimer.TIMERBase,true);
    TimerIntClear(systemTimer.TIMERBase,status);
#endif
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

static uint32_t getTimerHalfTimeoutPart(TIMER_AB timerHalfWidthPart)
{
    switch(timerHalfWidthPart)
    {
    case TIMER_HALF_A :
    case TIMER_FULL :
        return TIMER_TIMA_TIMEOUT ;
    case TIMER_HALF_B :
        return TIMER_TIMB_TIMEOUT ;
    }
    return 100 ;
}
