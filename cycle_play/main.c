// title: cycle play example
// Author: poyu39

#include <stdio.h>
#include <math.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"

#define PI 3.14159265

// cycle play: 0 1 2 3 2 1
void cycle_play()  {
    uint8_t i = 0;
    uint8_t j = 0;
    uint8_t on_index = 0;
    for (i = 0; i < 6; i++) {
        // y = 3 * sin(x * PI / 6)
        on_index = (int)(3 * sin(i * PI / 6));
        for (j = 0; j < 4; j++) {
            // PC12 == GPIO_PIN_DATA(2, 12)
            GPIO_PIN_DATA(2, 12 + j) = (j == on_index) ? 0 : 1;
        }
        CLK_SysTickDelay(300000);
    }
}


int main() {
	SYS_Init();
    
    // 設定 mode
    GPIO_SetMode(PC, BIT12, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PC, BIT13, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PC, BIT14, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PC, BIT15, GPIO_MODE_OUTPUT);
	
    // 0 為亮，1 為暗
	PC12 = 1; PC13 = 1; PC14 = 1; PC15 = 1;
    
    while(TRUE) {
        cycle_play();
	}
}
