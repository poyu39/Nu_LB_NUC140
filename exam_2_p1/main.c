#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "NewSevenSegment.h"
#include "NewLCD.h"
#include "NewScankey.h"
#include "NewBuzzer.h"
#include <string.h>

const char menu[6][6] = {"WATER", "MILK", "COLA", "JUICE", "TEA", "EXIT"};
const uint8_t menu_price[6] = {15, 50, 30, 25, 10, 0};
volatile uint32_t timer2_5ms=0, timer2_1s=0;
volatile uint8_t select_state=0, check_out_flag=0, reset_state_timer=0;
volatile int16_t refund=0, money=0;
uint8_t select_line=0;


void start_state() {
    money = 0;
    refund = 0;
    SEG_LOOP = 1;
    clear_seg_buffer();
    clear_lcd_buffer();
    print_line_in_buffer(1, "PLZ PUT IN COINS", 8);
    show_lcd_buffer();
}

void menu_state() {
    int i, start_index;
    char text_temp[16];
    clear_lcd_buffer();
    if (select_line > 3) start_index = select_line - 3;
    else start_index = 0;
    for (i = 0; i < 4; i++) {
        strcpy(text_temp, "");
        if (i == select_line - start_index && select_state == 1) strcat(text_temp, ">");
        strcat(text_temp, menu[i + start_index]);
        if (start_index == 2 && i == 3) {
            sprintf(text_temp, "%-10s", text_temp);
        } else {
            sprintf(text_temp, "%-10s $%2d", text_temp, menu_price[i + start_index]);
        }
        print_line_in_buffer(i, text_temp, 8);
    }
    show_lcd_buffer();
    select_state = (select_state + 1) % 2;
}

void checkout_state() {
    int i;
    char text_temp[16];
    clear_lcd_buffer();
    if (refund >= 0) {
        sprintf(text_temp, "Thank you!!");
    } else {
        sprintf(text_temp, "Not enough $%d", -refund);
    }
    print_line_in_buffer(1, text_temp, 8);
    if (refund >= 0) {
        sprintf(text_temp, "Refund $%d", refund);
    } else {
        sprintf(text_temp, "Refund $%d", money);
        refund = money;
    }
    print_line_in_buffer(2, text_temp, 8);
    show_lcd_buffer();

    for (i = 0; i < refund / 50; i++) {
        buzzer_play(400);
        while (buzzer_is_playing());
        CLK_SysTickDelay(200000);
    }
    refund %= 50;
    for (i = 0; i < refund / 10; i++) {
        buzzer_play(200);
        while (buzzer_is_playing());
        CLK_SysTickDelay(200000);
    }
    refund %= 10;
    for (i = 0; i < refund / 5; i++) {
        buzzer_play(100);
        while (buzzer_is_playing());
        CLK_SysTickDelay(200000);
    }
    money = 0;
    select_line = 0;
    refund = 0;
    check_out_flag = 0;
    reset_state_timer = 1;
}

void TMR2_IRQHandler(void) {
    TIMER_ClearIntFlag(TIMER2);
    timer2_5ms = (timer2_5ms + 1) % INT32_MAX;
    if (timer2_5ms % 200 == 0) {
        timer2_1s = (timer2_1s + 1) % INT32_MAX;
        if (reset_state_timer > 0) reset_state_timer++;
    }
}

void init_timer2(void) {
    TIMER_Open(TIMER2, TIMER_PERIODIC_MODE, 200);
    TIMER_EnableInt(TIMER2);
    NVIC_EnableIRQ(TMR2_IRQn);
    TIMER_Start(TIMER2);
}

void EINT1_IRQHandler(void) {
    GPIO_CLR_INT_FLAG(PB, BIT15);
    refund = money - menu_price[select_line];
    check_out_flag = 1;
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
    init_buzzer();

    start_state();
    PD14 = 0;

    while (TRUE) {
        if (KEY_FLAG != 0) {
            switch (KEY_FLAG) {
            case 1:
                money += 5;
                buzzer_play(100);
                set_seg_buffer_number(money, FALSE);
                break;
            case 5:
                money += 10;
                buzzer_play(200);
                set_seg_buffer_number(money, FALSE);
                break;
            case 9:
                money += 50;
                buzzer_play(400);
                set_seg_buffer_number(money, FALSE);
                break;
            case 2:
                if (select_line > 0) select_line--;
                break;
            case 8:
                if (select_line < 5) select_line++;
                break;
            default:
                break;
            }
            KEY_FLAG = 0;
            if (reset_state_timer > 0) reset_state_timer = 0;
        }
        if (money != 0) {
            if (timer2_5ms % 100 == 60 || timer2_5ms % 100 == 0) menu_state();
        }
        if (check_out_flag) {
            set_seg_buffer_number(0, FALSE);
            SEG_LOOP = 0;
            show_one_seg(0, 0);
            checkout_state();
        }
        if (reset_state_timer > 5) {
            reset_state_timer = 0;
            start_state();
        }
    }
}
