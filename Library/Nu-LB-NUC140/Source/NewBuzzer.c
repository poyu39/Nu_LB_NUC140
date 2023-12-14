/*
    NewBuzzer
    Author: 邱柏宇
    Discord: poyu39
*/
#include <stdio.h>
#include "NUC100Series.h"
#include "SYS_init.h"
#include "GPIO.h"
#include "SYS.h"
#include "NewBuzzer.h"

volatile uint16_t play_time = 0;
volatile uint8_t pwm_state = 0;

void TMR1_IRQHandler(void) {
    TIMER_ClearIntFlag(TIMER1);
    if (play_time > 0) {
        if (!pwm_state) {
            PWM_EnableOutput(PWM1, PWM_CH_0_MASK);
            pwm_state = TRUE;
        }
        play_time--;
    }
    if (play_time == 0 && pwm_state) {
        PWM_DisableOutput(PWM1, PWM_CH_0_MASK);
        pwm_state = FALSE;
    }
}

void init_timer1(void) {
    TIMER_Open(TIMER1, TIMER_PERIODIC_MODE, 200); // 5ms
    TIMER_EnableInt(TIMER1);
    NVIC_EnableIRQ(TMR1_IRQn);
    TIMER_Start(TIMER1);
}

void init_buzzer(void) {
    init_timer1();
    play_time = 0;
}

void buzzer_play(uint16_t freq, uint16_t t) {
    PWM_ConfigOutputChannel(PWM1, PWM_CH0, freq, 95);
    PWM_EnableOutput(PWM1, PWM_CH_0_MASK);
    play_time = t;
}

void buzzer_play_song(uint16_t *freq, uint16_t *t, uint8_t size) {
    static uint8_t t_index = 0;
    buzzer_play(freq[t_index], t[t_index]);
    t_index = (t_index + 1) % size;
}

void buzzer_stop(void) {
    play_time = 0;
    PB11 = 1;
}

uint8_t buzzer_is_playing(void) {
    return play_time > 0;
}
