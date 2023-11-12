//
// ADC_Joystick : ADC interrupt to update ADC value continuously
//
// Joystick
// VCC    : connected to Vcc
// X      : connected to ADC0/PA0 NUC140VE3CN LQFP100 pin71
// B      : connected to      PC0 NUC140VE3CN LQFP100 pin45
// Y      : connected to ADC1/PA1 NUC140VE3CN LQFP100 pin72
// GND    : connected to Gnd 
#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
volatile uint8_t u8ADF;
volatile uint16_t X, Y;
volatile uint8_t  B;

void ADC_IRQHandler(void)
{
    uint32_t u32Flag;

    // Get ADC conversion finish interrupt flag
    u32Flag = ADC_GET_INT_FLAG(ADC, ADC_ADF_INT);

    if(u32Flag & ADC_ADF_INT) {
        X = ADC_GET_CONVERSION_DATA(ADC, 0);
        Y = ADC_GET_CONVERSION_DATA(ADC, 1);
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
    Init_ADC(); 
    GPIO_SetMode(PC, BIT0, GPIO_MODE_INPUT); // set PC0 input for button

    while(1){
      B=PC0;
      printf("X= %d, Y= %d, B= %d\n", X, Y, B);
    }
}

