//
// UART0_RX : UART0 RX recieve and display Text
//
// Board       : Nu-LB-NUC140
// MCU         : NUC140VE3CN (LQFP100)
// UART0       : baudrate=115200, databit=8, stopbit=1, paritybit=0, flowcontrol=None

#include <stdio.h>
#include <math.h>
#include <string.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "LCD.h"

// Global Variables
char     Text[16];
uint8_t  comRbuf[256];
volatile uint8_t comRbytes = 0;
volatile uint8_t RX_Flag =0;

void UART02_IRQHandler(void)
{
	uint8_t c, i;
	uint32_t u32IntSts = UART0->ISR;
	
	if(u32IntSts & UART_IS_RX_READY(UART0)) // check ISR on & RX is ready
  {
		while (!(UART0->FSR & UART_FSR_RX_EMPTY_Msk)){ // check RX is not empty
			c = UART_READ(UART0); // read UART RX data
			if (c!='\n') {        // check line-end 
				comRbuf[comRbytes] = c;
				comRbytes++;
			} else {
				for (i=0; i<comRbytes; i++)	 Text[i]=comRbuf[i]; // store received data to Message
				for (i=comRbytes; i<16; i++) Text[i]=' ';
				comRbytes=0;
				RX_Flag=1;	                 // set flag when BT command input
				break;
			}
		}		
	}
}

void Init_UART0(void)
{ 
  UART_Open(UART0, 115200);                     // set UART0 baud rate
  UART_ENABLE_INT(UART0, UART_IER_RDA_IEN_Msk); // enable UART0 interrupt (triggerred by Read-Data-Available)
  NVIC_EnableIRQ(UART02_IRQn);		              // enable Cortex-M0 NVIC interrupt for UART02
}

int32_t main()
{
	SYS_Init();   // initialize MCU
  init_LCD();   // initialize LCD
  clear_LCD();  // clear LCD screen	
	print_Line(0, "UART0-RX");
	
	Init_UART0(); // initialize UART1 for Bluetooth

	
  while(1){
    if (RX_Flag==1) {
			print_Line(2, Text);
			RX_Flag=0;
		}
	}
}
