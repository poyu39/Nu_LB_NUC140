//
// TMR_Capture_SR04x4 : using Timer Capture for four Ultrasound Ranger SR-04
//
// EVB : Nu-LB-NUC140 (need to manually switch RSTCAPSEL)
// MCU : NUC140VE3CN
// Sensor: SR-04 x4

// SR-04 connection
// Trig connected to PB6,      PB7,     PB8,     PB9
// Echo connected to TC0_PB15, TC1_PE5, TC2_PB2, TC3_PB3

#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "LCD.h"

char Text[16];
uint8_t  Timer_flag[4] ={0, 0, 0, 0};
uint32_t Timer_count[4]={0, 0, 0, 0};
float distance[4]={0, 0, 0, 0};
uint8_t disp_index=0;
	
void TMR0_IRQHandler(void)
{		
    TIMER0->TEXCON &= ~(TIMER_TEXCON_RSTCAPSEL_Msk); // set RSTCAPSEL to 0 to read counter
    Timer_count[0] = TIMER_GetCaptureData(TIMER0);    
    Timer_flag[0] =1;
    TIMER_ClearCaptureIntFlag(TIMER0);
}

void TMR1_IRQHandler(void)
{		
    TIMER1->TEXCON &= ~(TIMER_TEXCON_RSTCAPSEL_Msk); // set RSTCAPSEL to 0 to read counter
    Timer_count[1] = TIMER_GetCaptureData(TIMER1);   
    Timer_flag[1] =1;
    TIMER_ClearCaptureIntFlag(TIMER1);
}

void TMR2_IRQHandler(void)
{		
    TIMER2->TEXCON &= ~(TIMER_TEXCON_RSTCAPSEL_Msk); // set RSTCAPSEL to 0 to read counter
    Timer_count[2] = TIMER_GetCaptureData(TIMER2);   
    Timer_flag[2] =1;
    TIMER_ClearCaptureIntFlag(TIMER2);
}

void TMR3_IRQHandler(void)
{		
    TIMER3->TEXCON &= ~(TIMER_TEXCON_RSTCAPSEL_Msk); // set RSTCAPSEL to 0 to read counter
    Timer_count[3] = TIMER_GetCaptureData(TIMER3);    
    Timer_flag[3] =1;
    TIMER_ClearCaptureIntFlag(TIMER3);
}

void Init_TC0(void)
{
    TIMER_Open(TIMER0, TMR0_OPERATING_MODE, TMR0_OPERATING_FREQ);
    TIMER_SET_PRESCALE_VALUE(TIMER0, 11);
    // Set compare value as large as possible, so don't need to worry about counter overrun too frequently.
    TIMER_SET_CMP_VALUE(TIMER0, 0xFFFFFF);	
    TIMER_EnableCapture(TIMER0, TMR0_CAPTURE_MODE, TMR0_CAPTURE_EDGE);	
    TIMER_EnableCaptureInt(TIMER0); // Enable timer interrupt
    NVIC_EnableIRQ(TMR0_IRQn);
}

void Init_TC1(void)
{
    TIMER_Open(TIMER1, TMR1_OPERATING_MODE, TMR1_OPERATING_FREQ);
    TIMER_SET_PRESCALE_VALUE(TIMER1, 11);
    // Set compare value as large as possible, so don't need to worry about counter overrun too frequently.
    TIMER_SET_CMP_VALUE(TIMER1, 0xFFFFFF);	
    TIMER_EnableCapture(TIMER1, TMR1_CAPTURE_MODE, TMR1_CAPTURE_EDGE);	
    TIMER_EnableCaptureInt(TIMER1); // Enable timer interrupt
    NVIC_EnableIRQ(TMR1_IRQn);
}

void Init_TC2(void)
{
    TIMER_Open(TIMER2, TMR2_OPERATING_MODE, TMR2_OPERATING_FREQ);
    TIMER_SET_PRESCALE_VALUE(TIMER2, 11);
    // Set compare value as large as possible, so don't need to worry about counter overrun too frequently.
    TIMER_SET_CMP_VALUE(TIMER2, 0xFFFFFF);	
    TIMER_EnableCapture(TIMER2, TMR2_CAPTURE_MODE, TMR2_CAPTURE_EDGE);	
    TIMER_EnableCaptureInt(TIMER2); // Enable timer interrupt
    NVIC_EnableIRQ(TMR2_IRQn);
}

void Init_TC3(void)
{
    TIMER_Open(TIMER3, TMR3_OPERATING_MODE, TMR3_OPERATING_FREQ);
    TIMER_SET_PRESCALE_VALUE(TIMER3, 11);
    // Set compare value as large as possible, so don't need to worry about counter overrun too frequently.
    TIMER_SET_CMP_VALUE(TIMER3, 0xFFFFFF);	
    TIMER_EnableCapture(TIMER3, TMR3_CAPTURE_MODE, TMR3_CAPTURE_EDGE);	
    TIMER_EnableCaptureInt(TIMER3); // Enable timer interrupt
    NVIC_EnableIRQ(TMR3_IRQn);
}

void Trigger_Ultrasound(void)
{
  PB6=1; PB7=1; PB8=1; PB9=1;  
  CLK_SysTickDelay(7); // generate 10us pulse to trigger SR04 
  PB6=0; PB7=0; PB8=0; PB9=0;
	
  TIMER0->TEXCON |= TIMER_TEXCON_RSTCAPSEL_Msk; // set RSTCAPSEL to 1 to reset counter	
  TIMER1->TEXCON |= TIMER_TEXCON_RSTCAPSEL_Msk; // set RSTCAPSEL to 1 to reset counter
  TIMER2->TEXCON |= TIMER_TEXCON_RSTCAPSEL_Msk; // set RSTCAPSEL to 1 to reset counter
  TIMER3->TEXCON |= TIMER_TEXCON_RSTCAPSEL_Msk; // set RSTCAPSEL to 1 to reset counter	

	distance[0]=9999;
	distance[1]=9999;
	distance[2]=9999;
	distance[3]=9999;
  TIMER_Start(TIMER0);
  TIMER_Start(TIMER1);
  TIMER_Start(TIMER2);	
  TIMER_Start(TIMER3);		
  CLK_SysTickDelay(30000); // Delay for Time-of-Fly
  TIMER_Stop(TIMER0);
  TIMER_Stop(TIMER1);
  TIMER_Stop(TIMER2);
  TIMER_Stop(TIMER3);	
}

void Read_Ultrasound(void)
{
	if (Timer_flag[0]==1) {
    distance[0] = Timer_count[0] * 340 /20000;
		Timer_count[0]=0;
    Timer_flag[0]=0;
  }
	if (Timer_flag[1]==1) {
    distance[1] = Timer_count[1] * 340 /20000;
		Timer_count[1]=0;
    Timer_flag[1]=0;
  }
	if (Timer_flag[2]==1) {
    distance[2] = Timer_count[2] * 340 /20000;
		Timer_count[2]=0;
    Timer_flag[2]=0;
  }
	if (Timer_flag[3]==1) {
    distance[3] = Timer_count[3] * 340 /20000;
		Timer_count[3]=0;
    Timer_flag[3]=0;
  }	
}

// Display sensor value rotatorily 
void Display_Message(void)
{
	if (disp_index==0) {
	  sprintf(Text,"D0=%5d cm", (int) distance[0]);	
    print_Line(0,Text);
	}
	if (disp_index==1) {
	  sprintf(Text,"D1=%5d cm", (int) distance[1]);	
    print_Line(1,Text);
	}
	if (disp_index==2) {
	  sprintf(Text,"D2=%5d cm", (int) distance[2]);	
    print_Line(2,Text);
	}
	if (disp_index==3) {
	  sprintf(Text,"D3=%5d cm", (int) distance[3]);	
    print_Line(3,Text);	
	}
	disp_index++;
	if (disp_index>3) disp_index=0;
}

int main(void)
{
  SYS_Init();
  init_LCD();
  clear_LCD();
	
  GPIO_SetMode(PB, BIT6, GPIO_MODE_OUTPUT);
  GPIO_SetMode(PB, BIT7, GPIO_MODE_OUTPUT);	
  GPIO_SetMode(PB, BIT8, GPIO_MODE_OUTPUT);	
  GPIO_SetMode(PB, BIT9, GPIO_MODE_OUTPUT);
  PB6=0; PB7=0; PB8=0; PB9=0;
	
  Init_TC0();	
  Init_TC1();
  Init_TC2();
  Init_TC3();
	  
  while(1) {
		Trigger_Ultrasound();
		Read_Ultrasound();
		Display_Message();
  }
}
