//
// LCD_keypad : 3x3 keypad input and display on LCD
//
// EVB : Nu-LB-NUC140
// MCU : NUC140VE3CN  (LQPF-100)
#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "LCD.h"
#include "Scankey.h"

int main(void)
{
	char Text[16] = "keypad:         ";
	uint8_t keyin;
	
	SYS_Init();
	  
	init_LCD();
	clear_LCD();
	
	OpenKeyPad();	              // initialize 3x3 keypad
	print_Line(0,"LCD_Keypad"); // print title
	 
	while(1)
	{
	  keyin = ScanKey(); 	         // scan keypad to input
		sprintf(Text+8,"%d", keyin); // print scankey input to string			  
		print_Line(1, Text);         // display string on LCD
		CLK_SysTickDelay(5000); 	   // delay 																	 
	}
}

