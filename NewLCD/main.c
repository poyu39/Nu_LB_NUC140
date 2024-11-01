#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "NewLCD.h"


uint8_t block_bmp[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};


volatile uint32_t timer0_tick = 0;

void TMR0_IRQHandler(void) {
    timer0_tick = (timer0_tick + 1) % UINT32_MAX;
    TIMER_ClearIntFlag(TIMER0);
}

void Init_Timer0(void) {
    TIMER_Open(TIMER0, TMR0_OPERATING_MODE, TMR0_OPERATING_FREQ);
    TIMER_EnableInt(TIMER0);
    NVIC_EnableIRQ(TMR0_IRQn);
    TIMER_Start(TIMER0);
}

int main(void) {
    uint8_t i;
    uint32_t stop_time;
    SYS_Init();
    Init_Timer0();
    init_lcd(TRUE, SPI3_CLOCK_FREQUENCY);
    
    while (TRUE) {
        for (i = 0; i < LCD_Xmax - 8; i++) {
            printf_line_in_buffer(0, 5, "Hello poyu%d", i);
            draw_bitmap_in_buffer(block_bmp, i, 16, 8, 8, FG_COLOR);
            printf_line_in_buffer(3, 5, "timer: %dms",  timer0_tick);
            show_lcd_buffer();
        }
        stop_time = timer0_tick + 5000;
        while(timer0_tick < stop_time);
        timer0_tick = 0;
    }
}
