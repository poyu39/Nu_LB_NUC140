#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "LCD.h"
#include "Scankey.h"
#include "Seven_Segment.h"
#include <math.h>

#define DELAY_LED  250000		// 250ms
#define DELAY_7SEG 5000			// 5ms
#define LINE_SPACE "        "	// 8 空白
#define NID "D1009212"

void init_GPIO(void){
    GPIO_SetMode(PC, BIT12, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PC, BIT13, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PC, BIT14, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PC, BIT15, GPIO_MODE_OUTPUT);
	PC12=1; PC13=1; PC14=1; PC15=1;
}

void win_led(void) {
	uint32_t leg_pc[4] = {PC12, PC13, PC14, PC15};
    int i;
	for (i = 0; i < 4; i++) {
		leg_pc[i] = 0;
		CLK_SysTickDelay(DELAY_LED);
		leg_pc[i] = 1;
	}
	for (i = 0; i < 4; i++) {
		leg_pc[3-i] = 0;
		CLK_SysTickDelay(DELAY_LED);
		leg_pc[3-i] = 1;
	}
}

void show_7seg (int PC_values[], int *index_7seg) {
	if (index_7seg == 4) index_7seg = 0;
	while (PC_values[*index_7seg] == 0) {
		index_7seg++;
		if (index_7seg == 4) index_7seg = 0;
	}
	if (PC_values[*index_7seg] != 0) {
		CloseSevenSegment();
		ShowSevenSegment(index_7seg, PC_values[*index_7seg]);
		CLK_SysTickDelay(DELAY_7SEG);
	}
	*index_7seg++;
}

void show_ans(int input_pwd, int rand_pwd, int line_index, int *win_line_index) {
	char line_text[17];
	uint8_t a=0, b=0;
	int i;
	if (input_pwd == rand_pwd && input_pwd != 0) {
		sprintf(line_text, "%4d%sWIN", input_pwd, LINE_SPACE);
		*win_line_index = line_index;
	} else if (input_pwd == 0) {
		sprintf(line_text, "    %sNULL", LINE_SPACE);
		*win_line_index = -1;
	} else if (input_pwd != rand_pwd) {
		for (i = 0; i < 4; i++) {
			if (input_pwd % 10 == rand_pwd % 10)
				a++;
			else
				b++;
			input_pwd /= 10;
			rand_pwd /= 10;
		}
		sprintf(line_text, "    %s%dA%dB", LINE_SPACE, a, b);
		*win_line_index = -1;
	}
	print_Line(line_index, line_text);
}

void update_7seg(int *PC_values, int rand_pwd) {
	PC_values[3] = rand_pwd / 1000;
	PC_values[2] = (rand_pwd % 1000) / 100;
	PC_values[1] = (rand_pwd % 100) / 10;
	PC_values[0] = rand_pwd % 10;
}

int get_rand_pwd() {
	int rand_pwd=0, i;
	for (i = 0; i < 4; i++)
		rand_pwd = rand_pwd * 10 + rand() % 6 + 1;
	return rand_pwd;
}

int main(void) {
	uint8_t keyin=0;
	uint8_t isPressed=1, input_pwd_index=0, line_index=0, nid_index=0, index_7seg=0;
	uint8_t PC_values[4] = {0, 0, 0, 0};
	uint16_t input_pwd=0, rand_pwd=0;
	uint8_t win_line_index=-1;
	int i;
	char line_text[17];

	SYS_Init();
	init_LCD();
	init_GPIO();
	clear_LCD();
	OpenKeyPad();

	while (TRUE) {
		keyin = ScanKey();
		if (keyin != 0) {
			if (isPressed == 1) goto show_7seg;
			isPressed = 1;
			switch (keyin) {
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
				if (line_index > 3) goto show_7seg;
				if (input_pwd_index < 4) {
					input_pwd = input_pwd * 10 + keyin;
					input_pwd_index++;
					sprintf(line_text, "%4d%s", input_pwd, LINE_SPACE);
					print_Line(line_index, line_text);
				}
				break;
			case 7:
				rand_pwd = get_rand_pwd();
				update_7seg(PC_values, rand_pwd);
				break;
			case 8:
				clear_LCD();
				input_pwd_index = 0;
				input_pwd = 0;
				line_index = 0;
			case 9:
				if (line_index > 3) goto show_7seg;
				show_ans(input_pwd, rand_pwd, line_index, &win_line_index);
				break;
			default:
				break;
			}
		} else {
			isPressed = 0;
		}
		show_7seg:
		show_7_seg(PC_values, &index_7seg);
		if (win_line_index != -1) {
			win_led();
			printC(14, win_line_index, NID[nid_index]);
			nid_index = (nid_index++) % 8;
		}
	}
}

