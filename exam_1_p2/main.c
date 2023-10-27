#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "LCD.h"
#include "Scankey.h"
#include "Seven_Segment.h"
#include <math.h>

#define DELAY_LED  250000			// 250ms
#define DELAY_SEVEN_SEG 5000		// 5ms
#define SPACE_16 "                "	// 16 空白
#define SPACE_12 "            "		// 12 空白
#define SPACE_8 "        "			// 8 空白
#define NID "D1009212"

void init_GPIO(void){
    GPIO_SetMode(PC, BIT12, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PC, BIT13, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PC, BIT14, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PC, BIT15, GPIO_MODE_OUTPUT);
	PC12=1; PC13=1; PC14=1; PC15=1;
}

void PC_control(int pin, int v) {
	switch (pin) {
	case 12:
		PC12 = v;
		break;
	case 13:
		PC13 = v;
		break;
	case 14:
		PC14 = v;
		break;
	case 15:
		PC15 = v;
		break;
	default:
		break;
	}
}

void win_led(void) {
    int i;
	for (i = 0; i < 4; i++) {
		PC_control(12 + i, 0);
		CLK_SysTickDelay(DELAY_LED);
		PC_control(12 + i, 1);
	}
	for (i = 0; i < 4; i++) {
		PC_control(15 - i, 0);
		CLK_SysTickDelay(DELAY_LED);
		PC_control(15 - i, 1);
	}
}

void update_seven_seg(int seven_seg_buffer[], uint16_t rand_pwd) {
	seven_seg_buffer[3] = rand_pwd / 1000;
	seven_seg_buffer[2] = (rand_pwd % 1000) / 100;
	seven_seg_buffer[1] = (rand_pwd % 100) / 10;
	seven_seg_buffer[0] = rand_pwd % 10;
}

int show_seven_seg(int seven_seg_buffer[], int seven_seg_index) {
	if (seven_seg_index == 4) seven_seg_index = 0;
	while (seven_seg_buffer[seven_seg_index] == 16) {
		seven_seg_index++;
		if (seven_seg_index == 4) seven_seg_index = 0;
	}
	if (seven_seg_buffer[seven_seg_index] != 0) {
		CloseSevenSegment();
		ShowSevenSegment(seven_seg_index, seven_seg_buffer[seven_seg_index]);
		CLK_SysTickDelay(DELAY_SEVEN_SEG);
	}
	seven_seg_index++;
	return seven_seg_index;
}

int get_ans(int input_pwd, int rand_pwd, int line_index, int win_line_index, char lcd_buffer[3][17]) {
	uint8_t a=0, b=0;
	int i;
	int temp_input_pwd = input_pwd;
	int temp_rand_pwd = rand_pwd;
	if (input_pwd == rand_pwd && input_pwd != 0) {
		sprintf(lcd_buffer[line_index], "%4d%s%4s", input_pwd, SPACE_8, "WIN");
		win_line_index = line_index;
	} else if (input_pwd == 0) {
		sprintf(lcd_buffer[line_index], "%s%4s", SPACE_12, "NULL");
		win_line_index = -1;
	} else if (input_pwd != rand_pwd) {
		for (i = 0; i < 4; i++) {
			if (temp_input_pwd % 10 == temp_rand_pwd % 10)
				a++;
			else
				b++;
			temp_input_pwd /= 10;
			temp_rand_pwd /= 10;
		}
		sprintf(lcd_buffer[line_index], "%4d%s%dA%dB", input_pwd, SPACE_8, a, b);
		win_line_index = -1;
	}
	return win_line_index;
}

void show_lcd(int lcd_x, int lcd_y, char lcd_buffer[3][17], char lcd_now[3][17]) {
	if (lcd_buffer[lcd_y][lcd_x] != lcd_now[lcd_y][lcd_x]) {
		lcd_now[lcd_y][lcd_x] = lcd_buffer[lcd_y][lcd_x];
		printC(lcd_x * 8, lcd_y* 16, lcd_now[lcd_y][lcd_x]);
	}
}

int get_rand_pwd() {
	int rand_pwd=0, i;
	for (i = 0; i < 4; i++)
		rand_pwd = rand_pwd * 10 + rand() % 6 + 1;
	return rand_pwd;
}

int main(void) {
	uint8_t keyin = 0,
			isPressed = 1,
			input_pwd_index = 0,
			line_index = 0,
			nid_index = 0,
			seven_seg_index = 0,
			win_line_index = 4,
			update_lcd = 0;
	uint16_t input_pwd = 0, rand_pwd = 0;
	char lcd_buffer[3][17] = {SPACE_16, SPACE_16, SPACE_16};
	char lcd_now[3][17] = {SPACE_16, SPACE_16, SPACE_16};
	int seven_seg_buffer[4] = {16, 16, 16, 16};
	int lcd_x = 0, lcd_y = 0;

	SYS_Init();
	init_LCD();
	init_GPIO();
	clear_LCD();
	OpenKeyPad();

	while (TRUE) {
		keyin = ScanKey();
		if (keyin != 0) {
			if (isPressed == 1) goto display;
			isPressed = 1;
		}
		switch (keyin) {
		case 0:
			isPressed = 0;
			break;
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
			if (input_pwd_index < 4) {
				input_pwd = input_pwd * 10 + keyin;
				input_pwd_index++;
				sprintf(lcd_buffer[line_index], "%4d%s", input_pwd, SPACE_12);
				update_lcd = 1;
			}
			break;
		case 7:
			// R 隨機
			rand_pwd = get_rand_pwd();
			update_seven_seg(seven_seg_buffer, rand_pwd);
			break;
		case 8:
			// C 初始狀態
			clear_LCD();
			input_pwd_index = 0;
			input_pwd = 0;
			line_index = 0;
			break;
		case 9:
			// A 解答
			win_line_index = get_ans(input_pwd, rand_pwd, line_index, win_line_index, lcd_buffer);
			line_index++;
			input_pwd_index = 0;
			input_pwd = 0;
			update_lcd = 1;
			break;
		default:
			break;
		}
		display:
		if (rand_pwd != 0)
			seven_seg_index = show_seven_seg(seven_seg_buffer, seven_seg_index);
		if (update_lcd == 1) {
			show_lcd(lcd_x, lcd_y, lcd_buffer, lcd_now);
			if (lcd_y < 3) {
				if (lcd_x < 16) {
					lcd_x++;
				} else {
					lcd_x = 0;
					lcd_y++;
				}
			}
			if (lcd_y == 3) {
				lcd_x = 0;
				lcd_y = 0;
				update_lcd = 0;
			}
		}
	}
}

