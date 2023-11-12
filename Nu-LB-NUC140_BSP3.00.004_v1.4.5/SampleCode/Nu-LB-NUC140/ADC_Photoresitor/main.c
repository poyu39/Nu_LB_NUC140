//
// ADC_Photoresistor : use ADC0 to read a photoresistor 
//
// EVB : Nu-LB-NUC140
// MCU : NUC140VE3CN
// Sensor: Photoresitor GL5516 (or other sensors)

// GL5516 connecitons:
// GL5516 pin1 to VCC
// GL5516 pin2 to ADC0, and connect to 10K ohm to Gnd

#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "LCD.h"

char Text[16];
	
void ADC_IRQHandler(void)
{
    uint32_t u32Flag;
    uint32_t u32ADCvalue;

    // Get ADC conversion finish interrupt flag
    u32Flag = ADC_GET_INT_FLAG(ADC, ADC_ADF_INT);

    if(u32Flag & ADC_ADF_INT) {
        u32ADCvalue = ADC_GET_CONVERSION_DATA(ADC, 0);
        sprintf(Text,"ADC0= %4d",u32ADCvalue);
			  print_Line(1, Text);
    }
    ADC_CLR_INT_FLAG(ADC, u32Flag);
}

void Init_ADC(void)
{
    ADC_Open(ADC, ADC_INPUT_MODE, ADC_OPERATION_MODE, ADC_CHANNEL_MASK);
    ADC_POWER_ON(ADC);
    ADC_EnableInt(ADC, ADC_ADF_INT);
    NVIC_EnableIRQ(ADC_IRQn);
    ADC_START_CONV(ADC);	
}

int32_t main (void)
{
	SYS_Init();
  init_LCD();  
  clear_LCD();
	
  Init_ADC();
  print_Line(0, "ADC0 reading");
	
  while(1);
			
}
