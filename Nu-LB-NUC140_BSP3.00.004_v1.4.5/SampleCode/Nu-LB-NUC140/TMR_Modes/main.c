//
// TMR_Modes : 4 Timers running different modes
//
#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"

volatile uint8_t ledState[4] ={0,0,0,0};

void TMR0_IRQHandler(void)
{
  ledState[0] = ~ ledState[0];  // changing ON/OFF state
  if(ledState[0]) PC12=0;
  else            PC12=1;
  TIMER_ClearIntFlag(TIMER0); // Clear Timer0 time-out interrupt flag
}

void TMR1_IRQHandler(void)
{
  ledState[1] = ~ ledState[1];  // changing ON/OFF state
  if(ledState[1]) PC13=0;
  else            PC13=1;
  TIMER_ClearIntFlag(TIMER1); // Clear Timer1 time-out interrupt flag
}

void TMR2_IRQHandler(void)
{
  ledState[2] = ~ ledState[2];  // changing ON/OFF state
  if(ledState[2]) PC14=0;
  else            PC14=1;
  TIMER_ClearIntFlag(TIMER2); // Clear Timer2 time-out interrupt flag
}

void TMR3_IRQHandler(void)
{
  ledState[3] = ~ ledState[3];  // changing ON/OFF state
  if(ledState[3]) PC15=0;
  else            PC15=1;
  TIMER_ClearIntFlag(TIMER3); // Clear Timer3 time-out interrupt flag
}

void Init_Timer0(void)
{
  TIMER_Open(TIMER0, TMR0_OPERATING_MODE, TMR0_OPERATING_FREQ);
  TIMER_EnableInt(TIMER0);
  NVIC_EnableIRQ(TMR0_IRQn);
  TIMER_Start(TIMER0);	
}

void Init_Timer1(void)
{
  TIMER_Open(TIMER1, TMR1_OPERATING_MODE, TMR1_OPERATING_FREQ);
  TIMER_EnableInt(TIMER1);
  NVIC_EnableIRQ(TMR1_IRQn);
  TIMER_Start(TIMER1);	
}

void Init_Timer2(void)
{
	TIMER_Open(TIMER2, TMR2_OPERATING_MODE, TMR2_OPERATING_FREQ);
  TIMER_EnableInt(TIMER2);
  NVIC_EnableIRQ(TMR2_IRQn);
  TIMER_Start(TIMER2);	
}

void Init_Timer3(void)
{
	TIMER_Open(TIMER3, TMR3_OPERATING_MODE, TMR3_OPERATING_FREQ);
  TIMER_EnableInt(TIMER3);
  NVIC_EnableIRQ(TMR3_IRQn);
  TIMER_Start(TIMER3);	
}

int main(void)
{
  SYS_Init();   // Intialize System/Peripheral clocks & multi-function I/Os

	GPIO_SetMode(PC, (BIT12 | BIT13 | BIT14 | BIT15), GPIO_MODE_OUTPUT); // set LED GPIO pin
	 
	Init_Timer0();
	Init_Timer1();
	Init_Timer2();
	Init_Timer3();
		
  while(1);
}

