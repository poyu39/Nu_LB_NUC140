/**************************************************************************//**
 * @file     main.c
 * @version  V2.00
 * $Revision: 3 $
 * $Date: 15/09/14 4:14p $
 * @brief    Implement timer1 event counter function to count the external input event.
 * @note
 * Copyright (C) 2013 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NUC100Series.h"


#define PLL_CLOCK           50000000


/*---------------------------------------------------------------------------------------------------------*/
/* Global Interface Variables Declarations                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
volatile uint32_t g_au32TMRINTCount[4] = {0};


/*---------------------------------------------------------------------------------------------------------*/
/*  Create Counter Source by GPIOB                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
void GenerateGPIOBCounter(uint32_t u32Pin, uint32_t u32Counts)
{
    while(u32Counts--)
    {
        GPIO_PIN_DATA(1, u32Pin) = 1;
        GPIO_PIN_DATA(1, u32Pin) = 0;
    }
}

/**
 * @brief       Timer1 IRQ
 *
 * @param       None
 *
 * @return      None
 *
 * @details     The Timer1 default IRQ, declared in startup_NUC100Series.s.
 */
void TMR1_IRQHandler(void)
{
    if(TIMER_GetIntFlag(TIMER1) == 1)
    {
        /* Clear Timer1 time-out interrupt flag */
        TIMER_ClearIntFlag(TIMER1);

        g_au32TMRINTCount[1]++;
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

    /* Waiting for clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_XTL12M_STB_Msk);

    /* Set core clock as PLL_CLOCK from PLL */
    CLK_SetCoreClock(PLL_CLOCK);
    
    /* Enable UART module clock */
    CLK_EnableModuleClock(UART0_MODULE);    
    
    /* Select UART module clock source */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART_S_HXT, CLK_CLKDIV_UART(1));

    /* Enable Timer1 module clock */
    CLK_EnableModuleClock(TMR1_MODULE);    

    /* Select Timer1 module clock source */
    CLK_SetModuleClock(TMR1_MODULE, CLK_CLKSEL1_TMR1_S_HCLK, NULL);

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set PB multi-function pins for UART0 RXD, TXD and TM1 */
    SYS->GPB_MFP &= ~(SYS_GPB_MFP_PB0_Msk | SYS_GPB_MFP_PB1_Msk | SYS_GPB_MFP_PB9_Msk);
    SYS->GPB_MFP |= SYS_GPB_MFP_PB0_UART0_RXD | SYS_GPB_MFP_PB1_UART0_TXD | SYS_GPB_MFP_PB9_TM1;

    
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
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();

    /* Init UART0 for printf */
    UART0_Init();

    printf("\n\nCPU @ %d Hz\n", SystemCoreClock);
    printf("+-------------------------------------------------+\n");
    printf("|    Timer1 External Counter Input Sample Code    |\n");
    printf("+-------------------------------------------------+\n\n");

    printf("# Timer Settings:\n");
    printf("  Timer1: Clock source is HCLK(50 MHz); Continuous counting mode; Interrupt enable;\n");
    printf("          External counter input enable; TCMP is 56789.\n");
    printf("# Connect PB.8 to TM1 pin and pull PB.8 High/Low as TM1 counter input source.\n\n");

    /* Configure PB.8 as GPIO output pin and pull pin status to Low first */
    GPIO_SetMode(PB, BIT8, GPIO_PMD_OUTPUT);
    PB8 = 0;

    /* Initial Timer1 default setting */
    TIMER_Open(TIMER1, TIMER_CONTINUOUS_MODE, 1);

    /* Configure Timer1 setting for external counter input function */
    TIMER_SET_PRESCALE_VALUE(TIMER1, 0);
    TIMER_SET_CMP_VALUE(TIMER1, 56789);
    TIMER_EnableEventCounter(TIMER1, TIMER_COUNTER_FALLING_EDGE);
    TIMER_EnableInt(TIMER1);

    /* Enable Timer1 NVIC */
    NVIC_EnableIRQ(TMR1_IRQn);

    /* Clear Timer1 interrupt counts to 0 */
    g_au32TMRINTCount[1] = 0;

    /* Start Timer1 counting */
    TIMER_Start(TIMER1);

    /* To check if TDR of Timer1 must be 0 as default value */
    if(TIMER_GetCounter(TIMER1) != 0)
    {
        printf("Default counter value is not 0. (%d)\n", TIMER_GetCounter(TIMER1));

        /* Stop Timer1 counting */
        TIMER_Close(TIMER1);
        while(1);
    }

    /* To generate one counter event to T1 pin */
    GenerateGPIOBCounter(8, 1);

    /* To check if TDR of Timer1 must be 1 */
    while(TIMER_GetCounter(TIMER1) == 0);
    if(TIMER_GetCounter(TIMER1) != 1)
    {
        printf("Get unexpected counter value. (%d)\n", TIMER_GetCounter(TIMER1));

        /* Stop Timer1 counting */
        TIMER_Close(TIMER1);
        while(1);
    }

    /* To generate remains counts to T1 pin */
    GenerateGPIOBCounter(8, (56789 - 1));

    while(1)
    {
        if((g_au32TMRINTCount[1] == 1) && (TIMER_GetCounter(TIMER1) == 56789))
        {
            printf("Timer1 external counter input function ... PASS.\n");
            break;
        }
    }

    /* Stop Timer1 counting */
    TIMER_Close(TIMER1);

    while(1);
}

/*** (C) COPYRIGHT 2013 Nuvoton Technology Corp. ***/
