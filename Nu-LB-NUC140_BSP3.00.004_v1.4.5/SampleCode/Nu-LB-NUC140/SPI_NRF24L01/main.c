//
// SPI_NRF24L01 : SPI interfacing with nRF24L01 RF 2.4GHz RF module
//
// MCU : NUC140VE3CN
// EVB : Nu-LB-NUC140
// SPI : NRF24L01
//
// Connections
// SPI1_SS  : GPC8
// SPI1_CLK : GPC9
// SPI1_MISO: GPC10
// SPI1_MOSI: GPC11
// SPI1_CE  : GPC12

#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "LCD.h"
#include "NRF24L01.h"
//#define TRANSMITTER

uint16_t TransferCount =0;

int main(void)
{
	char Text[16] = "                ";
  char Data[NRF24L01_PAYLOAD] = "Hello";
	uint8_t Tx_address[5] = {0x34, 0x43, 0x10, 0x10, 0x01};
	uint8_t Rx_address[5] = {0x34, 0x43, 0x10, 0x10, 0x01};

	SYS_Init();	

	nRF_Init();
	nRF_config(NRF24L01_CHANNEL, NRF24L01_PAYLOAD); // channel #, payload
  nRF_setTADDR(Tx_address);
  nRF_setRADDR(Rx_address);   
	
	init_LCD();  //call initial pannel function
	clear_LCD();
	print_Line(0,"SPI_NRF24L01");
#ifdef TRANSMITTER
  print_Line(1,"Transmitting...");		
#else
	print_Line(1,"Receiving...");	
#endif

	while(1)
	{		
#ifdef TRANSMITTER	
			print_Line(2,Data);			
			nRF_send(Data);
			sprintf(Text,"%d",TransferCount);
			print_Line(3,Text);
			TransferCount++;
		  CLK_SysTickDelay(50000);
#else	
		  if (nRF_dataReady()) {
			nRF_getData(Data);					
			print_Line(2, Data);
			sprintf(Text,"%d",TransferCount);
			print_Line(3, Text);
			TransferCount++;
			}	
#endif	
	}			
}
