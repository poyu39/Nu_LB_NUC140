//
// ADC : ADC single cycle scan conversion repeating in a while loop
//
// EVB : Nu-LB-NUC140
// MCU : NUC140VE3CN
// ADC : ADC0/PA0

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

    if(u32Flag & ADC_ADF_INT) u8ADF = 1;

    ADC_CLR_INT_FLAG(ADC, u32Flag);
}

void Init_ADC(void)
{
    ADC_Open(ADC, ADC_INPUT_MODE, ADC_OPERATION_MODE, ADC_CHANNEL_MASK); // set ADC modes & channels
    ADC_POWER_ON(ADC);                          // Power on ADC
    ADC_EnableInt(ADC, ADC_ADF_INT);            // Enable ADC ADC_IF interrupt
    NVIC_EnableIRQ(ADC_IRQn);	                  // Enable CPU NVIC
}

int32_t main (void)
{
	int i;
  uint32_t u32Result;
	
    SYS_Init();
    Init_ADC();

    printf("\n\nCPU @ %dHz\n", SystemCoreClock);
    u8ADF = 0;

    while(1) {
      ADC_START_CONV(ADC); // start ADC conversion
      while (u8ADF == 0);  // wait for ADC sampling finished & generate interrupt to set this flag to 1
      // read ADC result & print it			
      u32Result = ADC_GET_CONVERSION_DATA(ADC, 0);  
      printf("ADC0 = %d\n", u32Result);		
      u8ADF =0; // clear the flag
    }
}
