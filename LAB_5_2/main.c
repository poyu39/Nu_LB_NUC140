
#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "LCD.h"
#include "Scankey.h"
#include "Draw2D.h"

#define LOOP_DELAY 10000

uint16_t loop_count = 0;

uint8_t ooxx[9] = {-1, -1, -1, -1, -1, -1, -1, -1, -1};

int8_t check_who_win() {
    int8_t win = -1;
    if (ooxx[4] != -1) {
        if (ooxx[4] == ooxx[3] && ooxx[4] == ooxx[5]) {
            win = ooxx[4];
        } else if (ooxx[4] == ooxx[1] && ooxx[4] == ooxx[7]) {
            win = ooxx[4];
        } else if (ooxx[4] == ooxx[0] && ooxx[4] == ooxx[8]) {
            win = ooxx[4];
        } else if (ooxx[4] == ooxx[2] && ooxx[4] == ooxx[6]) {
            win = ooxx[4];
        }
    }

    if (ooxx[0] != -1) {
        if (ooxx[0] == ooxx[1] && ooxx[0] == ooxx[2]) {
            win = ooxx[0];
        } else if (ooxx[0] == ooxx[3] && ooxx[0] == ooxx[6]) {
            win = ooxx[0];
        }
    }

    if (ooxx[8] != -1) {
        if (ooxx[8] == ooxx[2] && ooxx[8] == ooxx[5]) {
            win = ooxx[8];
        } else if (ooxx[8] == ooxx[6] && ooxx[8] == ooxx[7]) {
            win = ooxx[8];
        }
    }
    return win;
}

void draw_ox_game_field() {
    draw_Line(0, 0, 127, 0, FG_COLOR, BG_COLOR);
    draw_Line(0, 64, 127, 64, FG_COLOR, BG_COLOR);
    draw_Line(0, 21, 127, 21, FG_COLOR, BG_COLOR);
    draw_Line(0, 42, 127, 42, FG_COLOR, BG_COLOR);
    draw_Line(43, 0, 43, 64, FG_COLOR, BG_COLOR);
    draw_Line(86, 0, 86, 64, FG_COLOR, BG_COLOR);
}

void draw_ox_game_select(uint8_t x, uint8_t y) {
    if (loop_count % 100 == 50)
        fill_Rectangle(0 + x * 43, 0 + y * 21, 41 + x * 43, 20 + y * 21, FG_COLOR, BG_COLOR);
    else if (loop_count % 100 == 0)
        fill_Rectangle(0 + x * 43, 0 + y * 21, 41 + x * 43, 20 + y * 21, BG_COLOR, BG_COLOR);
    // fill_Rectangle(0 + x * 43, 0 + y * 21, 41 + x * 43, 20 + y * 21, FG_COLOR, BG_COLOR);
    // CLK_SysTickDelay(500000);
    // fill_Rectangle(0 + x * 43, 0 + y * 21, 41 + x * 43, 20 + y * 21, BG_COLOR, BG_COLOR);
}

void draw_ox_game_now() {
    int i;
    for (i = 0; i < 9; i++) {
        if (ooxx[i] == 0) {
            draw_Circle(21 + (i % 3) * 43, 10 + (i / 3) * 21, 8, FG_COLOR, BG_COLOR);
        } else if (ooxx[i] == 1) {
            draw_Line(2 + (i % 3) * 43, 0 + (i / 3) * 21, 40 + (i % 3) * 43, 18 + (i / 3) * 21, FG_COLOR, BG_COLOR);
            draw_Line(40 + (i % 3) * 43, 0 + (i / 3) * 21, 2 + (i % 3) * 43, 18 + (i / 3) * 21, FG_COLOR, BG_COLOR);
        }
    }
}

int main(void) {
    uint8_t keyin = 0,
            i = 0,
            isPressed = 0,
            x = 0,
            y = 0,
            round = 0,
            ox = 0;
    int8_t win = -1;

    SYS_Init();

    init_LCD();
    clear_LCD();

    OpenKeyPad();

    while (keyin == 0) {
        keyin = ScanKey();
        draw_ox_game_field();
        draw_ox_game_now(0 , 0);
    }

    clear_LCD();

    while (TRUE) {
        keyin = ScanKey();
		if (keyin != 0) {
			if (isPressed == 0) goto display;
			isPressed = 1;
		}
		switch (keyin) {
		case 0:
			isPressed = 0;
			break;
        case 2:
            if (y > 0) y--;
            isPressed = 1;
            break;
        case 4:
            if (x > 0) x--;
            break;
        case 6:
            if (x < 2) x++;
            break;
        case 8:
            if (y < 2) y++;
            break;
        case 5:
            if ((round + 1) % 2 == 1) {
                if (ooxx[x + 3 * y] == -1) {
                    draw_Circle(21 + 43 * x, 10 + y * 21, 8, FG_COLOR, BG_COLOR);
                    ox = 1;
                    ooxx[x + 3 * y] = 0;
                    round++;
                }
            } else {
                if (ooxx[x + 3 * y] == -1) {
                    draw_Line(2 + x * 43, 0 + y * 21, 40 + x * 43, 18 + y * 21, FG_COLOR, BG_COLOR);
                    draw_Line(40 + x * 43, 0 + y * 21, 2 + x * 43, 18 + y * 21, FG_COLOR, BG_COLOR);
                    ox = 0;
                    ooxx[x + 3 * y] = 1;
                    round++;
                }
            }
            break;
        }

        display:
        draw_ox_game_field();
        draw_ox_game_select(x, y);
        draw_ox_game_now();

        win = check_who_win();
        if (win != -1) {
            if (round == 9) {
                clear_LCD();
                printS(0, 0, "Game Over");
                break;
            } else {
                if (win == 0) {
                    clear_LCD();
                    printS(0, 0, " O WIN");
                    break;
                } else if (win == 1) {
                    clear_LCD();
                    printS(0, 0, "X WIN");
                    break;
                }
            }
        }
        if (round == 9) {
            clear_LCD();
            printS(0, 0, "Game Over");
            break;
        }

        loop_counter:
        if ((loop_count + 1) >= UINT16_MAX) loop_count = 0;
        loop_count++;
    }
}
