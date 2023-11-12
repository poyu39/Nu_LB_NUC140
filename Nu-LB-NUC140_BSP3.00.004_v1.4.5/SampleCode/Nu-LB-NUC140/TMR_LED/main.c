//
// TMR_LED : change LED on/off by Timer1 interrupt
//
#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"

volatile uint8_t ledState =0;

void TMR1_IRQHandler(void)
{	
	ledState = ~ ledState;  // changing ON/OFF state
  if(ledState) PC12=0;
  else         PC12=1;
  TIMER_ClearIntFlag(TIMER1); // Clear Timer1 time-out interrupt flag
}

void Init_Timer1(void)
{
  TIMER_Open(TIMER1, TMR1_OPERATING_MODE, TMR1_OPERATING_FREQ);
  TIMER_EnableInt(TIMER1);
  NVIC_EnableIRQ(TMR1_IRQn);
  TIMER_Start(TIMER1);
}

int main(void)
{
  SYS_Init();   // Intialize System/Peripheral clocks & multi-function I/Os

  GPIO_SetMode(PC, BIT12, GPIO_MODE_OUTPUT); // set LED GPIO pin
  Init_Timer1();

  while(1);
}

