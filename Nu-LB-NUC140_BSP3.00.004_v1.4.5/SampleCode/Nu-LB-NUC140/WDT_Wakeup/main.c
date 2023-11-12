//
// WDT_Interrupt : WatchDog Timer timeout to wakeup MCU from powerdown mode 
//                 WDT clock source = 10KHz (LIRC)
// EVB : Nu-LB-NUC140
// MCU : NUC140VE3CN

#include <stdio.h>
#include <string.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "PowerDown.h"

void WDT_IRQHandler(void)
{
  Leave_PowerDown();
	
  if(WDT_GET_TIMEOUT_INT_FLAG()) {  // Check WDT interrupt flag
     printf("WDT TimeOut Interrupt !!!\n");
     WDT_CLEAR_TIMEOUT_INT_FLAG(); // Clear WDT interrupt flag
  }
  
  if(WDT_GET_TIMEOUT_WAKEUP_FLAG()) {  // Check WDT wake up flag		
     printf("WDT Wakeup !!!\n");
     WDT_CLEAR_TIMEOUT_WAKEUP_FLAG(); // Clear WDT wake up flag
  }
}

void Init_WDT(void)
{
  // WDT timeout every 2^14 WDT clock, disable system reset, enable wake up system
  SYS_UnlockReg();
  WDT_Open(WDT_TIMEOUT_2POW14, 0, FALSE, TRUE);
  WDT_EnableInt();          // Enable WDT timeout interrupt
  NVIC_EnableIRQ(WDT_IRQn); // Enable Cortex-M0 NVIC WDT interrupt vector
  SYS_LockReg();
}

int32_t main (void)
{
  SYS_Init();
  UART_Open(UART0, 115200);
	  
  printf("WatchDog Wakekup Test\n");
  Init_WDT();
	
  while(1) {
	  printf("Entering Power Down !\n");
    Enter_PowerDown();
  }
}
