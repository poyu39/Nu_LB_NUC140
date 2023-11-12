//
// SPI_RC522 : SPI interfacing with RC522 RFID reader
//
// MCU : NUC140VE3CN
// EVB : Nu-LB-NUC140
// SPI : RFID-RC522
//
// RC522 RFID Reader
// SDA (SS) : connected to GPC8 (SPI1_SS)
// SCK (SCK): connected to GPC9 (SPI1_CLK)
// MOSI     : connected to GPC11(SPI1_MOSI)
// MISO     : connected to GPC10(SPI1_MISO)
// IRQ      : no connection
// GND      : connected to Gnd
// RST      : connected to 3.3V
// VCC      : connected to 3.3V

#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "LCD.h"
#include "RC522.h"
#include "rfid.h"

char     Text[16];
uint16_t cardType;
unsigned char UID[4];
unsigned char Password[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
unsigned char RF_Buffer[MAXRLEN];
unsigned char des_on      = 0;

void Flash_RedLED(uint8_t num)
{
	uint8_t i;
	for (i=0; i<num; i++) {
		PC12=0;
		CLK_SysTickDelay(100000);
		PC12=1;
		CLK_SysTickDelay(100000);
	}
}

void Init_RedLED(void)
{
	GPIO_SetMode(PC, BIT12, GPIO_MODE_OUTPUT);
	PC12=1; 
}

void Init_SPI(void)
{
	SPI_Open(RC522_SPI_PORT, SPI_MASTER, SPI_MODE_0, 8, 500000);
	SPI_DisableAutoSS(RC522_SPI_PORT);
	SPI_SET_MSB_FIRST(RC522_SPI_PORT);
}

int main( void )
{
	SYS_Init();

  Init_RedLED();

	init_LCD();  // initialize LCD
	clear_LCD();
	print_Line(0,"SPI RC522");
	
  Flash_RedLED(2);	
	
	Init_SPI();  // initialize SPI1	
  initRC522(); // initialize RC522

  // Auto Reader	
  while(1)
  {
    if(find_tag(&cardType)==TAG_OK)
    {
			switch(cardType) {
				case 0x0400:
					sprintf(Text, "MFOne-S50    ");
				  break;
				case 0x0200:
					sprintf(Text,"MFOne-S70    ");
				  break;
				case 0x4400:
					sprintf(Text, "MF-UltraLight");
				  break;
        case 0x0800:
				  sprintf(Text,"MF-Pro       ");
				  break;
        case 0x4403:
				  sprintf(Text,"MF Desire    ");
				  break;
				default:
					sprintf(Text,"Unknown Card ");
				  break;
			}
			print_Line(1, Text);
			
      if(PcdAnticoll(PICC_ANTICOLL1, UID)==TAG_OK)
      { 
        sprintf(Text,"UID :%x %x %x %x", UID[0],UID[1],UID[2],UID[3]);
				print_Line(2,Text);
        Flash_RedLED(1); 
        //while (!(IFG1 & UTXIFG0));
        //TXBUF0 = '\n';
      }
			else 
			  print_Line(2,"UID :           ");
			
			
    }
		else 
			print_Line(1,"No Card      ");    
  }
}
