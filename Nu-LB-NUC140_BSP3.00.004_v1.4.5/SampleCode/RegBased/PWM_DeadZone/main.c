/**************************************************************************//**
 * @file     main.c
 * @version  V2.00
 * $Revision: 3 $
 * $Date: 16/07/22 9:54a $
 * @brief    Demonstrate how to use PWM Dead Zone function.
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
    static uint32_t cnt;
    static uint32_t out;

    // Channel 0 frequency is 100Hz, every 1 second enter this IRQ handler 100 times.
    if(++cnt == 100)
    {
        if(out)
            PWMA->POE |= 0xF;
        else
            PWMA->POE &= ~0xF;
        out ^= 1;
        cnt = 0;
    }
    // Clear channel 0 period interrupt flag
    PWMA->PIIR = PWM_PIIR_PWMIF0_Msk;
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

    /* Set GPA multi-function pins for PWMA Channel0~3 */
    SYS->GPA_MFP &= ~(SYS_GPA_MFP_PA12_Msk | SYS_GPA_MFP_PA13_Msk | SYS_GPA_MFP_PA14_Msk | SYS_GPA_MFP_PA15_Msk);
    SYS->GPA_MFP |= SYS_GPA_MFP_PA12_PWM0 | SYS_GPA_MFP_PA13_PWM1 | SYS_GPA_MFP_PA14_PWM2 | SYS_GPA_MFP_PA15_PWM3;

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
    printf("  This sample code will output all PWMA channels with different\n");
    printf("  frequency and duty, enable dead zone function of all PWMA pairs.\n");
    printf("  And also enable/disable PWM output every 1 second.\n");
    printf("  I/O configuration:\n");
    printf("    waveform output pin: PWM0(PA.12), PWM1(PA.13), PWM2(PA.14), PWM3(PA.15)\n");

    // PWM0 frequency is 100Hz, duty 30%,
    /* Assume PWM output frequency is 100Hz and duty ratio is 30%, user can calculate PWM settings by follows.
       duty ratio = (CMR+1)/(CNR+1)
       cycle time = CNR+1
       High level = CMR+1
       PWM clock source frequency = __HXT = 12000000
       (CNR+1) = PWM clock source frequency/prescaler/clock source divider/PWM output frequency
               = 12000000/2/1/100 = 60000
       (Note: CNR is 16 bits, so if calculated value is larger than 65536, user should increase prescale value.)
       CNR = 59999
       duty ratio = 30% ==> (CMR+1)/(CNR+1) = 30% ==> CMR = (CNR+1)*0.3-1 = 60000*30/100-1
       CMR = 17999
       Prescale value is 1 : prescaler= 2
       Clock divider is PWM_CSR_DIV1 : clock divider =1
    */

    /*Set Pwm mode*/
    PWMA->PCR |= PWM_PCR_CH0MOD_Msk;

    /*Set PWM Timer clock prescaler*/
    PWM_SET_PRESCALER(PWMA, PWM_CH0, 1); // Divided by 2

    /*Set PWM Timer clock divider select*/
    PWM_SET_DIVIDER(PWMA, PWM_CH0, PWM_CLK_DIV_1);

    /*Set PWM Timer duty*/
    PWMA->CMR0 = 17999;

    /*Set PWM Timer period*/
    PWMA->CNR0 = 59999;

    /* enable and configure dead zone */
    PWMA->PPR = (PWMA->PPR & ~PWM_PPR_DZI01_Msk) | (400 << PWM_PPR_DZI01_Pos);
    PWMA->PCR |= PWM_PCR_DZEN01_Msk;

    // PWM2 frequency is 300Hz, duty 50%
    /* Assume PWM output frequency is 300Hz and duty ratio is 50%, user can calculate PWM settings by follows.
       duty ratio = (CMR+1)/(CNR+1)
       cycle time = CNR+1
       High level = CMR+1
       PWM clock source frequency = __HXT = 12000000
       (CNR+1) = PWM clock source frequency/prescaler/clock source divider/PWM output frequency
               = 12000000/2/1/300 = 20000
       (Note: CNR is 16 bits, so if calculated value is larger than 65536, user should increase prescale value.)
       CNR = 19999
       duty ratio = 50% ==> (CMR+1)/(CNR+1) = 50% ==> CMR = (CNR+1)*0.5-1 = 20000*50/100-1
       CMR = 9999
       Prescale value is 1 : prescaler= 2
       Clock divider is PWM_CSR_DIV1 : clock divider =1
    */

    /*Set Pwm mode*/
    PWMA->PCR |= PWM_PCR_CH2MOD_Msk;

    /*Set PWM Timer clock prescaler*/
    PWM_SET_PRESCALER(PWMA, PWM_CH2, 1); // Divided by 2

    /*Set PWM Timer clock divider select*/
    PWM_SET_DIVIDER(PWMA, PWM_CH2, PWM_CLK_DIV_1);

    /*Set PWM Timer duty*/
    PWMA->CMR2 = 9999;

    /*Set PWM Timer period*/
    PWMA->CNR2 = 19999;

    /* enable and configure dead zone */
    PWMA->PPR = (PWMA->PPR & ~PWM_PPR_DZI01_Msk) | (200 << PWM_PPR_DZI01_Pos);
    PWMA->PCR |= PWM_PCR_DZEN23_Msk;

    // Enable output of all PWMA channels
    PWMA->POE |= 0xF;

    // Enable PWMA channel 0 period interrupt, use channel 0 to measure time.
    PWMA->PIER = (PWMA->PIER & ~PWM_PIER_INT01TYPE_Msk) | PWM_PIER_PWMIE0_Msk;
    NVIC_EnableIRQ(PWMA_IRQn);

    // Start
    PWMA->PCR |= PWM_PCR_CH0EN_Msk | PWM_PCR_CH1EN_Msk | PWM_PCR_CH2EN_Msk | PWM_PCR_CH3EN_Msk;

    while(1);
}

/*** (C) COPYRIGHT 2014 Nuvoton Technology Corp. ***/
