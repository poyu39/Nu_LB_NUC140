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
#include "sound.h"

volatile uint32_t timer2_100ms = 0, timer2_1s = 0;

uint8_t paddle_bmp[16] = {0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F};
uint8_t ball_bmp[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t state = 0;
uint8_t SLEEP = FALSE;
uint8_t buzzer_flag = 0;
uint16_t game_play_time = 0;

player player1;
player player2;
ball ball1;

void sleep(uint32_t count) {
    SLEEP = TRUE;
    timer2_100ms = 0;
    while (timer2_100ms < count) {
        __NOP();
    }
    SLEEP = FALSE;
}

uint16_t mapping(uint16_t input, uint16_t input_min, uint16_t input_max, uint16_t output_min, uint16_t output_max) {
    return (input - input_min) * (output_max - output_min) / (input_max - input_min) + output_min;
}

void reset_game(void) {
    player1.x = 63;
    player2.x = 63;
    player1.score = 0;
    player2.score = 0;
    ball1.x = 63;
    ball1.y = 31;
    ball1.dx = 1;
    ball1.dy = 1;
    ball1.speed = 1;
}

void check_who_win(void) {
    if (player1.score >= 3 || player2.score >= 3) {
        state = 3;
    }
}

void wait_start(void) {
    printf_line_in_buffer(1, 5, "  Press any key");
    printf_line_in_buffer(2, 5, "    to start");
    if (KEY_FLAG != 0) {
        state = 1;
        KEY_FLAG = 0;
    }
}

void start_countdown() {
    uint8_t i;
    player1.x = 63;
    player2.x = 63;
    for (i = 3; i > 0; i--) {
        print_c_in_buffer(60, 28, 5, i + '0', FG_COLOR);
        show_player_paddle(&player1);
        show_player_paddle(&player2);
        show_lcd_buffer();
        sleep(10);
    }
    state = 2;
}

void show_ST_on_seg(void) {
    if (seg_buffer[3] != game_play_time / 10) seg_buffer[3] = game_play_time / 10;
    if (seg_buffer[2] != game_play_time % 10) seg_buffer[2] = game_play_time % 10;
    if (seg_buffer[1] != player1.score) seg_buffer[1] = player1.score;
    if (seg_buffer[0] != player2.score) seg_buffer[0] = player2.score;
}

void update_player_paddle(void) {
    player1.x = mapping(get_turn_button(), 0, 4095, 8, 127 - 8);
    player2.x = 127 - mapping(get_turn_button(), 0, 4095, 8, 127 - 8);
}

void update_ball(void) {
    if (check_hit_ground(&ball1, &player1, &player2)) {
        buzzer_play(point_sound_freq, point_sound_t);
        // add score
        print_c_in_buffer(58, 28, 5, player1.score + '0', FG_COLOR);
        print_c_in_buffer(63, 28, 5, '-', FG_COLOR);
        print_c_in_buffer(68, 28, 5, player2.score + '0', FG_COLOR);
        show_lcd_buffer();
        reset_ball(&ball1);
        check_who_win();
        show_ST_on_seg();
        if (state != 2) return;
        sleep(30);
        state = 1;
        return;
    }
    if (check_edge(&ball1)) buzzer_play(wall_sound_freq, wall_sound_t);
    if (check_paddle(&ball1, &player1) || check_paddle(&ball1, &player2)) buzzer_play(paddle_sound_freq, paddle_sound_t);
    // move
    ball1.x += ball1.dx * ball1.speed;
    ball1.y += ball1.dy * ball1.speed;
}

void TMR2_IRQHandler(void) {
    TIMER_ClearIntFlag(TIMER2);
    timer2_100ms = (timer2_100ms + 1) % INT32_MAX;
    if (timer2_100ms % 10 == 0) {
        timer2_1s = (timer2_1s + 1) % INT32_MAX;
        if (state == 2 && !SLEEP) game_play_time = (game_play_time + 1) % INT32_MAX;
    }
}

void init_timer2(void) {
    TIMER_Open(TIMER2, TIMER_PERIODIC_MODE, 10);
    TIMER_EnableInt(TIMER2);
    NVIC_EnableIRQ(TMR2_IRQn);
    TIMER_Start(TIMER2);
}

int main(void) {
    // init
    PD14 = 0;
    SYS_Init();
    init_seg(TRUE, 200);
    init_turn_button();
    init_lcd(TRUE, TRUE);
    init_keypad_INT();
    init_timer2();
    init_buzzer();

    // obj init
    init_player(&player1, 1, 63, 2, paddle_bmp);
    init_player(&player2, 2, 63, 62, paddle_bmp);
    init_ball(&ball1, 63, 31, 1, 1, 1, ball_bmp);

    // main loop
    while (TRUE) {
        switch (state) {
        case 0:
            wait_start();
            break;
        case 1:
            start_countdown();
            break;
        case 2:
            // update
            change_ball_speed(&ball1);
            update_ball();
            update_player_paddle();
            update_player_paddle();
            if (state != 2) break;
            // render
            show_ball(&ball1);
            show_player_paddle(&player1);
            show_player_paddle(&player2);
            show_ST_on_seg();
            break;
        case 3:
            if (player1.score >= 3) {
                printf_line_in_buffer(1, 5, "  Player 1 win!");
            } else if (player2.score >= 3) {
                printf_line_in_buffer(1, 5, "  Player 2 win!");
            }
            if (KEY_FLAG != 0) {
                reset_game();
                state = 0;
                KEY_FLAG = 0;
            }
            buzzer_play_song(game_over_song_tone, game_over_song_beat, game_over_song_length);
            break;
        default:
            break;
        }
        show_lcd_buffer();
    }
}
