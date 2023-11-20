#include <stdio.h>
#include <NUC100Series.h>
#include "GPIO.h"
#include "Scankey.h"
#include "SYS_init.h"

volatile uint8_t KEY_Flag;
volatile uint32_t index_key_scan;

void init_keypad_INT(void) {
    GPIO_SetMode(PA, (BIT0 | BIT1 | BIT2 |BIT3 | BIT4 | BIT5), GPIO_MODE_QUASI);
	GPIO_EnableInt(PA, 0, GPIO_INT_LOW);
	GPIO_EnableInt(PA, 1, GPIO_INT_LOW);
	GPIO_EnableInt(PA, 2, GPIO_INT_LOW);
	NVIC_EnableIRQ(GPAB_IRQn);
	NVIC_SetPriority(GPAB_IRQn,3);
	GPIO_SET_DEBOUNCE_TIME(GPIO_DBCLKSRC_LIRC, GPIO_DBCLKSEL_256);
	GPIO_ENABLE_DEBOUNCE(PA, (BIT0 | BIT1 | BIT2));
	index_key_scan = 0;
}

void scan_keypad() {
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
	index_key_scan = index_key_scan++ % 3;
	NVIC_EnableIRQ(GPAB_IRQn);
}

#ifdef KEYPAD_INT
void GPAB_IRQHandler(void) {
    NVIC_DisableIRQ(GPAB_IRQn);

    if (PA->ISRC & BIT0) {
        PA0 = 1;
        PA->ISRC |= BIT0;

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
    if (PA->ISRC & BIT1) {
        PA1 = 1;
        PA->ISRC |= BIT1;
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
    if (PA->ISRC & BIT2) {
        PA2 = 1;
        PA->ISRC |= BIT2;
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
    }
    PA->ISRC = PA->ISRC;
}
#endif