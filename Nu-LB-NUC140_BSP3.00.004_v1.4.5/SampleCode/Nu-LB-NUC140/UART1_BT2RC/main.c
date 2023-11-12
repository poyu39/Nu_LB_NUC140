//
// UART1_BT2RC  : Android App remote control car/robot
//               MCU-UART1 will receive text from HC-05, 1st byte as COMMAND to control GPIOs
//               GPIOs to drive DC motors / LEDs
//
// AppInventor2: myBT2RC.aia
//
// Board       : Nu-LB-NUC140
// MCU         : NUC140VE3CN (LQFP100)
// Module      : HC-05 (Bluetooth 2.0)
//               baudrate=9600, databit=8, stopbit=1, paritybit=0, flowcontrol=None
//
// Bluetooth module (HC05)
// pin1 KEY  : N.C
// pin2 VCC  : to VCC +5V
// pin3 GND  : to GND
// pin4 TXD  : to UART1-RX/PB4 (NUC140VE3CN pin 19)
// pin5 RXD  : to UART1-TX/PB5 (NUC140VE3CN pin 20)
// pin6 STATE: N.C.
//
// DC motor driver board (L298N)
// ENA : to VCC         
// IN1 : to PC12 (NUC140VE3CN pin 57)
// IN2 : to PC13 (NUC140VE3CN pin 56)
// IN3 : to PC14 (NUC140VE3CN pin 90)
// IN4 : to PC15 (NUC140VE3CN pin 89)
// ENB : to VCC

#include <stdio.h>
#include <math.h>
#include <string.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "LCD.h"

// Global Variables
char     Text[16];
char     BT_Command=' ';
uint8_t  BTcomRbuf[256];
volatile uint8_t BTcomRbytes = 0;
volatile uint8_t BT_Flag =0;
char     BT_Message[16];

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
				BTcomRbuf[BTcomRbytes] = c;
				BTcomRbytes++;
			} else {
				BT_Command=BTcomRbuf[0];       // COMMAND = first charactor received
				for (i=0; i<BTcomRbytes; i++)	 BT_Message[i]=BTcomRbuf[i]; // store received data to Message
				for (i=BTcomRbytes; i<16; i++) BT_Message[i]=' ';
				BTcomRbytes=0;                 // clear Read buffer pointer
				BT_Flag=1;	                   // set flag when BT command input
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

void Init_GPIO(void)
{
  GPIO_SetMode(PC, BIT12, GPIO_MODE_OUTPUT);
  GPIO_SetMode(PC, BIT13, GPIO_MODE_OUTPUT);
  GPIO_SetMode(PC, BIT14, GPIO_MODE_OUTPUT);
  GPIO_SetMode(PC, BIT15, GPIO_MODE_OUTPUT);
  PC12=0; PC13=0; PC14=0; PC15=0;
}

void Motor_Forward(void) // To move forward
{ PC12=1; PC13=0; PC14=1; PC15=0;}	// GPIO pins control Motor

void Motor_Backward(void) // To move backward
{ PC12=0; PC13=1; PC14=0; PC15=1;}

void Motor_Left(void) // To turn left
{ PC12=1; PC13=0; PC14=0; PC15=1;}

void Motor_Right(void) // To turn right
{ PC12=0; PC13=1; PC14=1; PC15=0;}	

void Motor_Stop(void) // To stop motors
{ PC12=0; PC13=0; PC14=0; PC15=0;}

void Check_BTCommand(void)
{ 
	if      (BT_Command=='F') Motor_Forward();
	else if (BT_Command=='B') Motor_Backward();
	else if (BT_Command=='R') Motor_Right();
	else if (BT_Command=='L') Motor_Left();
	else if (BT_Command=='S') Motor_Stop();		
}

void Display_Message(void)
{
	print_Line(2, BT_Message);
	sprintf(Text,"Command = %c", BT_Command);
	print_Line(3, Text);
}

int32_t main()
{
  SYS_Init();   // initialize MCU	
  Init_UART1(); // initialize UART1 for Bluetooth
  Init_GPIO();  // initialize GPIO for DC motor driver
	init_LCD();
	clear_LCD();
	print_Line(0, "proj_BT2RC");
	
  while(1){
		if (BT_Flag==1) { // check if bluetooth sent new command
		  Check_BTCommand();
	    Display_Message();
			BT_Flag=0;
		}
	}
}
