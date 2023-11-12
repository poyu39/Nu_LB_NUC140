//
// GPIO_LED16x16: GPIO outputs to control 16x16 Matrix LED display
//
// 16x16 LED Matrix Display module
// pin connections
// Vcc :              to VCC +5V
// Gnd :              to GND
// P24 : 74HC164 CLK  to PA4
// P23 : 74HC164 AB   to PA3 
// P22 : 74HC595 RCK  to PA2
// P21 : 74HC595 SRCK to PA1
// P20 : 74HC595 SER  to PA0 

#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "LED16x16.h"
#include "littlegreenman.h"

#define  REPEAT 100

int32_t main (void)
{
	uint8_t i,j;
	int16_t r;
	uint8_t dotmap[32];
	
    SYS_Init();
    init_LED16x16();

  	while(1) {		
	  	for (i=0;i<18;i++) { //18 greenman patterns
		    for(j=0;j<32;j++) dotmap[j]=littlegreenman[i*32+j]; // load little-green-man to dotmap
		    for(r=0;r<REPEAT;r++) display_LED16x16(dotmap);     // keep displaying dotmap on LED16x16
		  }
    }
}
