//
// proj_BT2Firebase : RFID read student ID, then send to Bluetooth
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

// Bluetooth module (HC05) : baudrate=9600, databit=8, stopbit=1, paritybit=0, flowcontrol=None
// pin1 KEY  : N.C
// pin2 VCC  : to VCC +5V
// pin3 GND  : to GND
// pin4 TXD  : to UART1-RX/PB4 (NUC140VE3CN pin 19)
// pin5 RXD  : to UART1-TX/PB5 (NUC140VE3CN pin 20)
// pin6 STATE: N.C.


#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "uart.h"
#include "LCD.h"
#include "RC522.h"
#include "rfid.h"

char     Text[16], BT_Text[16];
uint16_t cardType;
unsigned char UID[4];
unsigned char Password[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
unsigned char RF_Buffer[MAXRLEN];
unsigned char des_on      = 0;

uint8_t  comRbuf[256];
volatile uint8_t comRbytes = 0;
volatile uint8_t BT_flag =0;
uint8_t RFID_flag =0;

float Forehead_T = 36.5;

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
				for (i=0; i<comRbytes; i++)	 BT_Text[i]=comRbuf[i]; // store received data to Message
				for (i=comRbytes; i<16; i++) BT_Text[i]=' ';
				comRbytes=0;
				BT_flag=1;	                 // set flag when BT command input
				break;
			}
		}		
	}
}

void Buzz(uint8_t num)
{
	uint8_t i;
	for (i=0; i<num; i++) {
		PB11=0;
		CLK_SysTickDelay(100000);
		PB11=1;
		CLK_SysTickDelay(100000);
	}
}

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

void Init_Buzzer(void)
{
	GPIO_SetMode(PB, BIT11, GPIO_MODE_OUTPUT);
	PB11=1;
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

void Init_UART1(void)
{ 
  UART_Open(UART1, 9600);                       // set UART1 baud rate
  UART_ENABLE_INT(UART1, UART_IER_RDA_IEN_Msk); // enable UART1 interrupt (triggerred by Read-Data-Available)
  NVIC_EnableIRQ(UART1_IRQn);		                // enable Cortex-M0 NVIC interrupt for UART1
}

int main( void )
{
	SYS_Init();
	Init_Buzzer();   // initialize GPIO for Buzzer
  Init_RedLED();   // initialize GPIO for Red LED

	init_LCD();      // initialize LCD
	clear_LCD();
	print_Line(0,"RC522 + HC05");
	
	Init_SPI();      // initialize SPI1	
  initRC522();     // initialize RC522
	Flash_RedLED(2); // flash red LED twice
	
  Init_UART1();    // initialize UART1 for Bluetooth
	Buzz(2);         // Buzz 2 times
	
  while(1)
  {
	  // find RFID
    if(find_tag(&cardType)==TAG_OK)
    {
		  RFID_flag=1; // set RFID flag
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
			
			// print UID
      if(PcdAnticoll(PICC_ANTICOLL1, UID)==TAG_OK)
      { 
        sprintf(Text,"UID=%2x %2x %2x %2x", UID[0],UID[1],UID[2],UID[3]);
			  print_Line(2,Text);
        Flash_RedLED(1);
			}
			else 
			  print_Line(2,"UID=            ");			
    } else {
			print_Line(1,"No Card         "); 
      print_Line(2,"UID=            ");
			
			if (RFID_flag==1) { // No Card & RFID flag was set
				// send string to HC05
        sprintf(Text,"%2x%2x%2x%2x=%2f", UID[0],UID[1],UID[2],UID[3], Forehead_T);
        UART_Write(UART1, Text, strlen(Text));
			  Buzz(1);
			  RFID_flag=0; // clear RFID flag
			}
		} 
		
		// print Bluetooth received message if any
		if (BT_flag==1) {
			print_Line(3, BT_Text);
			BT_flag=0;
		}
  }
}
