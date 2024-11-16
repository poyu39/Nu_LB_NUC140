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

#define LED_UPDATE_TICK 50
#define SEVEN_SEG_UPDATE_TICK 5
#define LCD_UPDATE_TICK 10
#define TICK_PER_MS 1000

#define LCD_LINE_SPACE "                "
#define NID "D1009212"
#define MATH_SYMBOLS "+-*/%^^"


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
    case 4:
        return x % y;
    case 5:
        return pow(x, 2);
    case 6:
        return pow(y, 2);
    default:
        return 0;
    }
}

void update_result(int result[], int x, int y) {
    uint8_t i;
    for (i = 0; i < 7; i++) {
        result[i] = cal(x, y, i);
    }
}

void update_lcd_buffer(int x, int y, int math_op_index, int result[], char lcd_buffer[3][17]) {
    uint8_t i;
    for (i = 0; i < 3; i++) {
        int index = math_op_index + i;
        if (index < 5) {
            sprintf(lcd_buffer[i], "%2d %c %2d = %4d  ", x, MATH_SYMBOLS[index], y, result[index]);
        } else if (index == 5) {
            sprintf(lcd_buffer[i], "%2d %c %2d = %4d  ", x, MATH_SYMBOLS[index], 2, result[index]);
        } else if (index == 6) {
            sprintf(lcd_buffer[i], "%2d %c %2d = %4d  ", y, MATH_SYMBOLS[index], 2, result[index]);
        } else if (index == 7) {
            sprintf(lcd_buffer[i], "END             ");
        }
    }
}

void show_seven_seg(int s_num) {
    static uint8_t sindex = 0;
    uint8_t this_num = 0;
    sindex = (sindex + 1) % 4;
    // get the number of this index
    this_num = s_num / (int)pow(10, sindex) % 10;
    if (this_num == 0) return;
    CloseSevenSegment();
    ShowSevenSegment(sindex, this_num);
}

void show_lcd(uint8_t* lcd_need_update, char lcd_buffer[3][17], char lcd_now[3][17]) {
    // int lcd_x, int lcd_y;
    static uint8_t lcd_x = 0, lcd_y = 0;
    if (lcd_buffer[lcd_y][lcd_x] != lcd_now[lcd_y][lcd_x]) {
        lcd_now[lcd_y][lcd_x] = lcd_buffer[lcd_y][lcd_x];
        printC(lcd_x * 8, (lcd_y + 1) * 16, lcd_now[lcd_y][lcd_x]);
    }
    if (lcd_y < 3) {
        if (lcd_x < 16) {
            lcd_x++;
        } else {
            lcd_x = 0;
            lcd_y++;
        }
    } else {
        lcd_x = 0;
        lcd_y = 0;
        *lcd_need_update = 0;
    }
}

void led_full(uint8_t* show_led_times) {
    uint8_t i;
    uint8_t is_on = 1;
    if (*show_led_times > 0) {
        is_on = 0;
        (*show_led_times)--;
    } else {
        is_on = 1;
    }
    for (i = 0; i < 4; i++) GPIO_PIN_DATA(2, 12 + i) = is_on;
}

void init_led() {
    uint8_t i;
    for (i = 0; i < 4; i++) {
        GPIO_SetMode(PC, BIT12 + i, GPIO_MODE_OUTPUT);
        GPIO_PIN_DATA(2, 12 + i) = 1;
    }
}

int main(void) {
    uint8_t keyin = 0, is_pressed = 0;
    uint16_t loop_count = 0;
    
    int x = 0, y = 0;
    int result[7];
    uint8_t math_op_index = 0;
    uint8_t show_led_times = 0;
    uint8_t lcd_need_update = 0;
    char lcd_buffer[3][17] = {LCD_LINE_SPACE, LCD_LINE_SPACE, LCD_LINE_SPACE};
    char lcd_now[3][17] = {LCD_LINE_SPACE, LCD_LINE_SPACE, LCD_LINE_SPACE};
    
    SYS_Init();
    init_led();
    OpenKeyPad();
    init_LCD();
    clear_LCD();
    
    // print NID
    print_Line(0, NID);
    
    while (TRUE) {
        CLK_SysTickDelay(TICK_PER_MS);
        
        // let loop_count increase and avoid overflow
        loop_count = (loop_count + 1) % UINT16_MAX;
        
        // use mod to control the update rate of seven segment and led
        if (loop_count % SEVEN_SEG_UPDATE_TICK == 0) show_seven_seg(x * 100 + y);
        if (loop_count % LED_UPDATE_TICK == 0) led_full(&show_led_times);
        if (loop_count % LCD_UPDATE_TICK == 0 && lcd_need_update) show_lcd(&lcd_need_update, lcd_buffer, lcd_now);
        
        keyin = ScanKey();
        
        // if no key is pressed, release the is_pressed flag and skip
        if (keyin == 0) {
            is_pressed = 0;
            continue;
        }
        
        // if the key is pressed, then skip
        if (is_pressed) continue;
        is_pressed = 1;
        
        // random
        if (keyin == 5) {
            srand(loop_count);
            x = (rand() % 9 + 1) * 10 + rand() % 9 + 1;
            y = (rand() % 9 + 1) * 10 + rand() % 9 + 1;
            update_result(result, x, y);
            lcd_need_update = 1;
        }
        
        if (x == 0 && y == 0) continue;
        
        // up
        if (keyin == 2) {
            if (math_op_index == 0) {
                show_led_times = 10;
            } else {
                math_op_index--;
                lcd_need_update = 1;
            }
        }
        
        // left
        if (keyin == 4) {
            int temp_x = x;
            x = (x % 10 * 10) + y / 10;
            y = (y % 10 * 10) + temp_x / 10;
            update_result(result, x, y);
            lcd_need_update = 1;
        }
        
        // right
        if (keyin == 6) {
            int temp_x = x;
            x = (x / 10) + (y % 10 * 10);
            y = (y / 10) + (temp_x % 10 * 10);
            update_result(result, x, y);
            lcd_need_update = 1;
        }
        
        // down
        if (keyin == 8) {
            if (math_op_index == 5) {
                show_led_times = 10;
            } else {
                math_op_index++;
                lcd_need_update = 1;
            }
        }
        
        if (lcd_need_update) update_lcd_buffer(x, y, math_op_index, result, lcd_buffer);
    }
}

