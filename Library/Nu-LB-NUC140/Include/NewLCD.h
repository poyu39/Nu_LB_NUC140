#define LCD_Xmax 128
#define LCD_Ymax 64
#define FG_COLOR 0xFFFF
#define BG_COLOR 0x0000

extern void init_LCD(void);

extern void clear_LCD(void);

extern int lcdSetAddr(uint8_t PageAddr, uint8_t ColumnAddr);

extern int lcdWriteData(uint8_t temp);

extern void show_lcd_buffer(void);

extern void init_lcd_buffer(void);

extern void clear_lcd_buffer(void);

extern uint8_t get_lcd_buffer_bin(int16_t x, int16_t y);

extern void draw_pixel_in_buffer(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor);

extern void draw_line_in_buffer(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t fgColor, uint16_t bgColor);

extern void draw_circle_in_buffer(int16_t xc, int16_t yc, int16_t r, uint16_t fgColor, uint16_t bgColor, uint8_t isFill);

extern void draw_rectangle_in_buffer(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t fgColor, uint16_t bgColor, uint8_t isFill);

extern void draw_triangle_in_buffer(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t fgColor, uint16_t bgColor, uint8_t isFill);