#include <stdio.h>
#include <stdlib.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "NewLCD.h"
#include "NewScanKey.h"
#include "NewSevenSegment.h"

volatile uint8_t rx_data[4] = {0};
volatile uint8_t rx_index = 0;
volatile uint8_t printc_index = 0, printc_line_index = 0;
uint8_t pwd[4] = {0};

uint16_t get4rand(void) {
    uint8_t i, j;
	uint8_t rands[4] = {0, 0, 0, 0};
    uint16_t rand_num = 0;
	uint8_t temp = 0;
	uint8_t check = 0;
	for (i = 0; i < 4; i++) {
		while (TRUE) {
			temp = rand() % 9 + 1;
			check = 0;
			for (j = 0; j < 4; j++) {
				if (temp == rands[j]) {
					temp = rand() % 9 + 1;
				} else {
					check++;
				}
			}
			if (check == 4) break;
		}
		rands[i] = temp;
        rand_num = rand_num * 10 + temp;
	}
    return rand_num;
}

void check_num_is_correct(void) {
    uint8_t a = 0, b = 0;
	uint8_t i = 0, j = 0;
    for (i = 0; i < 4; i++) {
        if (rx_data[i] == pwd[3 - i]) {
            a++;
        } else {
            for (j = 0; j < 4; j++) {
                if (rx_data[i] == pwd[3 - j] && i != j) {
                    b++;
                    break;
                }
            }
        }
    }
    print_c_in_buffer(10 * 8, printc_line_index * 16, 8, a + '0', FG_COLOR);
    print_c_in_buffer(11 * 8, printc_line_index * 16, 8, 'A', FG_COLOR);
    print_c_in_buffer(12 * 8, printc_line_index * 16, 8, b + '0', FG_COLOR);
    print_c_in_buffer(13 * 8, printc_line_index * 16, 8, 'B', FG_COLOR);
    show_lcd_buffer();
}

void UART02_IRQHandler(void) {
    uint8_t c;
    uint32_t u32IntSts = UART0->ISR;

    if (u32IntSts & UART_IS_RX_READY(UART0)) { // check ISR on & RX is ready
        while (!(UART0->FSR & UART_FSR_RX_EMPTY_Msk)) {  // check RX is not empty
            c = UART_READ(UART0); // read UART RX data
            rx_data[rx_index] = c - 48;
            rx_index++;
            if (c != '0') {
                print_c_in_buffer(8 * printc_index, 16 * (printc_line_index % 4), 8, c, FG_COLOR);
                show_lcd_buffer();
                if (++printc_index == 4) {
                    check_num_is_correct();
                    printc_index = 0;
                    printc_line_index++;
                    rx_index = 0;
                }
            }
        }
    }
}

void Init_UART0(void) {
    UART_Open(UART0, 115200);                         // set UART0 baud rate
    UART_ENABLE_INT(UART0, UART_IER_RDA_IEN_Msk);     // enable UART0 interrupt (triggerred by Read-Data-Available)
    NVIC_EnableIRQ(UART02_IRQn);                      // enable Cortex-M0 NVIC interrupt for UART02
}

int main(void) {
    uint16_t pwd_int;
    uint8_t send_temp;

    // init
    SYS_Init();
    init_keypad_INT();
    init_lcd(TRUE, FALSE);
    init_seg(TRUE, 200);
    Init_UART0();

    // 產生 4 位數的密碼
    pwd_int = get4rand();
    pwd[0] = pwd_int / 1000;
    pwd[1] = (pwd_int % 1000) / 100;
    pwd[2] = (pwd_int % 100) / 10;
    pwd[3] = pwd_int % 10;

    // 七段顯示器顯示密碼
    seg_buffer[0] = pwd[0];
    seg_buffer[1] = pwd[1];
    seg_buffer[2] = pwd[2];
    seg_buffer[3] = pwd[3];

    while (TRUE) {
        if (KEY_FLAG != 0) {
            send_temp = KEY_FLAG + 48;
            UART_Write(UART0, &send_temp, 1);
            KEY_FLAG = 0;
        }
    }
}
