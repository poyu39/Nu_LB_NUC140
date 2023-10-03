//
// LED16x16 Driver 
//
#include <stdio.h>
#include "NUC100Series.h"
#include "SYS_init.h"

#define  CLK_Hi        PA4=1
#define  CLK_Lo        PA4=0
#define  AB_Hi         PA3=1
#define  AB_Lo         PA3=0
#define  RCK_Hi        PA2=1
#define  RCK_Lo        PA2=0
#define  SRCK_Hi       PA1=1
#define  SRCK_Lo       PA1=0
#define  SER_Hi        PA0=1
#define  SER_Lo        PA0=0

void init_LED16x16(void)
{
	GPIO_SetMode(PA, 4, GPIO_MODE_OUTPUT);
	GPIO_SetMode(PA, 3, GPIO_MODE_OUTPUT);
	GPIO_SetMode(PA, 2, GPIO_MODE_OUTPUT);
	GPIO_SetMode(PA, 1, GPIO_MODE_OUTPUT);
	GPIO_SetMode(PA, 0, GPIO_MODE_OUTPUT);	
	// set to default
  PA4=0;
	PA3=0;
	PA2=0;
	PA1=0;
	PA0=0;
}

void display_LED16x16(uint8_t data[32])
{	
	uint8_t i,ia,j,tmp;		

  AB_Lo;
	for(i=0;i<16;i++)
	{
		CLK_Lo;
		RCK_Lo;
		SRCK_Lo;
		for(ia=2; ia>0; ia--)	
		{	
			tmp = ~ data[i*2+ia-1];	// even-byte will shift to Right, odd-byte at Left			
			for(j=0; j<8; j++) 
			{	
				SRCK_Lo;
				if ((tmp>>j) & 0x01) SER_Hi; 
				else                 SER_Lo;
        SRCK_Hi;			
		  }
		}
		RCK_Hi;
		CLK_Hi;
		AB_Hi;
		CLK_SysTickDelay(1000);
	}
  SRCK_Lo;
	RCK_Lo;
	CLK_Lo;
	CLK_SysTickDelay(1000);
}
