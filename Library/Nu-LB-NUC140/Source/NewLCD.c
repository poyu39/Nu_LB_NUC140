/*
    NewLCD
    Author: poyu39
    Discord: poyu39
*/
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "NUC100Series.h"
#include "SYS.h"
#include "SPI.h"
#include "GPIO.h"
#include "LCD.h"
#include "Font5x7.h"
#include "Font8x16.h"

uint8_t lcd_buffer_hex[LCD_Xmax * (LCD_Ymax / 8)];
uint8_t lcd_buffer_hex_last[LCD_Xmax * (LCD_Ymax / 8)];

void init_SPI3(void) {
    /* Configure as a master, clock idle low, 9-bit transaction, drive output on falling clock edge and latch input on rising edge. */
    /* Set IP clock divider. SPI clock rate = 1MHz */
    SPI_Open(SPI3, SPI_MASTER, SPI_MODE_0, 9, 1000000);
    SPI_DisableAutoSS(SPI3);
}

void lcdWriteCommand(unsigned char temp) {
    SPI_SET_SS0_LOW(SPI3);
    SPI_WRITE_TX0(SPI3, temp);  // Write Data
    SPI_TRIGGER(SPI3);          // Trigger SPI data transfer
    while (SPI_IS_BUSY(SPI3))
        ;  // Check SPI3 busy status
    SPI_SET_SS0_HIGH(SPI3);
}

// Wrtie data to LCD
void lcdWriteData(uint8_t temp) {
    SPI_SET_SS0_LOW(SPI3);
    SPI_WRITE_TX0(SPI3, 0x100 + temp);  // Write Data
    SPI_TRIGGER(SPI3);                  // Trigger SPI data transfer
    while (SPI_IS_BUSY(SPI3))
        ;  // Check SPI3 busy status
    SPI_SET_SS0_HIGH(SPI3);
}

// Set Address to LCD
void lcdSetAddr(uint8_t PageAddr, uint8_t ColumnAddr) {
    // Set PA
    SPI_SET_SS0_LOW(SPI3);
    SPI_WRITE_TX0(SPI3, 0xB0 | PageAddr);  // Write Data
    SPI_TRIGGER(SPI3);                     // Trigger SPI data transfer
    while (SPI_IS_BUSY(SPI3))
        ;  // Check SPI3 busy status
    SPI_SET_SS0_HIGH(SPI3);
    // Set CA MSB
    SPI_SET_SS0_LOW(SPI3);
    SPI_WRITE_TX0(SPI3, 0x10 | (ColumnAddr >> 4) & 0xF);  // Write Data
    SPI_TRIGGER(SPI3);                                    // Trigger SPI data transfer
    while (SPI_IS_BUSY(SPI3))
        ;  // Check SPI3 busy status
    SPI_SET_SS0_HIGH(SPI3);
    // Set CA LSB
    SPI_SET_SS0_LOW(SPI3);
    SPI_WRITE_TX0(SPI3, 0x00 | (ColumnAddr & 0xF));  // Write Data
    SPI_TRIGGER(SPI3);                               // Trigger SPI data transfer
    while (SPI_IS_BUSY(SPI3))
        ;  // Check SPI3 busy status
    SPI_SET_SS0_HIGH(SPI3);
}

void init_LCD(void) {
    init_SPI3();
    lcdWriteCommand(0xEB);
    lcdWriteCommand(0x81);
    lcdWriteCommand(0xA0);
    lcdWriteCommand(0xC0);
    lcdWriteCommand(0xAF);  // Set Display Enable
}

void show_lcd_buffer() {
    uint8_t x, y;
    for (x = 0; x < LCD_Xmax; x++) {
        for (y = 0; y < (LCD_Ymax / 8); y++) {
            if (lcd_buffer_hex[x + y * LCD_Xmax] == lcd_buffer_hex_last[x + y * LCD_Xmax]) continue;
            lcd_buffer_hex_last[x + y * LCD_Xmax] = lcd_buffer_hex[x + y * LCD_Xmax];
            lcdSetAddr(y, (LCD_Xmax + 1 - x));
            lcdWriteData(lcd_buffer_hex[x + y * LCD_Xmax]);
        }
    }
}

void init_lcd_buffer() {
    int i, x, y;
    for (i = 0; i < LCD_Xmax * LCD_Ymax / 8; i++) {
        lcd_buffer_hex[i] = 0x00;
        lcd_buffer_hex_last[i] = 0x00;
    }
    for (x = 0; x < LCD_Xmax; x++) {
        for (y = 0; y < (LCD_Ymax / 8); y++) {
            lcdSetAddr(y, (LCD_Xmax + 1 - x));
            lcdWriteData(lcd_buffer_hex[x + y * LCD_Xmax]);
        }
    }
}

void clear_lcd_buffer() {
    int i;
    for (i = 0; i < LCD_Xmax * LCD_Ymax / 8; i++) {
        lcd_buffer_hex[i] = 0x00;
    }
}

void draw_pixel_in_buffer(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor) {
    if (fgColor == FG_COLOR)
        lcd_buffer_hex[x + y / 8 * LCD_Xmax] |= (0x01 << (y % 8));
    else if (fgColor == BG_COLOR)
        lcd_buffer_hex[x + y / 8 * LCD_Xmax] &= (0xFE << (y % 8));
}

void draw_bitmap_in_buffer(int16_t x, int16_t y, uint8_t bitmap[], uint16_t fgColor, uint16_t bgColor) {
    uint8_t t, i, k, kx, ky;
    if (x < (LCD_Xmax - 7) && y < (LCD_Ymax - 7)) {
        for (i = 0; i < 8; i++) {
            kx = x + i;
            t = bitmap[i];
            for (k = 0; k < 8; k++) {
                ky = y + k;
                if (t & (0x01 << k))
                    draw_pixel_in_buffer(kx, ky, fgColor, bgColor);
            }
        }
    }
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

void print_c_in_buffer(int16_t x, int16_t y, unsigned char ascii_code, uint8_t size) {
    int8_t i, j;
    uint8_t char_bitmap[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    if (size == 8) {
        for (i = 0; i < 2; i++) {
            for (j = 0; j < 8; j++) {
                char_bitmap[j] = Font8x16[(ascii_code - 0x20) * 16 + i * 8 + j];
            }
            draw_bitmap_in_buffer(x, y + i * 8, char_bitmap, FG_COLOR, BG_COLOR);
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
            draw_bitmap_in_buffer(x, y, char_bitmap, FG_COLOR, BG_COLOR);
        }
    }
}

void print_s_in_buffer(int16_t x, int16_t y, char text[], uint8_t size) {
    int8_t i;
    for (i = 0; i < strlen(text); i++)
        print_c_in_buffer(x + i * size, y, text[i], size);
}

void print_line_in_buffer(int8_t line, char text[]) {
    int8_t i;
    for (i = 0; i < strlen(text); i++)
        print_c_in_buffer(i * 8, line * 16, text[i], 8);
}

uint8_t get_lcd_buffer_bin(int16_t x, int16_t y) {
    return (lcd_buffer_hex[x + y / 8 * LCD_Xmax] >> (y % 8)) & 0x01;
}
