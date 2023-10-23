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

#define DELAY_LED 50000
#define DELAY_7SEG 5000
#define DELAY_LCD 50000
#define DELAY_KEYPAD 500000

void init_GPIO(void){
    GPIO_SetMode(PC, BIT12, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PC, BIT13, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PC, BIT14, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PC, BIT15, GPIO_MODE_OUTPUT);
	PC12 =1; PC13 =1;PC14 =1; PC15 =1;
}

void led_F2R(void) {
    PC12 = 0;
    CLK_SysTickDelay(DELAY_LED);
    PC12 = 1;
    PC13 = 0;
    CLK_SysTickDelay(DELAY_LED);
    PC13 = 1;
    PC14 = 0;
    CLK_SysTickDelay(DELAY_LED);
    PC14 = 1;
    PC15 = 0;
    CLK_SysTickDelay(DELAY_LED);
    PC15 = 1;
    CLK_SysTickDelay(DELAY_LED);
}

void led_R2F(void) {
    PC15 = 0;
    CLK_SysTickDelay(DELAY_LED);
    PC15 = 1;
    PC14 = 0;
    CLK_SysTickDelay(DELAY_LED);
    PC14 = 1;
    PC13 = 0;
    CLK_SysTickDelay(DELAY_LED);
    PC13 = 1;
    PC12 = 0;
    CLK_SysTickDelay(DELAY_LED);
    PC12 = 1;
    CLK_SysTickDelay(DELAY_LED);
}

void show_7_seg(int PC7_value, int PC6_value, int PC5_value, int PC4_value){
	if (PC7_value != 0) {
		CloseSevenSegment();
		if (PC7_value != 8){
			ShowSevenSegment(3, PC7_value);
			CLK_SysTickDelay(DELAY_7SEG);
		}
	}
	if (PC6_value != 0) {
		CloseSevenSegment();
		if (PC6_value != 8) {
			ShowSevenSegment(2, PC6_value);
			CLK_SysTickDelay(DELAY_7SEG);
		}
	}
	if (PC5_value != 0) {
		CloseSevenSegment();
		if (PC5_value != 8) {
			ShowSevenSegment(1, PC5_value);
			CLK_SysTickDelay(DELAY_7SEG);
		}
	}
	if (PC4_value != 0) {
		CloseSevenSegment();
		if (PC4_value != 8 || PC4_value != -1) {
			ShowSevenSegment(0, PC4_value);
			CLK_SysTickDelay(DELAY_7SEG);
		}
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
	uint8_t isPressed = 1, keyin_temp = 0, input_pwd_index = 0, line_index = 0;
	uint16_t input_pwd = 0, rand_pwd = 0;
	char line_text[16] = "        ";
	int i, j;

	SYS_Init();
	init_LCD();
	init_GPIO();
	clear_LCD();
	OpenKeyPad();

	while (TRUE) {
		keyin = ScanKey();
		if (keyin != 0) {
			if (isPressed == 0) {
				if (keyin > 0 && keyin < 7 && input_pwd_index < 4 && line_index < 4) {
					input_pwd = input_pwd * 10 + keyin;
					input_pwd_index++;
					sprintf(line_text, "%d      ", input_pwd);
					CLK_SysTickDelay(DELAY_KEYPAD);
					print_Line(line_index, line_text);
				}

				if (keyin == 7) {
					rand_pwd = 0;
					for (i = 0; i < 4; i++) {
						rand_pwd = rand_pwd * 10 + rand() % 6 + 1;
					}
				}

				if (keyin == 8) {
					clear_LCD();
					input_pwd_index = 0;
					input_pwd = 0;
					line_index = 0;
				}

				if (keyin == 9 && line_index < 4) {
					if (input_pwd == rand_pwd && input_pwd != 0) {
						strcat(line_text, "PASS");
						led_F2R();
						led_R2F();
						led_F2R();
						led_R2F();
						print_Line(line_index, line_text);
						strcpy(line_text, "          ");
						line_index++;
						input_pwd_index = 0;
						input_pwd = 0;
					} else if (input_pwd == 0) {
						strcat(line_text, "NULL");
						print_Line(line_index, line_text);
						strcpy(line_text, "          ");
						line_index++;
						input_pwd_index = 0;
						input_pwd = 0;
					} else if (input_pwd != rand_pwd) {
						strcat(line_text, "ERROR");
						for (i = 0; i < 50; i++) {
							PB11 = 0;
							CLK_SysTickDelay(3000);
							PB11 = 1;
							CLK_SysTickDelay(6000);
						}
						print_Line(line_index, line_text);
						strcpy(line_text, "          ");
						line_index++;
						input_pwd_index = 0;
						input_pwd = 0;
					}
				}
				isPressed = 1;
			}
		} else {
			isPressed = 0;
		}
		show_7_seg(rand_pwd / 1000, (rand_pwd % 1000) / 100, (rand_pwd % 100) / 10, rand_pwd % 10);
	}
}

