#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "NewLCD.h"
#include "NewScankey.h"

int main(void) {
    char TEXT[16];
    int print_temp = 0;
    SYS_Init();
    init_keypad_INT();
    init_lcd();
    while (TRUE) {
        if (KEY_Flag != 0) {
            print_temp = print_temp * 10 + KEY_Flag;
            sprintf(TEXT, "%d", print_temp);
            print_line_in_buffer(0, TEXT, 8);
            show_lcd_buffer();
            if (print_temp > 9999) {
                print_temp = 0;
                clear_lcd_buffer();
            }
            KEY_Flag = 0;
        }
    }
}
