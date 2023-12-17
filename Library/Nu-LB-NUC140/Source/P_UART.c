#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "NUC100Series.h"
#include "uart.h"
#include "P_UART.h"

#define RXDATABUFSIZE 64

volatile char rx_buffer[RXDATABUFSIZE];
volatile uint8_t rx_index = 0;
volatile uint8_t RX_FLAG = 0;

void p_send_int(int32_t num) {
    uint8_t i;
    uint8_t packet[6];
    sprintf(packet, "%-6d", num);
    // 檢查位數
    for (i = 0; i < 6; i++) {
        if (packet[i] < '0' || packet[i] > '9') {
            if (packet[i] != '-') break;
        }
    }
    packet[i + 1] = '\n';
    UART_Write(UART0, packet, i + 2);
    CLK_SysTickDelay(1000);
}

int32_t get_rx_int(void) {
    uint8_t i = 0;
    uint8_t is_negative = 0;
    int32_t num = 0;
    if (rx_buffer[0] == '-') {
        is_negative = 1;
        i++;
    }
    while (rx_buffer[i] >= '0' && rx_buffer[i] <= '9') {
        num = num * 10 + rx_buffer[i] - 48;
        i++;
    }
    if (is_negative) num *= -1;
    for (i = 0; i < RXDATABUFSIZE; i++) rx_buffer[i] = '\n';
    RX_FLAG = 0;
    return num;
}

void UART02_IRQHandler(void) {
    uint8_t c;
    uint32_t u32IntSts = UART0->ISR;

    if (u32IntSts & UART_IS_RX_READY(UART0)) {              // 檢查 ISR 是否為 RX 就緒
        while (!(UART0->FSR & UART_FSR_RX_EMPTY_Msk)) {     // 檢查 RX 是否為空的
            c = UART_READ(UART0);                           // 讀取 UART RX 資料
            if (RX_FLAG) continue;
            if (c >= '0' && c <= '9') {
                rx_buffer[rx_index] = c;
                rx_index++;
            } else if (c == '\n') {
                rx_buffer[rx_index] = '\n';
                rx_index = 0;
                RX_FLAG = 1;
                break;
            }
        }
    }
}

void init_p_uart(uint32_t baud_rate) {
    UART_Open(UART0, baud_rate);
    UART_ENABLE_INT(UART0, UART_IER_RDA_IEN_Msk);
    NVIC_EnableIRQ(UART02_IRQn);
}
