//
// UART1_ESP8266 : WiFi module interface with MCU
//
// Board       : Nu-LB-NUC140
// MCU         : NUC140VE3CN (LQFP100)
// Module      : ESP8266 (WiFi)
// UART1       : baudrate=9600, databit=8, stopbit=1, paritybit=0, flowcontrol=None
//
// ESP01 Connections 
// VCC   to VCC33
// CH_PD to VCC33
// RXD   to PB5 /UART1-TX
// TXD   to PB4 /UART1-RX
// GND   to Gnd

#include <stdio.h>
#include <math.h>
#include <string.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"

// ESP8266 AT commands
#define AT_CMD    "AT\r\n"
#define AT_RST    "AT+RST\r\n"
#define AT_CWMODE "AT+CWMODE=3\r\n"
#define AT_CWJAP  "AT+CWJAP=\"Kuo\",\"0972211921\"\r\n"
#define AT_CIFSR  "AT+CIFSR\r\n"
#define AT_CIPMUX "AT+CIPMUX=1\r\n"
#define AT_CIPSERVER "AT+CIPSERVER=1,8088\r\n"

// Global Variables
uint8_t  comRbuf[256];
volatile uint8_t comRbytes = 0;
volatile uint8_t comRlength= 0;
volatile uint8_t WiFi_Flag =0;

void UART1_IRQHandler(void)
{
	uint8_t i, c;
	uint32_t u32IntSts = UART1->ISR;
	
	if(u32IntSts & UART_IS_RX_READY(UART1)) // check ISR on & RX is ready
  {
		while (!(UART1->FSR & UART_FSR_RX_EMPTY_Msk)){ // check RX is not empty
			c = UART_READ(UART1); // read UART RX dat
			if (c!='\n') {
        comRbuf[comRbytes]=c;
			  comRbytes++;
			} else {
				comRbuf[comRbytes]=c;
				comRbytes++;
				for (i=0; i<comRbytes; i++) printf("%c", comRbuf[i]);
				comRbytes=0;
			}
    }			
	}	
}

void Init_UART1(void)
{ 
  UART_Open(UART1, 115200);                     // set UART baud rate
  UART_ENABLE_INT(UART1, UART_IER_RDA_IEN_Msk); // enable UART interrupt (triggerred by Read-Data-Available)
  NVIC_EnableIRQ(UART1_IRQn);		              // enable Cortex-M0 NVIC interrupt for UART
}
	
int32_t main()
{
	uint8_t i;
	SYS_Init();   // initialize MCU	
	UART_Open(UART0, 115200);

	Init_UART1(); // initialize UART1 for Bluetooth

	// AT
	UART_Write(UART1, AT_CMD, strlen(AT_CMD));
  CLK_SysTickDelay(2000);
	
	// set WiFi mode = Station+SoftAP (CWMODE=3)
	UART_Write(UART1, AT_CWMODE, strlen(AT_CWMODE));
  CLK_SysTickDelay(2000);
	
  // connect to WiFi AP 
	UART_Write(UART1, AT_CWJAP, strlen(AT_CWJAP));
  CLK_SysTickDelay(2000);
	
	for (i=0; i<25; i++) CLK_SysTickDelay(1000000);
	
	// get local IP address 
	UART_Write(UART1, AT_CIFSR, strlen(AT_CIFSR));
  CLK_SysTickDelay(2000);
	
  // enable multiple connections
	UART_Write(UART1, AT_CIPMUX, strlen(AT_CIPMUX));
  CLK_SysTickDelay(2000);
	
	// create TCP server
	UART_Write(UART1, AT_CIPSERVER, strlen(AT_CIPSERVER));
  CLK_SysTickDelay(200000);
	
  while(1);
}
