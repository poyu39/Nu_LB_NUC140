/**************************************************************************//**
 * @file     main.c
 * @version  V2.00
 * $Revision: 1 $
 * $Date: 14/12/08 11:48a $
 * @brief    Get the current RTC data/time per tick.
 * @note
 * Copyright (C) 2013 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NUC100Series.h"

#define PLLCON_SETTING      CLK_PLLCON_50MHz_HXT
#define PLL_CLOCK           50000000


/*---------------------------------------------------------------------------------------------------------*/
/* Global Interface Variables Declarations                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
volatile uint32_t g_u32RTCTickINT;


/**
 * @brief       IRQ Handler for RTC Interrupt
 *
 * @param       None
 *
 * @return      None
 *
 * @details     The RTC_IRQHandler is default IRQ of RTC, declared in startup_NUC100Series.s.
 */
void RTC_IRQHandler(void)
{
    /* To check if RTC Tick interrupt occurred */
    if(RTC_GET_TICK_INT_FLAG() == 1)
    {
        /* Clear RTC tick interrupt flag */
        RTC_CLEAR_TICK_INT_FLAG();

        g_u32RTCTickINT++;

        PB8 ^= 1;
    }
}

void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Enable IRC22M clock */
    CLK->PWRCON |= CLK_PWRCON_IRC22M_EN_Msk;

    /* Waiting for IRC22M clock ready */
    while(!(CLK->CLKSTATUS & CLK_CLKSTATUS_IRC22M_STB_Msk));

    /* Switch HCLK clock source to HIRC */
    CLK->CLKSEL0 = CLK_CLKSEL0_HCLK_S_HIRC;

    /* Set PLL to Power-down mode and PLL_STB bit in CLKSTATUS register will be cleared by hardware.*/
    CLK->PLLCON |= CLK_PLLCON_PD_Msk;

    /* Enable external 12 MHz XTAL, 32 kHz XTAL */
    CLK->PWRCON |= CLK_PWRCON_XTL12M_EN_Msk | CLK_PWRCON_XTL32K_EN_Msk;

    /* Enable PLL and Set PLL frequency */
    CLK->PLLCON = PLLCON_SETTING;

    /* Waiting for clock ready */
    while(!(CLK->CLKSTATUS & CLK_CLKSTATUS_PLL_STB_Msk));
    while(!(CLK->CLKSTATUS & CLK_CLKSTATUS_XTL12M_STB_Msk));
    while(!(CLK->CLKSTATUS & CLK_CLKSTATUS_XTL32K_STB_Msk));

    /* Switch HCLK clock source to PLL, STCLK to HCLK/2 */
    CLK->CLKSEL0 = CLK_CLKSEL0_STCLK_S_HCLK_DIV2 | CLK_CLKSEL0_HCLK_S_PLL;

    /* Enable peripheral clock */
    CLK->APBCLK = CLK_APBCLK_UART0_EN_Msk | CLK_APBCLK_RTC_EN_Msk;

    /* Peripheral clock source */
    CLK->CLKSEL1 = CLK_CLKSEL1_UART_S_PLL;

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate PllClock, SystemCoreClock and CycylesPerUs automatically. */
    SystemCoreClockUpdate();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set PB multi-function pins for UART0 RXD, TXD */
    SYS->GPB_MFP &= ~(SYS_GPB_MFP_PB0_Msk | SYS_GPB_MFP_PB1_UART0_Msk);
    SYS->GPB_MFP |= SYS_GPB_MFP_PB0_UART0_RXD | SYS_GPB_MFP_PB1_UART0_TXD;
}

void UART0_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Reset UART */
    SYS->IPRSTC2 |=  SYS_IPRSTC2_UART0_RST_Msk;
    SYS->IPRSTC2 &= ~SYS_IPRSTC2_UART0_RST_Msk;

    /* Configure UART0 and set UART0 Baudrate */
    UART0->BAUD = UART_BAUD_MODE2 | UART_BAUD_MODE2_DIVIDER(PllClock, 115200);
    UART0->LCR = UART_WORD_LEN_8 | UART_PARITY_NONE | UART_STOP_BIT_1;
}

/*---------------------------------------------------------------------------------------------------------*/
/*  MAIN function                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
int main(void)
{
    uint32_t u32Sec, u32CurSec;;

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();

    /* Init UART0 for printf */
    UART0_Init();

    printf("\n\nCPU @ %dHz\n", SystemCoreClock);
    printf("+-----------------------------------------+\n");
    printf("|    RTC Date/Time and Tick Sample Code   |\n");
    printf("+-----------------------------------------+\n\n");

    /* Initial RTC and stay in normal state */
    if(RTC->INIR != 0x1)
    {
        RTC->INIR = RTC_INIT_KEY;
        while(RTC->INIR != 0x1);
    }

    /* Setting RTC current date/time */
    RTC_WaitAccessEnable();
    RTC->TSSR = RTC_CLOCK_24;
    RTC->DWR  = RTC_THURSDAY;
    RTC->CLR  = 0x00140206;         /* Date: 2014/02/06 */
    RTC->TLR  = 0x00153030;         /* Time: 15:30:30 */
    RTC->RIER = RTC_RIER_TIER_Msk;
    RTC->TTR  = RTC_TICK_1_4_SEC;   /* One RTC tick is 1/4 second */


    /* Enable RTC tick interrupt */
    NVIC_EnableIRQ(RTC_IRQn);

    printf("# Showing RTC Date/Time on UART-0.\n\n");
    printf("Date/Time is: (Use PB.8 to check tick period time is 1/4 second) \n");

    /* Use PB.8 to check tick period time */
    PB->PMD = (PB->PMD & ~0x00030000) | (0x00010000);

    u32Sec = 0;
    g_u32RTCTickINT = 0;
    while(1)
    {
        if(g_u32RTCTickINT == 4)
        {
            g_u32RTCTickINT = 0;

            /* Read current RTC date/time */
            printf("    20%02x/%02x/%02x %02x:%02x:%02x\r",
                   (RTC->CLR >> RTC_CLR_1YEAR_Pos) & 0xFF, (RTC->CLR >> RTC_CLR_1MON_Pos) & 0xFF, (RTC->CLR >> RTC_CLR_1DAY_Pos) & 0xFF,
                   (RTC->TLR >> RTC_TLR_1HR_Pos) & 0xFF, (RTC->TLR >> RTC_TLR_1MIN_Pos) & 0xFF, (RTC->TLR >> RTC_TLR_1SEC_Pos) & 0xFF);

            /* Check RTC tick period time is reasonable or not */
            u32CurSec = (((RTC->TLR & RTC_TLR_10SEC_Msk) >> RTC_TLR_10SEC_Pos) * 10) + (RTC->TLR & RTC_TLR_1SEC_Msk);
            if(u32Sec == u32CurSec)
            {
                printf("\nRTC tick period time is incorrect.\n");
                while(1);
            }

            u32Sec = u32CurSec;
        }
    }
}

/*** (C) COPYRIGHT 2013 Nuvoton Technology Corp. ***/
