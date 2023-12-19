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

void reset_ball(ball *b) {
    srand(TIMER2->TDR);
    b->x = 63;
    b->y = 32;
    b->dx = rand() % 2 == 0 ? 1 : -1;
    b->dy = rand() % 2 == 0 ? 1 : -1;
    // b->speed = 1;
}

uint8_t check_edge(ball *b) {
    uint8_t is_hit_edge = FALSE;
    if (b->x <= 4 || b->x >= 127 - 4) {
        b->dx = -b->dx;
        is_hit_edge = TRUE;
    }
    if (b->y <= 4 || b->y >= 63 - 4) {
        b->dy = -b->dy;
        is_hit_edge = TRUE;
    }
    return is_hit_edge;
}

uint8_t check_paddle(ball *b, player *p) {
    uint8_t is_hit_paddle = FALSE;
    if (b->y - 4 <= p->y + 2 && b->y - 4 > p->y - 2) {
        if (b->x >= p->x - 8 && b->x <= p->x + 8) {
            b->dy = -b->dy;
            b->y = p->y + 2 + 4;
            is_hit_paddle = TRUE;
        }
    }
    if (b->y + 4>= p->y - 2 && b->y + 4 < p->y + 2) {
        if (b->x >= p->x - 8 && b->x <= p->x + 8) {
            b->dy = -b->dy;
            b->y = p->y - 2 - 4;
            is_hit_paddle = TRUE;
        }
    }
    return is_hit_paddle;
}

uint8_t check_hit_ground(ball *b, player *p1, player *p2) {
    uint8_t is_hit_ground = FALSE;
    if (b->y <= 4) {
        p1->score++;
        is_hit_ground = TRUE;
    } else if (b->y >= 63 - 4) {
        p2->score++;
        is_hit_ground = TRUE;
    }
    return is_hit_ground;
}

uint8_t change_ball_speed(ball *b) {
    uint8_t is_change = FALSE;
    if (KEY_FLAG == 2) {
        b->speed += 1;
        KEY_FLAG = 0;
        is_change = TRUE;
    } else if (KEY_FLAG == 8) {
        b->speed -= 1;
        if (b->speed == 0) b->speed = 1;
        KEY_FLAG = 0;
        is_change = TRUE;
    }
    return is_change;
}

void show_ball(ball *b) {
    draw_bitmap_in_buffer(b->ball_bmp, b->x - 4, b->y - 4, 8, 1, FG_COLOR);
}
