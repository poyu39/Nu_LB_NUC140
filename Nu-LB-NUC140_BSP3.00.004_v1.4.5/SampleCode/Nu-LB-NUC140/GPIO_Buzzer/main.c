//
// GPIO_Buzzer: GPIO output to drive Buzzer
//
// EVB : Nu-LB-NUC140
// MCU : NUC140VE3CN

// PB11 : NUC140 LQFP100 pin48

#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"

void Buzz(int number)
{
	int i;
	for (i=0; i<number; i++) {
      PB11=0; // PB11 = 0 to turn on Buzzer
	  CLK_SysTickDelay(100000);	 // Delay 
	  PB11=1; // PB11 = 1 to turn off Buzzer	
	  CLK_SysTickDelay(100000);	 // Delay 
	}
}

int32_t main (void)
{
    SYS_Init();
	
	  GPIO_SetMode(PB, BIT11, GPIO_MODE_OUTPUT);
	
	  Buzz(2); // Buzz 2 times
}
