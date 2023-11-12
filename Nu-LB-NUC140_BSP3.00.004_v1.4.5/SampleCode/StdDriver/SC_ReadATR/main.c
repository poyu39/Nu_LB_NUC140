/****************************************************************************
 * @file     main.c
 * @version  V2.0
 * $Revision: 2 $
 * $Date: 15/04/09 11:04a $
 * @brief    Read the smartcard ATR from smartcard 0 interface.
 * @note
 * Copyright (C) 2014 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/
#include <stdio.h>
#include "NUC100Series.h"
#include "sclib.h"

#define PLL_CLOCK       50000000

/*---------------------------------------------------------------------------------------------------------*/
/* The interrupt services routine of smartcard port 0                                                      */
/*---------------------------------------------------------------------------------------------------------*/
void SC012_IRQHandler(void)
{
    // Please don't remove any of the function calls below
    if(SCLIB_CheckCDEvent(0))
        return; // Card insert/remove event occurred, no need to check other event...
    SCLIB_CheckTimeOutEvent(0);
    SCLIB_CheckTxRxEvent(0);
    SCLIB_CheckErrorEvent(0);

    return;
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

    /* Enable SC0 module clock */
    CLK_EnableModuleClock(SC0_MODULE);

    /* Select UART module clock source */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART_S_PLL, CLK_CLKDIV_UART(1));

    /* Select SC0 module clock source */
    CLK_SetModuleClock(SC0_MODULE, CLK_CLKSEL3_SC0_S_HXT, CLK_CLKDIV1_SC0(3));

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Set GPB multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFP &= ~(SYS_GPB_MFP_PB0_Msk | SYS_GPB_MFP_PB1_Msk);
    SYS->GPB_MFP |= SYS_GPB_MFP_PB0_UART0_RXD | SYS_GPB_MFP_PB1_UART0_TXD;

    /* Set PA.0 ~ PA.3 and PC.6 for SC0 interface */
    SYS->GPA_MFP &= ~(SYS_GPA_MFP_PA0_Msk | SYS_GPA_MFP_PA1_Msk | SYS_GPA_MFP_PA2_Msk | SYS_GPA_MFP_PA3_Msk);
    SYS->GPC_MFP &= ~SYS_GPC_MFP_PC6_Msk;
    SYS->GPA_MFP |= SYS_GPA_MFP_PA0_SC0_PWR | SYS_GPA_MFP_PA1_SC0_RST | SYS_GPA_MFP_PA2_SC0_CLK | SYS_GPA_MFP_PA3_SC0_DAT;
    SYS->GPC_MFP |= SYS_GPC_MFP_PC6_SC0_CD;
    SYS->ALT_MFP1 &= ~(SYS_ALT_MFP1_PA0_Msk | SYS_ALT_MFP1_PA1_Msk | SYS_ALT_MFP1_PA2_Msk | SYS_ALT_MFP1_PA3_Msk);
    SYS->ALT_MFP1 &= ~SYS_ALT_MFP1_PC6_Msk;
    SYS->ALT_MFP1 |= SYS_ALT_MFP1_PA0_SC0_PWR | SYS_ALT_MFP1_PA1_SC0_RST | SYS_ALT_MFP1_PA2_SC0_CLK | SYS_ALT_MFP1_PA3_SC0_DAT;
    SYS->ALT_MFP1 |= SYS_ALT_MFP1_PC6_SC0_CD;
}

/*---------------------------------------------------------------------------------------------------------*/
/* MAIN function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/

int main(void)
{

    SCLIB_CARD_INFO_T s_info;
    int retval, i;

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, IP clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();

    /*---------------------------------------------------------------------------------------------------------*/
    /* SAMPLE CODE                                                                                             */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Init UART to 115200-8n1 for print message */
    UART_Open(UART0, 115200);

    printf("\n\nCPU @ %dHz\n", SystemCoreClock);
    printf("+------------------------------------------------------------------------+\n");
    printf("|                       Smart Card Driver Sample Code                    |\n");
    printf("+------------------------------------------------------------------------+\n");
    printf("  This sample code will get smart card ATR data via SC0 port.\n");
    printf("  I/O configuration:\n");
    printf("  SC0CLK <--> smart card slot clock pin\n");
    printf("  SC0DAT <--> smart card slot data pin\n");
    printf("  SC0PWR <--> smart card slot power pin\n");
    printf("  SC0RST <--> smart card slot reset pin\n");
    printf("  SC0CD  <--> smart card slot card detect pin\n");
    printf("\nThis sample code reads ATR from smartcard\n");

    // Open smartcard interface 0. CD pin state low indicates card insert and PWR pin low raise VCC pin to card
    SC_Open(SC0, SC_PIN_STATE_LOW, SC_PIN_STATE_HIGH);
    NVIC_EnableIRQ(SC012_IRQn);

    // Wait 'til card insert
    while(SC_IsCardInserted(SC0) == FALSE);
    // Activate slot 0
    retval = SCLIB_Activate(0, FALSE);

    if(retval == SCLIB_SUCCESS)
    {
        SCLIB_GetCardInfo(0, &s_info);
        printf("\nATR: ");
        for(i = 0; i < s_info.ATR_Len; i++)
            printf("%x ", s_info.ATR_Buf[i]);
        printf("\n");
    }
    else
        printf("Smartcard activate failed\n");

    while(1);
}
