#include <stdio.h>
#include <stdlib.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "NewLCD.h"

void rand_hello_world(uint8_t size) {
    // size == 8 -> font8x16
    // size == 5 -> font5x7
    char text[] = "Hello World!";
    int x = rand() % (128 - sizeof(text) * size);
    int y;
    if (size == 8)
        y = rand() % (64 - size);
    else if (size == 5)
        y = rand() % (64 - size);
    print_s_in_buffer(x, y, text, size);
}

int main(void) {
    SYS_Init();
    init_lcd();
    while (TRUE) {
        rand_hello_world(5);
        show_lcd_buffer();
        CLK_SysTickDelay(200000);
        clear_lcd_buffer();
    }
}
