//
// UART_Loopback : UART loopback test with RX connected to TX
//                  
// EVB : Nu-LB-NUC140
// MCU : NUC140VE3CN
//
// Connections 
// PB4 /UART1-RX (NUC140VE3CN LQFP100 pin19) 
// PB5 /UART1-TX (NUC140VE3CN LQFP100 pin20) 
// connect PB4 to PB5 (loopback)

#include <stdio.h>
#include <math.h>
#include <string.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"

void UART1_IRQHandler(void)
{   
    uint8_t in_char;
    if(UART_IS_RX_READY(UART1)) {			
      in_char=UART_READ(UART1);
      printf("%c",in_char);
    }
}

void Init_UART(void)
{
    UART_Open(UART1,9600);	// enable UART1 at 9600 baudrate
    UART_ENABLE_INT(UART1, UART_IER_RDA_IEN_Msk);
    NVIC_EnableIRQ(UART1_IRQn);	
}

int32_t main()
{
    uint32_t i =0;
    char Text[16];
    SYS_Init();	
    Init_UART();

    printf("NUC140 UART1 running\n");
    while(1) {
      CLK_SysTickDelay(1000000);
      sprintf(Text,"Loopback %5d\r\n", i);
      UART_Write(UART1, Text, 16);
      i++;
    }
}
