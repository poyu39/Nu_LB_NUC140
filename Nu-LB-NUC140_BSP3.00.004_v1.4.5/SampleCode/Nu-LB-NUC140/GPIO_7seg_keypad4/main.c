//
// GPIO_7seg_keypad4 : keypad input 4 digits and display on 7-segment LEDs
//
#include <stdio.h>
#include <math.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "Seven_Segment.h"
#include "Scankey.h"

// display an integer on four 7-segment LEDs
void Display_7seg(uint16_t value)
{
  uint8_t digit;
	digit = value / 1000;
	CloseSevenSegment();
	ShowSevenSegment(3,digit);
	CLK_SysTickDelay(5000);
			
	value = value - digit * 1000;
	digit = value / 100;
	CloseSevenSegment();
	ShowSevenSegment(2,digit);
	CLK_SysTickDelay(5000);

	value = value - digit * 100;
	digit = value / 10;
	CloseSevenSegment();
	ShowSevenSegment(1,digit);
	CLK_SysTickDelay(5000);

	value = value - digit * 10;
	digit = value;
	CloseSevenSegment();
	ShowSevenSegment(0,digit);
	CLK_SysTickDelay(5000);
}

int main(void)
{
	int i=3;
	int j=0;
	int k=0;
	int s=0;
	int code;	
	
    SYS_Init();

    OpenSevenSegment();
	  OpenKeyPad();

 	  while(1) {
		  k=ScanKey();
			if (k!=0) {
				s = s*10 + k;			
	      printf("key=%d i=%d s=%d\n", k, i, s);
				i--;
				if (i<0) {
					i=3;
					code=s;
					s=0;					
			    for (j=0; j<50; j++) Display_7seg(code);
				}
			}
			for (j=0; j<10; j++) Display_7seg(s);
	  }
}
