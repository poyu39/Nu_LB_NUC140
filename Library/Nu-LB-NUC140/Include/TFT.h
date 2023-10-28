 
#define LCD_Xmax 320
#define LCD_Ymax 240
#define FG_COLOR 0xFFFF
#define BG_COLOR 0x0000

extern void init_LCD(void);

extern void clear_LCD(void);

extern void printC(int16_t x, int16_t y, unsigned char ascii_code);

extern void printC_TFT(int16_t x, int16_t y, char c, uint16_t fgColor, uint16_t bgColor);

extern void print_Line(int8_t line, char text[]);

extern void printS(int16_t x, int16_t y, char text[]);

extern void printS_TFT(unsigned short x, unsigned short y, char text[], unsigned int fgColor, unsigned int bgColor);

extern void draw_Bmp8x8(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor, unsigned char bitmap[]);
extern void draw_Bmp16x8(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor, unsigned char bitmap[]);
extern void draw_Bmp16x16(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor, unsigned char bitmap[]);
extern void draw_Bmp16x32(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor, unsigned char bitmap[]);
extern void draw_Bmp16x48(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor, unsigned char bitmap[]);
extern void draw_Bmp16x64(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor, unsigned char bitmap[]);
extern void draw_Bmp32x16(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor, unsigned char bitmap[]);
extern void draw_Bmp32x32(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor, unsigned char bitmap[]);
extern void draw_Bmp32x48(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor, unsigned char bitmap[]);
extern void draw_Bmp32x64(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor, unsigned char bitmap[]);
extern void draw_Bmp64x64(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor, unsigned char bitmap[]);

extern void draw_Pixel(int16_t x, int16_t y, uint16_t fgColor, uint16_t bgColor);

extern void draw_LCD(unsigned char *buffer);

extern void draw_TFT(unsigned char *buffer, uint16_t fgColor, uint16_t bgColor);
