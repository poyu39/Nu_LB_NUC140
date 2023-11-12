//
// GPIO_LED8x8 : GPIO outputs to control MAX7219 to drive LED Matrix 8x8
// 
// EVB : Nu-LB-NUC140
// MCU : NUC140VE3CN

// MAX7219 pin connections
// VCC connected to Vcc
// GND connected to Gnd
// DIN connected to NUC140 GPA0
// CS  connected to NUC140 GPA1
// CLK connected to NUC140 GPA2
#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "MAX7219.h"

int main(void)
{
	uint8_t ascii;
    SYS_Init(); 
	  Init_MAX7219();
	
    while(1) {
			for(ascii=0;ascii<0x80;ascii++) {
       printC_MAX7219(ascii);
       CLK_SysTickDelay(500000);
      }  
		}
}
