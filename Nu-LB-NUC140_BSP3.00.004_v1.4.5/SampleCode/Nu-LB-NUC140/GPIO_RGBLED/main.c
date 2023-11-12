//
// GPIO_RGBLED: GPIO output to control R/G/B LED
//
// PA12 : NUC140VE3CN to control blue  LED
// PA13 : NUC140VE3CN to control green LED
// PA14 : NCU140VE3CN to control red   LED
#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"

void Init_GPIO(void)
{
	GPIO_SetMode(PA, BIT12, GPIO_MODE_OUTPUT);
	GPIO_SetMode(PA, BIT13, GPIO_MODE_OUTPUT);
	GPIO_SetMode(PA, BIT14, GPIO_MODE_OUTPUT);
	PA12=1;
	PA13=1;
	PA14=1;
}

int32_t main (void)
{
	SYS_Init();
	Init_GPIO();

  // PA12 = Blue,  0 : on, 1 : off
  // PA13 = Green, 0 : on, 1 : off
  // PA14 = Red,   0 : on, 1 : off	
  while(1){
		PA12=0; PA13=1; PA14=1;   // set RGB LED to Blue
		CLK_SysTickDelay(500000);	// delay 
		PA12=1; PA13=0; PA14=1;   // set RGB LED to Green
		CLK_SysTickDelay(500000);	// delay 
		PA12=1; PA13=1; PA14=0;   // set RGB LED to Red
		CLK_SysTickDelay(500000);	// delay 			
	}
}
