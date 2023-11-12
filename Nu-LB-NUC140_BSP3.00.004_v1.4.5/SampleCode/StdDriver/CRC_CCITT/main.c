/**************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * $Revision: 2 $
 * $Date: 15/04/13 10:08a $
 * @brief    Implement CRC in CRC-CCITT mode and get the CRC checksum result.
 * @note
 * Copyright (C) 2011 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "NUC100Series.h"

#define PLL_CLOCK           50000000


/*---------------------------------------------------------------------------------------------------------*/
/* Global Interface Variables Declarations                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
volatile uint8_t g_u8IsTargetAbortINTFlag = 0, g_u8IsBlockTransferDoneINTFlag = 0;


/**
 * @brief       PDMA Handler for PDMA and CRC Interrupt
 *
 * @param       None
 *
 * @return      None
 *
 * @details     The PDMA_IRQHandler is default IRQ of PDMA and CRC, declared in startup_NUC100Series.s.
 */
void PDMA_IRQHandler(void)
{
    volatile uint32_t u32IntStatus = CRC_GET_INT_FLAG();

    /* Check CRC interrupt status */
    if(u32IntStatus & CRC_DMAISR_CRC_BLKD_IF_Msk)
    {
        /* Clear Block Transfer Done interrupt flag */
        CRC_CLR_INT_FLAG(CRC_DMAISR_CRC_BLKD_IF_Msk);

        g_u8IsBlockTransferDoneINTFlag++;
    }
    else if(u32IntStatus & CRC_DMAISR_CRC_TABORT_IF_Msk)
    {
        /* Clear Target Abort interrupt flag */
        CRC_CLR_INT_FLAG(CRC_DMAISR_CRC_TABORT_IF_Msk);

        g_u8IsTargetAbortINTFlag++;
    }
    else
    {
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

    /* Waiting for clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_XTL12M_STB_Msk);

    /* Set core clock as PLL_CLOCK from PLL */
    CLK_SetCoreClock(PLL_CLOCK);
    
    /* Enable UART module clock */
    CLK_EnableModuleClock(UART0_MODULE);    
    
    /* Select UART module clock source */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART_S_HXT, CLK_CLKDIV_UART(1));
    
    /* Enable PDMA module clock for CRC operation */
    CLK_EnableModuleClock(PDMA_MODULE);    

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set PB multi-function pins for UART0 RXD, TXD */
    SYS->GPB_MFP &= ~(SYS_GPB_MFP_PB0_Msk | SYS_GPB_MFP_PB1_Msk);
    SYS->GPB_MFP |= SYS_GPB_MFP_PB0_UART0_RXD | SYS_GPB_MFP_PB1_UART0_TXD;
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
    const uint8_t acCRCSrcPattern[] = "123456789";
    uint32_t u32TargetChecksum = 0x29B1, u32CalChecksum = 0;

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();

    /* Init UART0 for printf */
    UART0_Init();

    printf("\n\nCPU @ %dHz\n", SystemCoreClock);
    printf("+--------------------------------+\n");
    printf("|    CRC CCITT Mode Sample Code   |\n");
    printf("+--------------------------------+\n\n");

    printf("# Calculate string \"123456789\" CRC-CCITT checksum value by CRC DMA mode.\n");
    printf("    - Seed value is 0xFFFF          \n");
    printf("    - Checksum Complement disable   \n");
    printf("    - Checksum Reverse disable      \n");
    printf("    - Write Data Complement disable \n");
    printf("    - Write Data Reverse disable    \n");
    printf("    - Checksum should be 0x%X       \n\n", u32TargetChecksum);

    /* Clear all CRC interrupt source flag status to 0 */
    g_u8IsTargetAbortINTFlag = g_u8IsBlockTransferDoneINTFlag = 0;

    /* Enable CRC NVIC (the same channel as PDMA) */
    NVIC_EnableIRQ(PDMA_IRQn);

    /* Configure CRC operation settings for CRC-CCITT DMA mode */
    CRC_Open(CRC_CCITT, 0, 0xFFFF, 0);

    /* Enable DMA Target Abort and Block Transfer Done interrupt function */
    CRC_ENABLE_INT(CRC_DMAIER_CRC_TABORT_IE_Msk | CRC_DMAIER_CRC_BLKD_IE_Msk);

    /* Trigger CRC DMA transfer */
    CRC_StartDMATransfer((uint32_t)acCRCSrcPattern, strlen((char *)acCRCSrcPattern));

    /* Wait CRC interrupt flag occurred */
    while(1)
    {
        if(g_u8IsTargetAbortINTFlag == 1)
        {
            printf("DMA Target Abort interrupt occurred.\n");
            break;
        }
        if(g_u8IsBlockTransferDoneINTFlag == 1)
        {
            break;
        }
    }

    /* Get CRC-CCITT checksum value */
    u32CalChecksum = CRC_GetChecksum();
    if(g_u8IsBlockTransferDoneINTFlag == 1)
    {
        printf("CRC checksum is 0x%X ... %s.\n", u32CalChecksum, (u32CalChecksum == u32TargetChecksum) ? "PASS" : "FAIL");
    }
    else
    {
        printf("CRC fail.\n");
    }

    /* Disable CRC function */
    CRC->CTL &= ~CRC_CTL_CRCCEN_Msk;

    /* Disable CRC NVIC */
    NVIC_DisableIRQ(PDMA_IRQn);

    while(1);
}

/*** (C) COPYRIGHT 2013 Nuvoton Technology Corp. ***/
