/**************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * $Revision: 1 $
 * $Date: 14/12/08 11:48a $
 * @brief    Implement CRC in CRC-CCITT mode and get the CRC checksum result.
 * @note
 * Copyright (C) 2011 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "NUC100Series.h"

#define PLLCON_SETTING      CLK_PLLCON_50MHz_HXT
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

    /* Enable external 12 MHz XTAL */
    CLK->PWRCON |= CLK_PWRCON_XTL12M_EN_Msk;

    /* Enable PLL and Set PLL frequency */
    CLK->PLLCON = PLLCON_SETTING;

    /* Waiting for clock ready */
    while(!(CLK->CLKSTATUS & CLK_CLKSTATUS_PLL_STB_Msk));
    while(!(CLK->CLKSTATUS & CLK_CLKSTATUS_XTL12M_STB_Msk));

    /* Switch HCLK clock source to PLL, STCLK to HCLK/2 */
    CLK->CLKSEL0 = CLK_CLKSEL0_STCLK_S_HCLK_DIV2 | CLK_CLKSEL0_HCLK_S_PLL;

    /* Enable peripheral clock */
    CLK->AHBCLK |= CLK_AHBCLK_PDMA_EN_Msk;
    CLK->APBCLK = CLK_APBCLK_UART0_EN_Msk;

    /* Peripheral clock source */
    CLK->CLKSEL1 = CLK_CLKSEL1_UART_S_PLL;

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate PllClock, SystemCoreClock and CycylesPerUs automatically. */
    SystemCoreClockUpdate();

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

    /* Enable CRC channel clock */
    PDMA_GCR->GCRCSR |= PDMA_GCRCSR_CRC_CLK_EN_Msk;

    /* Enable CRC NVIC (the same channel as PDMA) */
    NVIC_EnableIRQ(PDMA_IRQn);

    /* Enable CRC channel clock */
    PDMA_GCR->GCRCSR |= PDMA_GCRCSR_CRC_CLK_EN_Msk;

    /* Disable CRC function */
    CRC->CTL &= ~CRC_CTL_CRCCEN_Msk;

    /* Configure CRC operation settings for CRC-CCITT DMA mode */
    CRC_SET_SEED(0xFFFF);
    CRC->DMASAR = (uint32_t)acCRCSrcPattern;
    CRC->DMABCR = strlen((char *)acCRCSrcPattern);
    CRC->CTL = CRC_CCITT | CRC_CTL_CRCCEN_Msk;

    /* Enable DMA Target Abort and Block Transfer Done interrupt function */
    CRC->DMAIER = CRC_DMAIER_CRC_BLKD_IE_Msk | CRC_DMAIER_CRC_TABORT_IE_Msk;

    /* Reset CRC peripheral */
    CRC->CTL |= CRC_CTL_CRC_RST_Msk;

    /* Trigger CRC-CCITT DMA transfer */
    CRC->CTL |= CRC_CTL_TRIG_EN_Msk;

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

    /* Disable CRC NVIC */
    NVIC_DisableIRQ(PDMA_IRQn);

    /* Disable CRC function */
    CRC->CTL &= ~CRC_CTL_CRCCEN_Msk;

    /* Get CRC-CCITT checksum value */
    u32CalChecksum = CRC->CHECKSUM;
    u32CalChecksum &= 0xFFFF;
    if(g_u8IsBlockTransferDoneINTFlag == 1)
    {
        printf("CRC checksum is 0x%X ... %s.\n", u32CalChecksum, (u32CalChecksum == u32TargetChecksum) ? "PASS" : "FAIL");
    }
    else
    {
        printf("CRC fail.\n");
    }

    while(1);
}

/*** (C) COPYRIGHT 2013 Nuvoton Technology Corp. ***/
