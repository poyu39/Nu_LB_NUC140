/**************************************************************************//**
 * @file     main.c
 * @version  V2.00
 * $Revision: 1 $
 * $Date: 14/12/08 11:49a $
 * @brief    Show how to use the timer2 capture function to capture timer2 counter value.
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
extern int IsDebugFifoEmpty(void);
volatile uint8_t  g_u8IsWDTTimeoutINT = 0;
volatile uint32_t g_au32TMRINTCount[4] = {0};


/**
 * @brief       Timer0 IRQ
 *
 * @param       None
 *
 * @return      None
 *
 * @details     The Timer0 default IRQ, declared in startup_NUC100Series.s.
 */
void TMR0_IRQHandler(void)
{
    /* Clear Timer0 time-out interrupt flag */
    TIMER_ClearIntFlag(TIMER0);

    g_au32TMRINTCount[0]++;
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
    /* Clear Timer1 time-out interrupt flag */
    TIMER_GetIntFlag(TIMER1);

    g_au32TMRINTCount[1]++;
}

/**
 * @brief       Timer2 IRQ
 *
 * @param       None
 *
 * @return      None
 *
 * @details     The Timer2 default IRQ, declared in startup_NUC100Series.s.
 */
void TMR2_IRQHandler(void)
{
    if(TIMER_GetCaptureIntFlag(TIMER2) == 1)
    {
        /* Clear Timer2 capture interrupt flag */
        TIMER_ClearCaptureIntFlag(TIMER2);

        g_au32TMRINTCount[2]++;
    }
}

/**
 * @brief       Timer3 IRQ
 *
 * @param       None
 *
 * @return      None
 *
 * @details     The Timer3 default IRQ, declared in startup_NUC100Series.s.
 */
void TMR3_IRQHandler(void)
{
    /* Clear Timer3 time-out interrupt flag */
    TIMER_GetIntFlag(TIMER3);

    g_au32TMRINTCount[3]++;
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

    /* Enable external 12 MHz XTAL, IRC10K */
    CLK->PWRCON |= CLK_PWRCON_XTL12M_EN_Msk | CLK_PWRCON_OSC10K_EN_Msk;

    /* Enable PLL and Set PLL frequency */
    CLK->PLLCON = PLLCON_SETTING;

    /* Waiting for clock ready */
    while(!(CLK->CLKSTATUS & CLK_CLKSTATUS_PLL_STB_Msk));
    while(!(CLK->CLKSTATUS & CLK_CLKSTATUS_XTL12M_STB_Msk));
    while(!(CLK->CLKSTATUS & CLK_CLKSTATUS_IRC10K_STB_Msk));

    /* Switch HCLK clock source to PLL, STCLK to HCLK/2 */
    CLK->CLKSEL0 = CLK_CLKSEL0_STCLK_S_HCLK_DIV2 | CLK_CLKSEL0_HCLK_S_PLL;

    /* Enable peripheral clock */
    CLK->APBCLK = CLK_APBCLK_UART0_EN_Msk |
                  CLK_APBCLK_TMR0_EN_Msk | CLK_APBCLK_TMR2_EN_Msk | CLK_APBCLK_TMR3_EN_Msk;

    /* Peripheral clock source */
    CLK->CLKSEL1 = CLK_CLKSEL1_UART_S_PLL |
                   CLK_CLKSEL1_TMR0_S_HXT | CLK_CLKSEL1_TMR2_S_HCLK | CLK_CLKSEL1_TMR3_S_HXT;

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate PllClock, SystemCoreClock and CycylesPerUs automatically. */
    SystemCoreClockUpdate();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set PB multi-function pins for UART0 RXD, TXD, TM0, TM2, TM3 and TM2_EXT */
    SYS->GPB_MFP &= ~(SYS_GPB_MFP_PB0_Msk | SYS_GPB_MFP_PB1_Msk);
    SYS->GPB_MFP |= SYS_GPB_MFP_PB0_UART0_RXD | SYS_GPB_MFP_PB1_UART0_TXD;
    
    SYS->GPB_MFP &= ~(SYS_GPB_MFP_PB0_Msk | SYS_GPB_MFP_PB1_Msk |
                      SYS_GPB_MFP_PB8_Msk | SYS_GPB_MFP_PB10_Msk | SYS_GPB_MFP_PB11_Msk |
                      SYS_GPB_MFP_PB2_Msk);
    SYS->GPB_MFP |= SYS_GPB_MFP_PB0_UART0_RXD | SYS_GPB_MFP_PB1_UART0_TXD |
                    SYS_GPB_MFP_PB8_TM0 | SYS_GPB_MFP_PB10_TM2 | SYS_GPB_MFP_PB11_TM3 |
                    SYS_GPB_MFP_PB2_TM2_EXT;

    SYS->ALT_MFP  &= ~SYS_ALT_MFP_PB2_Msk;
    SYS->ALT_MFP  |= SYS_ALT_MFP_PB2_TM2_EXT;
    
    SYS->ALT_MFP2 = SYS_ALT_MFP2_PB2_TM2_EXT;
}

void UART0_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Reset IP */
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
    volatile uint32_t u32InitCount;
    uint32_t au32CAPValus[10];

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();

    /* Init UART0 for printf */
    UART0_Init();

    printf("\n\nCPU @ %d Hz\n", SystemCoreClock);
    printf("+---------------------------------------------------+\n");
    printf("|    Timer External Capture Function Sample Code    |\n");
    printf("+---------------------------------------------------+\n\n");

    printf("# Timer Settings:\n");
    printf("  Timer0: Clock source is 12 MHz; Toggle-output mode and frequency is 500 Hz.\n");
    printf("  Timer3: Clock source is 12 MHz; Toggle-output mode and frequency is 1 Hz.\n");
    printf("  Timer2: Clock source is HCLK(50 MHz); Continuous counting mode; TCMP is 0xFFFFFF;\n");
    printf("          Counter pin enable; Capture pin and capture interrupt enable;\n");
    printf("# Generate 500 Hz frequency from TM0 and connect TM0 pin to Timer2 counter pin.\n");
    printf("# Generate 1 Hz frequency from TM3 and connect TM3 pin to TM2_EXT capture pin.\n");
    printf("# Get 500 event counts from Timer2 counter pin when each TM2_EXT pin interrupt occurred.\n\n");

    /* Start Timer0 counting and output T0 frequency to 500 Hz*/
    TIMER0->TCMPR = (__HXT / 1000);
    TIMER0->TCSR = TIMER_TCSR_CEN_Msk | TIMER_TOGGLE_MODE;

    /* Start Timer3 counting and output T3 frequency to 1 Hz */
    TIMER3->TCMPR = __HXT / 2;
    TIMER3->TCSR = TIMER_TCSR_CEN_Msk | TIMER_TOGGLE_MODE;

    /* Enable Timer2 NVIC */
    NVIC_EnableIRQ(TMR2_IRQn);

    /* Clear Timer2 interrupt counts to 0 */
    u32InitCount = g_au32TMRINTCount[2] = 0;

    /* Enable Timer2 external counter input and capture function */
    TIMER2->TCMPR = 0xFFFFFF;
    TIMER2->TCSR = TIMER_TCSR_CEN_Msk | TIMER_TCSR_IE_Msk | TIMER_TCSR_CTB_Msk | TIMER_CONTINUOUS_MODE;
    TIMER2->TEXCON = TIMER_TEXCON_TEXEN_Msk | TIMER_CAPTURE_FREE_COUNTING_MODE | TIMER_CAPTURE_FALLING_EDGE | TIMER_TEXCON_TEXIEN_Msk;

    /* Check TM2_EXT interrupt counts */
    while(g_au32TMRINTCount[2] <= 10)
    {
        if(g_au32TMRINTCount[2] != u32InitCount)
        {
            au32CAPValus[u32InitCount] = TIMER_GetCaptureData(TIMER2);
            printf("[%2d] - %4d\n", g_au32TMRINTCount[2], au32CAPValus[u32InitCount]);
            if(u32InitCount > 1)
            {
                if((au32CAPValus[u32InitCount] - au32CAPValus[u32InitCount - 1]) != 500)
                {
                    printf("*** FAIL ***\n");
                    while(1);
                }
            }
            u32InitCount = g_au32TMRINTCount[2];
        }
    }

    /* Stop Timer0, Timer2 and Timer3 counting */
    TIMER0->TCSR = 0;
    TIMER2->TCSR = 0;
    TIMER3->TCSR = 0;

    printf("*** PASS ***\n");

    while(1);
}

/*** (C) COPYRIGHT 2013 Nuvoton Technology Corp. ***/
