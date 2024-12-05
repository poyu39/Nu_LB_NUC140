#include <stdio.h>
#include <stdlib.h>
#include "NUC100Series.h"
#include "NewLCD.h"
#include "pokemon.h"

#define SPACE16 "                "

void attack(Pokemon *attacker, Pokemon *defender) {
    /**
     * 屬性相剋表
     * 火 > 草 > 水 > 火
     * > : x2
     * < : /2
     */
    uint8_t skill_index = attacker->now_skill_index;
    uint8_t damage = attacker->skill[skill_index].power;
    uint8_t skill_type = attacker->skill[skill_index].type;
    if (skill_type == TYPE_FIRE && defender->type == TYPE_GRASS) {
        damage *= 2;
    } else if (skill_type == TYPE_GRASS && defender->type == TYPE_FIRE) {
        damage /= 2;
    } else if (skill_type == TYPE_WATER && defender->type == TYPE_FIRE) {
        damage *= 2;
    } else if (skill_type == TYPE_FIRE && defender->type == TYPE_WATER) {
        damage /= 2;
    } else if (skill_type == TYPE_GRASS && defender->type == TYPE_WATER) {
        damage *= 2;
    } else if (skill_type == TYPE_WATER && defender->type == TYPE_GRASS) {
        damage /= 2;
    }
    defender->hp -= damage;
    if (defender->hp < 0) {
        defender->hp = 0;
    }
    attacker->state = STATE_ATTACK;
    defender->state = STATE_HURT;
}

void print_skill(Pokemon* pokemon, uint8_t skill_index) {
    char* skill_type_name;
    if (pokemon->skill[skill_index].type == TYPE_FIRE) {
        skill_type_name = TYPE_FIRE_STR;
    } else if (pokemon->skill[skill_index].type == TYPE_WATER) {
        skill_type_name = TYPE_WATER_STR;
    } else if (pokemon->skill[skill_index].type == TYPE_GRASS) {
        skill_type_name = TYPE_GRASS_STR;
    } else {
        skill_type_name = TYPE_NONE_STR;
    }
    if (pokemon->is_player) {
        printf_line_in_buffer(7, 5, "%-13s%s%2d", pokemon->skill[skill_index].name, skill_type_name, pokemon->skill[skill_index].power);
    } else {
        printf_line_in_buffer(7, 5, "%s%2d%13s", skill_type_name, pokemon->skill[skill_index].power, pokemon->skill[skill_index].name);
    }
}

void play_anime(Pokemon *pokemon, Pokemon *other) {
    int i;
    uint8_t *bmp = (pokemon->is_player) ? pokemon->back_bmp : pokemon->front_bmp;
    uint8_t x = (pokemon->is_player) ? 4 : 90;
    uint8_t y = (pokemon->is_player) ? 15 : 0;
    
    // 左右晃動
    if (pokemon->state == STATE_ATTACK) {
        for (i = 0; i < 5; i++) {
            clear_lcd_buffer();
            draw_bitmap_in_buffer(bmp, x + ((i % 2 == 0) ? -2 : 2), y, 32, 32, FG_COLOR);
            show_pokemon(other);
            print_skill(pokemon, pokemon->now_skill_index);
            show_lcd_buffer();
            CLK_SysTickDelay(500000);
        }
        pokemon->state = STATE_NONE;
    }
    
    // 閃爍
    if (pokemon->state == STATE_HURT) {
        for (i = 0; i < 4; i++) {
            clear_lcd_buffer();
            draw_bitmap_in_buffer(bmp, x, y, 32, 32, (i % 2 == 0) ? FG_COLOR : BG_COLOR);
            show_pokemon(other);
            print_skill(other, other->now_skill_index);
            show_lcd_buffer();
            CLK_SysTickDelay(500000);
        }
        pokemon->state = STATE_NONE;
    }
}

void mapping(uint8_t max, uint8_t min, uint8_t *value) {
    *value = (*value - min) * 100 / (max - min);
}

void print_hp(uint8_t hp_max, uint8_t hp, uint8_t x, uint8_t y) {
    uint8_t i;
    uint8_t hp_bar = hp;
    mapping(hp_max, 0, &hp_bar);
    for (i = 0; i < 30; i++) {
        draw_pixel_in_buffer(x + i, y, (i < hp_bar) ? FG_COLOR : BG_COLOR);
    }
}

void show_pokemon(Pokemon* pokemon) {
    uint8_t *bmp = (pokemon->is_player) ? pokemon->back_bmp : pokemon->front_bmp;
    uint8_t x = (pokemon->is_player) ? 4 : 90;
    uint8_t y = (pokemon->is_player) ? 15 : 0;
    draw_bitmap_in_buffer(bmp, x, y, 32, 32, FG_COLOR);
}

void show_name(Pokemon* pokemon) {
    if (pokemon->is_player) {
        printf_line_in_buffer(4, 5, "%16s", pokemon->name);
    } else {
        printf_line_in_buffer(0, 5, "%-16s", pokemon->name);
    }
}

void show_hp(Pokemon* pokemon) {
    if (pokemon->is_player) {
        printf_line_in_buffer(5, 5, "%9s%-3d/%-3d", "<HP:", pokemon->hp, pokemon->max_hp);
    } else {
        printf_line_in_buffer(1, 5, "HP:%3d/%3d%-6s", pokemon->hp, pokemon->max_hp, ">");
    }
}
