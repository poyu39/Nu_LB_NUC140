/**************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * $Revision: 2 $
 * $Date: 15/04/13 10:05a $
 * @brief    Implement CRC in CRC-8 mode and get the CRC checksum result.
 * @note
 * Copyright (C) 2011 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "NUC100Series.h"

#define PLL_CLOCK           50000000


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

    /* Configure CRC operation settings for CRC-8 CPU mode */
    CRC_Open(CRC_8, 0, 0x5A, CRC_CPU_WDATA_8);

    /* Start to execute CRC-8 CPU operation */
    for(i = 0; i < strlen((char *)acCRCSrcPattern); i++)
    {
        CRC_WRITE_DATA((p8SrcAddr[i] & 0xFF));
    }

    /* Get CRC-8 checksum value */
    u32CalChecksum = CRC_GetChecksum();
    printf("CRC checksum is 0x%X ... %s.\n", u32CalChecksum, (u32CalChecksum == u32TargetChecksum) ? "PASS" : "FAIL");

    /* Disable CRC function */
    CRC->CTL &= ~CRC_CTL_CRCCEN_Msk;

    while(1);
}

/*** (C) COPYRIGHT 2013 Nuvoton Technology Corp. ***/
