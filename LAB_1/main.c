#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"

void led_F2R(void) {
    PC12 = 0;
    CLK_SysTickDelay(300000);
    PC12 = 1;
    PC13 = 0;
    CLK_SysTickDelay(300000);
    PC13 = 1;
    PC14 = 0;
    CLK_SysTickDelay(300000);
    PC14 = 1;
    PC15 = 0;
    CLK_SysTickDelay(300000);
    PC15 = 1;
    CLK_SysTickDelay(300000);
}

void led_R2F(void) {
    PC15 = 0;
    CLK_SysTickDelay(300000);
    PC15 = 1;
    PC14 = 0;
    CLK_SysTickDelay(300000);
    PC14 = 1;
    PC13 = 0;
    CLK_SysTickDelay(300000);
    PC13 = 1;
    PC12 = 0;
    CLK_SysTickDelay(300000);
    PC12 = 1;
    CLK_SysTickDelay(300000);
}

void led_full_on(void) {
    PC12 = 0;
    PC13 = 0;
    PC14 = 0;
    PC15 = 0;
    CLK_SysTickDelay(300000);
}

void led_full_off(void) {
    PC12 = 1;
    PC13 = 1;
    PC14 = 1;
    PC15 = 1;
    CLK_SysTickDelay(300000);
}

void lab(int lab_num) {
    if (lab_num == 11) {
        led_R2F();
    } else if (lab_num == 12) {
        led_F2R();
        led_R2F();
    } else if (lab_num == 13) {
        led_R2F();
        led_full_on();
        led_full_off();
        led_full_on();
        led_full_off();
        
        led_F2R();
        led_full_on();
        led_full_off();
        led_full_on();
        led_full_off();
    }
}


int main(void) {
	SYS_Init();

    // 設定 mode
    GPIO_SetMode(PC, BIT12, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PC, BIT13, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PC, BIT14, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PC, BIT15, GPIO_MODE_OUTPUT);
	
    // 0 為亮，1 為暗
	PC12 = 1;
    PC13 = 1;
    PC14 = 1;
    PC15 = 1;

    while(TRUE) {
        lab(13);
	}
}