#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "NewLCD.h"
#include "NewSevenSegment.h"

#define MATH_SYMBOLS "+-*/"

volatile uint8_t KEY_Flag;
volatile uint32_t timer1_5ms, timer1_1s, timer1_100ms, index_key_scan;
volatile uint32_t digit[4];

volatile uint8_t minute;
volatile uint8_t second;
volatile uint8_t pause;

void TMR1_IRQHandler(void) {
    timer1_5ms++;
    close_seven_segment();
    show_seven_segment(timer1_5ms % 4, digit[timer1_5ms % 4]);
    if (timer1_5ms % 20 == 0) {
        timer1_100ms++;
        index_key_scan = timer1_100ms++ % 3;
        if (index_key_scan == 0) {
            PA0 = 1;
            PA1 = 1;
            PA2 = 1;
            PA3 = 1;
            PA4 = 1;
            PA5 = 0;
        }
        if (index_key_scan == 1) {
            PA0 = 1;
            PA1 = 1;
            PA2 = 1;
            PA3 = 1;
            PA4 = 0;
            PA5 = 1;
        }
        if (index_key_scan == 2) {
            PA0 = 1;
            PA1 = 1;
            PA2 = 1;
            PA3 = 0;
            PA4 = 1;
            PA5 = 1;
        }
        NVIC_EnableIRQ(GPAB_IRQn);
    }

    if (timer1_5ms % 200 == 0) {
        timer1_1s++;
        if (pause == 0) {
            second++;
            if (second >= 60) {
                second = 0;
                minute++;
            }
        }
    }
    TIMER_ClearIntFlag(TIMER1);  // Clear Timer1 time-out interrupt flag
}

void GPAB_IRQHandler(void) {
    NVIC_DisableIRQ(GPAB_IRQn);

    if (PA->ISRC & BIT0) {  // check if PA0 interrupt occurred
        PA0 = 1;
        PA->ISRC |= BIT0;  // clear PA0 interrupt status

        if (PA3 == 0) {
            KEY_Flag = 3;
            PA3 = 1;
        }
        if (PA4 == 0) {
            KEY_Flag = 6;
            PA4 = 1;
        }
        if (PA5 == 0) {
            KEY_Flag = 9;
            PA5 = 1;
        }
        return;
    }
    if (PA->ISRC & BIT1) {  // check if PA1 interrupt occurred
        PA1 = 1;
        PA->ISRC |= BIT1;  // clear PA1 interrupt status
        if (PA3 == 0) {
            KEY_Flag = 2;
            PA3 = 1;
        }
        if (PA4 == 0) {
            KEY_Flag = 5;
            PA4 = 1;
        }
        if (PA5 == 0) {
            KEY_Flag = 8;
            PA5 = 1;
        }
        return;
    }
    if (PA->ISRC & BIT2) {  // check if PB14 interrupt occurred
        PA2 = 1;
        PA->ISRC |= BIT2;  // clear PA interrupt status
        if (PA3 == 0) {
            KEY_Flag = 1;
            PA3 = 1;
        }
        if (PA4 == 0) {
            KEY_Flag = 4;
            PA4 = 1;
        }
        if (PA5 == 0) {
            KEY_Flag = 7;
            PA5 = 1;
        }
        return;
    }                     // else it is unexpected interrupts
    PA->ISRC = PA->ISRC;  // clear all GPB pins
}

void Init_Timer1(void) {
    // TIMER_Open(TIMER1, TIMER_PERIODIC_MODE, 500);
    TIMER_Open(TIMER1, TIMER_PERIODIC_MODE, 200);
    TIMER_EnableInt(TIMER1);
    NVIC_EnableIRQ(TMR1_IRQn);
    TIMER_Start(TIMER1);
}

void Init_KEY(void) {
    GPIO_SetMode(PA, (BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5), GPIO_MODE_QUASI);
    GPIO_EnableInt(PA, 0, GPIO_INT_LOW);
    GPIO_EnableInt(PA, 1, GPIO_INT_LOW);
    GPIO_EnableInt(PA, 2, GPIO_INT_LOW);
    NVIC_EnableIRQ(GPAB_IRQn);
    NVIC_SetPriority(GPAB_IRQn, 3);
    GPIO_SET_DEBOUNCE_TIME(GPIO_DBCLKSRC_LIRC, GPIO_DBCLKSEL_256);
    GPIO_ENABLE_DEBOUNCE(PA, (BIT0 | BIT1 | BIT2));
}

int main(void) {
    char TEXT[16];
    int i;
    int line = 0;

    timer1_5ms = timer1_1s = timer1_100ms = 0;
    digit[3] = digit[2] = digit[1] = digit[0] = 16;
    KEY_Flag = 0;

    minute = 0;
    second = 0;
    pause = 1;

    SYS_Init();
    Init_Timer1();
    Init_KEY();
    // Init_EXTINT();
    // Init_WDT();

    init_seven_segment();
    init_lcd();
    init_lcd_buffer();
    clear_lcd();

    while (TRUE) {
        // 更新七段顯示器
        digit[3] = minute % 100 / 10;
        digit[2] = minute % 10;
        digit[1] = second % 100 / 10;
        digit[0] = second % 10;

        // 監聽鍵盤
        switch (KEY_Flag) {
        case 0:
            break;
        case 1:
            // clear_lcd();
            // clear_lcd_buffer();
            // show_lcd_buffer();
            pause++;
            if (pause > 1) pause = 0;
            KEY_Flag = 0;
            break;
        case 2:
            if (line < 3) {
                sprintf(TEXT, "%d: %d:%d", line, minute, second);
                print_line_in_buffer(line, TEXT, 8);
                line++;
                show_lcd_buffer();
            }
            KEY_Flag = 0;
            break;
        case 3:
            minute = 0;
            second = 0;
            pause = 1;
            clear_lcd();
            clear_lcd_buffer();
            KEY_Flag = 0;
            break;
        default:
            break;
        }
    }
}
