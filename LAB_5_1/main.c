#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "LCD.h"
#include "Scankey.h"


unsigned char block_horizontal_16x8[16] = {
    0xFF, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0xFF};

unsigned char block_vertical_8x16[16] = {
    0xFF, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0xFF, 0xFF, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xFF};

void Buzz(void) {
    PB11 = 0;
    CLK_SysTickDelay(50000);
    PB11 = 1;
    CLK_SysTickDelay(50000);
}

void black_white_change(int nx, int ny, int pattern) {
    if (pattern == 0) {
        draw_Bmp16x8(nx, ny, FG_COLOR, BG_COLOR, block_horizontal_16x8);
        CLK_SysTickDelay(500000);
        draw_Bmp16x8(nx, ny, BG_COLOR, BG_COLOR, block_horizontal_16x8);
    } else {
        draw_Bmp8x16(nx, ny, FG_COLOR, BG_COLOR, block_vertical_8x16);
        CLK_SysTickDelay(500000);
        draw_Bmp8x16(nx, ny, BG_COLOR, BG_COLOR, block_vertical_8x16);
    }
}

int main(void) {
    int8_t x = 0, y = 0;
    int8_t keyin = 0;
    int8_t movX = 3, dirX = 0;
    int8_t movY = 3, dirY = 0;
    int isPressed = 0;
    int is_vertical = 0;
    SYS_Init();
    init_LCD();
    clear_LCD();
    OpenKeyPad();

    x = 56;
    y = 28;

    while (keyin == 0) {
        keyin = ScanKey();
        black_white_change(x, y, is_vertical);
    }

    clear_LCD();
    while (TRUE) {
        keyin = ScanKey();
        if (keyin != 0) {
			if (isPressed == 1) goto display;
			isPressed = 1;
		}
		switch (keyin) {
		case 0:
			isPressed = 0;
			break;
        case 2:
			dirX = 0;
			dirY = -1;
			break;
		case 4:
			dirX = -1;
			dirY = 0;
			break;
		case 5:
			if (is_vertical == 1) {
				is_vertical = 0;
				if (x > LCD_Xmax - 16) {
					x -= 8;
				}
				if (y > LCD_Ymax - 16) {
					y -= 16;
				}
			} else {
				is_vertical = 1;
				if (x > LCD_Xmax - 16) {
					x -= 8;
				}
				if (y > LCD_Ymax - 16) {
					y -= 8;
				}
			}
			break;
		case 6:
			dirX = +1;
			dirY = 0;
			break;
		case 8:
			dirX = 0;
			dirY = +1;
            break;
		default:
			break;
        }

		display:
		x = x + dirX * movX;
        y = y + dirY * movY;
        if (is_vertical == 0) {
            if (x < 0 || x > LCD_Xmax - 16) {
                x -= dirX * movX;
                black_white_change(x, y, is_vertical);
                dirX *= -1;
                Buzz();
            }
            if (y < 0 || y > LCD_Ymax - 8) {
                y -= dirY * movY;
                black_white_change(x, y, is_vertical);
                dirY *= -1;
                Buzz();
            }
        } else {
            if (x < 0 || x > LCD_Xmax - 8) {
                x -= dirX * movX;
                black_white_change(x, y, is_vertical);
                dirX *= -1;
                Buzz();
            }
            if (y < 0 || y > LCD_Ymax - 16) {
                y -= dirY * movY;
                black_white_change(x, y, is_vertical);
                dirY *= -1;
                Buzz();
            }
        }
		if (is_vertical == 0) {
            draw_Bmp16x8(x, y, FG_COLOR, BG_COLOR, block_horizontal_16x8);
            CLK_SysTickDelay(100000);
            draw_Bmp16x8(x, y, BG_COLOR, BG_COLOR, block_horizontal_16x8);
        } else {
            draw_Bmp8x16(x, y, FG_COLOR, BG_COLOR, block_vertical_8x16);
            CLK_SysTickDelay(100000);
            draw_Bmp8x16(x, y, BG_COLOR, BG_COLOR, block_vertical_8x16);
        }
    }
}
