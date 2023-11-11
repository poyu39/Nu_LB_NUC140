#include <stdio.h>
#include <stdlib.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "NewLCD.h"

int main(void) {
    SYS_Init();
    init_LCD();
    init_lcd_buffer();
    clear_lcd_buffer();
    show_lcd_buffer();

    // demo
    while (TRUE) {
        draw_circle_in_buffer(64, 32, 16, FG_COLOR, BG_COLOR, 1);
        show_lcd_buffer();
        CLK_SysTickDelay(10000000);
        // 這邊清空 buffer 後再畫入圓形，也不會閃爍，可以更直觀看到動態更新的效果
        clear_lcd_buffer();
        draw_circle_in_buffer(64, 32, 16, FG_COLOR, FG_COLOR, 0);
        show_lcd_buffer();
        CLK_SysTickDelay(10000000);
    }
}
