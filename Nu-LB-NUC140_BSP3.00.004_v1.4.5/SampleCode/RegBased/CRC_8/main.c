/**************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * $Revision: 1 $
 * $Date: 14/12/08 11:48a $
 * @brief    Implement CRC in CRC-8 mode and get the CRC checksum result.
 * @note
 * Copyright (C) 2011 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "NUC100Series.h"

#define PLLCON_SETTING      CLK_PLLCON_50MHz_HXT
#define PLL_CLOCK           50000000


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
    uint32_t i, u32TargetChecksum = 0x58, u32CalChecksum = 0;
    uint8_t *p8SrcAddr;

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();

    /* Init UART0 for printf */
    UART0_Init();

    printf("\n\nCPU @ %dHz\n", SystemCoreClock);
    printf("+---------------------------------+\n");
    printf("|    CRC CRC-8 Mode Sample Code   |\n");
    printf("+---------------------------------+\n\n");

    printf("# Calculate string \"123456789\" CRC-8 checksum value by CRC CPU mode.\n");
    printf("    - Seed value is 0x5A            \n");
    printf("    - CPU Write Length is 8-bit     \n");
    printf("    - Checksum Complement disable   \n");
    printf("    - Checksum Reverse disable      \n");
    printf("    - Write Data Complement disable \n");
    printf("    - Write Data Reverse disable    \n");
    printf("    - Checksum should be 0x%X       \n\n", u32TargetChecksum);

    /* Set CRC source buffer address for CRC-8 CPU mode */
    p8SrcAddr = (uint8_t *)acCRCSrcPattern;

    /* Enable CRC channel clock */
    PDMA_GCR->GCRCSR |= PDMA_GCRCSR_CRC_CLK_EN_Msk;

    /* Disable CRC function */
    CRC->CTL &= ~CRC_CTL_CRCCEN_Msk;

    /* Configure CRC operation settings for CRC-8 CPU mode */
    CRC_SET_SEED(0x5A);
    CRC->CTL = CRC_8 | CRC_CPU_WDATA_8 | CRC_CTL_CRCCEN_Msk;

    /* Reset CRC peripheral */
    CRC->CTL |= CRC_CTL_CRC_RST_Msk;

    /* Start to execute CRC-8 CPU operation */
    for(i = 0; i < strlen((char *)acCRCSrcPattern); i++)
    {
        CRC_WRITE_DATA((p8SrcAddr[i] & 0xFF));
    }

    /* Disable CRC function */
    CRC->CTL &= ~CRC_CTL_CRCCEN_Msk;

    /* Get CRC-8 checksum value */
    u32CalChecksum = CRC->CHECKSUM;
    u32CalChecksum &= 0xFF;
    printf("CRC checksum is 0x%X ... %s.\n", u32CalChecksum, (u32CalChecksum == u32TargetChecksum) ? "PASS" : "FAIL");

    while(1);
}

/*** (C) COPYRIGHT 2013 Nuvoton Technology Corp. ***/
