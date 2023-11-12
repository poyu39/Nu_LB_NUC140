//
// GPIO_L298N: GPIO outputs to control DC motor driver
//
// EVB : Nu-LB-NUC140
// MCU : NUC140VE3CN
//
// L298N board connected to +6/+12V battery, and supply 5V to MCU
// L298N-IN1 connected to PA0 (NUC140 LQFP100-pin71)
// L298N-IN2 connected to PA1 (NUC140 LQFP100-pin72)
// L298N-IN3 connected to PA2 (NUC140 LQFP100-pin73)
// L298N-IN4 connected to PA3 (NUC140 LQFP100-pin74)

#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"

void Init_GPIO(void)
{
	GPIO_SetMode(PA, BIT0, GPIO_MODE_OUTPUT);
  GPIO_SetMode(PA, BIT1, GPIO_MODE_OUTPUT);
  GPIO_SetMode(PA, BIT2, GPIO_MODE_OUTPUT);
  GPIO_SetMode(PA, BIT3, GPIO_MODE_OUTPUT);
  PA0=0; PA1=0; PA2=0; PA3=0;
}

void Forward()
{ PA0=1; PA1=0; PA2=1; PA3=0; }

void Backward()
{ PA0=0; PA1=1; PA2=0; PA3=1; }

void Right()
{ PA0=1; PA1=0; PA2=0; PA3=1; }

void Left()
{ PA0=0; PA1=1; PA2=1; PA3=0; }

void Stop()
{ PA0=0; PA1=0; PA2=0; PA3=0; }

int main(void)
{
	SYS_Init(); 
  Init_GPIO();
	
  Forward();
  CLK_SysTickDelay(500000); // Delay for driving Motor
  Backward();
  CLK_SysTickDelay(500000); // Delay for driving Motor
  Left();
  CLK_SysTickDelay(500000); // Delay for driving Motor
  Right();
  CLK_SysTickDelay(500000); // Delay for driving Motor	
  Stop();
}
