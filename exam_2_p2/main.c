#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "NewSevenSegment.h"
#include "NewLCD.h"
#include "NewBuzzer.h"
#include "open_bmp.c"
#include "closing_bmp.c"
#include "close_bmp.c"

volatile uint8_t task_active=0;
volatile uint32_t timer2_5ms=0, timer2_1s=0;

void rgb(uint8_t r, uint8_t g, uint8_t b) {
    r = !r; g = !g; b = !b;
    PA12 = b; PA13 = g; PA14 = r;
}

void init_gpio(void) {
	GPIO_SetMode(PA, (BIT12 | BIT13 | BIT14), GPIO_MODE_OUTPUT);
	PA12=1; PA13=1; PA14=1;
    GPIO_SetMode(PC, (BIT12 | BIT15), GPIO_MODE_OUTPUT);
    PC12=1; PC15=1;
}

void TMR2_IRQHandler(void) {
    TIMER_ClearIntFlag(TIMER2);
    timer2_5ms = (timer2_5ms + 1) % INT32_MAX;
    if (timer2_5ms % 100 == 40 && task_active) buzzer_play(40);
    if (timer2_5ms % 100 == 0 && task_active) {
        PC12 = ~PC12;
        PC15 = ~PC15;
    }
    if (timer2_5ms % 200 == 0 && task_active)
        timer2_1s = (timer2_1s + 1) % INT32_MAX;
}

void init_timer2(void) {
    TIMER_Open(TIMER2, TIMER_PERIODIC_MODE, 200);
    TIMER_EnableInt(TIMER2);
    NVIC_EnableIRQ(TMR2_IRQn);
    TIMER_Start(TIMER2);
}

void EINT1_IRQHandler(void) {
    GPIO_CLR_INT_FLAG(PB, BIT15);
    task_active = 1;
    timer2_1s = 0;
    timer2_5ms = 0;
    PC12 = 0;
    set_seg_buffer_number(15 - timer2_1s, TRUE);
}

void init_EXTINT(void) {
    GPIO_SetMode(PB, BIT15, GPIO_MODE_INPUT);
    GPIO_EnableEINT1(PB, 15, GPIO_INT_RISING);
    NVIC_EnableIRQ(EINT1_IRQn);
    GPIO_SET_DEBOUNCE_TIME(GPIO_DBCLKSRC_LIRC, GPIO_DBCLKSEL_64);
    GPIO_ENABLE_DEBOUNCE(PB, BIT15);
}

int main(void) {
    // init
    SYS_Init();
    init_EXTINT();
    init_timer2();
    init_seg(TRUE, 200);
    init_keypad_INT();
    init_lcd();
    init_gpio();
    init_buzzer();

    rgb(0, 1, 0);
    draw_bitmap_in_buffer(open_bmp_hex, 0, 0, 128, 8, FG_COLOR);
    show_lcd_buffer();

    while (TRUE) {
        if (!task_active) continue;
        set_seg_buffer_number(15 - timer2_1s, TRUE);

        // rgb
        if ((15 - timer2_1s) <= 15 && (15 - timer2_1s) > 10) rgb(1, 1, 0);
        else rgb(1, 0, 0);

        // lcd
        clear_lcd_buffer();
        if ((15 - timer2_1s) <= 15 && (15 - timer2_1s) > 10) {
            draw_bitmap_in_buffer(open_bmp_hex, 0, 0, 128, 8, FG_COLOR);
        } else if ((15 - timer2_1s) <= 10 && (15 - timer2_1s) >= 9) {
            draw_bitmap_in_buffer(closing_bmp_hex, 0, 0, 128, 8, FG_COLOR);
        } else if ((15 - timer2_1s) <= 8 && (15 - timer2_1s) >= 3) {
            draw_bitmap_in_buffer(close_bmp_hex, 0, 0, 128, 8, FG_COLOR);
        } else if ((15 - timer2_1s) <= 2 && (15 - timer2_1s) >= 1) {
            draw_bitmap_in_buffer(closing_bmp_hex, 0, 0, 128, 8, FG_COLOR);
        }
        show_lcd_buffer();

        // reset
        if (15 - timer2_1s == 0) {
            rgb(0, 1, 0);
            timer2_1s = 0;
            PC12 = 1; PC15 = 1;
            task_active = 0;
            clear_lcd_buffer();
            draw_bitmap_in_buffer(open_bmp_hex, 0, 0, 128, 8, FG_COLOR);
            show_lcd_buffer();
            clear_seg_buffer();
            SEG_LOOP = 0;
            rgb(0, 1, 0);
        }
    }
}
