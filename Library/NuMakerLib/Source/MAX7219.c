//
// MAX7219 Driver : drive 8x8 LEDs
//
#include <stdio.h>
#include "NUC100Series.h"
#include "SYS.h"
#include "GPIO.h"
#include "Font8x8.h"
#include "SYS_init.h"

// Define GPA2,1,0 as CLK, CS, DIN pins
#define  CLK_HI  PA2=1
#define  CLK_LO  PA2=0
#define  CS_HI   PA1=1
#define  CS_LO   PA1=0
#define  DIN_HI  PA0=1
#define  DIN_LO  PA0=0

// Initialize GPIOs for connecting MAX7219
void Init_GPIO_MAX7219(void)
{
  GPIO_SetMode(PA, BIT0, GPIO_MODE_OUTPUT);
  GPIO_SetMode(PA, BIT1, GPIO_MODE_OUTPUT);
  GPIO_SetMode(PA, BIT2, GPIO_MODE_OUTPUT);	
  PA0=0; PA1=0; PA2=0;
}

void WriteData_MAX7219(uint8_t DATA)         
{
   uint8_t i;    
   CS_LO;		
   for(i=8;i>=1;i--) {		  
     CLK_LO;
	 if (DATA&0x80) DIN_HI;
	 else           DIN_LO;
         DATA=DATA<<1;
     CLK_HI;
     CLK_SysTickDelay(10);
   }                                 
}

void write_MAX7219(uint8_t address, uint8_t dat)
{ 
	CS_LO;
  WriteData_MAX7219(address);           
  WriteData_MAX7219(dat);     
  CS_HI;                        
}

void Init_MAX7219(void)
{
	Init_GPIO_MAX7219();
  write_MAX7219(0x09, 0x00);
  write_MAX7219(0x0a, 0x03);
  write_MAX7219(0x0b, 0x07);
  write_MAX7219(0x0c, 0x01);
  write_MAX7219(0x0f, 0x00);
}

void printC_MAX7219(char ascii)
{
	uint8_t i;
  for(i=1;i<9;i++) write_MAX7219(i, Font8x8[ascii*8+i-1]);
}
