#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "NewLCD.h"
#include "PowerDown.h"

#define MATH_SYMBOLS "+-*/"

volatile uint8_t KEY_Flag;
volatile uint32_t timer1_5ms, timer1_1s, timer1_100ms, index_key_scan;

volatile uint8_t line = 0;
volatile int A = 0;
volatile int B = 0;
volatile int ANS = 0;
volatile uint8_t math_symbol = 0;

volatile uint8_t sleep = 0;

void TMR1_IRQHandler(void) {
    timer1_5ms++;
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
        if (timer1_1s % 5 == 0 && sleep == 0) {
            clear_lcd();
            sleep = 1;
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

void EINT1_IRQHandler(void) {
    clear_lcd();
    clear_lcd_buffer();
    line = 0;
    A = 0;
    B = 0;
    ANS = 0;
    math_symbol = 0;
    GPIO_CLR_INT_FLAG(PB, BIT15);
}

void Init_EXTINT(void) {
    GPIO_SetMode(PB, BIT15, GPIO_MODE_INPUT);
    GPIO_EnableEINT1(PB, 15, GPIO_INT_RISING);
    NVIC_EnableIRQ(EINT1_IRQn);

    GPIO_SET_DEBOUNCE_TIME(GPIO_DBCLKSRC_LIRC, GPIO_DBCLKSEL_64);
    GPIO_ENABLE_DEBOUNCE(PB, BIT15);
}

int math(int x, int y, int i) {
	switch (i) {
	case 0:
		return x + y;
	case 1:
		return x - y;
	case 2:
		return x * y;
	case 3:
		return x / y;
    default:
        break;
    }
}

int main(void) {
    char TEXT[16];
    int i;

    timer1_5ms = timer1_1s = timer1_100ms = 0;
    KEY_Flag = 0;

    SYS_Init();
    Init_Timer1();
    Init_KEY();
    Init_EXTINT();

    init_seven_segment();
    init_lcd();
    init_lcd_buffer();
    clear_lcd();

    while (TRUE) {
        // 監聽鍵盤
        switch (KEY_Flag) {
        case 0:
            break;
        case 1:
        case 2:
            if (line == 0 && A < 4445) {
                A = A * 10 + KEY_Flag;
                sprintf(TEXT, "%d", A);
                print_line_in_buffer(line, TEXT, 8);
            } else if (line == 2 && B < 4445) {
                B = B * 10 + KEY_Flag;
                sprintf(TEXT, "%d", B);
                print_line_in_buffer(line, TEXT, 8);
            }
            show_lcd_buffer();
            sleep = 0;
            KEY_Flag = 0;
            break;
        case 4:
        case 5:
            if (line == 0 && A < 4445) {
                A = A * 10 + KEY_Flag - 1;
                sprintf(TEXT, "%d", A);
                print_line_in_buffer(line, TEXT, 8);
            } else if (line == 2 && B < 4445) {
                B = B * 10 + KEY_Flag - 1;
                sprintf(TEXT, "%d", B);
                print_line_in_buffer(line, TEXT, 8);
            }
            show_lcd_buffer();
            sleep = 0;
            KEY_Flag = 0;
            break;
        case 3:
            if (line == 0 && A != 0) {
                line++;
                print_line_in_buffer(line, "+", 8);
                show_lcd_buffer();
                math_symbol = 0;
                line++;
            }
            KEY_Flag = 0;
            break;
        case 6:
            if (line == 0 && A != 0) {
                line++;
                print_line_in_buffer(line, "-", 8);
                show_lcd_buffer();
                math_symbol = 1;
                line++;
            }
            KEY_Flag = 0;
            break;
        case 8:
            if (line == 0 && A != 0) {
                line++;
                print_line_in_buffer(line, "/", 8);
                show_lcd_buffer();
                math_symbol = 3;
                line++;
            }
            KEY_Flag = 0;
            break;
        case 9:
            if (line == 0 && A != 0) {
                line++;
                print_line_in_buffer(line, "*", 8);
                show_lcd_buffer();
                math_symbol = 2;
                line++;
            }
            KEY_Flag = 0;
            break;
        case 7:
            if (line == 2 && B != 0) {
                line++;
                sprintf(TEXT, "%d", math(A, B, math_symbol));
                print_line_in_buffer(line, TEXT, 8);
                show_lcd_buffer();
            }
            KEY_Flag = 0;
            break;
        default:
            break;
        }
    }
}
