//
// UART_RC522 : UART to read RC522 RFID reader
// 
// EVB     : Nu-LB-NUC140
// MCU     : NUC140VE3CN (LQFP100)
// Module  : RC522 RFID reader, comm. port = 9600, N, 1, 8, 1

// Bluetooth module
// STATE N.C. (LED indication)
// RXD   to PB5 /UART1-TX (NUC140VE3CN LQFP100 pin20) 
// TXD   to PB4 /UART1-RX (NUC140VE3CN LQFP100 pin19) 
// GND   to Gnd
// VCC   to Vcc (3.3~6V)
// KEY   N.C. (AT command mode)
//
// compact command (1 byte)
// 0x01 : Search Card
// 0x02 : Read the card serial number
// 0x03 : Record the card's serial number into an authorization list
// 0x04 : check if a card is in authorization list
// 0x05 : Remove a card's serial number from authorization list

#include <stdio.h>
#include <math.h>
#include <string.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "LCD.h"

void Init_RC522(void)
{
	uint8_t write_buf[1];
	UART_Open(UART1,9600);	// enable UART1 at 9600 baudrate
	write_buf[0]= 0x02;
	UART_Write(UART1, write_buf, 1);
}

int32_t main()
{
	char Text[16];
	uint8_t read_buf[4];
  
	SYS_Init();
  init_LCD();
	clear_LCD();
  print_Line(0,"RC522 RFID");
	
  Init_RC522();	
	
  while(1) {
		UART_Read(UART1, read_buf, 4);
		sprintf(Text,"%2x%2x%2x%2x",read_buf[0],read_buf[1],read_buf[2],read_buf[3]);
		print_Line(1,Text);
  }
}
