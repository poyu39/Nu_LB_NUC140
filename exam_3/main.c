#include <stdio.h>
#include <stdlib.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "NewTurnButton.h"
#include "NewLCD.h"
#include "NewScanKey.h"
#include "Note_Freq.h"
#include "NewSevenSegment.h"
#include "NewBuzzer.h"
#include "ball.h"
#include "player.h"

volatile uint32_t timer2_5ms = 0, timer2_1s = 0;

uint8_t paddle_bmp[32] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
uint8_t ball_bmp[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t state = 0;
uint16_t game_play_time = 0;

player player1;
player player2;
ball ball1;

uint16_t mapping(uint16_t input, uint16_t input_min, uint16_t input_max, uint16_t output_min, uint16_t output_max) {
    return (input - input_min) * (output_max - output_min) / (input_max - input_min) + output_min;
}

void show_ST_on_seg(void) {
    if (seg_buffer[3] != game_play_time / 10) seg_buffer[3] = game_play_time / 10;
    if (seg_buffer[2] != game_play_time % 10) seg_buffer[2] = game_play_time % 10;
    if (seg_buffer[1] != player2.score) seg_buffer[1] = player2.score;
    if (seg_buffer[0] != player1.score) seg_buffer[0] = player1.score;
}

void update_player_paddle(void) {
    player1.x = mapping(get_turn_button(), 0, 4095, 16, 127 - 16);
    player2.x = 127 - mapping(get_turn_button(), 0, 4095, 16, 127 - 16);
}

void update_ball(void) {
    uint8_t who_hit = check_paddle(&ball1, &player1, &player2);
    uint8_t is_hit_edge = 0;
    if (who_hit > 0) {
        if (who_hit == 1) buzzer_play(1046, 20);
        if (who_hit == 2) buzzer_play(523, 20);
        show_ST_on_seg();
    }
    is_hit_edge = check_edge(&ball1);
    if (is_hit_edge > 0 && who_hit == 0) buzzer_play(784, 40);
    // move
    ball1.x += ball1.dx * ball1.speed;
    ball1.y += ball1.dy * ball1.speed;
}

void TMR2_IRQHandler(void) {
    TIMER_ClearIntFlag(TIMER2);
    timer2_5ms = (timer2_5ms + 1) % INT32_MAX;
    if (timer2_5ms % 50 == 0) {
        timer2_1s = (timer2_1s + 1) % INT32_MAX;
        if (state == 2 && !SLEEP) game_play_time = (game_play_time + 1) % 59;
    }
}

void init_timer2(void) {
    TIMER_Open(TIMER2, TIMER_PERIODIC_MODE, 200);
    TIMER_EnableInt(TIMER2);
    NVIC_EnableIRQ(TMR2_IRQn);
    TIMER_Start(TIMER2);
}

int main(void) {
    // init
    PD14 = 0;
    state = 2;
    SYS_Init();
    init_seg(TRUE, 200);
    init_turn_button();
    init_lcd(TRUE, TRUE);
    init_keypad_INT();
    init_timer2();
    init_buzzer();

    // obj init
    init_player(&player1, 1, 63, 62, paddle_bmp);
    init_player(&player2, 2, 63, 2, paddle_bmp);
    init_ball(&ball1, 63, 31, 1, 1, 1, ball_bmp);

    // main loop
    while (TRUE) {
        switch (state) {
        case 2:
            // update
            change_ball_speed(&ball1);
            if (timer2_5ms % 20) update_ball();
            update_player_paddle();
            update_player_paddle();
            // render
            show_ball(&ball1);
            show_player_paddle(&player1);
            show_player_paddle(&player2);
            show_ST_on_seg();
            break;
        default:
            break;
        }
        show_lcd_buffer();
    }
}
