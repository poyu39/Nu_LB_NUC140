//
// UART_ADC : Continuously sampling ADC 8 channels and send out to UART
//
// EVB : Nu-LB-NUC140
// MCU : NUC140VE3CN
//
// UART1 connected to USB-TTL-UART (to PC)
// PB4/RX1 connected to USB-TTL-UART TXD
// PB5/TX1 connected to USB-TTL-UART RXD
//
// ADC0~7  connected to Sensors
// ADC0/PA0 : NUC140 LQFP100 pin71
// ADC1/PA1 : NUC140 LQFP100 pin72
// ADC2/PA2 : NUC140 LQFP100 pin73
// ADC3/PA3 : NUC140 LQFP100 pin74
// ADC4/PA4 : NUC140 LQFP100 pin75
// ADC5/PA5 : NUC140 LQFP100 pin76
// ADC6/PA6 : NUC140 LQFP100 pin77
// ADC7/PA7 : NUC140 LQFP100 pin78

#include <stdio.h>
#include <string.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"

#define TOTAL_CHANNELS 8
char LineEnd[2] = "\n\a";

volatile uint32_t u32ADCvalue[TOTAL_CHANNELS];

volatile char TX_buffer[40];

void ADC_IRQHandler(void)
{
  uint8_t  i;
  uint32_t u32Flag;

  // Get ADC conversion finish interrupt flag
  u32Flag = ADC_GET_INT_FLAG(ADC, ADC_ADF_INT);

  if(u32Flag & ADC_ADF_INT) {
    for (i=0; i<TOTAL_CHANNELS; i++) {
      u32ADCvalue[i] = ADC_GET_CONVERSION_DATA(ADC, i);			
      sprintf(TX_buffer+i*5, "%4d,",u32ADCvalue[i]);
      UART_Write(UART1, TX_buffer, 40);
    }
    UART_Write(UART1, LineEnd, 2);
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
  UART_Open(UART1,115200);	// enable UART1 at 11500 baudrate
  Init_ADC();

  while(1);
}
