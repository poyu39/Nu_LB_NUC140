/**************************************************************************//**
 * @file     main.c
 * @version  V3.0
 * $Revision: 1 $
 * $Date: 15/04/20 4:04p $
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

#define PLL_CLOCK           50000000

#define TEST_COUNT 64

#define SPI_MASTER_TX_DMA_CH 3
#define SPI_MASTER_RX_DMA_CH 2
#define SPI_SLAVE_TX_DMA_CH  0
#define SPI_SLAVE_RX_DMA_CH  1

/* Global variable declaration */
uint32_t g_au32MasterToSlaveTestPattern[TEST_COUNT];
uint32_t g_au32SlaveToMasterTestPattern[TEST_COUNT];
uint32_t g_au32MasterRxBuffer[TEST_COUNT];
uint32_t g_au32SlaveRxBuffer[TEST_COUNT];

/* Function prototype declaration */
void SYS_Init(void);
void SPI_Init(void);
void SpiLoopTest_WithPDMA(void);

/* ------------- */
/* Main function */
/* ------------- */
int main(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();
    /* Init system, IP clock and multi-function I/O. */
    SYS_Init();
    /* Lock protected registers */
    SYS_LockReg();

    /* Configure UART0: 115200, 8-bit word, no parity bit, 1 stop bit. */
    UART_Open(UART0, 115200);

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

    /* Reset SPI0 */
    SPI_Close(SPI0);
    /* Reset SPI1 */
    SPI_Close(SPI1);
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
    
    /* Enable external 12 MHz XTAL */
    CLK_EnableXtalRC(CLK_PWRCON_XTL12M_EN_Msk);

    /* Waiting for clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_XTL12M_STB_Msk);

    /* Set core clock rate as PLL_CLOCK from PLL */
    CLK_SetCoreClock(PLL_CLOCK);

    /* Select HXT as the clock source of UART0 */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART_S_HXT, CLK_CLKDIV_UART(1));

    /* Select HCLK as the clock source of SPI0 */
    CLK_SetModuleClock(SPI0_MODULE, CLK_CLKSEL1_SPI0_S_HCLK, MODULE_NoMsk);

    /* Select HCLK as the clock source of SPI1 */
    CLK_SetModuleClock(SPI1_MODULE, CLK_CLKSEL1_SPI1_S_HCLK, MODULE_NoMsk);

    /* Enable UART peripheral clock */
    CLK_EnableModuleClock(UART0_MODULE);
    /* Enable SPI0 peripheral clock */
    CLK_EnableModuleClock(SPI0_MODULE);
    /* Enable SPI1 peripheral clock */
    CLK_EnableModuleClock(SPI1_MODULE);
    /* Enable PDMA peripheral clock */
    CLK_EnableModuleClock(PDMA_MODULE);

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

void SPI_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init SPI                                                                                                */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Configure SPI0 */
    /* Configure as a master, clock idle low, 32-bit transaction, drive output on falling clock edge and latch input on rising edge. */
    /* Set IP clock divider. SPI clock rate = 1 MHz */
    SPI_Open(SPI0, SPI_MASTER, SPI_MODE_0, 32, 1000000);
    /* Enable the automatic hardware slave select function. Select the SPI0_SS0 pin and configure as low-active. */
    SPI_EnableAutoSS(SPI0, SPI_SS0, SPI_SS_ACTIVE_LOW);
    
    /* Configure SPI1 */
    /* Configure SPI1 as a slave, clock idle low, 32-bit transaction, drive output on falling clock edge and latch input on rising edge. */
    /* Configure SPI1 as a low level active device. */
    SPI_Open(SPI1, SPI_SLAVE, SPI_MODE_0, 32, NULL);
    /* Enable FIFO mode */
    SPI1->CNTRL |= SPI_CNTRL_FIFO_Msk;
}

void SpiLoopTest_WithPDMA(void)
{
    PDMA_T *pdma;
    uint32_t u32DataCount;
    int32_t i32Err;


    printf("\nSPI0/1 loop test with PDMA ... ");

    /* Source data initiation */
    for(u32DataCount = 0; u32DataCount < TEST_COUNT; u32DataCount++)
    {
        g_au32MasterToSlaveTestPattern[u32DataCount] = 0x55000000 | (u32DataCount + 1);
        g_au32SlaveToMasterTestPattern[u32DataCount] = 0xAA000000 | (u32DataCount + 1);
    }

    /* Enable PDMA channels */
    PDMA_Open((1 << SPI_MASTER_TX_DMA_CH) | (1 << SPI_MASTER_RX_DMA_CH) | (1 << SPI_SLAVE_RX_DMA_CH) | (1 << SPI_SLAVE_TX_DMA_CH));

    /* SPI master PDMA TX channel configuration */
    /* Set transfer width (32 bits) and transfer count */
    PDMA_SetTransferCnt(SPI_MASTER_TX_DMA_CH, PDMA_WIDTH_32, TEST_COUNT);
    /* Set source/destination address and attributes */
    PDMA_SetTransferAddr(SPI_MASTER_TX_DMA_CH, (uint32_t)g_au32MasterToSlaveTestPattern, PDMA_SAR_INC, (uint32_t)&SPI0->TX, PDMA_DAR_FIX);
    /* Set request source. */
    PDMA_SetTransferMode(SPI_MASTER_TX_DMA_CH, PDMA_SPI0_TX, FALSE, 0);
    /* Set Memory-to-Peripheral mode */
    pdma = (PDMA_T *)((uint32_t) PDMA0_BASE + (0x100 * SPI_MASTER_TX_DMA_CH));
    pdma->CSR = (pdma->CSR & (~PDMA_CSR_MODE_SEL_Msk)) | (0x2<<PDMA_CSR_MODE_SEL_Pos);
    
    /* SPI master PDMA RX channel configuration */
    /* Set transfer width (32 bits) and transfer count */
    PDMA_SetTransferCnt(SPI_MASTER_RX_DMA_CH, PDMA_WIDTH_32, TEST_COUNT);
    /* Set source/destination address and attributes */
    PDMA_SetTransferAddr(SPI_MASTER_RX_DMA_CH, (uint32_t)&SPI0->RX, PDMA_SAR_FIX, (uint32_t)g_au32MasterRxBuffer, PDMA_DAR_INC);
    /* Set request source. */
    PDMA_SetTransferMode(SPI_MASTER_RX_DMA_CH, PDMA_SPI0_RX, FALSE, 0);
    /* Set Peripheral-to-Memory mode */
    pdma = (PDMA_T *)((uint32_t) PDMA0_BASE + (0x100 * SPI_MASTER_RX_DMA_CH));
    pdma->CSR = (pdma->CSR & (~PDMA_CSR_MODE_SEL_Msk)) | (0x1<<PDMA_CSR_MODE_SEL_Pos);
    
    /* SPI slave PDMA RX channel configuration */
    /* Set transfer width (32 bits) and transfer count */
    PDMA_SetTransferCnt(SPI_SLAVE_RX_DMA_CH, PDMA_WIDTH_32, TEST_COUNT);
    /* Set source/destination address and attributes */
    PDMA_SetTransferAddr(SPI_SLAVE_RX_DMA_CH, (uint32_t)&SPI1->RX, PDMA_SAR_FIX, (uint32_t)g_au32SlaveRxBuffer, PDMA_DAR_INC);
    /* Set request source. */
    PDMA_SetTransferMode(SPI_SLAVE_RX_DMA_CH, PDMA_SPI1_RX, FALSE, 0);
    /* Set Peripheral-to-Memory mode */
    pdma = (PDMA_T *)((uint32_t) PDMA0_BASE + (0x100 * SPI_SLAVE_RX_DMA_CH));
    pdma->CSR = (pdma->CSR & (~PDMA_CSR_MODE_SEL_Msk)) | (0x1<<PDMA_CSR_MODE_SEL_Pos);
    
    /* SPI slave PDMA TX channel configuration */
    /* Set transfer width (32 bits) and transfer count */
    PDMA_SetTransferCnt(SPI_SLAVE_TX_DMA_CH, PDMA_WIDTH_32, TEST_COUNT);
    /* Set source/destination address and attributes */
    PDMA_SetTransferAddr(SPI_SLAVE_TX_DMA_CH, (uint32_t)g_au32SlaveToMasterTestPattern, PDMA_SAR_INC, (uint32_t)&SPI1->TX, PDMA_DAR_FIX);
    /* Set request source. */
    PDMA_SetTransferMode(SPI_SLAVE_TX_DMA_CH, PDMA_SPI1_TX, FALSE, 0);
    /* Set Memory-to-Peripheral mode */
    pdma = (PDMA_T *)((uint32_t) PDMA0_BASE + (0x100 * SPI_SLAVE_TX_DMA_CH));
    pdma->CSR = (pdma->CSR & (~PDMA_CSR_MODE_SEL_Msk)) | (0x2<<PDMA_CSR_MODE_SEL_Pos);
    
    /* Trigger PDMA */
    PDMA_Trigger(SPI_SLAVE_RX_DMA_CH);
    PDMA_Trigger(SPI_SLAVE_TX_DMA_CH);
    PDMA_Trigger(SPI_MASTER_TX_DMA_CH);
    PDMA_Trigger(SPI_MASTER_RX_DMA_CH);
    
    /* Enable SPI slave DMA function */
    SPI_TRIGGER_RX_PDMA(SPI1);
    SPI_TRIGGER_TX_PDMA(SPI1);
    /* Enable SPI master DMA function */
    SPI_TRIGGER_TX_PDMA(SPI0);
    SPI_TRIGGER_RX_PDMA(SPI0);
    
    /* Check Master RX DMA transfer done interrupt flag */
    while((PDMA_GET_CH_INT_STS(SPI_MASTER_RX_DMA_CH) & PDMA_ISR_BLKD_IF_Msk)==0);
    /* Clear the transfer done interrupt flag */
    PDMA_CLR_CH_INT_FLAG(SPI_MASTER_RX_DMA_CH, PDMA_ISR_BLKD_IF_Msk);
    
    /* Check Master TX DMA transfer done interrupt flag */
    while((PDMA_GET_CH_INT_STS(SPI_MASTER_TX_DMA_CH) & PDMA_ISR_BLKD_IF_Msk)==0);
    /* Clear the transfer done interrupt flag */
    PDMA_CLR_CH_INT_FLAG(SPI_MASTER_TX_DMA_CH, PDMA_ISR_BLKD_IF_Msk);
    
    /* Check Slave TX DMA transfer done interrupt flag */
    while((PDMA_GET_CH_INT_STS(SPI_SLAVE_TX_DMA_CH) & PDMA_ISR_BLKD_IF_Msk)==0);
    /* Clear the transfer done interrupt flag */
    PDMA_CLR_CH_INT_FLAG(SPI_SLAVE_TX_DMA_CH, PDMA_ISR_BLKD_IF_Msk);
    
    /* Check Slave RX DMA transfer done interrupt flag */
    while((PDMA_GET_CH_INT_STS(SPI_SLAVE_RX_DMA_CH) & PDMA_ISR_BLKD_IF_Msk)==0);
    /* Clear the transfer done interrupt flag */
    PDMA_CLR_CH_INT_FLAG(SPI_SLAVE_RX_DMA_CH, PDMA_ISR_BLKD_IF_Msk);
    
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

