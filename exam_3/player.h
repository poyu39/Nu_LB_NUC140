typedef struct player player;
typedef struct ball ball;

struct player {
    uint8_t id;
    uint8_t x;
    uint8_t y;
    uint8_t score;
    uint8_t *paddle_bmp;
};

extern void init_player(player *p, uint8_t id, uint8_t x, uint8_t y, uint8_t *paddle_bmp);

extern void show_player_paddle(player *p);
