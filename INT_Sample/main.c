#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "NewScankey.h"
#include "NewSevenSegment.h"

#define KEYPAD_INT

volatile uint32_t index_5ms, timer0_5ms, timer0_100ms, timer0_1s;
volatile uint32_t digit[4];

void Init_Timer0(void) {
	TIMER_Open(TIMER0, TIMER_PERIODIC_MODE, 200);
	TIMER_EnableInt(TIMER0);
	NVIC_EnableIRQ(TMR0_IRQn);
	TIMER_Start(TIMER0);
}

void TMR0_IRQHandler(void) {
    timer0_5ms++;
    index_5ms = timer0_5ms % 4;
    CloseSevenSegment();
    ShowSevenSegment(index_5ms % 4, digit[index_5ms % 4]);
    if (timer0_5ms % 20 == 0) {
        scan_keypad();
		timer0_100ms++;
	}
    if (timer0_5ms % 200 == 0)
        timer0_1s++;
    TIMER_ClearIntFlag(TIMER0);
}