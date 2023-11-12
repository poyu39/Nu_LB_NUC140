//
// LCD_Graph2D : draw line, circle, triangle, rectangle onto LCD
//
// EVB : Nu-LB-NUC140
// MCU : NUC140VE3CN
#include <stdio.h>
#include <math.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "LCD.h"
#include "Draw2D.h"

#define  PI 3.14159265

int main(void)
{
	SYS_Init();
	  
	init_LCD();
	clear_LCD();

	draw_Rectangle(0,0,127,63, FG_COLOR, BG_COLOR); // draw a rectangle	
	fill_Rectangle(10,10,20,20,FG_COLOR, BG_COLOR); // draw a box
	draw_Circle(63,30,20,FG_COLOR, BG_COLOR);	 // draw a circle
	draw_Line(20,0,60,50,FG_COLOR, BG_COLOR);  // draw a line
	draw_Triangle(20,40,90,10,60,60,FG_COLOR, BG_COLOR);// draw a triangle
}
