//
// PWM_Capture : PWM0 ch0~3 output to PWM1 ch0~3 Capture 
//
// EVB : Nu-LB-NUC140
// MCU : NUC140VE3CN  (LQPF-100)

// Connections:
// PMW0-CH0/PA12 to PWM1-CH0/PB11
// PMW0-CH1/PA13 to PWM1-CH1/PE5
// PMW0-CH2/PA14 to PWM1-CH2/PE0
// PMW0-CH3/PA15 to PWM1-CH3/PE1

#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"

void PWMB_IRQHandler(void)
{
    uint32_t u32CapIntFlag0;	
    uint32_t u32CapIntFlag1;

    u32CapIntFlag0 = PWM1->CCR0;
    u32CapIntFlag1 = PWM1->CCR2;
	
    if(u32CapIntFlag0 & PWM_CCR0_CAPIF0_Msk)
    {
        PWMB->CCR0 &= (PWM_CCR_MASK | PWM_CCR0_CAPIF0_Msk);
    }	
    if(u32CapIntFlag0 & PWM_CCR0_CAPIF1_Msk)
    {
        PWMB->CCR0 &= (PWM_CCR_MASK | PWM_CCR0_CAPIF1_Msk);
    }
    if(u32CapIntFlag1 & PWM_CCR2_CAPIF2_Msk)
    {
        PWMB->CCR2 &= (PWM_CCR_MASK | PWM_CCR2_CAPIF2_Msk);
    }	
    if(u32CapIntFlag1 & PWM_CCR2_CAPIF3_Msk)
    {
        PWMB->CCR2 &= (PWM_CCR_MASK | PWM_CCR2_CAPIF3_Msk);
    }
}

/*--------------------------------------------------------------------------------------*/
/* Capture function to calculate the input waveform information                         */
/* u32Count[4] : Keep the internal counter value when input signal rising / falling     */
/*               happens                                                                */
/*                                                                                      */
/* time    A    B     C     D                                                           */
/*           ___   ___   ___   ___   ___   ___   ___   ___                              */
/*      ____|   |_|   |_|   |_|   |_|   |_|   |_|   |_|   |_____                        */
/* index              0 1   2 3                                                         */
/*                                                                                      */
/* The capture internal counter down count from 0x10000, and reload to 0x10000 after    */
/* input signal falling happens (Time B/C/D)                                            */
/*--------------------------------------------------------------------------------------*/
uint16_t CalPeriodTime(PWM_T *PWM, uint32_t u32Ch)
{
    uint16_t u32Count[4];
    uint32_t u32i;
    uint16_t u16RisingTime, u16FallingTime, u16HighPeriod, u16LowPeriod, u16TotalPeriod;

    u32i = 0;

    while(u32i < 4)
    {
        /* Wait for Capture Falling Indicator */
        while(PWM_GetCaptureIntFlag(PWM, u32Ch) < 2);

        /* Clear Capture Falling and Rising Indicator */
        PWM_ClearCaptureIntFlag(PWM, u32Ch, PWM_CAPTURE_INT_FALLING_LATCH | PWM_CAPTURE_INT_RISING_LATCH);
			
        /* Get Capture Falling Latch Counter Data */
        u32Count[u32i++] = PWM_GET_CAPTURE_FALLING_DATA(PWM, u32Ch);

        /* Wait for Capture Rising Indicator */
        while(PWM_GetCaptureIntFlag(PWM, u32Ch) < 2);

        /* Clear Capture Rising Indicator */
        PWM_ClearCaptureIntFlag(PWM, u32Ch, PWM_CAPTURE_INT_RISING_LATCH);

        /* Get Capture Rising Latch Counter Data */
        u32Count[u32i++] = PWM_GET_CAPTURE_RISING_DATA(PWM, u32Ch);
    }

    u16RisingTime = u32Count[1];
    u16FallingTime = u32Count[0];
    u16HighPeriod = u32Count[1] - u32Count[2];
    return u16HighPeriod;	
		
    //u16LowPeriod = 0x10000 - u32Count[1];
    //u16TotalPeriod = 0x10000 - u32Count[2];	
		
    //printf("\nPWM generate: \nHigh Period=7199 ~ 7201, Low Period=16799 ~ 16801, Total Period=23999 ~ 24001\n");
    //printf("Capture Result: Rising Time = %d, Falling Time = %d \nHigh Period = %d, Low Period = %d, Total Period = %d.\n\n",
    //       u16RisingTime, u16FallingTime, u16HighPeriod, u16LowPeriod, u16TotalPeriod);
    //printf("Capture Result: Rising Time = %d, Falling Time = %d \n",u16RisingTime, u16FallingTime);
		//printf("High Period = %d, Low Period = %d, Total Period = %d.\n",u16HighPeriod, u16LowPeriod, u16TotalPeriod);
    //if((u16HighPeriod < 7199) || (u16HighPeriod > 7201) || (u16LowPeriod < 16799) || (u16LowPeriod > 16801) || (u16TotalPeriod < 23999) || (u16TotalPeriod > 24001))
    //    printf("Capture Test Fail!!\n");
    //else
    //    printf("Capture Test Pass!!\n");
}

void init_PWM(void)
{
  PWM_ConfigOutputChannel(PWM0, PWM_CH0, 310, 47);
  PWM_ConfigOutputChannel(PWM0, PWM_CH1, 310, 47);
  PWM_ConfigOutputChannel(PWM0, PWM_CH2, 310, 47);
  PWM_ConfigOutputChannel(PWM0, PWM_CH3, 310, 47);	
  PWM_EnableOutput(PWM0, PWM_CH_0_MASK | PWM_CH_1_MASK | PWM_CH_2_MASK | PWM_CH_3_MASK);
  PWM_Start(PWM0, PWM_CH_0_MASK | PWM_CH_1_MASK | PWM_CH_2_MASK | PWM_CH_3_MASK);	
}

void init_PWM_Capture(void)
{
  PWM_ConfigCaptureChannel(PWM1, PWM_CH0, 250, 0);	
  PWM_ConfigCaptureChannel(PWM1, PWM_CH1, 250, 0);	
  PWM_ConfigCaptureChannel(PWM1, PWM_CH2, 250, 0);
  PWM_ConfigCaptureChannel(PWM1, PWM_CH3, 250, 0);	
  PWM1->PBCR = 1;
  PWM_EnableCaptureInt(PWM1, PWM_CH0, PWM_CAPTURE_INT_FALLING_LATCH);	
  PWM_EnableCaptureInt(PWM1, PWM_CH1, PWM_CAPTURE_INT_FALLING_LATCH);		
  PWM_EnableCaptureInt(PWM1, PWM_CH2, PWM_CAPTURE_INT_FALLING_LATCH);	
  PWM_EnableCaptureInt(PWM1, PWM_CH3, PWM_CAPTURE_INT_FALLING_LATCH);
  NVIC_EnableIRQ((IRQn_Type)(PWMB_IRQn));
  PWM_Start(PWM1, PWM_CH_0_MASK | PWM_CH_1_MASK | PWM_CH_2_MASK | PWM_CH_3_MASK);
  PWM_EnableCapture(PWM1, PWM_CH_0_MASK | PWM_CH_1_MASK | PWM_CH_2_MASK | PWM_CH_3_MASK);
}

int32_t main(void)
{
  uint16_t u16HighPeriod[4];
	
  SYS_Init();
  init_PWM();
  init_PWM_Capture();
	
  while(1)
  {
    u16HighPeriod[0]=CalPeriodTime(PWM1, PWM_CH0);
    u16HighPeriod[1]=CalPeriodTime(PWM1, PWM_CH1);
    u16HighPeriod[2]=CalPeriodTime(PWM1, PWM_CH2);
    u16HighPeriod[3]=CalPeriodTime(PWM1, PWM_CH3);		
    printf("High Period = %d, %d, %d, %d\n", u16HighPeriod[0], u16HighPeriod[1],u16HighPeriod[2],u16HighPeriod[3]);			
  }
}




