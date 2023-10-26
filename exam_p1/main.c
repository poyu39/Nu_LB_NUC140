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

#define NID "D1009212"
#define MATH_SYMBOLS "+-*/%^^"

void init_GPIO(void){
    GPIO_SetMode(PC, BIT12, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PC, BIT13, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PC, BIT14, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PC, BIT15, GPIO_MODE_OUTPUT);
	PC12 = 1; PC13 = 1; PC14 = 1; PC15 = 1;
}

int show_7_seg (int PC_values[], int index_7seg) {
	if (index_7seg == 4) index_7seg = 0;
	while (PC_values[index_7seg] == 0) {
		index_7seg++;
		if (index_7seg == 4) index_7seg = 0;
	}
	if (PC_values[index_7seg] != 0) {
		CloseSevenSegment();
		ShowSevenSegment(index_7seg, PC_values[index_7seg]);
		CLK_SysTickDelay(DELAY_7SEG);
	}
	index_7seg++;
	return index_7seg;
}

int math(int x, int y, int i) {
	switch (i) {
	case 0:
		return x + y;
	case 1:
		return x - y;
	case 2:
		return x * y;
	case 3:
		return x / y;
	case 4:
		return x % y;
	case 5:
		return pow(x, 2);
	case 6:
		return pow(x, 2);
	default:
		return 0;
	}
}

void show_lcd(int x, int y, int math_op_index) {
	int i;
	print_Line(0, "D1009212");
	for (i = 0; i < 3; i++) {
		char line_text[16];
		int result = 0;
		int index = (math_op_index + i) % 7;
		if (index < 5) {
			result = math(x, y, index % 7);
			sprintf(line_text, "%d %c %d = %d", x, MATH_SYMBOLS[index], y, result);
		} else if (index == 5) {
			result = math(x, 2, index % 7);
			sprintf(line_text, "%d %c %d = %d", x, MATH_SYMBOLS[index], 2, result);
		} else if (index == 6) {
			result = math(y, 2, index % 7);
			sprintf(line_text, "%d %c %d = %d", y, MATH_SYMBOLS[index], 2, result);
		}
		print_Line(1 + i, line_text);
	}
}

void update_7seg(int *PC_values, int x, int y) {
	PC_values[3] = x / 10;
	PC_values[2] = x % 10;
	PC_values[1] = y / 10;
	PC_values[0] = y % 10;
}

int main(void) {
	uint8_t keyin=0, isPressed=1, index_7seg=0;
	int x=0, y=0, temp_x, math_op_index=0;
	int PC_values[4] = {0, 0, 0, 0};
	SYS_Init();
	init_LCD();
	init_GPIO();
	clear_LCD();
	OpenKeyPad();

	while (TRUE) {
		keyin = ScanKey();
		if (keyin != 0) {
			// 用 goto 節省每次迴圈執行
			if (isPressed == 1) goto show_7_seg;
			isPressed = 1;
			switch (keyin) {
			case 2:
				// 運算式向上滾動
				math_op_index = (math_op_index--) % 7;
				if (math_op_index < 0) math_op_index += 7;
				break;
			case 4:
				// 向左偏移, ex: 1234 -> 2341
				temp_x = x;
				x = (x % 10 * 10) + y / 10;
				y = (y % 10 * 10) + temp_x / 10;
				break;
			case 5:
				// 亂數
				x = rand() % 99 + 1;
				y = rand() % 99 + 1;
				break;
			case 6:
				// 向右偏移, ex: 1234 -> 4123
				temp_x = x;
				x = (x / 10) + (y % 10 * 10);
				y = (y / 10) + (temp_x % 10 * 10);
				break;
			case 8:
				// 運算式向下滾動
				math_op_index = (math_op_index++) % 7;
				break;
			default:
				break;
			}
			update_7seg(PC_values, x, y);
			clear_LCD();
			show_lcd(x, y, math_op_index);
		} else {
			isPressed = 0;
		}
		show_7_seg:
		if (x != 0 && y != 0) {
			index_7seg = show_7_seg(PC_values, index_7seg);
		}
	}
}

