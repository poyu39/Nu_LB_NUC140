
/******************************************************************************
 * @file     main.c
 * @version  V2.00
 * $Revision: 3 $
 * $Date: 15/04/15 5:29p $
 * @brief    Demonstrate how ACMP works with internal band-gap voltage.
 * @note
 * Copyright (C) 2014 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include "NUC100Series.h"

#define PLLCON_SETTING      CLK_PLLCON_50MHz_HXT
#define PLL_CLOCK           50000000


/* Function prototype declaration */
void SYS_Init(void);
void UART_Init(void);

int32_t main(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();

    /* Init UART for print message */
    UART_Init();

    printf("\n\n");
    printf("+---------------------------------------+\n");
    printf("|        NUC100 ACMP Sample Code        |\n");
    printf("+---------------------------------------+\n");

    printf("\nThis sample code demonstrates ACMP0 function. Using ACMP0_P (PC.6) as ACMP0\n");
    printf("positive input and using internal band-gap voltage as the negative input\n");
    printf("The compare result reflects on ACMP0_O (PB.12)\n");

    /* Configure ACMP0. Enable ACMP0, enable interrupt and select internal reference voltage as negative input. */
    ACMP->CMPCR[0] = ACMP_CMPCR_CMPEN_Msk | ACMP_CMPCR_CMPIE_Msk | ACMP_CMPCR_CMPCN_Msk;

    /* Enable ACMP interrupt */
    NVIC_EnableIRQ(ACMP_IRQn);

    while(1);

}

void ACMP_IRQHandler(void)
{
    static uint32_t u32Cnt = 0;

    /* Clear ACMP0 interrupt flag */
    ACMP->CMPSR = ACMP_CMPSR_CMPF0_Msk;
    /* Check Comparator 0 Output Status */
    if(ACMP->CMPSR & ACMP_CMPSR_CO0_Msk)
        printf("ACMP0_P voltage > Band-gap voltage (%d)\n", u32Cnt);
    else
        printf("ACMP0_P voltage <= Band-gap voltage (%d)\n", u32Cnt);

    u32Cnt++;
}


void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Enable Internal RC 22.1184 MHz clock. Some peripherals select internal RC oscillator as default clock source. */
    CLK->PWRCON |= CLK_PWRCON_OSC22M_EN_Msk;

    /* Waiting for Internal RC clock ready */
    while(!(CLK->CLKSTATUS & CLK_CLKSTATUS_OSC22M_STB_Msk));
    
    /* Enable external 12 MHz XTAL */
    CLK->PWRCON |= CLK_PWRCON_XTL12M_EN_Msk;

    /* Waiting for clock ready */
    while(!(CLK->CLKSTATUS & CLK_CLKSTATUS_XTL12M_STB_Msk));

    /* Configure PLL */
    CLK->PLLCON = PLLCON_SETTING;
    while(!(CLK->CLKSTATUS & CLK_CLKSTATUS_PLL_STB_Msk));
    /* Select PLL as the system clock source */
    CLK->CLKSEL0 &= (~CLK_CLKSEL0_HCLK_S_Msk);
    CLK->CLKSEL0 |= CLK_CLKSEL0_HCLK_S_PLL;
    
    /* Select HXT as the clock source of UART */
    CLK->CLKSEL1 &= (~CLK_CLKSEL1_UART_S_Msk);
    CLK->CLKSEL1 |= CLK_CLKSEL1_UART_S_HXT;

    /* Enable UART and ACMP clock */
    CLK->APBCLK = CLK_APBCLK_UART0_EN_Msk | CLK_APBCLK_ACMP_EN_Msk;

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock and CyclesPerUs automatically. */
    SystemCoreClockUpdate();


    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set PC.6 multi-function pin for ACMP0 positive input pin */
    SYS->GPC_MFP &= ~SYS_GPC_MFP_PC6_Msk;
    SYS->GPC_MFP |= SYS_GPC_MFP_PC6_ACMP0_P;
    SYS->ALT_MFP1 &= ~SYS_ALT_MFP1_PC6_Msk;
    SYS->ALT_MFP1 |= SYS_ALT_MFP1_PC6_ACMP0_P;

    /* Set PB multi-function pins for UART0 RXD, TXD and ACMP0 output */
    SYS->GPB_MFP &= ~(SYS_GPB_MFP_PB0_Msk | SYS_GPB_MFP_PB1_Msk | SYS_GPB_MFP_PB12_Msk);
    SYS->GPB_MFP |= SYS_GPB_MFP_PB0_UART0_RXD | SYS_GPB_MFP_PB1_UART0_TXD | SYS_GPB_MFP_PB12_ACMP0_O;
    SYS->ALT_MFP &= ~SYS_ALT_MFP_PB12_Msk;
    SYS->ALT_MFP |= SYS_ALT_MFP_PB12_ACMP0_O;

    /* Disable digital input path of analog pin ACMP0_P to prevent leakage */
    PC->OFFD |= (1 << 6) << GPIO_OFFD_OFFD_Pos;

}

void UART_Init(void)
{
    /* Word length is 8 bits; 1 stop bit; no parity bit. */
    UART0->LCR = UART_LCR_WLS_Msk;
    /* Using mode 2 calculation: UART bit rate = UART peripheral clock rate / (BRD setting + 2) */
    /* UART peripheral clock rate 12MHz; UART bit rate 115200 bps. */
    /* 12000000 / 115200 bps ~= 104 */
    /* 104 - 2 = 0x66. */
    UART0->BAUD = UART_BAUD_DIV_X_EN_Msk | UART_BAUD_DIV_X_ONE_Msk | (0x66);
}

/*** (C) COPYRIGHT 2014 Nuvoton Technology Corp. ***/


