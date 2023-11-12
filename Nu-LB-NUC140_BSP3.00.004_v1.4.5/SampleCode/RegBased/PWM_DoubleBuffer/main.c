/**************************************************************************//**
 * @file     main.c
 * @version  V2.00
 * $Revision: 3 $
 * $Date: 16/07/22 9:55a $
 * @brief    Change duty cycle and period of output waveform by PWM Double Buffer function.
 * @note
 * Copyright (C) 2014 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NUC100Series.h"


#define PLLCON_SETTING  CLK_PLLCON_50MHz_HXT
#define PLL_CLOCK       50000000

/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/

/**
 * @brief       PWMA IRQ Handler
 *
 * @param       None
 *
 * @return      None
 *
 * @details     ISR to handle PWMA interrupt event
 */
void PWMA_IRQHandler(void)
{
    static int toggle = 0;

    // Update PWMA channel 0 period and duty
    if(toggle == 0)
    {
        PWMA->CNR0 = 99;
        PWMA->CMR0 = 39;
    }
    else
    {
        PWMA->CNR0 = 199;
        PWMA->CMR0 = 99;
    }
    toggle ^= 1;
    // Clear channel 0 period interrupt flag;
    PWMA->PIIR |= PWM_PIIR_PWMIF0_Msk;
}

void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Enable Internal RC 22.1184MHz clock */
    CLK->PWRCON |= CLK_PWRCON_OSC22M_EN_Msk;

    /* Waiting for Internal RC clock ready */
    while(!(CLK->CLKSTATUS & CLK_CLKSTATUS_OSC22M_STB_Msk));

    /* Switch HCLK clock source to Internal RC and HCLK source divide 1 */
    CLK->CLKSEL0 &= ~CLK_CLKSEL0_HCLK_S_Msk;
    CLK->CLKSEL0 |= CLK_CLKSEL0_HCLK_S_HIRC;
    CLK->CLKDIV &= ~CLK_CLKDIV_HCLK_N_Msk;
    CLK->CLKDIV |= CLK_CLKDIV_HCLK(1);

    /* Enable external XTAL 12MHz clock */
    CLK->PWRCON |= CLK_PWRCON_XTL12M_EN_Msk;

    /* Waiting for external XTAL clock ready */
    while(!(CLK->CLKSTATUS & CLK_CLKSTATUS_XTL12M_STB_Msk));

    /* Set core clock as PLL_CLOCK from PLL */
    CLK->PLLCON = PLLCON_SETTING;
    while(!(CLK->CLKSTATUS & CLK_CLKSTATUS_PLL_STB_Msk));
    CLK->CLKSEL0 &= (~CLK_CLKSEL0_HCLK_S_Msk);
    CLK->CLKSEL0 |= CLK_CLKSEL0_HCLK_S_PLL;

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate PllClock, SystemCoreClock and CycylesPerUs automatically. */
    //SystemCoreClockUpdate();
    PllClock        = PLL_CLOCK;            // PLL
    SystemCoreClock = PLL_CLOCK / 1;        // HCLK
    CyclesPerUs     = PLL_CLOCK / 1000000;  // For SYS_SysTickDelay()

    /* Enable UART module clock */
    CLK->APBCLK |= CLK_APBCLK_UART0_EN_Msk | CLK_APBCLK_PWM01_EN_Msk | CLK_APBCLK_PWM23_EN_Msk;

    /* Select UART module clock source */
    CLK->CLKSEL1 &= ~CLK_CLKSEL1_UART_S_Msk;
    CLK->CLKSEL1 |= CLK_CLKSEL1_UART_S_HXT;

    /* Select PWM clock source */
    CLK->CLKSEL1 = (CLK->CLKSEL1 & ~(CLK_CLKSEL1_PWM01_S_Msk | CLK_CLKSEL1_PWM23_S_Msk)) | \
                   (CLK_CLKSEL1_PWM01_S_HXT | CLK_CLKSEL1_PWM23_S_HXT);
    CLK->CLKSEL2 = (CLK->CLKSEL2 & ~(CLK_CLKSEL2_PWM01_S_E_Msk | CLK_CLKSEL2_PWM23_S_E_Msk)) | \
                   (CLK->CLKSEL2 |= CLK_CLKSEL2_PWM01_EXT_HXT | CLK_CLKSEL2_PWM23_EXT_HXT);
    
    /* User can select PWM module clock source from LIRC as below */
    //CLK->CLKSEL1 = (CLK->CLKSEL1 & ~(CLK_CLKSEL1_PWM01_S_Msk | CLK_CLKSEL1_PWM23_S_Msk)) | \
    //               (CLK_CLKSEL1_PWM01_S_LIRC | CLK_CLKSEL1_PWM23_S_LIRC);
    //CLK->CLKSEL2 = (CLK->CLKSEL2 & ~(CLK_CLKSEL2_PWM01_S_E_Msk | CLK_CLKSEL2_PWM23_S_E_Msk)) | \
    //               (CLK->CLKSEL2 |= CLK_CLKSEL2_PWM01_EXT_LIRC | CLK_CLKSEL2_PWM23_EXT_LIRC);

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Set GPB multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFP &= ~(SYS_GPB_MFP_PB0_Msk | SYS_GPB_MFP_PB1_Msk);
    SYS->GPB_MFP |= (SYS_GPB_MFP_PB0_UART0_RXD | SYS_GPB_MFP_PB1_UART0_TXD);

    /* Set GPA multi-function pins for PWMA Channel0 */
    SYS->GPA_MFP &= ~SYS_GPA_MFP_PA12_Msk;
    SYS->GPA_MFP |= SYS_GPA_MFP_PA12_PWM0;

}

void UART0_Init()
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Reset UART IP */
    SYS->IPRSTC2 |=  SYS_IPRSTC2_UART0_RST_Msk;
    SYS->IPRSTC2 &= ~SYS_IPRSTC2_UART0_RST_Msk;

    /* Configure UART0 and set UART0 Baudrate */
    UART0->BAUD = UART_BAUD_MODE2 | UART_BAUD_MODE2_DIVIDER(__HXT, 115200);
    UART0->LCR = UART_WORD_LEN_8 | UART_PARITY_NONE | UART_STOP_BIT_1;
}

/*---------------------------------------------------------------------------------------------------------*/
/* MAIN function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
int main(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();

    /* Init UART0 for printf */
    UART0_Init();

    printf("\nSystem clock rate: %d Hz\n", SystemCoreClock);

    printf("+------------------------------------------------------------------------+\n");
    printf("|                          PWM Driver Sample Code                        |\n");
    printf("|                                                                        |\n");
    printf("+------------------------------------------------------------------------+\n");
    printf("  This sample code will use PWMA channel 0 to output waveform\n");
    printf("  I/O configuration:\n");
    printf("    waveform output pin: PWM0(PA.12)\n");
    printf("\nUse double buffer feature.\n");

    /*
        PWMA channel 0 waveform of this sample shown below:

        |<-        CNR + 1  clk     ->|  CNR + 1 = 199 + 1 CLKs
                       |<-CMR+1 clk ->|  CMR + 1 = 99 + 1 CLKs
                                      |<-   CNR + 1  ->|  CNR + 1 = 99 + 1 CLKs
                                               |<CMR+1>|  CMR + 1 = 39 + 1 CLKs
      __                ______________          _______
        |_____ 100_____|     100      |___60___|  40   |_     PWM waveform

    */


    // Set channel 0 prescaler to 2. Actual value fill into register needs to minus 1.
    PWMA->PPR = (PWMB->PPR & ~(PWM_PPR_CP01_Msk)) | (1 << PWM_PPR_CP01_Pos);

    // Set channel 0 clock divider to 1
    PWMA->CSR = (PWMA->CSR & ~(PWM_CSR_CSR0_Msk)) | (PWM_CLK_DIV_1 << PWM_CSR_CSR0_Pos);

    // Enable PWMA channel 0 auto-reload mode
    PWMA->PCR |= PWM_PCR_CH0MOD_Msk;
    /*
      Configure PWMA channel 0 init period and duty.
      Period is __HXT / (prescaler * clock divider * (CNR + 1))
      Duty ratio = (CMR + 1) / (CNR + 1)
      Period = 12 MHz / (2 * 1 * (199 + 1)) =  30000 Hz
      Duty ratio = (99 + 1) / (199 + 1) = 50%
    */
    PWMA->CMR0 = 99;
    PWMA->CNR0 = 199;

    // Enable PWMA channel 0 output
    PWMA->POE |= PWM_POE_POE0_Msk;

    // Enable PWMA channel 0 period interrupt
    PWMA->PIER = PWM_PIER_PWMIE0_Msk;
    NVIC_EnableIRQ(PWMA_IRQn);

    // Start
    PWMA->PCR |= PWM_PCR_CH0EN_Msk;

    while(1);
}

/*** (C) COPYRIGHT 2014 Nuvoton Technology Corp. ***/
