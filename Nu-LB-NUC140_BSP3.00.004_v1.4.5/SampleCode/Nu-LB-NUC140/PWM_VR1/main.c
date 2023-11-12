//
// PWM : Variable Resistor control PWM0 Ch0 & Ch1 output duty cycle
//
// EVB : Nu-LB-NUC140
// MCU : NUC140VE3CN  (LQPF-100)

#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
volatile uint8_t u8ADF;

void ADC_IRQHandler(void)
{
    uint32_t u32Flag;

    // Get ADC conversion finish interrupt flag
    u32Flag = ADC_GET_INT_FLAG(ADC, ADC_ADF_INT);

    if(u32Flag & ADC_ADF_INT)
        u8ADF = 1;

    ADC_CLR_INT_FLAG(ADC, u32Flag);
}

void Init_ADC(void)
{
    ADC_Open(ADC, ADC_INPUT_MODE, ADC_OPERATION_MODE, ADC_CHANNEL_MASK);
    ADC_POWER_ON(ADC);
    ADC_EnableInt(ADC, ADC_ADF_INT);
    NVIC_EnableIRQ(ADC_IRQn);
}

void Init_PWM(void)
{
    PWM_ConfigOutputChannel(PWM0, PWM_CH0, 60, 50);
    PWM_ConfigOutputChannel(PWM0, PWM_CH1, 60, 50);
    PWM_EnableOutput(PWM0, (PWM_CH_0_MASK | PWM_CH_1_MASK));
    PWM_Start(PWM0, (PWM_CH_0_MASK | PWM_CH_1_MASK));
}

int32_t main(void)
{
    uint32_t u32ADCvalue;
	  uint32_t duty;
    SYS_Init();
    Init_ADC(); 
    Init_PWM();

    u8ADF = 0;

    while(1) {
      ADC_START_CONV(ADC);
      while (u8ADF == 0);
      u32ADCvalue = ADC_GET_CONVERSION_DATA(ADC, 7);
			duty = u32ADCvalue*100/4096;
			//printf("ADC7 = %d, duty =%d\n", u32ADCvalue, duty);
      PWM_ConfigOutputChannel(PWM0, PWM_CH0, 60, duty);
      PWM_ConfigOutputChannel(PWM0, PWM_CH1, 60, duty);      
      ADC_DisableInt(ADC, ADC_ADF_INT);
    }
}
