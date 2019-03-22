#include "stdint.h"
#include "stdbool.h"
#include "PERIPHERALS.h"
#include "GPIO.h"


/**
 * main.c
 */
int main(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ) ;
    GPIO redLED ;
    initGPIO(&redLED, GPIOF1, OUTPUT) ;
    while(true)
    {
        digitalWrite(&redLED, HIGH) ;
        SysCtlDelay(SysCtlClockGet()/12);
        digitalWrite(&redLED, LOW) ;
        SysCtlDelay(SysCtlClockGet()/12);
    }
}
