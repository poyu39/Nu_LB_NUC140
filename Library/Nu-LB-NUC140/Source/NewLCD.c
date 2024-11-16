/*
    NewLCD
    Author: 邱柏宇
    Discord: poyu39
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "NUC100Series.h"
#include "SYS.h"
#include "SPI.h"
#include "NewLCD.h"
#include "Font5x7.h"
#include "Font8x16.h"

uint8_t lcd_buffer_hex[LCD_Xmax * (LCD_Ymax / 8)];
uint8_t auto_clear_flag = 0;

void init_SPI3(uint32_t spi_clock_frequency) {
    SPI_Open(SPI3, SPI_MASTER, SPI_MODE_0, 9, spi_clock_frequency);
    SPI_DisableAutoSS(SPI3);
}

void lcdWriteCommand(uint8_t temp) {
    SPI_SET_SS0_LOW(SPI3);
    SPI_WRITE_TX0(SPI3, temp);
    SPI_TRIGGER(SPI3);
    while (SPI_IS_BUSY(SPI3));
    SPI_SET_SS0_HIGH(SPI3);
}

void lcdWriteData(uint8_t temp) {
    SPI_SET_SS0_LOW(SPI3);
    SPI_WRITE_TX0(SPI3, 0x100 + temp);
    SPI_TRIGGER(SPI3);
    while (SPI_IS_BUSY(SPI3));
    SPI_SET_SS0_HIGH(SPI3);
}

void lcdSetAddr(uint8_t PageAddr, uint8_t ColumnAddr) {
    SPI_SET_SS0_LOW(SPI3);
    SPI_WRITE_TX0(SPI3, 0xB0 | PageAddr);                   // Set Page Address
    SPI_TRIGGER(SPI3);
    while (SPI_IS_BUSY(SPI3));
    SPI_SET_SS0_HIGH(SPI3);
    SPI_SET_SS0_LOW(SPI3);
    SPI_WRITE_TX0(SPI3, 0x10 | (ColumnAddr >> 4) & 0xF);    // MSB: CA[7:4]
    SPI_TRIGGER(SPI3);
    while (SPI_IS_BUSY(SPI3));
    SPI_SET_SS0_HIGH(SPI3);
    SPI_SET_SS0_LOW(SPI3);
    SPI_WRITE_TX0(SPI3, 0x00 | (ColumnAddr & 0xF));         // LSB: CA[3:0]
    SPI_TRIGGER(SPI3);
    while (SPI_IS_BUSY(SPI3));
    SPI_SET_SS0_HIGH(SPI3);
}

// 初始化 LCD 的 buffer
void init_lcd_buffer() {
    int i, x, y;
    for (i = 0; i < LCD_Xmax * LCD_Ymax / 8; i++) {
        lcd_buffer_hex[i] = 0x00;
    }
    for (x = 0; x < LCD_Xmax; x++) {
        for (y = 0; y < (LCD_Ymax / 8); y++) {
            lcdSetAddr(y, (LCD_Xmax + 1 - x));
            lcdWriteData(lcd_buffer_hex[x + y * LCD_Xmax]);
        }
    }
}

// 只清除 LCD，不會清除 buffer。
void clear_lcd() {
    int16_t i;
    lcdSetAddr(0x00, 0x00);
    for (i = 0; i < LCD_Xmax * LCD_Ymax / 8; i++) {
        lcdWriteData(0x00);
    }
}

/**
 * @brief 初始化 LCD
 * @param auto_clear 是否每次 show_lcd_buffer 自動清除 buffer。
*/
void init_lcd(uint8_t auto_clear, uint32_t spi_clock_frequency) {
    init_SPI3(spi_clock_frequency);
    lcdWriteCommand(0xEB);
    lcdWriteCommand(0x81);
    lcdWriteCommand(0xA0);
    lcdWriteCommand(0xC0);
    lcdWriteCommand(0xAF);
    init_lcd_buffer();
    clear_lcd();
    auto_clear_flag = auto_clear;
}

// 將 buffer 的內容顯示到 LCD 上
void show_lcd_buffer() {
    uint8_t x, y;
    for (y = 0; y < (LCD_Ymax / 8); y++) {
        lcdSetAddr(y, 0x01);
        for (x = 0; x < LCD_Xmax; x++) {
            lcdWriteData(lcd_buffer_hex[(LCD_Xmax - 1 - x) + y * LCD_Xmax]);
        }
    }
    if (auto_clear_flag == 1) {
        clear_lcd_buffer();
    }
}


// 清除 buffer 的內容
void clear_lcd_buffer() {
    int i;
    for (i = 0; i < LCD_Xmax * LCD_Ymax / 8; i++) {
        lcd_buffer_hex[i] = 0x00;
    }
}

/**
 * @brief 畫一個 pixel 在 buffer 中
 * @param x x 座標
 * @param y y 座標
 * @param color 顏色
*/
void draw_pixel_in_buffer(int16_t x, int16_t y, uint16_t color) {
    if (color == FG_COLOR)
        lcd_buffer_hex[x + y / 8 * LCD_Xmax] |= (0x01 << (y % 8));
    else if (color == BG_COLOR)
        lcd_buffer_hex[x + y / 8 * LCD_Xmax] &= ~(0x01 << (y % 8));
}

/**
 * @brief 畫一個 bitmap 在 buffer 中
 * @param bitmap 圖形陣列
 * @param x x 座標
 * @param y y 座標
 * @param bitmap_x_size 圖形的 x 大小
 * @param bitmap_y_size 圖形的 y 大小
 * @param color 顏色
*/
void draw_bitmap_in_buffer(uint8_t bitmap[], int16_t x, int16_t y, int16_t bitmap_x_size, int16_t bitmap_y_size, uint16_t color) {
    uint16_t t, i, j, k, kx, ky;
    bitmap_y_size /= 8;
    for (i = 0; i < bitmap_y_size; i++) {
        for (j = 0; j < bitmap_x_size; j++) {
            kx = x + j;
            t = bitmap[j + i * bitmap_x_size];
            for (k = 0; k < 8; k++) {
                ky = y + k + i * 8;
                if (t & (0x01 << k))
                    draw_pixel_in_buffer(kx, ky, color);
            }
        }
    }
}

/**
 * @brief 畫一條線在 buffer 中
 * @param x1 起始 x 座標
 * @param y1 起始 y 座標
 * @param x2 結束 x 座標
 * @param y2 結束 y 座標
 * @param color 顏色
*/
void draw_line_in_buffer(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {
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
    
    draw_pixel_in_buffer(x1, y1, color);
    if (dx > dy) {
        int fraction = dy - (dx >> 1);  // same as 2*dy - dx
        while (x1 != x2) {
            if (fraction >= 0) {
                y1 += stepy;
                fraction -= dx;  // same as fraction -= 2*dx
            }
            x1 += stepx;
            fraction += dy;  // same as fraction -= 2*dy
            draw_pixel_in_buffer(x1, y1, color);
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
            draw_pixel_in_buffer(x1, y1, color);
        }
    }
}

/**
 * @brief 畫一個圓在 buffer 中
 * @param xc 圓心 x 座標
 * @param yc 圓心 y 座標
 * @param r 半徑
 * @param color 顏色
 * @param isFill 是否填滿
*/
void draw_circle_in_buffer(int16_t xc, int16_t yc, int16_t r, uint16_t color, uint8_t isFill) {
    int16_t x = 0;
    int16_t y = r;
    int16_t p = 3 - 2 * r;
    if (!r)
        return;
    while (y >= x) { // only formulate 1/8 of circle
        draw_pixel_in_buffer(xc - x, yc - y, color);  // upper left left
        draw_pixel_in_buffer(xc - y, yc - x, color);  // upper upper left
        draw_pixel_in_buffer(xc + y, yc - x, color);  // upper upper right
        draw_pixel_in_buffer(xc + x, yc - y, color);  // upper right right
        draw_pixel_in_buffer(xc - x, yc + y, color);  // lower left left
        draw_pixel_in_buffer(xc - y, yc + x, color);  // lower lower left
        draw_pixel_in_buffer(xc + y, yc + x, color);  // lower lower right
        draw_pixel_in_buffer(xc + x, yc + y, color);  // lower right right
        if (isFill == 1) {
            draw_line_in_buffer(xc - x, yc - y, xc + x, yc - y, color);
            draw_line_in_buffer(xc - y, yc - x, xc + y, yc - x, color);
            draw_line_in_buffer(xc - x, yc + y, xc + x, yc + y, color);
            draw_line_in_buffer(xc - y, yc + x, xc + y, yc + x, color);
        }
        if (p < 0)
            p += 4 * (x++) + 6;
        else
            p += 4 * ((x++) - y--) + 10;
    }
}

/**
 * @brief 畫一個矩形在 buffer 中
 * @param x0 左上角 x 座標
 * @param y0 左上角 y 座標
 * @param x1 右下角 x 座標
 * @param y1 右下角 y 座標
 * @param color 顏色
 * @param isFill 是否填滿
*/
void draw_rectangle_in_buffer(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color, uint8_t isFill) {
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
                draw_pixel_in_buffer(x, y, color);
            }
        }
    } else {
        for (x = x0; x <= x1; x++) draw_pixel_in_buffer(x, y0, color);
        for (y = y0; y <= y1; y++) draw_pixel_in_buffer(x0, y, color);
        for (x = x0; x <= x1; x++) draw_pixel_in_buffer(x, y1, color);
        for (y = y0; y <= y1; y++) draw_pixel_in_buffer(x1, y, color);
    }
}

/**
 * @brief 畫一個三角形在 buffer 中
 * @param x0 第一個點 x 座標
 * @param y0 第一個點 y 座標
 * @param x1 第二個點 x 座標
 * @param y1 第二個點 y 座標
 * @param x2 第三個點 x 座標
 * @param y2 第三個點 y 座標
 * @param color 顏色
 * @param isFill 是否填滿
*/
void draw_triangle_in_buffer(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color, uint8_t isFill) {
    int i;
    int dx = abs(x0 - x2);
    int dy = abs(y0 - y2);
    draw_line_in_buffer(x0, y0, x1, y1, color);
    draw_line_in_buffer(x1, y1, x2, y2, color);
    draw_line_in_buffer(x0, y0, x2, y2, color);
    if (isFill) {
        if (dx > dy) {
            for (i = 0; i < dx; i++) {
                draw_line_in_buffer(x1, y1, x0 + i, y0 + i * dy / dx + dy, color);
            }
        } else {
            for (i = 0; i < dy; i++) {
                draw_line_in_buffer(x1, y1, x0 + i * dx / dy + dx, y0 + i, color);
            }
        }
    }
}

/**
 * @brief 在 buffer 中印出一個字元
 * @param x x 座標
 * @param y y 座標
 * @param size 字元大小 (5 or 8)
 * @param ascii_code 字元
*/
void print_c_in_buffer(int16_t x, int16_t y, uint8_t size, unsigned char ascii_code, uint16_t color) {
    int8_t i, j;
    uint8_t char_bitmap[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    if (size == 8) {
        for (i = 0; i < 2; i++) {
            for (j = 0; j < 8; j++) {
                char_bitmap[j] = Font8x16[(ascii_code - 0x20) * 16 + i * 8 + j];
            }
            draw_bitmap_in_buffer(char_bitmap, x, y + i * 8, 8, 8, color);
        }
    } else if (size == 5) {
        if (x < (LCD_Xmax - 5) && y < (LCD_Ymax - 7)) {
            if (ascii_code < 0x20)
                ascii_code = 0x20;
            else if (ascii_code > 0x7F)
                ascii_code = 0x20;
            else
                ascii_code = ascii_code - 0x20;
            for (i = 0; i < 5; i++) {
                char_bitmap[i] = Font5x7[ascii_code * 5 + i];
            }
            draw_bitmap_in_buffer(char_bitmap, x, y, 5, 8, color);
        }
    }
}

/**
 * @brief 在 buffer 中印出一個字串
 * @param x x 座標
 * @param y y 座標
 * @param size 字元大小 (5 or 8)
 * @param format 格式化字串
*/
void printf_s_in_buffer(int16_t x, int16_t y, uint8_t size, const char *format, ...) {
    int8_t i;
    char buffer[100];
    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);
    for (i = 0; i < strlen(buffer); i++)
        print_c_in_buffer(x + i * size, y, size, buffer[i], FG_COLOR);
}

/**
 * @brief 在 buffer 中印出一行字串
 * @param line 行數
 * @param size 字元大小 (5 or 8)
 * @param format 格式化字串
*/
void printf_line_in_buffer(int8_t line, uint8_t size, const char *format, ...) {
    uint8_t line_height = (size == 5) ? 8 : 16;
    int8_t i;
    char buffer[100];
    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);
    for (i = 0; i < strlen(buffer); i++)
        print_c_in_buffer(i * 8, line * line_height, size, buffer[i], FG_COLOR);
}

/**
 * @brief 取得 buffer 中的一個 pixel
 * @param x x 座標
 * @param y y 座標
 * @return 0 or 1
*/
uint8_t get_lcd_buffer_pixel(int16_t x, int16_t y) {
    return (lcd_buffer_hex[x + y / 8 * LCD_Xmax] >> (y % 8)) & 0x01;
}
