//
// GPIO_StepMotor : GPIO output to control StepMotor
//
// EVB : Nu-LB-NUC140
// MCU : NUC140VE3CN
// DCM : 28BYJ-48 +5V Step Motor
//       Driver Board using ULN2003A
//
// StepMotor driver board connections:
// INA      to PA3
// INB      to PA2
// INC      to PA1
// IND      to PA0

#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"

// Definitions for Step Motor turning degree
#define d360 512
#define d180 512/2
#define d90  512/4
#define d45  512/8

unsigned char CW[8] ={0x09,0x01,0x03,0x02,0x06,0x04,0x0c,0x08}; //Clockwise Sequence
unsigned char CCW[8]={0x08,0x0c,0x04,0x06,0x02,0x03,0x01,0x09}; //Counter-Clockwise Sequence

void CW_MOTOR(uint16_t deg)
{
 int i=0,j=0;

for(j=0;j<(deg);j++)
{
    for(i=0;i<8;i++)
	{
	PA->DOUT=CW[i];
	CLK_SysTickDelay(2000);//delay 2000us = 2ms
	}
 }
}

void CCW_MOTOR(uint16_t deg)
{
 int i=0,j=0;

for(j=0;j<(deg);j++)
{
    for(i=0;i<8;i++)
	{
	PA->DOUT=CCW[i];
	CLK_SysTickDelay(2000);//delay 2000us = 2ms
	}
 }
}

int main (void)
{ 
	SYS_Init(); // initialize clocks & multifunction pins
	CW_MOTOR(d360); // Clockwise         for 360 degree
	CCW_MOTOR(d180);// Counter-Clockwise for 180 degree
}
