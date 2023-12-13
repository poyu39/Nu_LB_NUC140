#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "MPU6050.h"
#include "NVT_I2C.h"
#include "NewLCD.h"

#define PI 3.14159265359

int32_t main() {
    int16_t ax, ay, az;
    float pitch, roll;
    int8_t x = 8, y = 16, r = 4, dx = 0, dy = 0;   // 球的座標, 半徑, 方向
    int8_t vsp = 3;                                // 水平速度
    int8_t hsp = 3;                                // 垂直速度

    SYS_Init();
    init_lcd(TRUE, TRUE);

    printf_line_in_buffer(0, 8, "MPU6050 init...");
    show_lcd_buffer();
    NVT_I2C_Init(I2C0_CLOCK_FREQUENCY);
    MPU6050_address(MPU6050_DEFAULT_ADDRESS);
    MPU6050_initialize();

    while (TRUE) {
        MPU6050_getAcceleration(&ax, &ay, &az);
        pitch = atan(ax / sqrt(pow(ay, 2) + pow(az, 2))) * 180 / PI;
        roll = atan(ay / sqrt(pow(ax, 2) + pow(az, 2))) * 180 / PI;
        
        dx = 0;
        if (pitch > 10 || pitch < -10) dx = pitch / abs(pitch) * -1;

        dy = 0;
        if (roll > 10 || roll < -10) dy = roll / abs(roll);

        x += dx * vsp;
		y += dy * hsp;
		if(x - r < 0 || x + r > LCD_Xmax) x -= dx * vsp;
		if(y - r < 0 || y + r > LCD_Ymax) y -= dy * hsp;

        draw_circle_in_buffer(x, y, r, FG_COLOR, TRUE);
        show_lcd_buffer();
    }
}
