//
// TMR_Delay : using Timer to count number of us delay
//
#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"

int main(void)
{
	SYS_Init();   // Intialize System/Peripheral clocks & multi-function I/Os

  printf("\nThis sample code use timer to create a small delay \n");
  while(1) {
    printf("Delay 1 second\n");
    TIMER_Delay(TIMER0, 1000000);
  }
}
