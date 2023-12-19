#include "Note_Freq.h"
uint8_t game_over_song_length = 8;
uint16_t game_over_song_tone[] = {C4, G3, G3, A3, G3, 0, B3, C4};
uint16_t game_over_song_beat[] = {40, 80, 80, 40, 40, 40, 40, 40};

uint8_t wall_sound_t = 3;
uint16_t wall_sound_freq = 226;

uint8_t paddle_sound_t = 19;
uint16_t paddle_sound_freq = 459;

uint8_t point_sound_t = 51;
uint16_t point_sound_freq = 490;
