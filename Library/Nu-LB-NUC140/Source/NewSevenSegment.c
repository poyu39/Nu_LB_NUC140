/*
    NewSevenSegment
    Author: poyu39
    Discord: poyu39
*/
#include <stdio.h>
#include "NUC100Series.h"
#include "GPIO.h"
#include "SYS.h"
#include "NewSevenSegment.h"

#define SEG_NONE 0xFF
#define SEG_N0 0x82
#define SEG_N1 0xEE
#define SEG_N2 0x07
#define SEG_N3 0x46
#define SEG_N4 0x6A
#define SEG_N5 0x52
#define SEG_N6 0x12
#define SEG_N7 0xE6
#define SEG_N8 0x02
#define SEG_N9 0x62
#define SEG_N10 0x22
#define SEG_N11 0x1A
#define SEG_N12 0x93
#define SEG_N13 0x0E
#define SEG_N14 0x13
#define SEG_N15 0x33

uint8_t seven_segment_map[17] = {SEG_NONE, SEG_N0, SEG_N1, SEG_N2, SEG_N3, SEG_N4, SEG_N5, SEG_N6, SEG_N7, SEG_N8, SEG_N9, SEG_N10, SEG_N11, SEG_N12, SEG_N13, SEG_N14, SEG_N15};

// 用來儲存七段顯示器3~0的數字
int8_t seven_segment_buffer[4] = {-1, -1, -1, -1};

// 用來控制是否開啟七段顯示器3~0的輪播功能
uint8_t SEG_LOOP = 1;

volatile uint8_t seven_segment_index = 0;

/**
  * @brief                          將七段顯示器的數字設定為指定的數字
  *
  * @param[in]  no                  七段顯示器的編號： 從左到右為 3-0
  * @param[in]  sn                  要顯示的數字或英文字母： -1 不顯示任何數字，0-9 數字， 10-15 英文字母A-F。
  *
  */
void show_seven_segment(uint8_t no, int8_t sn) {
    PE->DOUT = seven_segment_map[sn + 1];
    GPIO_PIN_DATA(2, no + 4) = 1;
}

// 關閉七段顯示器
void close_seven_segment(void) {
    PC4 = 0; PC5 = 0; PC6 = 0; PC7 = 0;
}

void TMR0_IRQHandler(void) {
    TIMER_ClearIntFlag(TIMER0);
    if (SEG_LOOP & FALSE) return;
    close_seven_segment();
    show_seven_segment(seven_segment_index, seven_segment_buffer[seven_segment_index]);
    seven_segment_index = (seven_segment_index + 1) % 4;
}

void init_timer0(uint8_t timer_hz) {
    TIMER_Open(TIMER0, TIMER_PERIODIC_MODE, timer_hz);
    TIMER_EnableInt(TIMER0);
    NVIC_EnableIRQ(TMR0_IRQn);
    TIMER_Start(TIMER0);
}

/**
  * @brief                          初始化七段顯示器
  *
  * @param[in]  use_timer           是否使用 timer 來控制七段顯示器的輪播功能
  * @param[in]  timer_hz            timer 的頻率
  *
  * @note                           timer 輪播為使用 timer0 來控制七段顯示器的輪播功能
  */
void init_seven_segment(uint8_t use_timer, uint8_t timer_hz) {
    GPIO_SetMode(PC, (BIT4 | BIT5 | BIT6 | BIT7), GPIO_PMD_OUTPUT);
    PC4 = 0; PC5 = 0; PC6 = 0; PC7 = 0;
    GPIO_SetMode(PE, (BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT6 | BIT7), GPIO_PMD_QUASI);
    PE0 = 0; PE1 = 0; PE2 = 0; PE3 = 0; PE4 = 0; PE5 = 0; PE6 = 0; PE7 = 0;
    if (use_timer) init_timer0(timer_hz);
}
