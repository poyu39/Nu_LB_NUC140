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

#define LED_UPDATE_TICK 20
#define SEVEN_SEG_UPDATE_TICK 1
#define TICK_PER_MS 5000
#define PI 3.14159265

void init_GPIO(void){
    GPIO_SetMode(PC, BIT12, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PC, BIT13, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PC, BIT14, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PC, BIT15, GPIO_MODE_OUTPUT);
    PC12 = 1; PC13 = 1; PC14 = 1; PC15 = 1;
}

// led cycle play 0, 1, 2, 3, 2, 1, 0
void led_cycle_play()  {
    static uint8_t led_ctr = 0;
    uint8_t j = 0, this_index = 0;
    // y = 3 * sin(x * PI / 6)
    this_index = (int)(3 * sin(led_ctr * PI / 6));
    for (j = 0; j < 4; j++) {
        // PC12 == GPIO_PIN_DATA(2, 12)
        GPIO_PIN_DATA(2, 12 + j) = (j == this_index) ? 0 : 1;
    }
    led_ctr = (led_ctr + 1) % 7;
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

int main(void) {
    uint8_t keyin = 0, is_pressed = 0;
    uint16_t loop_count = 0;
    uint16_t s_num = 0;
    
    SYS_Init();
    init_GPIO();
    OpenKeyPad();
    init_LCD();
    clear_LCD();
    
    while (TRUE) {
        CLK_SysTickDelay(TICK_PER_MS);
        
        // let loop_count increase and avoid overflow
        loop_count = (loop_count + 1) % UINT16_MAX;
        
        // use mod to control the update rate of seven segment and led
        // if mod is 0 that means this time loop need to update
        if (loop_count % SEVEN_SEG_UPDATE_TICK == 0) show_seven_seg(s_num);
        if (loop_count % LED_UPDATE_TICK == 0) led_cycle_play();
        
        keyin = ScanKey();
        if (keyin == 0) {                                   // if no key is pressed, release the is_pressed flag and skip
            is_pressed = 0;
            continue;
        }
        if (is_pressed) continue;                           // if the key is pressed, then skip
        is_pressed = 1;
        s_num = (s_num * 10 + keyin) % 10000;               // update the number of seven segment
        printC(0, 0, ' ');                                  // use space to cover the last number
        printC(0, 0, keyin + '0');                          // print the key on lcd
    }
}

