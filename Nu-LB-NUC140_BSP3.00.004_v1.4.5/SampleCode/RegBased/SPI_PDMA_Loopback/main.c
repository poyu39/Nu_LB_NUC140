/**************************************************************************//**
 * @file     main.c
 * @version  V2.0
 * $Revision: 1 $
 * $Date: 15/04/20 4:03p $
 * @brief
 *           Demonstrate SPI data transfer with PDMA.
 *           SPI0 will be configured as Master mode and SPI1 will be configured as Slave mode.
 *           Both TX PDMA function and RX PDMA function will be enabled.
 * @note
 * Copyright (C) 2014 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/
#include <stdio.h>
#include "NUC100Series.h"

#define PLLCON_SETTING      CLK_PLLCON_50MHz_HXT

#define TEST_COUNT 64

/* Function prototype declaration */
void SYS_Init(void);
void UART0_Init(void);
void SPI_Init(void);
void SpiLoopTest_WithPDMA(void);

/* Global variable declaration */
uint32_t g_au32MasterToSlaveTestPattern[TEST_COUNT];
uint32_t g_au32SlaveToMasterTestPattern[TEST_COUNT];
uint32_t g_au32MasterRxBuffer[TEST_COUNT];
uint32_t g_au32SlaveRxBuffer[TEST_COUNT];

/* ------------- */
/* Main function */
/* ------------- */
int main(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init system, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();

    /* Init UART0 for printf */
    UART0_Init();

    /* Init SPI */
    SPI_Init();

    printf("\n\n");
    printf("+--------------------------------------------------------------+\n");
    printf("|                  SPI + PDMA Sample Code                      |\n");
    printf("+--------------------------------------------------------------+\n");
    printf("\n");
    printf("Configure SPI0 as a master and SPI1 as a slave.\n");
    printf("Bit length of a transaction: 32\n");
    printf("The I/O connection for SPI0/SPI1 loopback:\n");
    printf("    SPI0_SS0  (PC.0) <--> SPI1_SS0(PC.8)\n    SPI0_CLK  (PC.1) <--> SPI1_CLK(PC.9)\n");
    printf("    SPI0_MISO0(PC.2) <--> SPI1_MISO0(PC.10)\n    SPI0_MOSI0(PC.3) <--> SPI1_MOSI0(PC.11)\n\n");
    printf("Please connect SPI0 with SPI1, and press any key to start transmission ...");
    getchar();
    printf("\n");

    SpiLoopTest_WithPDMA();

    printf("\n\nExit SPI driver sample code.\n");

    /* Disable SPI0 and SPI1 peripheral clock */
    CLK->APBCLK &= (~(CLK_APBCLK_SPI0_EN_Msk|CLK_APBCLK_SPI1_EN_Msk));
    while(1);
}

void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
    
    /* Enable Internal RC 22.1184 MHz clock. Some peripherals select internal RC oscillator as default clock source. */
    CLK->PWRCON |= CLK_PWRCON_OSC22M_EN_Msk;

    /* Waiting for Internal RC clock ready */
    while(!(CLK->CLKSTATUS & CLK_CLKSTATUS_OSC22M_STB_Msk));
    
    /* Enable external 12 MHz XTAL */
    CLK->PWRCON |= CLK_PWRCON_XTL12M_EN_Msk;

    /* Waiting for clock ready */
    while(!(CLK->CLKSTATUS & CLK_CLKSTATUS_XTL12M_STB_Msk));

    /* Configure PLL */
    CLK->PLLCON = PLLCON_SETTING;
    while(!(CLK->CLKSTATUS & CLK_CLKSTATUS_PLL_STB_Msk));
    /* Select PLL as the system clock source */
    CLK->CLKSEL0 = (CLK->CLKSEL0 & (~CLK_CLKSEL0_HCLK_S_Msk)) | CLK_CLKSEL0_HCLK_S_PLL;
    
    /* Select HXT as the clock source of UART; select HCLK as the clock source of SPI0 and SPI1. */
    CLK->CLKSEL1 = (CLK->CLKSEL1 & (~(CLK_CLKSEL1_UART_S_Msk | CLK_CLKSEL1_SPI1_S_Msk | CLK_CLKSEL1_SPI0_S_Msk))) |
                   (CLK_CLKSEL1_UART_S_HXT | CLK_CLKSEL1_SPI1_S_HCLK | CLK_CLKSEL1_SPI0_S_HCLK);

    /* Enable UART, SPI0 and SPI1 peripheral clock */
    CLK->APBCLK = CLK_APBCLK_UART0_EN_Msk | CLK_APBCLK_SPI1_EN_Msk | CLK_APBCLK_SPI0_EN_Msk;

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

    /* Setup SPI0 and SPI1 multi-function pins */
    SYS->GPC_MFP &= ~(SYS_GPC_MFP_PC0_Msk | SYS_GPC_MFP_PC1_Msk | SYS_GPC_MFP_PC2_Msk | SYS_GPC_MFP_PC3_Msk |
                      SYS_GPC_MFP_PC8_Msk | SYS_GPC_MFP_PC9_Msk | SYS_GPC_MFP_PC10_Msk | SYS_GPC_MFP_PC11_Msk);
    SYS->GPC_MFP |= (SYS_GPC_MFP_PC0_SPI0_SS0 | SYS_GPC_MFP_PC1_SPI0_CLK | SYS_GPC_MFP_PC2_SPI0_MISO0 | SYS_GPC_MFP_PC3_SPI0_MOSI0 |
                     SYS_GPC_MFP_PC8_SPI1_SS0 | SYS_GPC_MFP_PC9_SPI1_CLK | SYS_GPC_MFP_PC10_SPI1_MISO0 | SYS_GPC_MFP_PC11_SPI1_MOSI0);
    SYS->ALT_MFP &= ~(SYS_ALT_MFP_PC0_Msk | SYS_ALT_MFP_PC1_Msk | SYS_ALT_MFP_PC2_Msk | SYS_ALT_MFP_PC3_Msk |
                      SYS_ALT_MFP_PC8_Msk);
    SYS->ALT_MFP |= (SYS_ALT_MFP_PC0_SPI0_SS0 | SYS_ALT_MFP_PC1_SPI0_CLK | SYS_ALT_MFP_PC2_SPI0_MISO0 | SYS_ALT_MFP_PC3_SPI0_MOSI0 |
                     SYS_ALT_MFP_PC8_SPI1_SS0 | SYS_ALT_MFP_PC9_SPI1_CLK | SYS_ALT_MFP_PC10_SPI1_MISO0 | SYS_ALT_MFP_PC11_SPI1_MOSI0);
}

void UART0_Init(void)
{
    /* Word length is 8 bits; 1 stop bit; no parity bit. */
    UART0->LCR = UART_LCR_WLS_Msk;
    /* Using mode 2 calculation: UART bit rate = UART peripheral clock rate / (BRD setting + 2) */
    /* UART peripheral clock rate 12 MHz; UART bit rate 115200 bps. */
    /* 12000000 / 115200 bps ~= 104 */
    /* 104 - 2 = 0x66. */
    UART0->BAUD = UART_BAUD_DIV_X_EN_Msk | UART_BAUD_DIV_X_ONE_Msk | (0x66);
}

void SPI_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init SPI                                                                                                */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Configure SPI0 */
    /* Configure SPI0 as a master, clock idle low, 32-bit transaction, drive output on falling clock edge and latch input on rising edge. */
    SPI0->CNTRL = SPI_MASTER | SPI_CNTRL_TX_NEG_Msk;
    /* Enable the automatic hardware slave select function. Select the SPI0_SS0 pin and configure as low-active. */
    SPI0->SSR = SPI_SSR_AUTOSS_Msk | SPI_SS0;
    /* Set IP clock divider. SPI clock rate = HCLK / ((24+1)*2) = 1 MHz */
    SPI0->DIVIDER = (SPI0->DIVIDER & (~SPI_DIVIDER_DIVIDER_Msk)) | 24;
    
    /* Configure SPI1 */
    /* Configure SPI1 as a slave, clock idle low, 32-bit transaction, drive output on falling clock edge and latch input on rising edge. */
    /* Eanble FIFO mode */
    SPI1->CNTRL = SPI_CNTRL_FIFO_Msk | SPI_CNTRL_SLAVE_Msk | SPI_CNTRL_TX_NEG_Msk;
    /* Configure SPI1 as a low level active device. */
    SPI1->SSR = SPI_SSR_SS_LTRIG_Msk;
    /* Set IP clock divider. SPI peripheral clock rate = HCLK / 2 = 25 MHz */
    SPI1->DIVIDER = 0;
}

void SpiLoopTest_WithPDMA(void)
{
    uint32_t u32DataCount;
    int32_t i32Err;


    printf("\nSPI0/1 loop test with PDMA ... ");

    /* Source data initiation */
    for(u32DataCount = 0; u32DataCount < TEST_COUNT; u32DataCount++)
    {
        g_au32MasterToSlaveTestPattern[u32DataCount] = 0x55000000 | (u32DataCount + 1);
        g_au32SlaveToMasterTestPattern[u32DataCount] = 0xAA000000 | (u32DataCount + 1);
    }

    /* SPI master PDMA TX channel configuration */
    /* Enable PDMA channel 3 clock */
    PDMA_GCR->GCRCSR |= PDMA_GCRCSR_CLK3_EN_Msk; 
    PDMA3->CSR = 
        PDMA_CSR_PDMACEN_Msk  |  /* PDMA channel enable */
        PDMA_SAR_INC  |          /* Increment source address */
        PDMA_DAR_FIX  |          /* Fixed destination address */
        PDMA_WIDTH_32 |          /* Transfer width 32 bits */
        (0x2 << PDMA_CSR_MODE_SEL_Pos); /* Memory-to-Peripheral mode */
    PDMA3->SAR = (uint32_t)g_au32MasterToSlaveTestPattern;    /* Source address */
    PDMA3->DAR = (uint32_t)&SPI0->TX;   /* Destination address */
    PDMA3->BCR = TEST_COUNT*4;          /* Transfer count */
    /* Service selection */
    PDMA_GCR->PDSSR0 = (PDMA_GCR->PDSSR0 & (~PDMA_PDSSR0_SPI0_TXSEL_Msk)) | (3<<PDMA_PDSSR0_SPI0_TXSEL_Pos);
    
    /* SPI master PDMA RX channel configuration */
    /* Enable PDMA channel 2 clock */
    PDMA_GCR->GCRCSR |= PDMA_GCRCSR_CLK2_EN_Msk; 
    PDMA2->CSR = 
        PDMA_CSR_PDMACEN_Msk |       /* PDMA channel enable */
        PDMA_SAR_FIX  |              /* Fixed source address */
        PDMA_DAR_INC  |              /* Increment destination address */
        PDMA_WIDTH_32 |              /* Transfer width 32 bits */
        (0x1 << PDMA_CSR_MODE_SEL_Pos); /* Peripheral-to-Memory mode */
    PDMA2->SAR = (uint32_t)&SPI0->RX;   /* Source address */
    PDMA2->DAR = (uint32_t)g_au32MasterRxBuffer;   /* Destination address */
    PDMA2->BCR = TEST_COUNT*4;          /* Transfer count */
    /* Service selection */
    PDMA_GCR->PDSSR0 = (PDMA_GCR->PDSSR0 & (~PDMA_PDSSR0_SPI0_RXSEL_Msk)) | (2<<PDMA_PDSSR0_SPI0_RXSEL_Pos);
    
    /* SPI slave PDMA RX channel configuration */
    /* Enable PDMA channel 1 clock */
    PDMA_GCR->GCRCSR |= PDMA_GCRCSR_CLK1_EN_Msk; 
    PDMA1->CSR = 
        PDMA_CSR_PDMACEN_Msk |       /* PDMA channel enable */
        PDMA_SAR_FIX  |              /* Fixed source address */
        PDMA_DAR_INC  |              /* Increment destination address */
        PDMA_WIDTH_32 |              /* Transfer width 32 bits */
        (0x1 << PDMA_CSR_MODE_SEL_Pos); /* Peripheral-to-Memory mode */
    PDMA1->SAR = (uint32_t)&SPI1->RX;   /* Source address */
    PDMA1->DAR = (uint32_t)g_au32SlaveRxBuffer;   /* Destination address */
    PDMA1->BCR = TEST_COUNT*4;          /* Transfer count */
    /* Service selection */
    PDMA_GCR->PDSSR0 = (PDMA_GCR->PDSSR0 & (~PDMA_PDSSR0_SPI1_RXSEL_Msk)) | (1<<PDMA_PDSSR0_SPI1_RXSEL_Pos);
    
    /* SPI slave PDMA TX channel configuration */
    /* Enable PDMA channel 0 clock */
    PDMA_GCR->GCRCSR |= PDMA_GCRCSR_CLK0_EN_Msk; 
    PDMA0->CSR = 
        PDMA_CSR_PDMACEN_Msk  |  /* PDMA channel enable */
        PDMA_SAR_INC  |          /* Increment source address */
        PDMA_DAR_FIX  |          /* Fixed destination address */
        PDMA_WIDTH_32 |          /* Transfer width 32 bits */
        (0x2 << PDMA_CSR_MODE_SEL_Pos); /* Memory-to-Peripheral mode */
    PDMA0->SAR = (uint32_t)g_au32SlaveToMasterTestPattern;    /* Source address */
    PDMA0->DAR = (uint32_t)&SPI1->TX;   /* Destination address */
    PDMA0->BCR = TEST_COUNT*4;          /* Transfer count */
    /* Service selection */
    PDMA_GCR->PDSSR0 = (PDMA_GCR->PDSSR0 & (~PDMA_PDSSR0_SPI1_TXSEL_Msk)) | (0<<PDMA_PDSSR0_SPI1_TXSEL_Pos);
    
    /* Trigger PDMA */
    PDMA0->CSR |= PDMA_CSR_TRIG_EN_Msk;
    PDMA1->CSR |= PDMA_CSR_TRIG_EN_Msk;
    PDMA2->CSR |= PDMA_CSR_TRIG_EN_Msk;
    PDMA3->CSR |= PDMA_CSR_TRIG_EN_Msk;
    SPI1->DMA |= (SPI_DMA_RX_DMA_GO_Msk | SPI_DMA_TX_DMA_GO_Msk);
    SPI0->DMA |= (SPI_DMA_RX_DMA_GO_Msk | SPI_DMA_TX_DMA_GO_Msk);
    
    /* Check Master RX DMA transfer done interrupt flag */
    while((PDMA2->ISR & PDMA_ISR_BLKD_IF_Msk)==0);
    /* Clear the transfer done interrupt flag */
    PDMA2->ISR = PDMA_ISR_BLKD_IF_Msk;
    
    /* Check Master TX DMA transfer done interrupt flag */
    while((PDMA3->ISR & PDMA_ISR_BLKD_IF_Msk)==0);
    /* Clear the transfer done interrupt flag */
    PDMA3->ISR = PDMA_ISR_BLKD_IF_Msk;
    
    /* Check Slave TX DMA transfer done interrupt flag */
    while((PDMA1->ISR & PDMA_ISR_BLKD_IF_Msk)==0);
    /* Clear the transfer done interrupt flag */
    PDMA1->ISR = PDMA_ISR_BLKD_IF_Msk;
    
    /* Check Slave RX DMA transfer done interrupt flag */
    while((PDMA0->ISR & PDMA_ISR_BLKD_IF_Msk)==0);
    /* Clear the transfer done interrupt flag */
    PDMA0->ISR = PDMA_ISR_BLKD_IF_Msk;
    
    i32Err = 0;
    /* Check the transfer data */
    for(u32DataCount=0; u32DataCount<TEST_COUNT; u32DataCount++)
    {
        if(g_au32MasterToSlaveTestPattern[u32DataCount] != g_au32SlaveRxBuffer[u32DataCount]){
            i32Err = 1;
            break;
        }
        if(g_au32SlaveToMasterTestPattern[u32DataCount] != g_au32MasterRxBuffer[u32DataCount]){
            i32Err = 1;
            break;
        }
    }
    
    /* Disable PDMA peripheral clock */
    CLK->AHBCLK &= ~CLK_AHBCLK_PDMA_EN_Msk;

    if(i32Err)
    {
        printf("[FAIL]\n");
    }
    else
    {
        printf("[PASS]\n");
    }

    return;
}


/*** (C) COPYRIGHT 2014 Nuvoton Technology Corp. ***/

