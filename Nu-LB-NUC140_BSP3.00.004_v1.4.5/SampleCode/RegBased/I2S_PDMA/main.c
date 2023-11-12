/**************************************************************************//**
 * @file     main.c
 * @version  V2.00
 * $Revision: 1 $
 * $Date: 15/04/21 2:27p $
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

#define PLLCON_SETTING      CLK_PLLCON_50MHz_HXT

#define TEST_COUNT 32

/* Function prototype declaration */
void SYS_Init(void);
void UART_Init(void);

/* Global variable declaration */
uint32_t g_au32TxBuffer[TEST_COUNT];
uint32_t g_au32RxBuffer[TEST_COUNT];

/*---------------------------------------------------------------------------------------------------------*/
/*  Main Function                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main(void)
{
    uint32_t u32DataCount;

    /* Unlock protected registers */
    SYS_UnlockReg();
    
    /* Init system, IP clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();
    
    /* Init UART for print message */
    UART_Init();

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
    CLK->CLKSEL2 = (CLK->CLKSEL2 & (~CLK_CLKSEL2_I2S_S_Msk)) | CLK_CLKSEL2_I2S_S_HXT;

    /* Master mode, 16-bit word width, stereo mode, I2S format. */
    I2S->CON = I2S_MODE_MASTER | I2S_DATABIT_16 | I2S_STEREO | I2S_FORMAT_I2S;
    /* Sampling rate 16000 Hz */
    I2S->CLKDIV = (I2S->CLKDIV & ~I2S_CLKDIV_BCLK_DIV_Msk) | (11 << I2S_CLKDIV_BCLK_DIV_Pos);
    /* Enable I2S */
    I2S->CON |= I2S_CON_I2SEN_Msk;

    /* Data initiation */
    g_au32TxBuffer[0] = 0x55005501;
    g_au32RxBuffer[0] = 0;
    for(u32DataCount = 1; u32DataCount < TEST_COUNT; u32DataCount++)
    {
        g_au32TxBuffer[u32DataCount] = g_au32TxBuffer[u32DataCount-1] + 0x00020002;
        g_au32RxBuffer[u32DataCount] = 0;
    }

    /* I2S PDMA TX channel configuration */
    /* Enable PDMA channel 3 clock */
    PDMA_GCR->GCRCSR |= PDMA_GCRCSR_CLK3_EN_Msk; 
    PDMA3->CSR = 
        PDMA_CSR_PDMACEN_Msk  |  /* PDMA channel enable */
        PDMA_SAR_INC  |          /* Increment source address */
        PDMA_DAR_FIX  |          /* Fixed destination address */
        PDMA_WIDTH_32 |          /* Transfer width 32 bits */
        (0x2 << PDMA_CSR_MODE_SEL_Pos); /* Memory-to-Peripheral mode */
    PDMA3->SAR = (uint32_t)g_au32TxBuffer;    /* Source address */
    PDMA3->DAR = (uint32_t)&I2S->TXFIFO;      /* Destination address */
    PDMA3->BCR = TEST_COUNT*4;                /* Transfer count */
    /* Service selection */
    PDMA_GCR->PDSSR2 = (PDMA_GCR->PDSSR2 & (~PDMA_PDSSR2_I2S_TXSEL_Msk)) | (3<<PDMA_PDSSR2_I2S_TXSEL_Pos);
    
    /* I2S PDMA RX channel configuration */
    /* Enable PDMA channel 2 clock */
    PDMA_GCR->GCRCSR |= PDMA_GCRCSR_CLK2_EN_Msk; 
    PDMA2->CSR = 
        PDMA_CSR_PDMACEN_Msk |       /* PDMA channel enable */
        PDMA_SAR_FIX  |              /* Fixed source address */
        PDMA_DAR_INC  |              /* Increment destination address */
        PDMA_WIDTH_32 |              /* Transfer width 32 bits */
        (0x1 << PDMA_CSR_MODE_SEL_Pos); /* Peripheral-to-Memory mode */
    PDMA2->SAR = (uint32_t)&I2S->RXFIFO;     /* Source address */
    PDMA2->DAR = (uint32_t)g_au32RxBuffer;   /* Destination address */
    PDMA2->BCR = TEST_COUNT*4;               /* Transfer count */
    /* Service selection */
    PDMA_GCR->PDSSR2 = (PDMA_GCR->PDSSR2 & (~PDMA_PDSSR2_I2S_RXSEL_Msk)) | (2<<PDMA_PDSSR2_I2S_RXSEL_Pos);

    /* Clear RX FIFO */
    I2S->CON |= I2S_CON_CLR_RXFIFO_Msk;
    /* Trigger PDMA */
    PDMA2->CSR |= PDMA_CSR_TRIG_EN_Msk;
    PDMA3->CSR |= PDMA_CSR_TRIG_EN_Msk;
    /* Enable RX DMA and TX DMA function */
    I2S->CON |= (I2S_CON_RXDMA_Msk | I2S_CON_TXDMA_Msk | I2S_CON_RXEN_Msk | I2S_CON_TXEN_Msk);
    
    /* Check I2S RX DMA transfer done interrupt flag */
    while((PDMA2->ISR & PDMA_ISR_BLKD_IF_Msk)==0);
    /* Clear the transfer done interrupt flag */
    PDMA2->ISR = PDMA_ISR_BLKD_IF_Msk;
    
    /* Check I2S TX DMA transfer done interrupt flag */
    while((PDMA3->ISR & PDMA_ISR_BLKD_IF_Msk)==0);
    /* Clear the transfer done interrupt flag */
    PDMA3->ISR = PDMA_ISR_BLKD_IF_Msk;

    /* Disable RX function and TX function */
    I2S->CON &= ~(I2S_CON_RXEN_Msk | I2S_CON_TXEN_Msk);
    
    /* Print the received data */
    for(u32DataCount = 0; u32DataCount < TEST_COUNT; u32DataCount++)
    {
        printf("%d:\t0x%X\n", u32DataCount, g_au32RxBuffer[u32DataCount]);
    }
    
    printf("\n\nExit I2S sample code.\n");
    
    /* Disable PDMA peripheral clock */
    CLK->AHBCLK &= ~CLK_AHBCLK_PDMA_EN_Msk;
    /* Disbale I2S peripheral clock */
    CLK->APBCLK &= ~CLK_APBCLK_I2S_EN_Msk;
    
    while(1);
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

    /* Enable external 12 MHz XTAL */
    CLK->PWRCON |= CLK_PWRCON_XTL12M_EN_Msk;

    /* Enable PLL and Set PLL frequency */
    CLK->PLLCON = PLLCON_SETTING;

    /* Waiting for clock ready */
    while(!(CLK->CLKSTATUS & CLK_CLKSTATUS_PLL_STB_Msk));
    while(!(CLK->CLKSTATUS & CLK_CLKSTATUS_XTL12M_STB_Msk));

    /* Switch HCLK clock source to PLL, STCLK to HCLK/2 */
    CLK->CLKSEL0 = CLK_CLKSEL0_STCLK_S_HCLK_DIV2 | CLK_CLKSEL0_HCLK_S_PLL;

    /* Select HXT as the clock source of UART */
    CLK->CLKSEL1 = (CLK->CLKSEL1 & (~CLK_CLKSEL1_UART_S_Msk)) | CLK_CLKSEL1_UART_S_HXT;

    /* Enable peripheral clock */
    CLK->APBCLK = CLK_APBCLK_UART0_EN_Msk | CLK_APBCLK_I2S_EN_Msk;

    /* Enable PDMA peripheral clock */
    CLK->AHBCLK |= CLK_AHBCLK_PDMA_EN_Msk;

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate PllClock, SystemCoreClock and CyclesPerUs automatically. */
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

void UART_Init(void)
{
    /* Word length is 8 bits; 1 stop bit; no parity bit. */
    UART0->LCR = UART_LCR_WLS_Msk;
    /* Using mode 2 calculation: UART bit rate = UART peripheral clock rate / (BRD setting + 2) */
    /* UART peripheral clock rate 12 MHz; UART bit rate 115200 bps. */
    /* 12000000 / 115200 bps ~= 104 */
    /* 104 - 2 = 0x66. */
    UART0->BAUD = UART_BAUD_DIV_X_EN_Msk | UART_BAUD_DIV_X_ONE_Msk | (0x66);
}

/*** (C) COPYRIGHT 2014 Nuvoton Technology Corp. ***/
