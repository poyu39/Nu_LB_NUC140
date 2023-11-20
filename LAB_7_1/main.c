#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "NewLCD.h"
#include "NewSevenSegment.h"

#define MATH_SYMBOLS "+-*/"

volatile uint8_t KEY_Flag;
volatile uint32_t index_5ms, timer1_5ms, timer1_1s, timer1_100ms, index_key_scan;
volatile uint32_t digit[4];

volatile uint8_t rand_num1 = 0;
volatile uint8_t rand_num2 = 0;
volatile uint8_t pause = 0;

void WDT_IRQHandler(void) {
    WDT_CLEAR_TIMEOUT_INT_FLAG();
}

void feeddog(void) {
    SYS_UnlockReg();
    WDT_RESET_COUNTER();
    SYS_LockReg();
}

void Init_WDT(void) {
    SYS_UnlockReg();
    WDT_Open(WDT_TIMEOUT_2POW16, 0, TRUE, TRUE);
    WDT_EnableInt();
    NVIC_EnableIRQ(WDT_IRQn);
    SYS_LockReg();
}

void EINT1_IRQHandler(void) {
    while (TRUE) {
        show_seven_segment(3, 14);
        CLK_SysTickDelay(100000);
        close_seven_segment();
    }
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

int get_rand() {
	int rand_num=0;
    rand_num = (rand() % 9 + 1) * 10;
    rand_num += rand() % 10;
    return rand_num;
}

void TMR1_IRQHandler(void) {
    timer1_5ms++;
    // index_5ms = timer1_5ms % 4;
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
            rand_num1 = get_rand();
            rand_num2 = get_rand();
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

    index_5ms = timer1_5ms = timer1_1s = timer1_100ms = 0;
    digit[3] = digit[2] = digit[1] = digit[0] = 16;
    KEY_Flag = 0;

    SYS_Init();
    Init_Timer1();
    Init_KEY();
    Init_EXTINT();
    Init_WDT();
    
    init_seven_segment();
    init_lcd();
    init_lcd_buffer();
    clear_lcd();

    rand_num1 = get_rand();
    rand_num2 = get_rand();

    while (TRUE) {
        // 更新七段顯示器
        digit[3] = rand_num1 % 100 / 10;
        digit[2] = rand_num1 % 10;
        digit[1] = rand_num2 % 100 / 10;
        digit[0] = rand_num2 % 10;

        // 監聽鍵盤
        switch (KEY_Flag) {
        case 0:
            break;
        case 5:
            clear_lcd();
            clear_lcd_buffer();
            if (pause == 0) {
                for (i = 0; i < 4; i++) {
                    sprintf(TEXT, "                ");
                    sprintf(TEXT, "%2d %c %2d = %4d ", rand_num1, MATH_SYMBOLS[i], rand_num2, math(rand_num1, rand_num2, i));
                    print_line_in_buffer(i, TEXT, 8);
                }
            }
            show_lcd_buffer();
            pause++;
            if (pause > 1) pause = 0;
            KEY_Flag = 0;
            break;
        default:
            break;
        }
        feeddog();
    }
}
