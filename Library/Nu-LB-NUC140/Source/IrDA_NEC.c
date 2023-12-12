#include <stdio.h>
#include "NUC100Series.h"
#include "SYS_init.h"
#include "IrDA_NEC.h"

/// @cond HIDDEN_SYMBOLS
volatile	uint8_t 			IR_State = 0;		// IR State
volatile	uint8_t 			IR_LDC_Ready = 0;	// LeaDer Code is Ready
volatile	uint8_t 			IR_CTC_Ready = 0;	// CusTomer Code is Ready
volatile	uint8_t 			IR_CTC0 = 0;		// Received CusTomer Code 0
volatile	uint8_t 			IR_CTC1 = 0;		// Received CusTomer Code 1
volatile	uint8_t  			IR_DAC = 0;			// Received Data Code
volatile	uint8_t  			IR_DAB = 0;			// Received Data Bar code
volatile	uint8_t  			IR_cnt = 0;	
volatile  uint8_t				IR_CODE[4]	=	{0x00, 0x00, 0x00, 0x00};
/// @endcond

void IrDA_NEC_Rx(uint32_t u32Time)
{	
    if(IR_State == 0)
    {
        IR_LDC_Ready = 0;           										// Clear LeaDer Code Ready
        IR_CTC_Ready = 0;           										// Clear CusTomer Code Ready
        IR_State++;
    }
    // Leader or Repeater code
    else if(IR_State == 1)
    {
        // Leader code
        if((u32Time >= IR_LDC_MIN) && (u32Time <= IR_LDC_MAX))
        {
            IR_LDC_Ready = 1;       										// Set LeaDer Code Ready
            IR_State++;
        }
        else
        {
            IR_State = 1;
            IR_LDC_Ready = 0;           									// Clear LeaDer Code Ready
            IR_CTC_Ready = 0;           									// Clear CusTomer Code Ready
        }
    }
    // Customer code 0
    else if((IR_State >= 2 && IR_State < 10) && (IR_LDC_Ready == 1))
    {
        IR_State++;
        IR_CTC0 = IR_CTC0 >> 1;
        if((u32Time >= IR_BIT_0_MIN) && (u32Time <= IR_BIT_0_MAX))          // 1.12ms = 0
            IR_CTC0 &= 0x7f;
        else if((u32Time >= IR_BIT_1_MIN) && (u32Time <= IR_BIT_1_MAX)) 	// 2.25ms = 1
            IR_CTC0 |= 0x80;
        else
            IR_State = 0;
    }
    // Customer code 1
    else if((IR_State >= 10 && IR_State < 18) && (IR_LDC_Ready == 1))
    {
        IR_State++;
        IR_CTC1 = IR_CTC1 >> 1;
        if((u32Time >= IR_BIT_0_MIN) && (u32Time <= IR_BIT_0_MAX))          // 1.12ms = 0
            IR_CTC1 &= 0x7f;
        else if((u32Time >= IR_BIT_1_MIN) && (u32Time <= IR_BIT_1_MAX)) 	// 2.25ms = 1
            IR_CTC1 |= 0x80;
        else
            IR_State = 0;
    }
    // Data
    else if((IR_State >= 18 && IR_State < 26) && (IR_LDC_Ready == 1))
    {
        IR_State++;
        IR_DAC = IR_DAC >> 1;
        if((u32Time >= IR_BIT_0_MIN) && (u32Time <= IR_BIT_0_MAX))          // 1.12ms = 0
            IR_DAC &= 0x7f;
        else if((u32Time >= IR_BIT_1_MIN) && (u32Time <= IR_BIT_1_MAX))     // 2.25ms = 1
            IR_DAC |= 0x80;
        else
            IR_State = 0;

    }
    // Data bar
    else if((IR_State >= 26 && IR_State < 34) && (IR_LDC_Ready == 1))
    {
        IR_State++;
        IR_DAB = IR_DAB >> 1;
        if((u32Time >= IR_BIT_0_MIN) && (u32Time <= IR_BIT_0_MAX))     		// 1.12ms = 0
            IR_DAB &= 0x7f;
        else if((u32Time >= IR_BIT_1_MIN) && (u32Time <= IR_BIT_1_MAX))     // 2.25ms = 1
            IR_DAB |= 0x80;
        else
            IR_State = 0;

        if(IR_State == 34)
        {
            if((IR_DAC ^ IR_DAB) == 0xff)
            {
                IR_LDC_Ready = 0;   										// Clear LeaDer Code Ready
                IR_CODE[0] = IR_CTC0;
                IR_CODE[1] = IR_CTC1;
                IR_CODE[2] = IR_DAC;
                IR_CODE[3] = IR_DAB;
                IR_cnt++;	               							
				        printf("Code=%2x,%2x,%2x,%2x\n", IR_CTC0, IR_CTC1, IR_DAC, IR_DAB);
            }
            IR_State = 0;
        }
    }
}

#define     NEC_LDC_MARK        16      // 16 x 560us = 8960us =   9ms
#define     NEC_LDC_SPACE       8       //  8 x 560us = 4480us = 4.5ms
#define     NEC_BIT_MARK        1       // 560us
#define     NEC_ONE_SPACE       3       //  3 x 560us = 1680us = 1690us
#define     NEC_ZERO_SPACE      1       // 560us
#define     NEC_BYTES           4

// Pulse = 1/3 duty @38KHz frequency
void Mark(uint8_t N)
{
    // Switch to PWM output waveform 
    PWM_EnableOutput(PWM0,PWM_CH_0_MASK);
    CLK_SysTickDelay(560*N);
    PWM_DisableOutput(PWM0,PWM_CH_0_MASK);
}

void SPACE(uint8_t N)
{
    CLK_SysTickDelay(560*N);
}

void SendNEC(uint8_t* data)
{
	uint8_t	nbyte;
	uint8_t	nbit;

  // Send out Leader code
	Mark(NEC_LDC_MARK);
	SPACE(NEC_LDC_SPACE);
   
	// Send out Customer code and Data code
	for (nbyte=0; nbyte < NEC_BYTES; nbyte++){
		for (nbit=0; nbit < 8; nbit++){
			Mark(NEC_BIT_MARK);
      		if (data[nbyte] & (1 << nbit))		// LSB first
	        	SPACE(NEC_ONE_SPACE);
      		else
        		SPACE(NEC_ZERO_SPACE); 
		}
	}
	// Send out Stop bit 
	Mark(NEC_BIT_MARK); 
}  
 
void IrDA_NEC_TxRx_Init(void)
{	

	// PWM Clock setup
    PWM_ConfigOutputChannel(PWM0, PWM_CH0, 38000, 50);
	PWM_SET_DIVIDER(PWM0, 0, PWM_CLK_DIV_1);							//Set PWM0_CH0 Clock Source Divider    
	PWM_SET_DIVIDER(PWM0, 1, PWM_CLK_DIV_4);							//Set PWM0_CH1 Clock Source Divider
    PWM_SET_PRESCALER(PWM0, 1, 2);	                      //Set PWM0_CH0 and PWM0_CH1 Prescaler
	PWM_SET_CNR(PWM0, 1, MaxValue);
    PWM_SET_CNR(PWM0, 0, ((120000000/3/38000+5)/10));
    PWM_SET_CMR(PWM0, 0, ((120000000/3/38000+5)/10)/2);

	// PWM Capture setup
	PWM_EnableCapture(PWM0, PWM_CH_1_MASK);
    PWM_EnableCaptureInt(PWM0, 1, PWM_CAPTURE_INT_FALLING_LATCH);
    NVIC_EnableIRQ(PWM0_IRQn);
	PWM_Start(PWM0, PWM_CH_0_MASK | PWM_CH_1_MASK);     	
}
