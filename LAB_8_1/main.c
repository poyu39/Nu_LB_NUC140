#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "NewLCD.h"
#include "Note_Freq.h"

volatile uint32_t u32ADCvalue;
volatile uint8_t u8ADF;

#define P125ms 125000
#define P250ms 250000
#define P500ms 500000
#define P1S 1000000

uint16_t music[72] = {
    E6, D6u, E6, D6u, E6, B5, D6, C6, A5, A5, 0, 0,
    C5, E5, A5, B5, B5, 0, C5, A5, B5, C6, C6, 0,
    E6, D6u, E6, D6u, E6, B5, D6, C6, A5, A5, 0, 0,
    C5, E5, A5, B5, B5, 0, E5, C6, B5, A5, A5, 0,
    B5, C6, D6, E6, E6, 0, G5, F6, E6, D6, D6, 0,
    F5, E6, D6, C6, C6, 0, E5, D6, C6, B5, B5, 0};

uint32_t pitch[72] = {
    P500ms, P500ms, P500ms, P500ms, P500ms, P500ms, P500ms, P500ms, P500ms, P500ms, P500ms, P500ms,
    P500ms, P500ms, P500ms, P500ms, P500ms, P500ms, P500ms, P500ms, P500ms, P500ms, P500ms, P500ms,
    P500ms, P500ms, P500ms, P500ms, P500ms, P500ms, P500ms, P500ms, P500ms, P500ms, P500ms, P500ms,
    P500ms, P500ms, P500ms, P500ms, P500ms, P500ms, P500ms, P500ms, P500ms, P500ms, P500ms, P500ms,
    P500ms, P500ms, P500ms, P500ms, P500ms, P500ms, P500ms, P500ms, P500ms, P500ms, P500ms, P500ms,
    P500ms, P500ms, P500ms, P500ms, P500ms, P500ms, P500ms, P500ms, P500ms, P500ms, P500ms, P500ms};

void ADC_IRQHandler(void) {
    uint32_t u32Flag;

    // Get ADC conversion finish interrupt flag
    u32Flag = ADC_GET_INT_FLAG(ADC, ADC_ADF_INT);

    if (u32Flag & ADC_ADF_INT) u8ADF = 1;

    ADC_CLR_INT_FLAG(ADC, u32Flag);
}

void Init_ADC(void) {
    ADC_Open(ADC, ADC_INPUT_MODE, ADC_OPERATION_MODE, ADC_CHANNEL_MASK);
    ADC_POWER_ON(ADC);
    ADC_EnableInt(ADC, ADC_ADF_INT);
    NVIC_EnableIRQ(ADC_IRQn);
}

int main(void) {
    int node_index = 0;
    char text[17];

    u8ADF = 0;

    SYS_Init();
    Init_ADC();
    init_lcd();

    PWM_EnableOutput(PWM1, PWM_CH_0_MASK);
    PWM_Start(PWM1, PWM_CH_0_MASK);
    ADC_START_CONV(ADC);
    while (TRUE) {
        while (u8ADF == 0);
        u32ADCvalue = ADC_GET_CONVERSION_DATA(ADC, 7);

        if (u32ADCvalue < 500) {
            PWM_DisableOutput(PWM1, PWM_CH_0_MASK);
            node_index = 0;
        } else if (u32ADCvalue >= 500 && u32ADCvalue < 4000) {
            PWM_ConfigOutputChannel(PWM1, PWM_CH0, u32ADCvalue, 90);
            PWM_EnableOutput(PWM1, PWM_CH_0_MASK);
            node_index = 0;
        } else if (u32ADCvalue >= 4000) {
            PWM_ConfigOutputChannel(PWM1, PWM_CH0, music[node_index], 90);  // 0=Buzzer ON
            if (music[node_index] != 0)
                PWM_EnableOutput(PWM1, PWM_CH_0_MASK);
            else
                PWM_DisableOutput(PWM1, PWM_CH_0_MASK);
            CLK_SysTickDelay(pitch[node_index]);
            node_index++;
            node_index %= 72;
        }
        ADC_DisableInt(ADC, ADC_ADF_INT);

        sprintf(text, "DATA: %04d", u32ADCvalue);
        clear_lcd_buffer();
        print_line_in_buffer(0, text, 8);
        show_lcd_buffer();
    }
}
