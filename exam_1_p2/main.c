#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "LCD.h"
#include "Scankey.h"
#include "Seven_Segment.h"
#include <math.h>

#define NID_UPDARE_TICK 500
#define LED_UPDATE_TICK 250
#define SEVEN_SEG_UPDATE_TICK 5
#define TICK_PER_MS 1000
#define SPACE_16 "                "
#define SPACE_12 "            "
#define NID "D1009212"

const uint8_t led_mode[11][4] = {
	{0, 1, 1, 1},
	{1, 0, 1, 1},
	{1, 1, 0, 1},
	{1, 1, 1, 0},
	{1, 1, 1, 0},
	{1, 1, 1, 1},
	{1, 1, 1, 0},
	{1, 1, 0, 1},
	{1, 0, 1, 1},
	{0, 1, 1, 1},
	{1, 1, 1, 1}
};

void init_GPIO(void){
    GPIO_SetMode(PC, BIT12, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PC, BIT13, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PC, BIT14, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PC, BIT15, GPIO_MODE_OUTPUT);
	PC12 = 1; PC13 = 1; PC14 = 1; PC15 = 1;
}

void win_led(uint8_t led_index) {
	PC12 = led_mode[led_index][0];
	PC13 = led_mode[led_index][1];
	PC14 = led_mode[led_index][2];
	PC15 = led_mode[led_index][3];
}

void update_seven_seg(int seven_seg_buffer[], uint16_t rand_pwd) {
	seven_seg_buffer[3] = rand_pwd / 1000;
	seven_seg_buffer[2] = (rand_pwd % 1000) / 100;
	seven_seg_buffer[1] = (rand_pwd % 100) / 10;
	seven_seg_buffer[0] = rand_pwd % 10;
}

void show_seven_seg(int seven_seg_buffer[], int seven_seg_index) {
	if (seven_seg_index == 4) seven_seg_index = 0;
	while (seven_seg_buffer[seven_seg_index] == 16) {
		seven_seg_index++;
		if (seven_seg_index == 4) seven_seg_index = 0;
	}
	if (seven_seg_buffer[seven_seg_index] != 0) {
		CloseSevenSegment();
		ShowSevenSegment(seven_seg_index, seven_seg_buffer[seven_seg_index]);
	}
}

int get_ans(int input_pwd, int rand_pwd, int line_index, int win_line_index, char lcd_buffer[4][17]) {
	uint8_t a=0, b=0;
	int i;
	int temp_input_pwd = input_pwd;
	int temp_rand_pwd = rand_pwd;
	if (input_pwd == rand_pwd && input_pwd != 0) {
		sprintf(lcd_buffer[line_index], "%4d      %-4s  ", input_pwd, "WIN");
		win_line_index = line_index;
	} else if (input_pwd == 0) {
		sprintf(lcd_buffer[line_index], "          %-4s  ", "NULL");
		if (win_line_index != 4) {
			lcd_buffer[win_line_index][14] = ' ';
		}
		win_line_index = 4;
	} else if (input_pwd != rand_pwd) {
		for (i = 0; i < 4; i++) {
			if (temp_input_pwd % 10 == temp_rand_pwd % 10)
				a++;
			else
				b++;
			temp_input_pwd /= 10;
			temp_rand_pwd /= 10;
		}
		sprintf(lcd_buffer[line_index], "%4d      %dA%dB  ", input_pwd, a, b);
		if (win_line_index != 4) {
			lcd_buffer[win_line_index][14] = ' ';
		}
		win_line_index = 4;
	}
	return win_line_index;
}

void win_show_nid(uint8_t win_line_index, uint8_t nid_index, char lcd_buffer[4][17]) {
	lcd_buffer[win_line_index][14] = NID[nid_index];
}

void show_lcd(int lcd_x, int lcd_y, char lcd_buffer[4][17], char lcd_now[4][17]) {
	if (lcd_buffer[lcd_y][lcd_x] != lcd_now[lcd_y][lcd_x]) {
		lcd_now[lcd_y][lcd_x] = lcd_buffer[lcd_y][lcd_x];
		printC(lcd_x * 8, lcd_y* 16, lcd_now[lcd_y][lcd_x]);
	}
}

int get_rand_pwd(int loop_count) {
	int rand_pwd=0, i;
	srand(loop_count);
	for (i = 0; i < 4; i++)
		rand_pwd = rand_pwd * 10 + rand() % 6 + 1;
	return rand_pwd;
}

void clear_lcd_buffer(char lcd_buffer[4][17]) {
	int i;
	for (i = 0; i < 4; i++) {
		sprintf(lcd_buffer[i], SPACE_16);
	}
}

int main(void) {
	uint8_t keyin = 0,
			isPressed = 1,
			input_pwd_index = 0,
			line_index = 0,
			seven_seg_index = 0,
			win_line_index = 4,
			led_index = 0,
			nid_index = 0,
			update_lcd = 0;
	int input_pwd = 0, rand_pwd = 0;
	char lcd_buffer[4][17] = {SPACE_16, SPACE_16, SPACE_16, SPACE_16};
	char lcd_now[4][17] = {SPACE_16, SPACE_16, SPACE_16, SPACE_16};
	int seven_seg_buffer[4] = {16, 16, 16, 16};
	int lcd_x = 0, lcd_y = 0;
	int loop_count = 0;

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
			if (input_pwd_index < 4 && line_index < 4) {
				input_pwd = input_pwd * 10 + keyin;
				input_pwd_index++;
				sprintf(lcd_buffer[line_index], "%4d%s", input_pwd, SPACE_12);
				update_lcd = 1;
			}
			break;
		case 7:
			rand_pwd = get_rand_pwd(loop_count);
			update_seven_seg(seven_seg_buffer, rand_pwd);
			break;
		case 8:
			clear_lcd_buffer(lcd_buffer);
			update_lcd = 1;
			input_pwd_index = 0;
			input_pwd = 0;
			line_index = 0;
			break;
		case 9:
			if (line_index < 4) {
				win_line_index = get_ans(input_pwd, rand_pwd, line_index, win_line_index, lcd_buffer);
				if (win_line_index == 4) {
					PC12 = 1; PC13 = 1; PC14 = 1; PC15 = 1;
					led_index = 0;
				}
				line_index++;
				input_pwd_index = 0;
				input_pwd = 0;
				update_lcd = 1;
			}
			break;
		default:
			break;
		}

		display:
		if (win_line_index != 4 && loop_count % LED_UPDATE_TICK == 0) {
			win_led(led_index);
			led_index++;
			if (led_index == 11) led_index = 0;
			if (loop_count % NID_UPDARE_TICK == 0) {
				win_show_nid(win_line_index, nid_index, lcd_buffer);
				update_lcd = 1;
				nid_index++;
				if (nid_index == 8) nid_index = 0;
			}
		}

		if (rand_pwd != 0 && loop_count % SEVEN_SEG_UPDATE_TICK == 0) {
			show_seven_seg(seven_seg_buffer, seven_seg_index);
			seven_seg_index++;
			if (seven_seg_index == 4) seven_seg_index = 0;
		}

		if (update_lcd == 1) {
			show_lcd(lcd_x, lcd_y, lcd_buffer, lcd_now);
			if (lcd_y < 4) {
				if (lcd_x < 16) {
					lcd_x++;
				} else {
					lcd_x = 0;
					lcd_y++;
				}
			}
			if (lcd_y == 4) {
				lcd_x = 0;
				lcd_y = 0;
				update_lcd = 0;
			}
		}

		if ((loop_count + 1) > INT32_MAX) loop_count = 0;
		loop_count++;
		CLK_SysTickDelay(TICK_PER_MS);
	}
}
