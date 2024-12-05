#ifndef __POKEMON_H__
#define __POKEMON_H__


#define TYPE_NONE 0x00
#define TYPE_FIRE 0x01
#define TYPE_WATER 0x02
#define TYPE_GRASS 0x03

#define TYPE_NONE_STR "N"
#define TYPE_FIRE_STR "F"
#define TYPE_WATER_STR "W"
#define TYPE_GRASS_STR "G"

#define STATE_NONE 0x00
#define STATE_HURT 0x01
#define STATE_ATTACK 0x02

typedef struct Skill Skill;

typedef struct Pokemon Pokemon;

struct Skill {
    char name[16];
    uint8_t power;
    uint8_t type;
};

struct Pokemon {
    char name[20];
    uint8_t type;
    uint8_t max_hp;
    int16_t hp;
    uint8_t *front_bmp;
    uint8_t *back_bmp;
    uint8_t state;
    uint8_t is_player;
    Skill skill[3];
    uint8_t now_skill_index;
};

extern void attack(Pokemon *attacker, Pokemon *defender);

extern void play_anime(Pokemon *pokemon, Pokemon *other);

extern void print_skill(Pokemon *pokemon, uint8_t skill_index);

extern void show_pokemon(Pokemon *pokemon);

extern void show_name(Pokemon *pokemon);

extern void show_hp(Pokemon *pokemon);

#endif
