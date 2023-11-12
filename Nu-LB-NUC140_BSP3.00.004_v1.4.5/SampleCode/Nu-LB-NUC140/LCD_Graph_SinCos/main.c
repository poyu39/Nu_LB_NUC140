//
// LCD_Graph_SinCos: draw Sine Wave & Cosine Wave on LCD 
//
// EVB : Nu-LB-NUC140
// MCU : NUC140VE3CN
//
#include <stdio.h>
#include <math.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "LCD.h"

#define  PI 3.14159265

int main(void)
{
	int x, y;
	SYS_Init();
	  
	init_LCD();
	clear_LCD();
	
	// draw Sine Wave
	printS_5x7(LCD_Xmax/2+30, (LCD_Ymax/2-1),"Sine");
	for (x=0; x<360; x++){
          y = sin(x * PI/180) * LCD_Ymax/2 + LCD_Ymax/2;
	  draw_Pixel(x/4, (LCD_Ymax-1)-y, FG_COLOR, BG_COLOR);
	}	

	// draw Cosine Wave
	printS_5x7(LCD_Xmax/2+30, 0,"CoSine");
	for (x=0; x<360; x++){
          y = cos(x * PI/180) * LCD_Ymax/2 + LCD_Ymax/2;
	  draw_Pixel(x/4, (LCD_Ymax-1)-y, FG_COLOR, BG_COLOR);
	}
}
