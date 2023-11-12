/******************************************************************************
 * @file     main.c
 * @version  V2.00
 * $Revision: 3 $
 * $Date: 15/04/15 7:55p $
 * @brief    Demonstrate how ACMP works with internal band-gap voltage.
 * @note
 * Copyright (C) 2014 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include "NUC100Series.h"

#define PLL_CLOCK           50000000

/* Function prototype declaration */
void SYS_Init(void);

int32_t main(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init system, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();

    /* Configure UART0: 115200, 8-bit word, no parity bit, 1 stop bit. */
    UART_Open(UART0, 115200);

    printf("\n\n");
    printf("+---------------------------------------+\n");
    printf("|        NUC100 ACMP Sample Code        |\n");
    printf("+---------------------------------------+\n");

    printf("\nThis sample code demonstrates ACMP0 function. Using ACMP0_P (PC.6) as ACMP0\n");
    printf("positive input and using internal band-gap voltage as the negative input\n");
    printf("The compare result reflects on ACMP0_O (PB.12)\n");

    /* Configure ACMP0. Enable ACMP0 and select internal reference voltage as negative input. */
    ACMP_Open(ACMP, 0, ACMP_CR_VNEG_BANDGAP, ACMP_CR_HYSTERESIS_DISABLE);
    /* Enable ACMP interrupt function */
    ACMP_ENABLE_INT(ACMP, 0);

    /* Enable ACMP interrupt */
    NVIC_EnableIRQ(ACMP_IRQn);

    while(1);

}

void ACMP_IRQHandler(void)
{
    static uint32_t u32Cnt = 0;

    /* Clear ACMP0 interrupt flag */
    ACMP_CLR_INT_FLAG(ACMP, 0);
    /* Check Comparator 0 Output Status */
    if(ACMP_GET_OUTPUT(ACMP, 0))
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

    /* Enable Internal RC 22.1184 MHz clock */
    CLK_EnableXtalRC(CLK_PWRCON_OSC22M_EN_Msk);

    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_OSC22M_STB_Msk);

    /* Enable external 12 MHz XTAL */
    CLK_EnableXtalRC(CLK_PWRCON_XTL12M_EN_Msk);

    /* Waiting for clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_XTL12M_STB_Msk);

    /* Set core clock as PLL_CLOCK from PLL */
    CLK_SetCoreClock(PLL_CLOCK);
    
    /* Select HXT as the clock source of UART */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART_S_HXT, CLK_CLKDIV_UART(1));

    /* Enable UART peripheral clock */
    CLK_EnableModuleClock(UART0_MODULE);
    /* Enable ACMP01 peripheral clock */
    CLK_EnableModuleClock(ACMP_MODULE);

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock and CyclesPerUs automatically. */
    SystemCoreClockUpdate();


    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set PC.6 multi-function pin for ACMP0 positive input pin */
    SYS->GPC_MFP = SYS_GPC_MFP_PC6_ACMP0_P;
    SYS->ALT_MFP1 = SYS_ALT_MFP1_PC6_ACMP0_P;

    /* Disable digital input path of analog pin ACMP0_P to prevent leakage */
    GPIO_DISABLE_DIGITAL_PATH(PC, (1ul << 6));

    /* Set PB multi-function pins for UART0 RXD, TXD and ACMP0 output */
    SYS->GPB_MFP = SYS_GPB_MFP_PB0_UART0_RXD | SYS_GPB_MFP_PB1_UART0_TXD | SYS_GPB_MFP_PB12_ACMP0_O;
    SYS->ALT_MFP = SYS_ALT_MFP_PB12_ACMP0_O;
    SYS->ALT_MFP2 = SYS_ALT_MFP2_PB12_ACMP0_O;

}

/*** (C) COPYRIGHT 2014 Nuvoton Technology Corp. ***/


