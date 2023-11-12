//
// proj_Hand_Music : 
//    Timer Capture to read Ultrasound Ranger SR-04
//    detect distance for playing tone
//
// EVB : Nu-LB-NUC140 (need to manually switch RSTCAPSEL)
// MCU : NUC140VE3CN
// Sensor: SR-04

// SR-04 connection
// Trig connected to PB8
// Echo connected to TC2/PB2 (TC0_PB15, TC1_PE5, TC2_PB2, TC3_PB3)
// PA12/PWM0 connected to PB11

#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "LCD.h"
#include "Note_Freq.h"

char Text[16];
uint8_t  Timer_flag =0;
uint32_t Timer_count =0;
float distance;
	
void TMR2_IRQHandler(void)
{		
    TIMER2->TEXCON &= ~(TIMER_TEXCON_RSTCAPSEL_Msk); // set RSTCAPSEL to 0 to read counter
    Timer_count = TIMER_GetCaptureData(TIMER2);
    
    Timer_flag =1;
    TIMER_ClearCaptureIntFlag(TIMER2);
}

void Init_TimerCapture(void)
{
    TIMER_Open(TIMER2, TMR2_OPERATING_MODE, TMR2_OPERATING_FREQ);
    TIMER_SET_PRESCALE_VALUE(TIMER2, 11);
    // Set compare value as large as possible, so don't need to worry about counter overrun too frequently.
    TIMER_SET_CMP_VALUE(TIMER2, 0xFFFFFF);	
    TIMER_EnableCapture(TIMER2, TMR2_CAPTURE_MODE, TMR2_CAPTURE_EDGE);	
    TIMER_EnableCaptureInt(TIMER2); // Enable timer interrupt
    NVIC_EnableIRQ(TMR2_IRQn);
}

void Trig_SR04(void)
{
  PB8=1;               // generate 10us pulse to trigger SR04 
  CLK_SysTickDelay(7); 
  PB8=0;			
  TIMER2->TEXCON |= TIMER_TEXCON_RSTCAPSEL_Msk; // set RSTCAPSEL to 1 to reset counter
			
  TIMER_Start(TIMER2);		
  CLK_SysTickDelay(30000);
  TIMER_Stop(TIMER2);
}

void PlayTone(void)
{
  int Tone;
	if (Timer_flag==1) {
    distance = Timer_count * 340 /20000;
    sprintf(Text,"D=%5d cm", (int) distance);	
    print_Line(1,Text);
		
		if      (distance<=10) Tone=C5;
		else if (distance<=12) Tone=D5;
		else if (distance<=14) Tone=E5;
		else if (distance<=16) Tone=F5;
		else if (distance<=18) Tone=G5;
		else if (distance<=20) Tone=A5;
		else if (distance<=22) Tone=B5;		
		else if (distance<=24) Tone=C6;
    else                   Tone=0;
		sprintf(Text,"Tone=%5d", Tone);
    print_Line(2,Text);
		
		PWM_ConfigOutputChannel(PWM0, PWM_CH0, Tone, 90);		
		if (Tone!=0) PWM_EnableOutput(PWM0, PWM_CH_0_MASK);
		else         PWM_DisableOutput(PWM0, PWM_CH_0_MASK);
    CLK_SysTickDelay(200000);
    Timer_flag=0;				

  }
}


int main(void)
{
	SYS_Init();
  init_LCD();
  clear_LCD();

  print_Line(0, "Hand Music");
	GPIO_SetMode(PB, BIT11, GPIO_MODE_OUTPUT);	
  GPIO_SetMode(PB, BIT8, GPIO_MODE_OUTPUT);
  PB8=0;
	
  Init_TimerCapture();	
	
  PWM_EnableOutput(PWM0, PWM_CH_0_MASK);
  PWM_Start(PWM0, PWM_CH_0_MASK);
  
  while(1) {
		Trig_SR04();
		PlayTone();
  }
}
