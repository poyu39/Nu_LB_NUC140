//
// ADC_Mic : using ADC0 to detect sound (clapping)
//
// EVB : Nu-LB-NUC140
// MCU : NUC140VE3CN
// ADC : Mic Sensor (amp=MAX9812/LM394)
//
// Connection:
// ADC0/PA0 to Mic sensor (with amplifier MAX9812 or LM394)

#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h" 
#include "SYS_init.h"
#include "LCD.h"

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
	char Text[16];
  uint32_t u32ADCvalue=4095;
	uint32_t count=0;
  SYS_Init();
	init_LCD();
	clear_LCD();
  Init_ADC();   
	
    u8ADF = 0;

    while(1) {
      ADC_START_CONV(ADC);
      while (u8ADF == 0);
      u32ADCvalue = ADC_GET_CONVERSION_DATA(ADC, 0);
      sprintf(Text, "ADC0 = %4d",u32ADCvalue);
			print_Line(0, Text);
			if (u32ADCvalue<2300) count++;
      sprintf(Text, "Sound = %4d", count);
			print_Line(1, Text);			
    }
}

