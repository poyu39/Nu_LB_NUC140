#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"

volatile uint8_t KEY_Flag;
/*
        PA2   PA1   PA0
    PA3   1     2     3
    PA4   4     5     6
    PA5   7     8     9
*/
void GPAB_IRQHandler(void) {
    uint8_t i;
    if (PA->ISRC & BIT0) {
        PA->ISRC |= BIT0;
        for (i = 0; i <= 6; i++) GPIO_PIN_DATA(0, i) = 1;
        for (i = 3; i <= 5; i++) {
            if (GPIO_PIN_DATA(0, i - 1) == 0) GPIO_PIN_DATA(0, i - 1) = 1;
            GPIO_PIN_DATA(0, i) = 0;
            if (PA0 == 0) {
                KEY_Flag = 3 + 3 * (i - 3);
                break;
            }
            CLK_SysTickDelay(5000);
        }
        PA0 = 1; PA1 = 1; PA2 = 1; PA3 = 0; PA4 = 0; PA5 = 0;
    }
    if (PA->ISRC & BIT1) {
        PA->ISRC |= BIT1;
        for (i = 0; i <= 6; i++) GPIO_PIN_DATA(0, i) = 1;
        for (i = 3; i <= 5; i++) {
            if (GPIO_PIN_DATA(0, i - 1) == 0) GPIO_PIN_DATA(0, i - 1) = 1;
            GPIO_PIN_DATA(0, i) = 0;
            if (PA1 == 0) {
                KEY_Flag = 2 + 3 * (i - 3);
                break;
            }
            CLK_SysTickDelay(5000);
        }
        PA0 = 1; PA1 = 1; PA2 = 1; PA3 = 0; PA4 = 0; PA5 = 0;
        return;
    }
    if (PA->ISRC & BIT2) {
        PA->ISRC |= BIT2;
        for (i = 0; i <= 6; i++) GPIO_PIN_DATA(0, i) = 1;
        for (i = 3; i <= 5; i++) {
            GPIO_PIN_DATA(0, i - 1) = 1;
            GPIO_PIN_DATA(0, i) = 0;
            if (PA2 == 0) {
                KEY_Flag = 1 + 3 * (i - 3);
                break;
            }
            CLK_SysTickDelay(5000);
        }
        PA0 = 1; PA1 = 1; PA2 = 1; PA3 = 0; PA4 = 0; PA5 = 0;
        return;
    }
    PA->ISRC = PA->ISRC;
}

void Init_KEY(void) {
    GPIO_SetMode(PA, (BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5), GPIO_MODE_QUASI);
    GPIO_EnableInt(PA, 0, GPIO_INT_LOW);
    GPIO_EnableInt(PA, 1, GPIO_INT_LOW);
    GPIO_EnableInt(PA, 2, GPIO_INT_LOW);
    NVIC_EnableIRQ(GPAB_IRQn);
    NVIC_SetPriority(GPAB_IRQn, 3);
    GPIO_SET_DEBOUNCE_TIME(GPIO_DBCLKSRC_LIRC, GPIO_DBCLKSEL_256);
    GPIO_ENABLE_DEBOUNCE(PA, (BIT0 | BIT1 | BIT2));
    PA0 = 1; PA1 = 1; PA2 = 1; PA3 = 0; PA4 = 0; PA5 = 0;
}

int main(void) {
    SYS_Init();
    Init_KEY();
    UART_Open(UART0, 115200);
    while (TRUE) {
        printf("%d\n", KEY_Flag);
    }
}
