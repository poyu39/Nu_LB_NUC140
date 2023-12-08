/*
    NewTurnButton
    Author: 邱柏宇
    Discord: poyu39
*/
#include <stdio.h>
#include "NUC100Series.h"
#include "adc.h"
#include "pwm.h"
#include "SYS_init.h"

// ADC 轉換完成 flag
volatile uint8_t isConverted = 0;

// ADC 中斷處理函式
void ADC_IRQHandler(void) {
    uint32_t u32Flag;
    u32Flag = ADC_GET_INT_FLAG(ADC, ADC_ADF_INT);
    if (u32Flag & ADC_ADF_INT) isConverted = 1;
    ADC_CLR_INT_FLAG(ADC, u32Flag);
}

// 初始化 ADC & PWM
void init_turn_button(void) {
    ADC_Open(ADC, ADC_INPUT_MODE_SINGLE_END, ADC_OPERATION_MODE_SINGLE, ADC_CH_7_MASK);
    ADC_POWER_ON(ADC);
    ADC_EnableInt(ADC, ADC_ADF_INT);
    NVIC_EnableIRQ(ADC_IRQn);

    PWM_EnableOutput(PWM1, PWM_CH_0_MASK);
    PWM_Start(PWM1, PWM_CH_0_MASK);
}

// 取得可變電阻數值
uint16_t get_turn_button(void) {
    ADC_START_CONV(ADC);
    while (isConverted == 0);
    isConverted = 0;
    return (uint16_t) ADC_GET_CONVERSION_DATA(ADC, 7);
}
