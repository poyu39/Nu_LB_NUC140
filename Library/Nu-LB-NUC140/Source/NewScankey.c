/*
    NewScankey
    Author: 邱柏宇
    Discord: poyu39
*/
#include <stdio.h>
#include <NUC100Series.h>
#include "GPIO.h"
#include "SYS_init.h"

// 哪個按鈕被按下
volatile uint8_t KEY_FLAG;

/*
        PA2   PA1   PA0
    PA3   1     2     3
    PA4   4     5     6
    PA5   7     8     9
*/
void GPAB_IRQHandler(void) {
    uint8_t i, which_PA_INT;
    // 查看是哪個 PA 觸發中斷
    if (PA->ISRC & BIT0) which_PA_INT = 0;
    if (PA->ISRC & BIT1) which_PA_INT = 1;
    if (PA->ISRC & BIT2) which_PA_INT = 2;
    
    // 檢查是哪個按鈕被按下
    // 將所有 PA 腳位設為 1，接下來輪流將PA3~5設為 0，並檢查是否有按鈕被按下。
    PA0 = 1; PA1 = 1; PA2 = 1; PA3 = 1; PA4 = 1; PA5 = 1;
    for (i = 3; i <= 5; i++) {
        GPIO_PIN_DATA(0, i - 1) = 1;
        while (GPIO_PIN_DATA(0, i - 1) != 1);
        GPIO_PIN_DATA(0, i) = 0;
        while (GPIO_PIN_DATA(0, i) != 0);
        if (GPIO_PIN_DATA(0, which_PA_INT) == 0) {
            KEY_FLAG = (3 - which_PA_INT) + 3 * (i - 3);
            break;
        }
    }
    PA0 = 1; PA1 = 1; PA2 = 1; PA3 = 0; PA4 = 0; PA5 = 0;
    PA->ISRC = PA->ISRC;
    return;
}

// 初始化 keypad interrupt 模式
void init_keypad_INT(void) {
    GPIO_SetMode(PA, (BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5), GPIO_MODE_QUASI);
    GPIO_EnableInt(PA, 0, GPIO_INT_FALLING);
    GPIO_EnableInt(PA, 1, GPIO_INT_FALLING);
    GPIO_EnableInt(PA, 2, GPIO_INT_FALLING);
    NVIC_EnableIRQ(GPAB_IRQn);
    NVIC_SetPriority(GPAB_IRQn, 3);
    GPIO_SET_DEBOUNCE_TIME(GPIO_DBCLKSRC_LIRC, GPIO_DBCLKSEL_128);
    GPIO_ENABLE_DEBOUNCE(PA, (BIT0 | BIT1 | BIT2));
    PA0 = 1; PA1 = 1; PA2 = 1; PA3 = 0; PA4 = 0; PA5 = 0;
}
