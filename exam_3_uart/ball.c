#include <stdio.h>
#include <stdlib.h>
#include "NUC100Series.h"
#include "NewScanKey.h"
#include "NewLCD.h"
#include "player.h"
#include "ball.h"

void init_ball(ball *b, uint8_t x, uint8_t y, int8_t dx, int8_t dy, uint8_t speed, uint8_t *ball_bmp) {
    b->x = x;
    b->y = y;
    b->dx = dx;
    b->dy = dy;
    b->speed = speed;
    b->ball_bmp = ball_bmp;
}

uint8_t check_edge(ball *b) {
    uint8_t is_hit_edge = FALSE;
    if (b->x <= 4) {
        b->dx = -b->dx;
        b->x = 4 + 1;
        is_hit_edge = TRUE;
    }
    if (b->x >= 127 - 4) {
        b->dx = -b->dx;
        b->x = 127 - 4 - 1;
        is_hit_edge = TRUE;
    }
    if (b->y <= 4) {
        b->dy = -b->dy;
        b->y = 4 + 1;
        is_hit_edge = TRUE;
    }
    if (b->y >= 63 - 4) {
        b->dy = -b->dy;
        b->y = 63 - 4 - 1;
        is_hit_edge = TRUE;
    }
    return is_hit_edge;
}

uint8_t check_paddle(ball *b, player *p1, player *p2) {
    uint8_t is_hit_paddle = FALSE;
    if (b->y <= 4) {
        if (b->x >= p2->x - 16 && b->x <= p2->x + 16) {
            p2->score = (p2->score + 1) % 9;
            is_hit_paddle = 1;
        }
    }
    if (b->y >= 63 - 4) {
        if (b->x >= p1->x - 16 && b->x <= p1->x + 16) {
            p1->score = (p1->score + 1) % 9;
            is_hit_paddle = 2;
        }
    }
    return is_hit_paddle;
}

void change_ball_speed(ball *b) {
    if (KEY_FLAG == 2) {
        b->speed++;
        if (b->speed > 3) b->speed = 3;
        KEY_FLAG = 0;
    } else if (KEY_FLAG == 8) {
        b->speed -= 1;
        if (b->speed == 0) b->speed = 1;
        KEY_FLAG = 0;
    } else if (KEY_FLAG == 5) {
        b->speed = 0;
        KEY_FLAG = 0;
    } else if (KEY_FLAG == 4) {
        srand(TIMER2->TDR);
        b->speed = 1;
        b->x = 63;
        b->y = 32;
        b->dx = rand() % 2 == 0 ? 1 : -1;
        b->dy = rand() % 2 == 0 ? 1 : -1;
        KEY_FLAG = 0;
    }
}

void show_ball(ball *b) {
    draw_bitmap_in_buffer(b->ball_bmp, b->x - 4, b->y - 4, 8, 1, FG_COLOR);
}
