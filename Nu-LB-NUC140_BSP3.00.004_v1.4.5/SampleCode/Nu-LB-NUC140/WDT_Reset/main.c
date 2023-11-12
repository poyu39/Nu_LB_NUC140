//
// WDT_Feset : WatchDog TimeOut generate System Reset
// 
// WDT clock source = 10KHz (LIRC)

// EVB : Nu-LB-NUC140
// MCU : NUC140VE3CN

#include <stdio.h>
#include <string.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"

volatile uint8_t ledState =0;

void WDT_IRQHandler(void)
{ 
  ledState = ~ledState;
  WDT_CLEAR_TIMEOUT_INT_FLAG(); // Clear WDT interrupt flag
}


void Init_WDT(void)
{
  // WDT timeout every 2^14 WDT clock, disable system reset, disable wake up system
  SYS_UnlockReg();
  WDT_Open(WDT_TIMEOUT_2POW14, 0, TRUE, FALSE);
  WDT_EnableInt();          // Enable WDT timeout interrupt
  NVIC_EnableIRQ(WDT_IRQn); // Enable Cortex-M0 NVIC WDT interrupt vector
  SYS_LockReg();
}

void Init_GPIO(void)
{
  GPIO_SetMode(PC, BIT12, GPIO_PMD_OUTPUT);
  PC12=1;
}

void LED_flash(uint8_t no)
{
	uint8_t i;
  for (i=0; i<no; i++) {
    PC12=0;
    CLK_SysTickDelay(100000);
    PC12=1;
    CLK_SysTickDelay(100000);
  }		 
}

int32_t main (void)
{
  SYS_Init();  
  Init_WDT();
  Init_GPIO();
	
  LED_flash(3);
	
  while(1) {
    if(ledState==0) PC12=0;
    else            PC12=1;
  }
}

