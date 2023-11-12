//
// proj_BT2LED5050 :  Android App remote control LED5050 strip
//
// AppInventor2: 
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
// pin4 TXD  : to UART1-RX/PB 
// pin6 STATE: N.C.
//
// LED5050 Driver       
// B : to PWM0/PA12 (NUC140VE3CN pin 65)
// R : to PWM1/PA13 (NUC140VE3CN pin 64)
// G : to PWM2/PA14 (NUC140VE3CN pin 63)

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "LCD.h"
#include "PWM.h"

// Global Variables
char Text[16];
volatile uint8_t BTcomRbuf[64], BTmessage[64];
volatile uint8_t BTcomRbytes = 0;
volatile uint8_t BTflag =0;

// UART1 to receive data from Bluetooth HC-05
void UART1_IRQHandler(void)
{
	uint8_t i;
	uint8_t c;
	uint32_t u32IntSts = UART1->ISR;
	
	if(u32IntSts & UART_IS_RX_READY(UART1)) // check ISR on & RX is ready
  {
		while (!(UART1->FSR & UART_FSR_RX_EMPTY_Msk)){ // check RX is not empty
			c = UART_READ(UART1); // read UART RX data
			if (c!='\n') {        // check line-end 
				BTcomRbuf[BTcomRbytes] = c;
				BTcomRbytes++;
			} else {
				for (i=0; i<BTcomRbytes; i++)	 BTmessage[i]=BTcomRbuf[i]; // store received data to Message
				BTmessage[BTcomRbytes]='\n';
				BTcomRbytes=0;                 // clear Read buffer pointer
				BTflag=1;	                   // set flag when BT command input
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

uint32_t PWM_ConfigOutputColor(PWM_T *pwm,
                                 uint32_t u32ChannelNum,
                                 uint32_t u32Frequency,
                                 uint8_t u8Color)
{
    uint32_t u32Src;
    uint32_t u32PWMClockSrc;
    uint32_t u32PWMClkTbl[8] = {__HXT, __LXT, NULL, __HIRC, NULL, NULL, NULL, __LIRC};
    uint32_t i;
    uint8_t  u8Divider = 1, u8Prescale = 0xFF;
    // this table is mapping divider value to register configuration
    uint32_t u32PWMDividerToRegTbl[17] = {NULL, 4, 0, NULL, 1, NULL, NULL, NULL, 2, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 3};
    uint16_t u16CNR = 0xFFFF;

    if(pwm == PWMA)
    {
        if(u32ChannelNum < 2)// channel 0 and channel 1
            u32Src = ((CLK->CLKSEL2 & (CLK_CLKSEL2_PWM01_S_EXT_Msk)) >> (CLK_CLKSEL2_PWM01_S_EXT_Pos - 2)) | (CLK->CLKSEL1 & (CLK_CLKSEL1_PWM01_S_Msk)) >> (CLK_CLKSEL1_PWM01_S_Pos);
        else // channel 2 and channel 3
            u32Src = ((CLK->CLKSEL2 & (CLK_CLKSEL2_PWM23_S_EXT_Msk)) >> (CLK_CLKSEL2_PWM23_S_EXT_Pos - 2)) | (CLK->CLKSEL1 & (CLK_CLKSEL1_PWM23_S_Msk)) >> (CLK_CLKSEL1_PWM23_S_Pos);
    }
    else //pwm == PWMB
    {
        if(u32ChannelNum < 2)// channel 0 and channel 1
            u32Src = ((CLK->CLKSEL2 & (CLK_CLKSEL2_PWM45_S_EXT_Msk)) >> (CLK_CLKSEL2_PWM45_S_EXT_Pos - 2)) | (CLK->CLKSEL2 & (CLK_CLKSEL2_PWM45_S_Msk)) >> (CLK_CLKSEL2_PWM45_S_Pos);
        else // channel 2 and channel 3
            u32Src = ((CLK->CLKSEL2 & (CLK_CLKSEL2_PWM67_S_EXT_Msk)) >> (CLK_CLKSEL2_PWM67_S_EXT_Pos - 2)) | (CLK->CLKSEL2 & (CLK_CLKSEL2_PWM67_S_Msk)) >> (CLK_CLKSEL2_PWM67_S_Pos);
    }

    if(u32Src == 2)
    {
        SystemCoreClockUpdate();
        u32PWMClockSrc = SystemCoreClock;
    }
    else
    {
        u32PWMClockSrc = u32PWMClkTbl[u32Src];
    }

    for(; u8Divider < 17; u8Divider <<= 1)    // clk divider could only be 1, 2, 4, 8, 16
    {
        i = (u32PWMClockSrc / u32Frequency) / u8Divider;
        // If target value is larger than CNR * prescale, need to use a larger divider
        if(i > (0x10000 * 0x100))
            continue;

        // CNR = 0xFFFF + 1, get a prescaler that CNR value is below 0xFFFF
        u8Prescale = (i + 0xFFFF) / 0x10000;

        // u8Prescale must at least be 2, otherwise the output stop
        if(u8Prescale < 3)
            u8Prescale = 2;

        i /= u8Prescale;

        if(i <= 0x10000)
        {
            if(i == 1)
                u16CNR = 1;     // Too fast, and PWM cannot generate expected frequency...
            else
                u16CNR = i;
            break;
        }
    }
    // Store return value here 'cos we're gonna change u8Divider & u8Prescale & u16CNR to the real value to fill into register
    i = u32PWMClockSrc / (u8Prescale * u8Divider * u16CNR);

    u8Prescale -= 1;
    u16CNR -= 1;
    // convert to real register value
    u8Divider = u32PWMDividerToRegTbl[u8Divider];

    // every two channels share a prescaler
    (pwm)->PPR = ((pwm)->PPR & ~(PWM_PPR_CP01_Msk << ((u32ChannelNum >> 1) * 8))) | (u8Prescale << ((u32ChannelNum >> 1) * 8));
    (pwm)->CSR = ((pwm)->CSR & ~(PWM_CSR_CSR0_Msk << (4 * u32ChannelNum))) | (u8Divider << (4 * u32ChannelNum));
    // set PWM to edge aligned type
    (pwm)->PCR &= ~(PWM_PCR_PWM01TYPE_Msk << (u32ChannelNum >> 1));
    (pwm)->PCR |= PWM_PCR_CH0MOD_Msk << (8 * u32ChannelNum);

    if(u8Color)
    {
        *((__IO uint32_t *)((((uint32_t) & ((pwm)->CMR0)) + u32ChannelNum * 12))) = u8Color * (u16CNR + 1) /255	- 1;
    }
    else
    {
        *((__IO uint32_t *)((((uint32_t) & ((pwm)->CMR0)) + u32ChannelNum * 12))) = 0;
    }
    *((__IO uint32_t *)((((uint32_t) & ((pwm)->CNR0)) + (u32ChannelNum) * 12))) = u16CNR;

    return(i);
}

void Init_PWM(void)
{
    // PWM0 frequency is 244Hz, Color=0
    PWM_ConfigOutputColor(PWM0, PWM_CH0, 244, 0);
    PWM_ConfigOutputColor(PWM0, PWM_CH1, 244, 0);
    PWM_ConfigOutputColor(PWM0, PWM_CH2, 244, 0);	
	
    // Enable output of all PWM0 channels
    PWM_EnableOutput(PWM0, PWM_CH_0_MASK | PWM_CH_1_MASK | PWM_CH_2_MASK);

    // Start
    PWM_Start(PWM0, PWM_CH_0_MASK | PWM_CH_1_MASK | PWM_CH_2_MASK);
}

int32_t main()
{
	uint8_t i=0, j=0,	beginPtr[3], endPtr[3];
	uint8_t offset[3]={0,0,0};
	char Red[3], Green[3], Blue[3];
	uint8_t R,G,B =0;
  SYS_Init();       // initialize MCU	
  Init_UART1();     // initialize UART1 for Bluetooth
	Init_PWM();       // initialize PWM0 for RGB
	init_LCD();
	clear_LCD();
	print_Line(0, "BT2LED5050");

  while(1){			
		if (BTflag==1) { // check if bluetooth sent new command
			
			// find begin/end pointer of R,G,B
			i=0; j=0;
			while(BTmessage[i]!='\n') {
				if (BTmessage[i]=='=') {j++; beginPtr[j-1]=i;};
		  	if (BTmessage[i]==',') endPtr[j-1]=i;
			  i++;
		  }
			endPtr[2]=i; //	B end pointer is \n
			
			// offset is the starting index of Red/Green/Blue to import BTmessage character
			Red[0]='0';Red[1]='0';Red[2]='0';
			offset[0]= 3- (endPtr[0]-beginPtr[0]-1); // 3 - number of digit = 3,2,1
			for (i=(beginPtr[0]+1);i<endPtr[0];i++) Red[i-(beginPtr[0]+1)+offset[0]]=BTmessage[i];		
			
			Green[0]='0';Green[1]='0';Green[2]='0';
			offset[1]= 3- (endPtr[1]-beginPtr[1]-1); // 3 - number of digit = 3,2,1	
			for (i=(beginPtr[1]+1);i<endPtr[1];i++) Green[i-(beginPtr[1]+1)+offset[1]]=BTmessage[i];
			
			Blue[0]='0';Blue[1]='0';Blue[2]='0';
			offset[2]= 3- (endPtr[2]-beginPtr[2]-1); // 3 - number of digit = 3,2,1				
			for (i=(beginPtr[2]+1);i<endPtr[2];i++) Blue[i-(beginPtr[2]+1)+offset[2]]=BTmessage[i];

// For debugging			
//			for (i=0; i<3; i++) {
//			  sprintf(Text,"%d %d %d", beginPtr[i], endPtr[i], offset[i]);
//			  print_Line(i+1, Text);
//			}
			
			sprintf(Text,"R=%s", Red);
			print_Line(1, Text);
			sprintf(Text,"G=%s", Green);
			print_Line(2, Text);
			sprintf(Text,"B=%s", Blue);
			print_Line(3, Text);
			
			// convert string to integer
			R=atoi(Red);
			G=atoi(Green);
			B=atoi(Blue);
			sprintf(Text,"R=%3dG=%3dB=%3d", R,G,B);
			print_Line(0, Text);
			
			//PWM_EnableOutput(PWM0, PWM_CH_0_MASK | PWM_CH_1_MASK | PWM_CH_2_MASK);
      PWM_ConfigOutputColor(PWM0, PWM_CH0, 244, B);
      PWM_ConfigOutputColor(PWM0, PWM_CH1, 244, R);
      PWM_ConfigOutputColor(PWM0, PWM_CH2, 244, G);	
		
			BTflag=0;
		}
	}

}
