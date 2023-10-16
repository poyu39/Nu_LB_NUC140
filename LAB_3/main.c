#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "Seven_Segment.h"
#include "Scankey.h"

#define DO 3816
#define RE 3401
#define MI 3030
#define FA 2865
#define SO 2551
#define LA 2273
#define SI 2028
#define DO2 1912
#define RE2 1703
#define MI2 1517
#define FA2 1432
#define SO2 1275
#define LA2 1136
#define SI2 1012
#define DO3 956
#define RE3 851
#define MI3 758
#define FA3 716
#define SO3 637
#define LA3 568
#define SI3 506

int wait = 50000;

void init_GPIO(void){
    GPIO_SetMode(PC, BIT12, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PC, BIT13, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PC, BIT14, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PC, BIT15, GPIO_MODE_OUTPUT);
	PC12 =1; PC13 =1; PC14 =1; PC15 =1;
}

void show_7_seg(int PC7_value, int PC6_value, int PC5_value, int PC4_value){
	if (PC7_value != 0) {
		CloseSevenSegment();
		if (PC7_value != 8 ){
			ShowSevenSegment(3, PC7_value);
			CLK_SysTickDelay(2000);
		}
	}
	if (PC6_value != 0) {
		CloseSevenSegment();
		if (PC6_value != 8) {
			ShowSevenSegment(2, PC6_value);
			CLK_SysTickDelay(2000);
		}
	}
	if (PC5_value != 0) {
		CloseSevenSegment();
		if (PC5_value != 8) {
			ShowSevenSegment(1, PC5_value);
			CLK_SysTickDelay(2000);
		}
	}
	if (PC4_value != 0) {
		CloseSevenSegment();
		if(PC4_value == 8 || PC4_value == -1){}
		else{
			ShowSevenSegment(0, PC4_value);
			CLK_SysTickDelay(2000);
		}
	}
}

void dis_buz(int val[3], int buzz) {
	int t_buzz = buzz;
	int t_wait = 100000 / t_buzz ;
	int i;
	for (i = 0; i < t_wait; i++) {
		show_7_seg(val[3], val[2], val[1], val[0]);
		PB11 = 0;
		CLK_SysTickDelay(t_buzz / 10 * 2);
		PB11 = 1;
		CLK_SysTickDelay(t_buzz / 10 * 8);
	}
	PB11 = 1;
	CLK_SysTickDelay(wait);
}

void play_buzz(int val[3], int number) {
	switch (number) {
	case 1:
		dis_buz(val, DO3);
		dis_buz(val, RE3);
		dis_buz(val, MI3);
		break;
	case 2:
		dis_buz(val, MI3);
		dis_buz(val, RE3);
		dis_buz(val, DO3);
		break;
	case 3:
		dis_buz(val, DO3);
		dis_buz(val, DO3);
		dis_buz(val, DO3);
		break;
	default:
		break;
	}
}

void add_number(int val[4], uint16_t keyin){
	val[3] = val[2];
	val[2] = val[1];
	val[1] = val[0];
	val[0] = keyin;
}

void clear_number(int val[4]){
	val[0] = 8;
	val[1] = 8;
	val[2] = 8;
	val[3] = 8;
}

void del_number(int val[4]){
	val[0] = val[1];
	val[1] = val[2];
	val[2] = 8;
	val[3] = 8;
}


int main(void){
	int val[4] = {0, 0, 0, 0};
	int is_press = 0, k = 0, count = 0;
	SYS_Init();
	init_GPIO();
	OpenSevenSegment();
	OpenKeyPad();
	GPIO_SetMode(PB, BIT11, GPIO_MODE_OUTPUT);

	while (TRUE) {
		k = ScanKey();
		if (k == 0) {
			if (is_press == 1) {
				is_press = 0;
			}
		} else {
			if (is_press == 0) {
				if (k < 4 && k > 0) {
					if (count == 3) {
						val[2] = 0;
						count--;
					}
					add_number(val, k);
					count++;
				} else if (k == 4) {
					clear_number(val);
					count = 0;
				} else if (k == 5) {
					while (count > 0) {
						val[3] = val[count];
						val[count] = 8;
						count--;
						play_buzz(val, val[3]);
						val[3] = 0;
					}
					clear_number(val);
					count = 0;
				} else if (k == 6) {
					del_number(val);
					count--;
					if (count == 0) {
						val[0] = 8;
					}
				} else if (k == 7) {
					int val[4] = {0, 0, 0, 0};
					dis_buz(val, DO2);
					dis_buz(val, RE2);
					dis_buz(val, MI2);
					dis_buz(val, FA2);
					dis_buz(val, SO2);
					dis_buz(val, LA2);
					dis_buz(val, SI2);
				}
				is_press = 1;
			}
		}
		show_7_seg(val[3], val[2], val[1], val[0]);
	}
}
