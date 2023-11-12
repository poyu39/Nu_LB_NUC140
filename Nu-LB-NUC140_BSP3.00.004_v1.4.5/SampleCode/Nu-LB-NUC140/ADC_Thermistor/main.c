//
// ADC_Thermistor : using ADC1 to read thermistor
//
// EVB : Nu-LB-NUC140
// MCU : NUC140VE3CN
// Sensor: Thermistor (resistance changed with the temperature)
//
// Connections:
// ADC1/PA1 : NUC140-LQFP100 pin72
// 10Kohm resistor connected from ADC1 to Vcc (pull-up)
// a thermistor    connected from ADC1 to Gnd
#include <stdio.h>
#include <math.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "LCD.h"

char Text[16];

void Thermistor(int16_t ADCvalue)
{
	double T, Temp;
  double T0 = 294.15;    // 273.15 + 21
  int16_t R0 = 9090;     // calibrated by reading R at 21 degree celsius
  int16_t B  = 3950;     // Thermistor parameter (see its datasheet)
  int16_t Pullup = 9900; // 10K ohm
  int16_t R;             // Thermistor resistence 
	
  // R / (Pullup + R)   = adc / 4096
  R = (Pullup * ADCvalue) / (4096 - ADCvalue);
		
  // B = (log(R) - log(R0)) / (1/T -  1/T0) 
  T = 1 / (1/T0 + (log(R)-log(R0)) / B );				
  Temp = T - 273.15;	
		
  sprintf(Text,"ADC : %8d", ADCvalue);
	print_Line(0, Text);
	sprintf(Text,"R   : %8d", R);
	print_Line(1, Text);
	sprintf(Text,"Temp:%f", Temp);
	print_Line(2, Text);	
}

void ADC_IRQHandler(void)
{
	uint32_t u32Flag;
  int16_t ADCvalue;	
	
  // Get ADC conversion finish interrupt flag
  u32Flag = ADC_GET_INT_FLAG(ADC, ADC_ADF_INT);

  if(u32Flag & ADC_ADF_INT) {
    ADCvalue = ADC_GET_CONVERSION_DATA(ADC, 1);
    Thermistor(ADCvalue);
  }
  ADC_CLR_INT_FLAG(ADC, u32Flag);
}

void Init_ADC(void)
{
  ADC_Open(ADC, ADC_INPUT_MODE, ADC_OPERATION_MODE, ADC_CHANNEL_MASK );
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

  while(1);
}
