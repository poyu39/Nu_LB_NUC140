//
// SPI_Loopback : SPI loopback test with MISO connected to MOSI
//
// MCU : NUC140VE3CN
// EVB : Nu-LB-NUC140
//
// Connections 
// GPC10/SPI1_MISO connected to GPC11
// GPC11/SPI1_MOSI connected to GPC10

#include <stdio.h>
#include <string.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"

void SPI_Init(void)
{   // set SPI1, mode 0, 32bit, 20MHz, enable AutoSS (active-low)
    SPI_Open(SPI1, SPI_MASTER, SPI_MODE_0, 32, 2000000);
    SPI_EnableAutoSS(SPI1, SPI_SS0, SPI_SS_ACTIVE_LOW);
}

int main(void)
{
	  uint8_t i;
    uint32_t TestCount =0;
	  uint8_t SrcData[16], DestData[16];

    SYS_Init();
    SPI_Init();

    printf("SPI1 Loopback Test\n");
	
	  TestCount=0;
	
	  while(1) {
			sprintf(SrcData,"TX = %6d", TestCount);
			printf("%s\n", SrcData);
			
			for (i=0; i<16; i++) {
			  // TX
        SPI_WRITE_TX0(SPI1, SrcData[i]);
        SPI_TRIGGER(SPI1);
        while(SPI_IS_BUSY(SPI1));			
			  // RX
			  DestData[i] = SPI_READ_RX0(SPI1);
			}
			DestData[0]='R';
			printf("%s\n", DestData);			
			TestCount++;
		}
}
