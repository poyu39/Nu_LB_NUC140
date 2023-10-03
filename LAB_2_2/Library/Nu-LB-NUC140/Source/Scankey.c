/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright (c) Nuvoton Technology Corp. All rights reserved.                                             */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <NUC100Series.h>
#include "GPIO.h"
#include "Scankey.h"
#include "SYS_init.h"

void delay(void)
{
	int j;
	for(j=0;j<1000;j++);
}

void OpenKeyPad(void)
{
  GPIO_SetMode(PA, BIT0, GPIO_MODE_QUASI);
  GPIO_SetMode(PA, BIT1, GPIO_MODE_QUASI);
  GPIO_SetMode(PA, BIT2, GPIO_MODE_QUASI);
  GPIO_SetMode(PA, BIT3, GPIO_MODE_QUASI);
  GPIO_SetMode(PA, BIT4, GPIO_MODE_QUASI);
  GPIO_SetMode(PA, BIT5, GPIO_MODE_QUASI);						
}

uint8_t ScanKey(void)
{
  PA0=1; PA1=1; PA2=0; PA3=1; PA4=1; PA5=1;
	if (PA3==0) return 1;
	if (PA4==0) return 4;
	if (PA5==0) return 7;
  PA0=1; PA1=0; PA2=1; PA3=1; PA4=1; PA5=1;
	if (PA3==0) return 2;
	if (PA4==0) return 5;
	if (PA5==0) return 8;
  PA0=0; PA1=1; PA2=1; PA3=1; PA4=1; PA5=1;
	if (PA3==0) return 3;
	if (PA4==0) return 6;
	if (PA5==0) return 9;
	return 0;
}
