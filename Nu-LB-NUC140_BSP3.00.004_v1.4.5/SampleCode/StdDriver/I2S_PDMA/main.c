/**************************************************************************//**
 * @file     main.c
 * @version  V2.00
 * $Revision: 1 $
 * $Date: 15/04/21 2:26p $
 * @brief
 *           Configure I2S as Master mode and demonstrate how I2S works with PDMA.
 *           Both TX PDMA function and RX PDMA function will be enabled.
 * @note
 * Copyright (C) 2014 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "NUC100Series.h"

#define I2S_RX_DMA_CH 2
#define I2S_TX_DMA_CH 3

#define TEST_COUNT 32

/* Function prototype declaration */
void SYS_Init(void);

/* Global variable declaration */
uint32_t g_au32TxBuffer[TEST_COUNT];
uint32_t g_au32RxBuffer[TEST_COUNT];

/*---------------------------------------------------------------------------------------------------------*/
/*  Main Function                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main(void)
{
    PDMA_T *pdma;
    uint32_t u32DataCount;

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init system, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();

    /* Init UART to 115200-8n1 for print message */
    UART_Open(UART0, 115200);

    printf("\n");
    printf("+----------------------------------------------+\n");
    printf("|            I2S + PDMA Sample Code            |\n");
    printf("+----------------------------------------------+\n");
    printf("  I2S configuration:\n");
    printf("      Sample rate 16 kHz\n");
    printf("      Word width 16 bits\n");
    printf("      Stereo mode\n");
    printf("      I2S format\n");
    printf("      TX value: 0x55005501, 0x55025503, ... \n");
    printf("  The I/O connection for I2S:\n");
    printf("      I2S_LRCLK (PC.0)\n      I2S_BCLK (PC.1)\n");
    printf("      I2S_DI (PC.2)\n      I2S_DO (PC.3)\n\n");
    printf("      This sample code will transmit and receive %d data with PDMA transfer.\n", TEST_COUNT);
    printf("      After PDMA transfer is finished, the received value will be printed.\n");
    
    /* Select I2S clock source from HXT (12 MHz) */
    CLK_SetModuleClock(I2S_MODULE, CLK_CLKSEL2_I2S_S_HXT, 0);

    /* Enable I2S TX and RX functions */
    /* Master mode, 16-bit word width, stereo mode, I2S format. */
    I2S_Open(I2S, I2S_MODE_MASTER, 16000, I2S_DATABIT_16, I2S_STEREO, I2S_FORMAT_I2S);
    
    /* Data initiation */
    g_au32TxBuffer[0] = 0x55005501;
    g_au32RxBuffer[0] = 0;
    for(u32DataCount = 1; u32DataCount < TEST_COUNT; u32DataCount++)
    {
        g_au32TxBuffer[u32DataCount] = g_au32TxBuffer[u32DataCount-1] + 0x00020002;
        g_au32RxBuffer[u32DataCount] = 0;
    }

    /* Enable PDMA channels */
    PDMA_Open((1 << I2S_TX_DMA_CH) | (1 << I2S_RX_DMA_CH));

    /* I2S PDMA TX channel configuration */
    /* Set transfer width (32 bits) and transfer count */
    PDMA_SetTransferCnt(I2S_TX_DMA_CH, PDMA_WIDTH_32, TEST_COUNT);
    /* Set source/destination address and attributes */
    PDMA_SetTransferAddr(I2S_TX_DMA_CH, (uint32_t)g_au32TxBuffer, PDMA_SAR_INC, (uint32_t)&I2S->TXFIFO, PDMA_DAR_FIX);
    /* Set request source. */
    PDMA_SetTransferMode(I2S_TX_DMA_CH, PDMA_I2S_TX, FALSE, 0);
    /* Set Memory-to-Peripheral mode */
    pdma = (PDMA_T *)((uint32_t) PDMA0_BASE + (0x100 * I2S_TX_DMA_CH));
    pdma->CSR = (pdma->CSR & (~PDMA_CSR_MODE_SEL_Msk)) | (0x2<<PDMA_CSR_MODE_SEL_Pos);
    
    /* I2S PDMA RX channel configuration */
    /* Set transfer width (32 bits) and transfer count */
    PDMA_SetTransferCnt(I2S_RX_DMA_CH, PDMA_WIDTH_32, TEST_COUNT);
    /* Set source/destination address and attributes */
    PDMA_SetTransferAddr(I2S_RX_DMA_CH, (uint32_t)&I2S->RXFIFO, PDMA_SAR_FIX, (uint32_t)g_au32RxBuffer, PDMA_DAR_INC);
    /* Set request source. */
    PDMA_SetTransferMode(I2S_RX_DMA_CH, PDMA_I2S_RX, FALSE, 0);
    /* Set Peripheral-to-Memory mode */
    pdma = (PDMA_T *)((uint32_t) PDMA0_BASE + (0x100 * I2S_RX_DMA_CH));
    pdma->CSR = (pdma->CSR & (~PDMA_CSR_MODE_SEL_Msk)) | (0x1<<PDMA_CSR_MODE_SEL_Pos);
    
    /* Clear RX FIFO */
    I2S_CLR_RX_FIFO(I2S);
    /* Trigger PDMA */
    PDMA_Trigger(I2S_TX_DMA_CH);
    PDMA_Trigger(I2S_RX_DMA_CH);
    /* Enable RX DMA and TX DMA function */
    I2S_ENABLE_TXDMA(I2S);
    I2S_ENABLE_RXDMA(I2S);
    
    /* Check I2S RX DMA transfer done interrupt flag */
    while((PDMA_GET_CH_INT_STS(I2S_RX_DMA_CH) & PDMA_ISR_BLKD_IF_Msk)==0);
    /* Clear the transfer done interrupt flag */
    PDMA_CLR_CH_INT_FLAG(I2S_RX_DMA_CH, PDMA_ISR_BLKD_IF_Msk);
    /* Check I2S TX DMA transfer done interrupt flag */
    while((PDMA_GET_CH_INT_STS(I2S_TX_DMA_CH) & PDMA_ISR_BLKD_IF_Msk)==0);
    /* Clear the transfer done interrupt flag */
    PDMA_CLR_CH_INT_FLAG(I2S_TX_DMA_CH, PDMA_ISR_BLKD_IF_Msk);
    
    /* Disable RX function and TX function */
    I2S_DISABLE_RX(I2S);
    I2S_DISABLE_TX(I2S);
    
    /* Print the received data */
    for(u32DataCount = 0; u32DataCount < TEST_COUNT; u32DataCount++)
    {
        printf("%d:\t0x%X\n", u32DataCount, g_au32RxBuffer[u32DataCount]);
    }
    
    printf("\n\nExit I2S sample code.\n");
    
    /* Disable PDMA peripheral clock */
    CLK->AHBCLK &= ~CLK_AHBCLK_PDMA_EN_Msk;
    /* Close I2S */
    I2S_Close(I2S);
    
    while(1);
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

    /* Enable External XTAL (4~24 MHz) */
    CLK_EnableXtalRC(CLK_PWRCON_XTL12M_EN_Msk);

    /* Waiting for 12 MHz clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_XTL12M_STB_Msk);

    /* Switch HCLK clock source to HXT */
    CLK_SetHCLK(CLK_CLKSEL0_HCLK_S_HXT, CLK_CLKDIV_HCLK(1));

    /* Set PLL to power down mode and PLL_STB bit in CLKSTATUS register will be cleared by hardware.*/
    CLK->PLLCON |= CLK_PLLCON_PD_Msk;

    /* Set core clock as PLL_CLOCK from PLL */
    CLK_SetCoreClock(50000000);

    /* Select IP clock source */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART_S_HXT, CLK_CLKDIV_UART(1));

    /* Enable IP peripheral clock */
    CLK_EnableModuleClock(UART0_MODULE);
    CLK_EnableModuleClock(I2S_MODULE);
    /* Enable PDMA peripheral clock */
    CLK_EnableModuleClock(PDMA_MODULE);

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    SystemCoreClockUpdate();


    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set PB multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFP &= ~(SYS_GPB_MFP_PB0_Msk | SYS_GPB_MFP_PB1_Msk);
    SYS->GPB_MFP |= (SYS_GPB_MFP_PB0_UART0_RXD | SYS_GPB_MFP_PB1_UART0_TXD);

    /* Set GPA multi-function pins for I2S MCLK. */
    SYS->GPA_MFP &= ~SYS_GPA_MFP_PA15_Msk;
    SYS->GPA_MFP |= SYS_GPA_MFP_PA15_I2S_MCLK;
    /* Set multi function pin for I2S: GPC0, GPC1, GPC2, GPC3, GPA15 */
    SYS->GPC_MFP &= ~(SYS_GPC_MFP_PC0_Msk | SYS_GPC_MFP_PC1_Msk | SYS_GPC_MFP_PC2_Msk | SYS_GPC_MFP_PC3_Msk);
    SYS->GPC_MFP |= (SYS_GPC_MFP_PC0_I2S_LRCLK | SYS_GPC_MFP_PC1_I2S_BCLK | SYS_GPC_MFP_PC2_I2S_DI | SYS_GPC_MFP_PC3_I2S_DO);
    SYS->ALT_MFP &= ~(SYS_ALT_MFP_PA15_Msk | SYS_ALT_MFP_PC0_Msk | SYS_ALT_MFP_PC1_Msk | SYS_ALT_MFP_PC2_Msk | SYS_ALT_MFP_PC3_Msk);
    SYS->ALT_MFP |= (SYS_ALT_MFP_PA15_I2S_MCLK | SYS_ALT_MFP_PC0_I2S_LRCLK | SYS_ALT_MFP_PC1_I2S_BCLK | SYS_ALT_MFP_PC2_I2S_DI | SYS_ALT_MFP_PC3_I2S_DO);

}

/*** (C) COPYRIGHT 2014 Nuvoton Technology Corp. ***/
