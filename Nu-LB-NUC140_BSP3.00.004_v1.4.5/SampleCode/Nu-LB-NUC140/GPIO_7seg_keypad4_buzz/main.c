//
// GPIO_7seg_keypad4_buzz : 
//     keypad input 4 digits and display on 7-segment LEDs
//     compare 4 digits to a passcode 
//     if input is equal to then passcode, then buzz twice
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

int main(void)
{
	int passcode = 1314;
	int i=3;
	int j=0;
	int k=0;
	int s=0;
	int code;	
	
    SYS_Init();

    OpenSevenSegment(); // for 7-segment
	  OpenKeyPad();       // for keypad
    GPIO_SetMode(PB, BIT11, GPIO_MODE_OUTPUT); // for Buzzer
	
 	  while(1) {
		  k=ScanKey();
			if (k!=0) {
				s = s*10 + k;			
	      printf("key=%d i=%d s=%d\n", k, i, s); // add retarget.c to library & set DEBUG_ENABLE_SEMIHOST in config.
				i--;
				if (i<0) {
					i=3;
					code=s;
					s=0;
          if (code==passcode) Buzz(2);					
			    for (j=0; j<50; j++) Display_7seg(code); // display code (50 times for vision)
				}
			}
			for (j=0; j<10; j++) Display_7seg(s); // display s (10 times for keypad debounce)
	  }
}
