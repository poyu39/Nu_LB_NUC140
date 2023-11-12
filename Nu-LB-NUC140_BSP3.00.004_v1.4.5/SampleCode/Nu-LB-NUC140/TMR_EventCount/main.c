//
// TMR_EventCount : using Timer0 to count times of external event
//
// EVB : Nu-LB-NUC140
// MCU : NUC140VE3CN

// TM0/PB8 connected to PB0

#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"

uint32_t Timer_count =0;

void TMR0_IRQHandler(void)
{
    Timer_count++;
    TIMER_ClearIntFlag(TIMER0);
    TIMER_Start(TIMER0);	
}

void Init_Timer(void)
{
    TIMER_Open(TIMER0, TMR0_OPERATING_MODE, TMR0_OPERATING_FREQ);
    TIMER_SET_PRESCALE_VALUE(TIMER0, 0);
    TIMER_SET_CMP_VALUE(TIMER0, 1000); // set compared value to 1000
    TIMER_EnableEventCounter(TIMER0, TMR0_COUNTING_EDGE);
    TIMER_EnableInt(TIMER0);
    NVIC_EnableIRQ(TMR0_IRQn);	
    TIMER_Start(TIMER0);	
}

int main(void)
{
    uint16_t i;
    SYS_Init();
    Init_Timer();

    GPIO_SetMode(PB, BIT0, GPIO_MODE_OUTPUT);
    PB0=0;    
	
    for(i = 0; i <5000; i++) {
        PB0=0;
			  CLK_SysTickDelay(10);
        PB0=1;
			  CLK_SysTickDelay(10);			
    }
		PB0=0;
		
		printf("Timer0 EVentCount = 1000 x %d\n", Timer_count);
		
		while(1);
}
