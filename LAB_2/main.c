#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "Seven_Segment.h"
#include "Scankey.h"

uint8_t SpScanKey(void) {
    PA0=1; PA1=1; PA2=0; PA3=1; PA4=1; PA5=1;
        if (PA3==0) return 7;
        if (PA4==0) return 4;
        if (PA5==0) return 1;
    PA0=1; PA1=0; PA2=1; PA3=1; PA4=1; PA5=1;
        if (PA3==0) return 8;
        if (PA4==0) return 5;
        if (PA5==0) return 2;
    PA0=0; PA1=1; PA2=1; PA3=1; PA4=1; PA5=1;
        if (PA3==0) return 9;
        if (PA4==0) return 6;
        if (PA5==0) return 3;
        return 0;
}

int main(void) {
    uint8_t SevenSegmentBuffer[4] = {0, 0, 0, 0};
    int SevenSegmentDisplay = 0;
    int i = 0;
    int pressed_key = 0;
    int temp_pressed_key = 0;
    SYS_Init();
    OpenSevenSegment();
    OpenKeyPad();

    while (TRUE) {
        pressed_key = SpScanKey();
        if (pressed_key != temp_pressed_key) {
            if (pressed_key != 0) {
                // 往左移動
                for (i = 3; i > 0; i--) {
                    SevenSegmentBuffer[i] = SevenSegmentBuffer[i - 1];
                }
                SevenSegmentBuffer[0] = pressed_key;
            }
        }
        for (i = 3; i >= 0; i--) {
            CloseSevenSegment();
            if (SevenSegmentBuffer[i] == 0) {
                PE0=1;
                PE1=1;
                PE2=1;
                PE3=1;
                PE4=1;
                PE5=1;
                PE6=1;
                PE7=1;
            } else {
                ShowSevenSegment(i, SevenSegmentBuffer[i]);
            }
            CLK_SysTickDelay(5000);
        }
        if (pressed_key != temp_pressed_key) {
            temp_pressed_key = pressed_key;
        }
    }
}
