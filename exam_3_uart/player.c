#include <stdio.h>
#include <stdlib.h>
#include "NUC100Series.h"
#include "NewLCD.h"
#include "ball.h"
#include "player.h"

void init_player(player *p, uint8_t id, uint8_t x, uint8_t y, uint8_t *paddle_bmp) {
    p->id = id;
    p->x = x;
    p->y = y;
    p->score = 0;
    p->paddle_bmp = paddle_bmp;
}

void show_player_paddle(player *p) {
    draw_bitmap_in_buffer(p->paddle_bmp, p->x - 16, p->y - 4, 32, 1, FG_COLOR);
}
