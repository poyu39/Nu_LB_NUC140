/*
    NewBuzzer
    Author: poyu39
    Discord: poyu39
*/
#include <stdio.h>
#include "NUC100Series.h"
#include "GPIO.h"
#include "SYS.h"
#include "NewBuzzer.h"

volatile uint16_t play_time;

void TMR1_IRQHandler(void) {
    TIMER_ClearIntFlag(TIMER1);
    if (play_time > 0) {
        PB11 = 0;
        play_time--;
    }
    if (play_time == 0 && PB11 == 0) {
        PB11 = 1;
    }
}

void init_timer1(void) {
    TIMER_Open(TIMER1, TIMER_PERIODIC_MODE, 200); // 1ms
    TIMER_EnableInt(TIMER1);
    NVIC_EnableIRQ(TMR1_IRQn);
    TIMER_Start(TIMER1);
}

void init_buzzer(void) {
    init_timer1();
    play_time = 0;
}

void buzzer_play(uint16_t t) {
    if (play_time == 0) play_time = t;
}

uint8_t buzzer_is_playing(void) {
    return play_time > 0;
}
