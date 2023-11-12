/**************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * $Revision: 1 $
 * $Date: 14/12/08 11:48a $
 * @brief    Use PDMA channel 6 to transfer data from memory to memory.
 * @note
 * Copyright (C) 2014 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/
#include <stdio.h>
#include "NUC100Series.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Macro, type and constant definitions                                                                    */
/*---------------------------------------------------------------------------------------------------------*/

#define PLLCON_SETTING      CLK_PLLCON_50MHz_HXT
#define PLL_CLOCK           50000000

uint32_t PDMA_TEST_LENGTH = 64;
uint8_t SrcArray[256];
uint8_t DestArray[256];
uint32_t volatile u32IsTestOver = 0xFF;

/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/

/**
 * @brief       DMA IRQ
 *
 * @param       None
 *
 * @return      None
 *
 * @details     The DMA default IRQ, declared in startup_nuc200Series.s.
 */
void PDMA_IRQHandler(void)
{
    uint32_t status = PDMA_GET_INT_STATUS();

    if(status & 0x1)    /* CH0 */
    {
        if(PDMA_GET_CH_INT_STS(0) & 0x2)
            u32IsTestOver = 0;
        PDMA_CLR_CH_INT_FLAG(0, PDMA_ISR_BLKD_IF_Msk);
    }
    else if(status & 0x2)      /* CH1 */
    {
        if(PDMA_GET_CH_INT_STS(1) & 0x2)
            u32IsTestOver = 1;
        PDMA_CLR_CH_INT_FLAG(1, PDMA_ISR_BLKD_IF_Msk);
    }
    else if(status & 0x4)      /* CH2 */
    {
        if(PDMA_GET_CH_INT_STS(2) & 0x2)
            u32IsTestOver = 2;
        PDMA_CLR_CH_INT_FLAG(2, PDMA_ISR_BLKD_IF_Msk);
    }
    else if(status & 0x8)      /* CH3 */
    {
        if(PDMA_GET_CH_INT_STS(3) & 0x2)
            u32IsTestOver = 3;
        PDMA_CLR_CH_INT_FLAG(3, PDMA_ISR_BLKD_IF_Msk);
    }
    else if(status & 0x10)      /* CH4 */
    {
        if(PDMA_GET_CH_INT_STS(4) & 0x2)
            u32IsTestOver = 4;
        PDMA_CLR_CH_INT_FLAG(4, PDMA_ISR_BLKD_IF_Msk);
    }
    else if(status & 0x20)      /* CH5 */
    {
        if(PDMA_GET_CH_INT_STS(5) & 0x2)
            u32IsTestOver = 5;
        PDMA_CLR_CH_INT_FLAG(5, PDMA_ISR_BLKD_IF_Msk);
    }
    else if(status & 0x40)      /* CH6 */
    {
        if(PDMA_GET_CH_INT_STS(6) & 0x2)
            u32IsTestOver = 6;
        PDMA_CLR_CH_INT_FLAG(6, PDMA_ISR_BLKD_IF_Msk);
    }
    else if(status & 0x80)      /* CH7 */
    {
        if(PDMA_GET_CH_INT_STS(7) & 0x2)
            u32IsTestOver = 7;
        PDMA_CLR_CH_INT_FLAG(7, PDMA_ISR_BLKD_IF_Msk);
    }
    else if(status & 0x100)      /* CH8 */
    {
        if(PDMA_GET_CH_INT_STS(8) & 0x2)
            u32IsTestOver = 8;
        PDMA_CLR_CH_INT_FLAG(8, PDMA_ISR_BLKD_IF_Msk);
    }
    else
        printf("unknown interrupt !!\n");
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
    SYS->GPB_MFP &= ~(SYS_GPB_MFP_PB1_Msk | SYS_GPB_MFP_PB0_Msk);
    SYS->GPB_MFP |= SYS_GPB_MFP_PB1_UART0_TXD | SYS_GPB_MFP_PB0_UART0_RXD;

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
/*  Main Function                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main(void)
{
    /* Init System, IP clock and multi-function I/O
       In the end of SYS_Init() will issue SYS_LockReg()
       to lock protected register. If user want to write
       protected register, please issue SYS_UnlockReg()
       to unlock protected register if necessary */

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, IP clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();

    /* Init UART0 for printf */
    UART0_Init();

    printf("\n\nCPU @ %dHz <%d>\n", SystemCoreClock, 6);

    printf("+--------------------------------------+ \n");
    printf("|    NUC100 PDMA Driver Sample Code   | \n");
    printf("+--------------------------------------+ \n");

    /* Open Channel 6 */
    PDMA_GCR->GCRCSR |= (1 << 6 << 8);
    /* set transfer byte count(transfer width is 32) */
    PDMA6->BCR = (PDMA_TEST_LENGTH << 2);
    /* set source and destination address */
    PDMA6->SAR = (uint32_t)SrcArray;
    PDMA6->DAR = (uint32_t)DestArray;
    /* set transfer direction */
    PDMA6->CSR = (PDMA6->CSR & ~(PDMA_CSR_SAD_SEL_Msk | PDMA_CSR_DAD_SEL_Msk)) | (PDMA_SAR_INC | PDMA_DAR_INC);
    /* enable block transfer done interrupt */
    PDMA6->IER |= PDMA_IER_BLKD_IE_Msk;
    NVIC_EnableIRQ(PDMA_IRQn);
    u32IsTestOver = 0xFF;
    /* trigger transfer */
    PDMA6->CSR |= (PDMA_CSR_TRIG_EN_Msk | PDMA_CSR_PDMACEN_Msk);
    while(u32IsTestOver == 0xFF);

    if(u32IsTestOver == 6)
        printf("test done...\n");

    PDMA_GCR->GCRCSR = 0;
    while(1);
}




