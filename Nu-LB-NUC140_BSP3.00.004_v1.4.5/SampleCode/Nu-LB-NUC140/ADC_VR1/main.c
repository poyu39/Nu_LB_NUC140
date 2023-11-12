//
// ADC_VR1 : using ADC7 to read Variable Resistor 
//
// EVB : Nu-LB-NUC140
// MCU : NUC140VE3CN
// VR1 : Variable Resistor on learning board
//
// ADC7/PA7 : NUC140VE3CN LQFP100 pin78

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

int32_t main (void)
{
    uint32_t u32ADCvalue;
    SYS_Init();
    Init_ADC();   
	
    u8ADF = 0;

    while(1) {
      ADC_START_CONV(ADC);
      while (u8ADF == 0);
      u32ADCvalue = ADC_GET_CONVERSION_DATA(ADC, 7);
      printf("VR = %d\n",u32ADCvalue);
      ADC_DisableInt(ADC, ADC_ADF_INT);
    }
}

