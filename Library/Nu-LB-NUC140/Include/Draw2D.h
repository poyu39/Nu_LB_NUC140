#ifndef __2D_Graphic_Driver_H__
#define __2D_Graphic_Driver_H__
     
extern void draw_Line(int x1, int y1, int x2, int y2, uint16_t fg_color, uint16_t bg_color);

extern void draw_Circle(int xc, int yc, int r, uint16_t fg_color, uint16_t bg_color);

extern void draw_Rectangle(int x0, int y0, int x1, int y1, uint16_t fg_color, uint16_t bg_color);
extern void fill_Rectangle(int x0, int y0, int x1, int y1, uint16_t fg_color, uint16_t bg_color);

extern void draw_Triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint16_t fg_color, uint16_t bg_color);

#endif
