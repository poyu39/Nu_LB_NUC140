/**************************************************************************//**
 * @file     main.c
 * @version  V2.00
 * $Revision: 1 $
 * $Date: 14/12/08 11:49a $
 * @brief    Show the usage of GPIO interrupt function.
 * @note
 * Copyright (C) 2013 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NUC100Series.h"


#define PLL_CLOCK   50000000


/**
 * @brief       GPIO PA/PB IRQ
 *
 * @param       None
 *
 * @return      None
 *
 * @details     The PA/PB default IRQ, declared in startup_NUC100Series.s.
 */
void GPAB_IRQHandler(void)
{
    /* To check if PB.3 interrupt occurred */

    if(GPIO_GET_INT_FLAG(PB, BIT3))
    {
        GPIO_CLR_INT_FLAG(PB, BIT3);
        printf("PB.3 INT occurred.\n");
    }
    else
    {
        /* Un-expected interrupt. Just clear all PA, PB interrupts */
        PA->ISRC = PA->ISRC;
        PB->ISRC = PB->ISRC;
        printf("Un-expected interrupts.\n");
    }
}

/**
 * @brief       GPIO PC/PD/PE/PF IRQ
 *
 * @param       None
 *
 * @return      None
 *
 * @details     The PC/PD/PE/PF default IRQ, declared in startup_NUC100Series.s.
 */
void GPCDEF_IRQHandler(void)
{
    /* To check if PE.5 interrupt occurred */
    if(GPIO_GET_INT_FLAG(PE, BIT5))
    {
        GPIO_CLR_INT_FLAG(PE, BIT5);
        printf("PE.5 INT occurred.\n");
    }
    else
    {
        /* Un-expected interrupt. Just clear all PC, PD, PE and PF interrupts */
        PC->ISRC = PC->ISRC;
        PD->ISRC = PD->ISRC;
        PE->ISRC = PE->ISRC;
        PF->ISRC = PF->ISRC;
        printf("Un-expected interrupts.\n");
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

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Set GPB multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFP &= ~(SYS_GPB_MFP_PB0_Msk | SYS_GPB_MFP_PB1_Msk);
    SYS->GPB_MFP |= (SYS_GPB_MFP_PB0_UART0_RXD | SYS_GPB_MFP_PB1_UART0_TXD);

}

void UART0_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Reset UART */
    SYS_ResetModule(UART0_RST);

    /* Configure UART0 and set UART0 Baudrate */
    UART_Open(UART0, 115200);
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

    printf("\n\nCPU @ %d Hz\n", SystemCoreClock);
    printf("+------------------------------------------------+\n");
    printf("|    GPIO PB.3 and PE.5 Interrupt Sample Code    |\n");
    printf("+------------------------------------------------+\n\n");

    /*-----------------------------------------------------------------------------------------------------*/
    /* GPIO Interrupt Function Test                                                                        */
    /*-----------------------------------------------------------------------------------------------------*/
    printf("PB.3 and PE.5 are used to test interrupt ......\n");

    /* Configure PB.3 as Input mode and enable interrupt by rising edge trigger */
    GPIO_SetMode(PB, BIT3, GPIO_PMD_INPUT);
    GPIO_EnableInt(PB, 3, GPIO_INT_RISING);
    NVIC_EnableIRQ(GPAB_IRQn);

    /*  Configure PE.5 as Quasi-bidirection mode and enable interrupt by falling edge trigger */
    GPIO_SetMode(PE, BIT5, GPIO_PMD_QUASI);
    GPIO_EnableInt(PE, 5, GPIO_INT_FALLING);
    NVIC_EnableIRQ(GPCDEF_IRQn);

    /* Waiting for interrupts */
    while(1);
}

/*** (C) COPYRIGHT 2013 Nuvoton Technology Corp. ***/
