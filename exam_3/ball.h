typedef struct ball ball;

typedef struct player player;

struct ball {
    uint8_t x;
    uint8_t y;
    int8_t dx;
    int8_t dy;
    uint8_t speed;
    uint8_t *ball_bmp;
};

extern void init_ball(ball *b, uint8_t x, uint8_t y, int8_t dx, int8_t dy, uint8_t speed, uint8_t *ball_bmp);

extern uint8_t check_edge(ball *b);

extern uint8_t check_paddle(ball *b, player *p1, player *p2);

extern void change_ball_speed(ball *b);

extern void show_ball(ball *b);

