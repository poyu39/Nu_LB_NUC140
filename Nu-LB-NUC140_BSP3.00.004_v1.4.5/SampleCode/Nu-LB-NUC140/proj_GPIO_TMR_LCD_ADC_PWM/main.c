//
// proj_GPIO_TMR_LCD_ADC_PWM
//
// GPIO : LEDs, RGBLED, 3x3 keypad input
// TMR : Timer1 interrupt 10 times a sec
// LCD : display text
// ADC : read Variable Resistor
// PWM : play siren 
//
// EVB : Nu-LB-NUC140
// MCU : NUC140VE3CN

// ------Board connections--------------------
// PA0~P5 for 3x3 Keypad input
// PC12,13,14,15 connected to LEDs
// PA13 : control green LED
// PA14 : control red   LED
// PA12 connected PB11 (Buzzer)

#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "Scankey.h"
#include "LCD.h"

#define Ambulance_Siren_Low   650
#define Ambulance_Siren_High  900
#define Ambulance_Siren_L_period 400000 // 0.4 sec
#define Ambulance_Siren_H_period 600000 // 0.6 sec
uint8_t ledState;
char text[16];	

void Ambulance_Siren(uint32_t siren_no)
{
	uint32_t i;
	PWM_EnableOutput(PWM0, PWM_CH_0_MASK);
	for (i=0; i<siren_no; i++) {
		PWM_ConfigOutputChannel(PWM0, PWM_CH0, Ambulance_Siren_Low, 90);
		CLK_SysTickDelay(Ambulance_Siren_L_period);
		PWM_ConfigOutputChannel(PWM0, PWM_CH0, Ambulance_Siren_High, 90);
		CLK_SysTickDelay(Ambulance_Siren_H_period);	
	}
	PWM_DisableOutput(PWM0, PWM_CH_0_MASK);
}

void ADC_IRQHandler(void)
{
    uint32_t u32Flag;
    int16_t ADCvalue;	
	
    // Get ADC conversion finish interrupt flag
    u32Flag = ADC_GET_INT_FLAG(ADC, ADC_ADF_INT);

    if(u32Flag & ADC_ADF_INT) {
      ADCvalue = ADC_GET_CONVERSION_DATA(ADC, 7);
      sprintf(text, "ADC= %4d", ADCvalue);
			print_Line(3, text);
    }
    ADC_CLR_INT_FLAG(ADC, u32Flag);
}

void Init_ADC(void)
{
    ADC_Open(ADC, ADC_INPUT_MODE, ADC_OPERATION_MODE, ADC_CHANNEL_MASK );
    ADC_POWER_ON(ADC);
    ADC_EnableInt(ADC, ADC_ADF_INT);
    NVIC_EnableIRQ(ADC_IRQn);
}

void TMR1_IRQHandler(void)
{	
	  int8_t i;
	  i=ScanKey();
		switch(i) {
			case 1 : PC12=0; break;
			case 2 : PC13=0; break;
			case 3 : PC14=0; break;
			case 4 : PC15=0; break;
			case 5 : Ambulance_Siren(5); break;
			case 6 : ADC_START_CONV(ADC); break;
			default: PC12=1; PC13=1; PC14=1; PC15=1; break;
		}	
		
    ledState = ~ ledState;  // changing ON/OFF state
    if(ledState) {PA13=0;}
    else         {PA13=1;} 
    TIMER_ClearIntFlag(TIMER1); // Clear Timer1 time-out interrupt flag
}

void Init_Timer1(void)
{
    TIMER_Open(TIMER1, TMR1_OPERATING_MODE, TMR1_OPERATING_FREQ);
    TIMER_EnableInt(TIMER1);
    NVIC_EnableIRQ(TMR1_IRQn);
    TIMER_Start(TIMER1);
}

void Init_GPIO(void)
{
	  // 4 red LEDs
	  GPIO_SetMode(PC, BIT12, GPIO_MODE_OUTPUT);
	  GPIO_SetMode(PC, BIT13, GPIO_MODE_OUTPUT);
	  GPIO_SetMode(PC, BIT14, GPIO_MODE_OUTPUT);
	  GPIO_SetMode(PC, BIT15, GPIO_MODE_OUTPUT);
	  PC12=1; PC13=1; PC14=1; PC15=1;
	  // RGB LED
	  GPIO_SetMode(PA, BIT12, GPIO_MODE_OUTPUT);
	  GPIO_SetMode(PA, BIT13, GPIO_MODE_OUTPUT);
	  GPIO_SetMode(PA, BIT14, GPIO_MODE_OUTPUT);
	  PA12=1;  PA13=1;  PA14=1;	
}

int main(void)
{	
	SYS_Init();
	OpenKeyPad();
	Init_GPIO();
	
	Init_Timer1();
	
  init_LCD();
  clear_LCD();

  Init_ADC(); 	

	PWM_EnableOutput(PWM0, PWM_CH_0_MASK);
  PWM_Start(PWM0, PWM_CH_0_MASK);
	
 	while(1) 
  {		
    print_Line(0, "Nu-LB-NUC140    ");
    print_Line(1, "Cortex-M0 @50MHz");
    print_Line(2, "128KB Flash ROM ");	
	}
}
