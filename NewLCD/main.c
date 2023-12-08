#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "NewLCD.h"

int main(void) {
    uint8_t i;
    SYS_Init();
    init_lcd();
    while (TRUE) {
        for (i = 0; i < 16; i++) {
            clear_lcd_buffer();
            printf_line_in_buffer(0, 8, "Hello World x %2d", i);
            show_lcd_buffer();
            CLK_SysTickDelay(100000);
        }
    }
}
