/**************************************************************************//**
 * @file     main.c
 * @version  V2.00
 * $Revision: 2 $
 * $Date: 15/04/13 10:18a $
 * @brief    Show how to reload the WWDT counter value.
 * @note
 * Copyright (C) 2013 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NUC100Series.h"


#define PLL_CLOCK           50000000


/*---------------------------------------------------------------------------------------------------------*/
/* Global Interface Variables Declarations                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
volatile uint32_t g_u32WWDTINTCount = 0;


/**
 * @brief       IRQ Handler for WDT and WWDT Interrupt
 *
 * @param       None
 *
 * @return      None
 *
 * @details     The WDT_IRQHandler is default IRQ of WDT and WWDT, declared in startup_NUC100Series.s.
 */
void WDT_IRQHandler(void)
{
    if(WWDT_GET_INT_FLAG() == 1)
    {
        /* Clear WWDT compare match interrupt flag */
        WWDT_CLEAR_INT_FLAG();

        g_u32WWDTINTCount++;

        if(g_u32WWDTINTCount < 10)
        {
            /* To reload the WWDT counter value to 0x3F */
            WWDT_RELOAD_COUNTER();
        }

        printf("WWDT compare match interrupt occurred. (%d)\n", g_u32WWDTINTCount);
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

    /* Enable external XTAL 12MHz clock */
    CLK_EnableXtalRC(CLK_PWRCON_XTL12M_EN_Msk);

    /* Waiting for external XTAL clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_XTL12M_STB_Msk);

    /* Set core clock as PLL_CLOCK from PLL */
    CLK_SetCoreClock(PLL_CLOCK);
    
    /* Enable UART module clock */
    CLK_EnableModuleClock(UART0_MODULE);    
    
    /* Select UART module clock source */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART_S_HXT, CLK_CLKDIV_UART(1));
    
    /* Enable WWDT module clock */
    CLK_EnableModuleClock(WWDT_MODULE);    
        
    /* Select WWDT module clock source */
    CLK_SetModuleClock(WWDT_MODULE, CLK_CLKSEL2_WWDT_S_HCLK_DIV2048, NULL);

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
    volatile uint32_t u32InitCount;
    double dPeriodTime;

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();

    /* Init UART0 for printf */
    UART0_Init();

    if(WWDT_GET_RESET_FLAG() == 1)
    {
        WWDT_CLEAR_RESET_FLAG();
        printf("\n\n*** WWDT time-out reset occurred. [WWDTCR: 0x%08X] ***\n", WWDT->WWDTCR);
        while(1);
    }

    printf("\n\nCPU @ %d Hz\n", SystemCoreClock);
    printf("+------------------------------------------------+\n");
    printf("|    WWDT Compare March Interrupt Sample Code    |\n");
    printf("+------------------------------------------------+\n\n");

    dPeriodTime = (((double)(1000000 * 2048) / (double)SystemCoreClock) * 1024) * 32;

    printf("# WWDT Settings: \n");
    printf("  Clock source is HCLK/2048(%d Hz); Enable interrupt; Window Compare value is 32;\n", SystemCoreClock / 2048);
    printf("  Select max WWDT time-out period is 1024 * (64 * WWDT_clock);\n");
    printf("# When WWDT start counting, system will generate first WWDT compare match interrupt after %.2f us.\n", dPeriodTime);
    printf("  1.) reload WWDT counter value to 0x3F when WWDT interrupt count less than 10 to avoid WWDT reset occurred.\n");
    printf("  2.) system will be reset by WWDT counting time-out while WWDT interrupt count reaches to 10.\n\n");

    u32InitCount = g_u32WWDTINTCount = 0;

    /* Enable WDT/WWDT NVIC */
    NVIC_EnableIRQ(WDT_IRQn);

    /* Configure WWDT compare value is 32; max time-out period is 1024*(64*WWDT_clk) and enable WWDT interrupt then start WWDT counting */
    /* Note: WWDTCR register can be written only once after chip is powered on or reset */
    WWDT_Open(WWDT_PRESCALER_1024, 32, TRUE);

    printf("[WWDTCR: 0x%08X]\n\n", WWDT->WWDTCR);

    while(1);
}

/*** (C) COPYRIGHT 2013 Nuvoton Technology Corp. ***/
