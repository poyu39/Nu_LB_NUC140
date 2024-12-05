/**
 * Pokemon Battle
 * Author: poyu39
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "NewSevenSegment.h"
#include "NewScankey.h"
#include "NewLCD.h"

#include "bmp/bulbasaur_back_bmp.h"
#include "bmp/bulbasaur_front_bmp.h"
#include "bmp/charmander_back_bmp.h"
#include "bmp/charmander_front_bmp.h"
#include "bmp/squirtle_back_bmp.h"
#include "bmp/squirtle_front_bmp.h"

#include "pokemon.h"

#define PLAYER_ROUND 0
#define PC_ROUND 1
#define NONE_ROUND 2

volatile uint8_t who_turn = NONE_ROUND;

volatile uint16_t round_timer = 0;


// 妙蛙種子
Pokemon bulbasaur = {
    "Bulbasaur",
    TYPE_GRASS,
    150,
    150,
    bulbasaur_front_bmp_hex,
    bulbasaur_back_bmp_hex,
    STATE_NONE,
    FALSE,
    {
        {"Tackle", 10, TYPE_NONE},          // 撞擊
        {"Razor Leaf", 20, TYPE_GRASS},     // 飛葉快刀
        {"Seed Bomb", 30, TYPE_GRASS}       // 種子炸彈
    },
    0
};

// 小火龍
Pokemon charmander = {
    "Charmander",
    TYPE_FIRE,
    120,
    120,
    charmander_front_bmp_hex,
    charmander_back_bmp_hex,
    STATE_NONE,
    FALSE,
    {
        {"Scratch", 10, TYPE_NONE},         // 抓
        {"Ember", 20, TYPE_FIRE},           // 火花
        {"Flamethrower", 30, TYPE_FIRE}     // 噴射火焰
    },
    0
};

// 傑尼龜
Pokemon squirtle = {
    "Squirtle",
    TYPE_WATER,
    110,
    110,
    squirtle_front_bmp_hex,
    squirtle_back_bmp_hex,
    STATE_NONE,
    FALSE,
    {
        {"Tackle", 10, TYPE_NONE},          // 撞擊
        {"Water Gun", 20, TYPE_WATER},      // 水槍
        {"Hydro Pump", 30, TYPE_WATER}      // 水砲
    },
    0
};


void gen_pc(Pokemon* pc) {
    uint8_t pc_pokemon;
    srand(TIMER0->TDR);
    pc_pokemon = rand() % 3;
    switch (pc_pokemon) {
        case 0:
            *pc = bulbasaur;
            break;
        case 1:
            *pc = charmander;
            break;
        case 2:
            *pc = squirtle;
            break;
    }
    pc->is_player = FALSE;
}

void select_skill(Pokemon* pokemon) {
    int8_t now_skill_index = pokemon->now_skill_index;
    if (KEY_FLAG == 4) {
        now_skill_index--;
        if (now_skill_index < 0) now_skill_index = 0;
    } else if (KEY_FLAG == 6) {
        now_skill_index++;
        if (now_skill_index > 2) now_skill_index = 2;
    }
    pokemon->now_skill_index = now_skill_index;
    print_skill(pokemon, now_skill_index);
}

void pc_select_skill(Pokemon* pc) {
    uint8_t skill_index = rand() % 3;
    pc->now_skill_index = skill_index;
}

void battle_anime_handler(Pokemon* pokemon, Pokemon* target) {
    attack(pokemon, target);
    play_anime(pokemon, target);
    play_anime(target, pokemon);
}

uint8_t is_end(Pokemon* player, Pokemon* pc) {
    if (player->hp == 0) {
        clear_lcd_buffer();
        draw_bitmap_in_buffer(pc->front_bmp, 48, 16, 32, 32, FG_COLOR);
        printf_line_in_buffer(7, 5, "pc win!");
        show_lcd_buffer();
        return 1;
    } else if (pc->hp == 0) {
        clear_lcd_buffer();
        draw_bitmap_in_buffer(player->front_bmp, 48, 16, 32, 32, FG_COLOR);
        printf_line_in_buffer(7, 5, "player win!");
        show_lcd_buffer();
        return 1;
    }
    return 0;
}

void update_display(Pokemon* player, Pokemon* pc) {
    show_pokemon(player);
    show_name(player);
    show_hp(player);
    show_pokemon(pc);
    show_name(pc);
    show_hp(pc);
}

// 選擇寶可夢
void select_pokemon(Pokemon* player) {
    // 妙蛙種子, 小火龍, 傑尼龜
    int8_t now_index = -1;
    while (TRUE) {
        if (KEY_FLAG == 4) {
            now_index--;
            if (now_index < 0) now_index = 0;
        } else if (KEY_FLAG == 6) {
            now_index++;
            if (now_index > 2) now_index = 2;
        }
        if (KEY_FLAG == 4 || KEY_FLAG == 6 || now_index == -1) {
            KEY_FLAG = 0;
            if (now_index == -1) now_index = 1;
            clear_lcd_buffer();
            draw_bitmap_in_buffer(bulbasaur_front_bmp_hex, 3, 16, 32, 32, FG_COLOR);
            draw_bitmap_in_buffer(charmander_front_bmp_hex, 42, 16, 32, 32, FG_COLOR);
            draw_bitmap_in_buffer(squirtle_front_bmp_hex, 84, 16, 32, 32, FG_COLOR);
            if (now_index == 0) {
                draw_rectangle_in_buffer(3, 16, 35, 48, FG_COLOR, FALSE);
            } else if (now_index == 1) {
                draw_rectangle_in_buffer(42, 16, 74, 48, FG_COLOR, FALSE);
            } else if (now_index == 2) {
                draw_rectangle_in_buffer(84, 16, 116, 48, FG_COLOR, FALSE);
            }
        }
        if (KEY_FLAG == 5) {
            KEY_FLAG = 0;
            if (now_index == 0) {
                *player = bulbasaur;
                player->is_player = TRUE;
                break;
            } else if (now_index == 1) {
                *player = charmander;
                player->is_player = TRUE;
                break;
            } else if (now_index == 2) {
                *player = squirtle;
                player->is_player = TRUE;
                break;
            }
        }
        show_lcd_buffer();
    }
}


void TMR1_IRQHandler(void) {
    TIMER_ClearIntFlag(TIMER1);
    if (who_turn == PLAYER_ROUND) {
        if (round_timer == 0) {
            round_timer = 10;
        } else {
            round_timer--;
        }
        if (round_timer == 0) {
            who_turn = PC_ROUND;
        }
        set_seg_buffer_number(round_timer, FALSE);
    } else if (who_turn == PC_ROUND) {
        round_timer = 0;
    }
}

void init_timer1(uint8_t timer_hz) {
    TIMER_Open(TIMER1, TIMER_PERIODIC_MODE, timer_hz);
    TIMER_EnableInt(TIMER1);
    NVIC_EnableIRQ(TMR1_IRQn);
    TIMER_Start(TIMER1);
}


int main(void) {
    Pokemon player, pc;
    PD14 = 0;   // on lcd backlight
    
    SYS_Init();
    init_keypad_INT();
    init_seg(TRUE, 200);
    init_timer1(1);
    init_lcd(FALSE, SPI3_CLOCK_FREQUENCY);
    
    clear_lcd_buffer();
    select_pokemon(&player);
    gen_pc(&pc);
    
    clear_lcd_buffer();
    select_skill(&player);
    update_display(&player, &pc);
    show_lcd_buffer();
    
    who_turn = PLAYER_ROUND;
    
    while (TRUE) {
        if (who_turn == PC_ROUND) {
            clear_lcd_buffer();
            pc_select_skill(&pc);
            battle_anime_handler(&pc, &player);
            if (is_end(&player, &pc)) {
                KEY_FLAG = 0;
                break;
            }
            clear_lcd_buffer();
            update_display(&player, &pc);
            select_skill(&player);
            show_lcd_buffer();
            who_turn = PLAYER_ROUND;
        } else {
            if (KEY_FLAG) {
                clear_lcd_buffer();
                if (KEY_FLAG == 5) {
                    who_turn = PC_ROUND;
                    battle_anime_handler(&player, &pc);
                    KEY_FLAG = 0;
                    continue;
                }
                select_skill(&player);
                if (is_end(&player, &pc)) {
                    KEY_FLAG = 0;
                    continue;
                }
                update_display(&player, &pc);
                show_lcd_buffer();
                KEY_FLAG = 0;
            }
        }
    }
    while(TRUE);
}

