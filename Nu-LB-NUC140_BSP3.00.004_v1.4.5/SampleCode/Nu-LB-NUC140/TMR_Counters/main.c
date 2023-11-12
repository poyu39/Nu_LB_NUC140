//
// TMR_Counters : using one Timer to generate 4 counters
//
#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"

volatile uint32_t counter[4] ={0,0,0,0};

void TMR0_IRQHandler(void)
{
	counter[0]++;
  counter[1]++;
  counter[2]++;
  counter[3]++;
  if (counter[0]>=10) counter[0]=0;
  if (counter[1]>=20) counter[1]=0;	
  if (counter[2]>=30) counter[2]=0;
  if (counter[3]>=40) counter[3]=0;
	
  printf("%d %d %d %d\n", counter[0], counter[1], counter[2], counter[3]);
  TIMER_ClearIntFlag(TIMER0);
}

void Init_Timer0(void)
{
  TIMER_Open(TIMER0, TMR0_OPERATING_MODE, TMR0_OPERATING_FREQ);
  TIMER_EnableInt(TIMER0);
  NVIC_EnableIRQ(TMR0_IRQn);
  TIMER_Start(TIMER0);
}

int main(void)
{
  SYS_Init();
  Init_Timer0();

  while(1);
}

