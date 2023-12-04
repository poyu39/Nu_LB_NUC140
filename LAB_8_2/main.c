#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "NewLCD.h"
#include "NewScankey.h"
#include "Note_Freq.h"

volatile uint32_t u32ADCvalue;
volatile uint8_t u8ADF;

volatile uint32_t timer2_5ms, timer2_1s;
volatile uint16_t player_time;

void ADC_IRQHandler(void) {
    uint32_t u32Flag;
    u32Flag = ADC_GET_INT_FLAG(ADC, ADC_ADF_INT);
    if (u32Flag & ADC_ADF_INT) u8ADF = 1;
    ADC_CLR_INT_FLAG(ADC, u32Flag);
}

void init_adc(void) {
    ADC_Open(ADC, ADC_INPUT_MODE, ADC_OPERATION_MODE, ADC_CHANNEL_MASK);
    ADC_POWER_ON(ADC);
    ADC_EnableInt(ADC, ADC_ADF_INT);
    NVIC_EnableIRQ(ADC_IRQn);
}

int main(void) {
    char text[17];
    uint16_t music[9] = {C4, D4, E4, F4, G4, A4, B4, C5, D5};
    char music_name[9][3] = {"C4", "D4", "E4", "F4", "G4", "A4", "B4", "C5", "D5"};

    u8ADF = 0;

    SYS_Init();
    init_lcd();
    init_adc();
    init_keypad_INT();

    PWM_EnableOutput(PWM1, PWM_CH_0_MASK);
    PWM_Start(PWM1, PWM_CH_0_MASK);

    while (TRUE) {
        if (KEY_FLAG == 0) {
            PWM_DisableOutput(PWM1, PWM_CH_0_MASK);
        } else {
            PWM_ConfigOutputChannel(PWM1, PWM_CH0, music[KEY_FLAG - 1], 90);
            PWM_EnableOutput(PWM1, PWM_CH_0_MASK);
            CLK_SysTickDelay(500000);
            PWM_DisableOutput(PWM1, PWM_CH_0_MASK);

            sprintf(text, "Music = %s", music_name[KEY_FLAG - 1]);
            clear_lcd_buffer();
            print_line_in_buffer(0, text, 8);
            show_lcd_buffer();
            KEY_FLAG = 0;
        }
    }
}
