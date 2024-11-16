/**
 * Author: poyu39
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "Seven_Segment.h"
#include "Scankey.h"
#include "LCD.h"

#define TARGET_FALL_TICK 500
#define SEVEN_SEG_UPDATE_TICK 5
#define LCD_UPDATE_TICK 1
#define TICK_PER_MS 1000

#define LCD_LINE_SPACE "                "
#define MATH_SYMBOLS "+-*/"
#define NUM_MAX 999999
#define NUM_MIN -999999

#define SEG_NONE 0xFF
#define SEG_N0 0x82
#define SEG_N1 0xEE
#define SEG_N2 0x07
#define SEG_N3 0x46
#define SEG_N4 0x6A
#define SEG_N5 0x52
#define SEG_N6 0x12
#define SEG_N7 0xE6
#define SEG_N8 0x02
#define SEG_N9 0x62
#define SEG_N10 0x22
#define SEG_N11 0x1A
#define SEG_N12 0x93
#define SEG_N13 0x0E
#define SEG_N14 0x13
#define SEG_N15 0x33
#define SEG_MINUS 0x7F

uint8_t seg_map[18] = {SEG_MINUS, SEG_NONE, SEG_N0, SEG_N1, SEG_N2, SEG_N3, SEG_N4, SEG_N5, SEG_N6, SEG_N7, SEG_N8, SEG_N9, SEG_N10, SEG_N11, SEG_N12, SEG_N13, SEG_N14, SEG_N15};

int cal(int x, int y, uint8_t i) {
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
        return 0;
    }
}

void show_one_seg(uint8_t no, int8_t sn) {
    PE->DOUT = seg_map[sn + 2];
    GPIO_PIN_DATA(2, no + 4) = 1;
}

int8_t* num_to_array(int num) {
    int8_t num_array[4];
    int i;
    uint8_t is_negative = 0;
    if (num < 0) {
        is_negative = 1;
        num = num * -1;
    }
    for (i = 0; i < 4; i++) {
        num_array[i] = num / (int)pow(10, i) % 10;
    }
    for (i = 3; i >= 0; i--) {
        if (num_array[i] == 0) {
            num_array[i] = -1;
        } else {
            break;
        }
    }
    if (is_negative) num_array[i + 1] = -2;
    return num_array;
}

void show_seven_seg(int s_num) {
    static uint8_t sindex = 0;
    int8_t* s_num_array = num_to_array(s_num);
    sindex = (sindex + 1) % 4;
    CloseSevenSegment();
    show_one_seg(sindex, s_num_array[sindex]);
}

void show_lcd(char lcd_buffer[4][17], char lcd_now[4][17]) {
    // int lcd_x, int lcd_y;
    static uint8_t lcd_x = 0, lcd_y = 0;
    if (lcd_buffer[lcd_y][lcd_x] != lcd_now[lcd_y][lcd_x]) {
        lcd_now[lcd_y][lcd_x] = lcd_buffer[lcd_y][lcd_x];
        printC(lcd_x * 8, lcd_y * 16, lcd_now[lcd_y][lcd_x]);
    }
    if (lcd_y < 4) {
        if (lcd_x < 16) {
            lcd_x++;
        } else {
            lcd_x = 0;
            lcd_y++;
        }
    } else {
        lcd_x = 0;
        lcd_y = 0;
    }
}

void fall_target(int* target, char lcd_buffer[4][17], int* num, uint8_t math_op_index) {
    static int target_queue[4] = {0};
    
    int i;
    char target_str[4];
    int8_t* target_array = num_to_array(*target);
    
    // to ascii code
    for (i = 0; i < 4; i++) {
        if (target_array[i] == -1) {
            target_str[i] = ' ';
        } else if (target_array[i] == -2) {
            target_str[i] = '-';
        } else {
            target_str[i] = target_array[i] + '0';
        }
    }
    
    // shift the line
    sprintf(lcd_buffer[2], "%s", lcd_buffer[1]);
    sprintf(lcd_buffer[1], "%s", lcd_buffer[0]);
    sprintf(lcd_buffer[0], "%s", LCD_LINE_SPACE);
    if (*target != 0) {
        sprintf(lcd_buffer[0], "  %c   %c    %c   %c", target_str[3], target_str[2], target_str[1], target_str[0]);
    }
    // shift the target
    target_queue[3] = target_queue[2];
    target_queue[2] = target_queue[1];
    target_queue[1] = target_queue[0];
    target_queue[0] = *target;
    if (target_queue[3] != 0) {
        *num = cal(*num, target_queue[3], math_op_index);
        if (*num > NUM_MAX) *num = NUM_MAX;
        if (*num < NUM_MIN) *num = -NUM_MAX;
    }
    // clear target
    *target = 0;
}

int main(void) {
    uint8_t keyin = 0, is_pressed = 0;
    uint16_t loop_count = 0;
    
    int target = 0, num = 0;
    uint8_t math_op_index = 0;
    
    char lcd_buffer[4][17] = {LCD_LINE_SPACE, LCD_LINE_SPACE, LCD_LINE_SPACE, LCD_LINE_SPACE};
    char lcd_now[4][17] = {LCD_LINE_SPACE, LCD_LINE_SPACE, LCD_LINE_SPACE, LCD_LINE_SPACE};
    
    SYS_Init();
    OpenKeyPad();
    init_LCD();
    clear_LCD();
    
    while (TRUE) {
        CLK_SysTickDelay(TICK_PER_MS);
        
        // let loop_count increase and avoid overflow
        loop_count = (loop_count + 1) % UINT16_MAX;
        
        // scheduler
        if (loop_count % SEVEN_SEG_UPDATE_TICK == 0) show_seven_seg(target);
        if (loop_count % TARGET_FALL_TICK == 0) fall_target(&target, lcd_buffer, &num, math_op_index);
        sprintf(lcd_buffer[3], "NUM: %-9d %c", num, MATH_SYMBOLS[math_op_index]);       // keep show num and math op
        if (loop_count % LCD_UPDATE_TICK == 0) show_lcd(lcd_buffer, lcd_now);
        
        keyin = ScanKey();
        
        // if no key is pressed, release the is_pressed flag and skip
        if (keyin == 0) {
            is_pressed = 0;
            continue;
        }
        
        // if the key is pressed, then skip
        if (is_pressed) continue;
        is_pressed = 1;
        
        // random -999 ~ 999, no 0
        if (keyin == 3) {
            if (target != 0) continue;
            srand(loop_count);
            target = rand() % 1999 - 999;
            while (target == 0) {
                target = rand() % 1999 - 999;
            }
            loop_count = 0;
        }
        
        if (keyin == 6) num = 0;
        if (keyin == 1) math_op_index = 0;
        if (keyin == 2) math_op_index = 1;
        if (keyin == 4) math_op_index = 2;
        if (keyin == 5) math_op_index = 3;
    }
}

