//
// PWM_Servo : PWM output to drive DC servo
//
// EVB : Nu-LB-NUC140
// MCU : NUC140VE3CN  (LQPF-100)

// using PWM to generate 50Hz (20ms) pulse to DC Servo signal pin
// 0.5 ~ 2.5ms high time (PWM clock at 10us per count)

// SG5010 DC servo
// pin1 : signal to PWM0/GPA12 (NUC140VE3CN LQFP100 pin65)
// pin2 : Vcc
// pin3 : Gnd
#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"

void Init_PWM0(void)
{
	PWM_ConfigOutputChannel(PWM0, PWM_CH0, 50,  3); // 0.544ms = ~3% duty_cycle
	PWM_EnableOutput(PWM0, PWM_CH_0_MASK);
	PWM_Start(PWM0, PWM_CH_0_MASK);
}

void Servo_Turn(uint8_t pos)
{
	switch(pos) {
		case  0: PWM_ConfigOutputChannel(PWM0, PWM_CH0, 50,  3); break; 
		case  1: PWM_ConfigOutputChannel(PWM0, PWM_CH0, 50,  4); break; 
		case  2: PWM_ConfigOutputChannel(PWM0, PWM_CH0, 50,  5); break; 
		case  3: PWM_ConfigOutputChannel(PWM0, PWM_CH0, 50,  6); break;
		case  4: PWM_ConfigOutputChannel(PWM0, PWM_CH0, 50,  7); break;
		case  5: PWM_ConfigOutputChannel(PWM0, PWM_CH0, 50,  8); break;
		case  6: PWM_ConfigOutputChannel(PWM0, PWM_CH0, 50,  9); break;
		case  7: PWM_ConfigOutputChannel(PWM0, PWM_CH0, 50, 10); break;
		case  8: PWM_ConfigOutputChannel(PWM0, PWM_CH0, 50, 11); break;
//		case  9: PWM_ConfigOutputChannel(PWM0, PWM_CH0, 50, 12); break;
//		case 10: PWM_ConfigOutputChannel(PWM0, PWM_CH0, 50, 13); break;
		default : PWM_ConfigOutputChannel(PWM0, PWM_CH0, 50, 3); break;
	}
	CLK_SysTickDelay(200000); // latency for turning motor
}

int32_t main(void)
{ 
  uint8_t i;
  
  SYS_Init();
  Init_PWM0();

  for (i=0; i<9; i++) Servo_Turn(i);
  Servo_Turn(0);
}
