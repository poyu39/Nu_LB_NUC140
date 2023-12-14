#include <stdio.h>
#include <stdlib.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "NewLCD.h"
#include "NewScanKey.h"
#include "P_UART.h"

int main(void) {
    int32_t rx_int = 0;
    SYS_Init();
    init_lcd(TRUE, TRUE);
    init_keypad_INT();
    init_p_uart(115200);

    while (TRUE) {
        if (KEY_FLAG != 0) {
            p_send_int(rand() % INT16_MAX);
            KEY_FLAG = 0;
        }
        if (RX_FLAG == 1) {
            rx_int = get_rx_int();
            printf_line_in_buffer(0, 8, "rx_int: %d", rx_int);
            show_lcd_buffer();
            RX_FLAG = 0;
        }
    }
}
