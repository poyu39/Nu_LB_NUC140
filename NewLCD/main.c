#include <stdio.h>
#include <stdlib.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "NewLCD.h"

int main(void) {
    int i;
    SYS_Init();
    init_LCD();
    init_lcd_buffer();
    while (TRUE) {
        for (i = 0; i < 9; i++) {
            print_s_in_buffer(0, i * 7, "Hello World!", 5);
        }
        for (i = 0; i < 4; i++) {
            print_s_in_buffer(70, i * 16, "Hello", 8);
        }
        show_lcd_buffer();
    }
}
