#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"

void TMR0_IRQHandler(void) {
    // pc12 set to pb8(toggle timer output)
    PC12 = PB8;
    TIMER_ClearIntFlag(TIMER0);
}

void Init_Timer0(void) {
    TIMER_Open(TIMER0, TMR0_OPERATING_MODE, TMR0_OPERATING_FREQ);
    TIMER_EnableInt(TIMER0);
    NVIC_EnableIRQ(TMR0_IRQn);
    TIMER_Start(TIMER0);
}

int main(void) {
    SYS_Init();
    Init_Timer0();
    
    while (TRUE);
}
