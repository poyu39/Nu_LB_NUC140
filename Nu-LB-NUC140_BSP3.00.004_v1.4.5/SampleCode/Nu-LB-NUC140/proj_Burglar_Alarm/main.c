//
// Burglar_Alarm : 
//    Timer Capture to read Ultrasound Ranger SR-04
//    Buzz when distance > 10cm
//
// EVB : Nu-LB-NUC140 (need to manually switch RSTCAPSEL)
// MCU : NUC140VE3CN
// Sensor: SR-04

// SR-04 connection
// Trig connected to PB8
// Echo connected to TC2/PB2 (TC0_PB15, TC1_PE5, TC2_PB2, TC3_PB3)

#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "LCD.h"

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

void Read_SR04(void)
{

	if (Timer_flag==1) {
    distance = Timer_count * 340 /20000;
    sprintf(Text,"T=%5d", Timer_count);
    print_Line(1,Text);				
    sprintf(Text,"D=%5d cm", (int) distance);	
    print_Line(2,Text);
    Timer_flag=0;
  }
}

void Buzz(int number)
{
	int i;
	for (i=0; i<number; i++) {
      PB11=0; // PB11 = 0 to turn on Buzzer
	  CLK_SysTickDelay(100000);	 // Delay 
	  PB11=1; // PB11 = 1 to turn off Buzzer	
	  CLK_SysTickDelay(100000);	 // Delay 
	}
}

void Burglar_Alarm(void)
{
	if (distance>10) Buzz(1);
}

int main(void)
{
	SYS_Init();
  init_LCD();
  clear_LCD();

  print_Line(0, "Burglar Alarm");
	GPIO_SetMode(PB, BIT11, GPIO_MODE_OUTPUT);	
  GPIO_SetMode(PB, BIT8, GPIO_MODE_OUTPUT);
  PB8=0;

	
  Init_TimerCapture();	
	  
  while(1) {
		Trig_SR04();
		Read_SR04();
		Burglar_Alarm();
  }
}
