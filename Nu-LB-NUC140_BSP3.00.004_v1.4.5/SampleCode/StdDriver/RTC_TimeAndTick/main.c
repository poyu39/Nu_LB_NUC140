/**************************************************************************//**
 * @file     main.c
 * @version  V2.00
 * $Revision: 2 $
 * $Date: 15/04/13 10:09a $
 * @brief    Get the current RTC data/time per tick.
 * @note
 * Copyright (C) 2013 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NUC100Series.h"


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
    /* Enable Internal RC 22.1184MHz clock */
    CLK_EnableXtalRC(CLK_PWRCON_OSC22M_EN_Msk);

    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_OSC22M_STB_Msk);

    /* Switch HCLK clock source to Internal RC and HCLK source divide 1 */
    CLK_SetHCLK(CLK_CLKSEL0_HCLK_S_HIRC, CLK_CLKDIV_HCLK(1));

    /* Enable external XTAL 12MHz clock and 32 kHz XTAL */
    CLK_EnableXtalRC((CLK_PWRCON_XTL12M_EN_Msk | CLK_PWRCON_XTL32K_EN_Msk));

    /* Waiting for clock ready */
    CLK_WaitClockReady((CLK_CLKSTATUS_XTL12M_STB_Msk | CLK_CLKSTATUS_XTL32K_STB_Msk));

    /* Set core clock as PLL_CLOCK from PLL */
    CLK_SetCoreClock(PLL_CLOCK);
    
    /* Enable UART module clock */
    CLK_EnableModuleClock(UART0_MODULE);    
    
    /* Select UART module clock source */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART_S_HXT, CLK_CLKDIV_UART(1));
    
    /* Enable RTC module clock */
    CLK_EnableModuleClock(RTC_MODULE);    
        
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set PB multi-function pins for UART0 RXD, TXD */
    SYS->GPB_MFP &= ~(SYS_GPB_MFP_PB0_Msk | SYS_GPB_MFP_PB1_Msk);
    SYS->GPB_MFP |= (SYS_GPB_MFP_PB0_UART0_RXD | SYS_GPB_MFP_PB1_UART0_TXD);
}

void UART0_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Reset UART0 */
    SYS_ResetModule(UART0_RST);

    /* Configure UART0 and set UART0 Baudrate */
    UART_Open(UART0, 115200);
}

/*---------------------------------------------------------------------------------------------------------*/
/*  MAIN function                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
int main(void)
{
    S_RTC_TIME_DATA_T sWriteRTC, sReadRTC;
    uint32_t u32Sec;

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

    /* Open RTC and start counting */
    sWriteRTC.u32Year       = 2014;
    sWriteRTC.u32Month      = 2;
    sWriteRTC.u32Day        = 6;
    sWriteRTC.u32DayOfWeek  = RTC_THURSDAY;
    sWriteRTC.u32Hour       = 15;
    sWriteRTC.u32Minute     = 30;
    sWriteRTC.u32Second     = 30;
    sWriteRTC.u32TimeScale  = RTC_CLOCK_24;
    RTC_Open(&sWriteRTC);

    /* Enable RTC tick interrupt, one RTC tick is 1/4 second */
    NVIC_EnableIRQ(RTC_IRQn);
    RTC_EnableInt(RTC_RIER_TIER_Msk);
    RTC_SetTickPeriod(RTC_TICK_1_4_SEC);

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
            RTC_GetDateAndTime(&sReadRTC);
            printf("    %d/%02d/%02d %02d:%02d:%02d\r",
                   sReadRTC.u32Year, sReadRTC.u32Month, sReadRTC.u32Day, sReadRTC.u32Hour, sReadRTC.u32Minute, sReadRTC.u32Second);

            if(u32Sec == sReadRTC.u32Second)
            {
                printf("\nRTC tick period time is incorrect.\n");
                while(1);
            }

            u32Sec = sReadRTC.u32Second;
        }
    }
}

/*** (C) COPYRIGHT 2013 Nuvoton Technology Corp. ***/
