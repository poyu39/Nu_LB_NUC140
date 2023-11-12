//
// UART1_HC05 : Bluetooth serial port
//             use Bluetooth Terminal to send message to MCU
//
// Board       : Nu-LB-NUC140
// MCU         : NUC140VE3CN (LQFP100)
// Module      : HC-05 (Bluetooth 2.0)
//
// Interfaces:
// UART1 to HC05 : transmit/receive data through Bluetooth
// baudrate=9600, databit=8, stopbit=1, paritybit=0, flowcontrol=None
//
// Bluetooth module (HC05)
// pin1 KEY  : N.C
// pin2 VCC  : to VCC +5V
// pin3 GND  : to GND
// pin4 TXD  : to UART1-RX/PB4 (NUC140VE3CN pin 19)
// pin5 RXD  : to UART1-TX/PB5 (NUC140VE3CN pin 20)
// pin6 STATE: N.C.

#include <stdio.h>
#include <math.h>
#include <string.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "LCD.h"

// Global Variables
char     Text[16];
uint8_t  comRbuf[16];
uint8_t  r;
volatile uint8_t comRbytes = 0;
volatile uint8_t BT_Flag =0;

// UART1 to receive data from Bluetooth HC-05
void UART1_IRQHandler(void)
{
	uint8_t c, i;
	uint32_t u32IntSts = UART1->ISR;
	
	if(u32IntSts & UART_IS_RX_READY(UART1)) // check ISR on & RX is ready
  {
		while (!(UART1->FSR & UART_FSR_RX_EMPTY_Msk)){ // check RX is not empty
			c = UART_READ(UART1); // read UART RX data
			if (c!='\n') {        // check line-end 
				comRbuf[comRbytes] = c;
				comRbytes++;
			} else {
        r=comRbytes;				
        strcpy(Text,comRbuf);
				comRbytes=0;
				BT_Flag=1;	                 // set flag when BT command input
				break;
			}
		}		
	}
}

void Init_UART1(void)
{ 
  UART_Open(UART1, 9600);                       // set UART1 baud rate
  UART_ENABLE_INT(UART1, UART_IER_RDA_IEN_Msk); // enable UART1 interrupt (triggerred by Read-Data-Available)
  NVIC_EnableIRQ(UART1_IRQn);		                // enable Cortex-M0 NVIC interrupt for UART1
}

int32_t main()
{
  SYS_Init();   // initialize MCU
  init_LCD();   // initialize LCD
  clear_LCD();  // clear LCD screen
	
  Init_UART1(); // initialize UART1 for Bluetooth
	
  print_Line(0, "UART1 HC05");
	
  while(1){
    if (BT_Flag==1) {
			print_Line(2, Text);
			sprintf(Text,"%d", r);
			print_Line(3, Text);
			BT_Flag=0;
		}
	}
}
