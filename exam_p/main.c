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
#include "Seven_Segment.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define DELAY_7SEG 5000
#define DELAY_LCD 50000

void init_GPIO(void){
    GPIO_SetMode(PC, BIT12, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PC, BIT13, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PC, BIT14, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PC, BIT15, GPIO_MODE_OUTPUT);
	PC12 = 1; PC13 = 1; PC14 = 1; PC15 = 1;
}

void show_7_seg (int PC7_value, int PC6_value, int PC5_value, int PC4_value, int index_7seg) {
	switch (index_7seg) {
	case 0:
		if (PC7_value != 0) {
			CloseSevenSegment();
			if (PC7_value != 8){
				ShowSevenSegment(3, PC7_value);
				CLK_SysTickDelay(DELAY_7SEG);
			}
		}
		break;
	case 1:
		if (PC6_value != 0) {
			CloseSevenSegment();
			if (PC6_value != 8) {
				ShowSevenSegment(2, PC6_value);
				CLK_SysTickDelay(DELAY_7SEG);
			}
		}
	case 2:
		if (PC5_value != 0) {
			CloseSevenSegment();
			if (PC5_value != 8) {
				ShowSevenSegment(1, PC5_value);
				CLK_SysTickDelay(DELAY_7SEG);
			}
		}
	case 3:
		if (PC4_value != 0) {
			CloseSevenSegment();
			if (PC4_value != 8 || PC4_value != -1) {
				ShowSevenSegment(0, PC4_value);
				CLK_SysTickDelay(DELAY_7SEG);
			}
		}
		break;
	default:
		break;
	}
}

int math(int x, int y, int i) {
	switch (i) {
	case 0:
		return x + y;
		break;
	case 1:
		return x - y;
		break;
	case 2:
		return x * y;
		break;
	case 3:
		return x / y;
		break;
	default:
		break;
	}
}

int main(void) {
	uint8_t keyin = 0;
	uint8_t isPressed = 1;
	uint8_t num_ctr = 0;
	uint8_t index_7seg = 0;
	int x=0, y=0, i;
	char math_op[4] = {'+', '-', '*', '/'};
	SYS_Init();
	init_LCD();
	init_GPIO();
	clear_LCD();
	OpenKeyPad();

	while (TRUE) {
		keyin = ScanKey();
		if (keyin != 0) {
			if (isPressed == 0) {
				if (num_ctr == 0) {
					x = keyin;
					num_ctr++;
					isPressed = 1;
				} else {
					y = keyin;
					num_ctr++;
					isPressed = 1;
				}
			}
		} else {
			isPressed = 0;
		}
		if (num_ctr == 2) {
			clear_LCD();
			for (i = 0; i < 4; i++) {
				char line_text[16];
				sprintf(line_text, "%d %c %d = %d", x, math_op[i], y, math(x, y, i));
				print_Line(i, line_text);
			}
			num_ctr = 0;
		}
		show_7_seg(x, 0, 0, y, index_7seg);
		if (index_7seg < 4)
			index_7seg++;
		else
			index_7seg = 0;
	}
}

