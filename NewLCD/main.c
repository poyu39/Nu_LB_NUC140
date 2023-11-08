/*
    NewLCD
    Author: poyu39
    Discord: poyu39
*/
#include <stdio.h>
#include <stdlib.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "LCD.h"

extern int lcdSetAddr(uint8_t PageAddr, uint8_t ColumnAddr);
extern int lcdWriteData(uint8_t temp);

uint8_t lcd_buffer_bin[LCD_Xmax * LCD_Ymax];
uint8_t lcd_buffer_bin_last[LCD_Xmax * LCD_Ymax];

void show_lcd_buffer(void);
void init_lcd_buffer(void);
void clear_lcd_buffer(void);
void draw_pixel_in_buffer(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor);
void draw_line_in_buffer(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t fgColor, uint16_t bgColor);
void draw_circle_in_buffer(int16_t xc, int16_t yc, int16_t r, uint16_t fgColor, uint16_t bgColor, uint8_t isFill);
void draw_rectangle_in_buffer(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t fgColor, uint16_t bgColor, uint8_t isFill);
void draw_triangle_in_buffer(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t fgColor, uint16_t bgColor, uint8_t isFill);

int main(void) {
    SYS_Init();
    init_LCD();
    clear_lcd_buffer();

    // demo
    draw_line_in_buffer(0, 0, 127, 63, FG_COLOR, BG_COLOR);
    draw_rectangle_in_buffer(10, 10, 20, 20, FG_COLOR, BG_COLOR, 0);
    draw_rectangle_in_buffer(30, 10, 40, 20, FG_COLOR, BG_COLOR, 1);
    draw_circle_in_buffer(20, 40, 10, FG_COLOR, BG_COLOR, 0);
    draw_circle_in_buffer(50, 40, 10, FG_COLOR, BG_COLOR, 1);
    draw_triangle_in_buffer(80, 10, 90, 20, 100, 10, FG_COLOR, BG_COLOR, 0);
    draw_triangle_in_buffer(80, 30, 90, 20, 100, 30, FG_COLOR, BG_COLOR, 1);
    show_lcd_buffer();
}

void show_lcd_buffer() {
    unsigned char temp;
    uint8_t x, y, z;
    for (x = 0; x < LCD_Xmax; x++) {
        for (y = 0; y < (LCD_Ymax / 8); y++) {
            if (lcd_buffer_bin[(x + y * 128 * 8)] == lcd_buffer_bin_last[(x + y * 128 * 8)]) continue;
            lcd_buffer_bin_last[(x + y * 128 * 8)] = lcd_buffer_bin[(x + y * 128 * 8)];
            temp = 0x00;
            for (z = 0; z < 8; z++) {
                // 1 2 4 8 16 32 64 128
                temp += lcd_buffer_bin_last[(x + y * 128 * 8) + z * 128] << z;
            }
            lcdSetAddr(y, (128 + 1 - x));
            lcdWriteData(temp);
        }
    }
}

void init_lcd_buffer() {
    int i;
    for (i = 0; i < LCD_Xmax * LCD_Ymax; i++) {
        lcd_buffer_bin[i] = 0;
        lcd_buffer_bin_last[i] = 0;
    }
}

void clear_lcd_buffer() {
    int i;
    for (i = 0; i < LCD_Xmax * LCD_Ymax; i++) {
        lcd_buffer_bin[i] = 0;
    }
}

void draw_pixel_in_buffer(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor) {
    if (fgColor == FG_COLOR)
        lcd_buffer_bin[x + y * 128] = 1;
    else if (fgColor == BG_COLOR)
        lcd_buffer_bin[x + y * 128] = 0;
}

void draw_line_in_buffer(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t fgColor, uint16_t bgColor) {
    int16_t dy = y2 - y1;
    int16_t dx = x2 - x1;
    int16_t stepx, stepy;

    if (dy < 0) {
        dy = -dy;
        stepy = -1;
    } else {
        stepy = 1;
    }
    if (dx < 0) {
        dx = -dx;
        stepx = -1;
    } else {
        stepx = 1;
    }
    dy <<= 1;  // dy is now 2*dy
    dx <<= 1;  // dx is now 2*dx

    draw_pixel_in_buffer(x1, y1, fgColor, bgColor);
    if (dx > dy) {
        int fraction = dy - (dx >> 1);  // same as 2*dy - dx
        while (x1 != x2) {
            if (fraction >= 0) {
                y1 += stepy;
                fraction -= dx;  // same as fraction -= 2*dx
            }
            x1 += stepx;
            fraction += dy;  // same as fraction -= 2*dy
            draw_pixel_in_buffer(x1, y1, fgColor, bgColor);
        }
    } else {
        int fraction = dx - (dy >> 1);
        while (y1 != y2) {
            if (fraction >= 0) {
                x1 += stepx;
                fraction -= dy;
            }
            y1 += stepy;
            fraction += dx;
            draw_pixel_in_buffer(x1, y1, fgColor, bgColor);
        }
    }
}

void draw_circle_in_buffer(int16_t xc, int16_t yc, int16_t r, uint16_t fgColor, uint16_t bgColor, uint8_t isFill) {
    int16_t x = 0;
    int16_t y = r;
    int16_t p = 3 - 2 * r;
    if (!r)
        return;
    while (y >= x) { // only formulate 1/8 of circle
        draw_pixel_in_buffer(xc - x, yc - y, fgColor, bgColor);  // upper left left
        draw_pixel_in_buffer(xc - y, yc - x, fgColor, bgColor);  // upper upper left
        draw_pixel_in_buffer(xc + y, yc - x, fgColor, bgColor);  // upper upper right
        draw_pixel_in_buffer(xc + x, yc - y, fgColor, bgColor);  // upper right right
        draw_pixel_in_buffer(xc - x, yc + y, fgColor, bgColor);  // lower left left
        draw_pixel_in_buffer(xc - y, yc + x, fgColor, bgColor);  // lower lower left
        draw_pixel_in_buffer(xc + y, yc + x, fgColor, bgColor);  // lower lower right
        draw_pixel_in_buffer(xc + x, yc + y, fgColor, bgColor);  // lower right right
        if (isFill == 1) {
            draw_line_in_buffer(xc - x, yc - y, xc + x, yc - y, fgColor, bgColor);
            draw_line_in_buffer(xc - y, yc - x, xc + y, yc - x, fgColor, bgColor);
            draw_line_in_buffer(xc - x, yc + y, xc + x, yc + y, fgColor, bgColor);
            draw_line_in_buffer(xc - y, yc + x, xc + y, yc + x, fgColor, bgColor);
        }
        if (p < 0)
            p += 4 * (x++) + 6;
        else
            p += 4 * ((x++) - y--) + 10;
    }
}

void draw_rectangle_in_buffer(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t fgColor, uint16_t bgColor, uint8_t isFill) {
    int16_t x, y, tmp;
    if (x0 > x1) {
        tmp = x1;
        x1 = x0;
        x0 = tmp;
    }
    if (y0 > y1) {
        tmp = y1;
        y1 = y0;
        y0 = tmp;
    }
    if (isFill == 1) {
        for (x = x0; x <= x1; x++) {
            for (y = y0; y <= y1; y++) {
                draw_pixel_in_buffer(x, y, fgColor, bgColor);
            }
        }
    } else {
        for (x = x0; x <= x1; x++) draw_pixel_in_buffer(x, y0, fgColor, bgColor);
        for (y = y0; y <= y1; y++) draw_pixel_in_buffer(x0, y, fgColor, bgColor);
        for (x = x0; x <= x1; x++) draw_pixel_in_buffer(x, y1, fgColor, bgColor);
        for (y = y0; y <= y1; y++) draw_pixel_in_buffer(x1, y, fgColor, bgColor);
    }
}

void draw_triangle_in_buffer(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t fgColor, uint16_t bgColor, uint8_t isFill) {
    int i;
    int dx = abs(x0 - x2);
    int dy = abs(y0 - y2);
    draw_line_in_buffer(x0, y0, x1, y1, fgColor, bgColor);
    draw_line_in_buffer(x1, y1, x2, y2, fgColor, bgColor);
    draw_line_in_buffer(x0, y0, x2, y2, fgColor, bgColor);
    if (isFill) {
        if (dx > dy) {
            for (i = 0; i < dx; i++) {
                draw_line_in_buffer(x1, y1, x0 + i, y0 + i * dy / dx + dy, fgColor, bgColor);
            }
        } else {
            for (i = 0; i < dy; i++) {
                draw_line_in_buffer(x1, y1, x0 + i * dx / dy + dx, y0 + i, fgColor, bgColor);
            }
        }
    }
}

