
/******************************************************************************
 * @file     main.c
 * @version  V3.00
 * $Revision: 3 $
 * $Date: 15/04/15 6:02p $
 * @brief    Show how to wake up MCU from Power-down mode by ACMP wake-up function.
 * @note
 * Copyright (C) 2014 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include "NUC100Series.h"

#define PLLCON_SETTING      CLK_PLLCON_50MHz_HXT
#define PLL_CLOCK           50000000

/* Function prototype declaration */
void SYS_Init(void);
void UART_Init(void);
void PowerDownFunction(void);
int IsDebugFifoEmpty(void);

int32_t main(void)
{
    uint32_t u32DelayCount;

    /* Unlock protected registers */
    SYS_UnlockReg();
    /* Init System, IP clock and multi-function I/O. */
    SYS_Init();
    /* Lock protected registers */
    SYS_LockReg();

    /* Init UART for print message */
    UART_Init();

    printf("\n\n");
    printf("+-----------------------------------------+\n");
    printf("|         NUC100 ACMP Sample Code         |\n");
    printf("+-----------------------------------------+\n");

    printf("\nThis sample code demonstrates ACMP0 wake-up function. Using ACMP0_P (PC.6) as ACMP0\n");
    printf("positive input and using internal band-gap voltage as the negative input.\n");
    printf("The compare result reflects on ACMP0_O (PB.12).\n");

    printf("When the voltage of the positive input is greater than the voltage of the negative input,\n");
    printf("the analog comparator outputs logical one; otherwise, it outputs logical zero.\n");
    printf("This chip will be waked up from power down mode when detecting a transition of analog comparator's output.\n");
    printf("Press any key to enter power down mode ...");
    getchar();
    printf("\n");

    /* Select band-gap voltage as the source of ACMP negative input */
    ACMP_SET_NEG_SRC(ACMP, 0, ACMP_CR_VNEG_BANDGAP);
    /* Enable ACMP0 */
    ACMP_ENABLE(ACMP, 0);
    __NOP();
    for(u32DelayCount = 0; u32DelayCount < 100; u32DelayCount++); /* For ACMP setup time */
    __NOP();
    /* Clear ACMP 0 interrupt flag */
    ACMP_CLR_INT_FLAG(ACMP, 0);

    /* Enable interrupt */
    ACMP_ENABLE_INT(ACMP, 0);
    /* Enable ACMP interrupt */
    NVIC_EnableIRQ(ACMP_IRQn);

    /* To program PWRCTL register, it needs to disable register protection first. */
    SYS_UnlockReg();
    PowerDownFunction();
    printf("Wake up by ACMP0!\n");
    while(1);

}

void ACMP_IRQHandler(void)
{
    printf("\nACMP0 interrupt!\n");
    /* Disable interrupt */
    ACMP_DISABLE_INT(ACMP, 0);
    /* Clear ACMP 0 interrupt flag */
    ACMP_CLR_INT_FLAG(ACMP, 0);
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
    CLK->CLKSEL0 &= (~CLK_CLKSEL0_HCLK_S_Msk);
    CLK->CLKSEL0 |= CLK_CLKSEL0_HCLK_S_PLL;
    
    /* Select HXT as the clock source of UART */
    CLK->CLKSEL1 &= (~CLK_CLKSEL1_UART_S_Msk);
    CLK->CLKSEL1 |= CLK_CLKSEL1_UART_S_HXT;

    /* Enable UART and ACMP clock */
    CLK->APBCLK = CLK_APBCLK_UART0_EN_Msk | CLK_APBCLK_ACMP_EN_Msk;

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock and CyclesPerUs automatically. */
    SystemCoreClockUpdate();


    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set PC.6 multi-function pin for ACMP0 positive input pin */
    SYS->GPC_MFP &= ~SYS_GPC_MFP_PC6_Msk;
    SYS->GPC_MFP |= SYS_GPC_MFP_PC6_ACMP0_P;
    SYS->ALT_MFP1 &= ~SYS_ALT_MFP1_PC6_Msk;
    SYS->ALT_MFP1 |= SYS_ALT_MFP1_PC6_ACMP0_P;

    /* Set PB multi-function pins for UART0 RXD, TXD and ACMP0 output */
    SYS->GPB_MFP &= ~(SYS_GPB_MFP_PB0_Msk | SYS_GPB_MFP_PB1_Msk | SYS_GPB_MFP_PB12_Msk);
    SYS->GPB_MFP |= SYS_GPB_MFP_PB0_UART0_RXD | SYS_GPB_MFP_PB1_UART0_TXD | SYS_GPB_MFP_PB12_ACMP0_O;
    SYS->ALT_MFP &= ~SYS_ALT_MFP_PB12_Msk;
    SYS->ALT_MFP |= SYS_ALT_MFP_PB12_ACMP0_O;
    

    /* Disable digital input path of analog pin ACMP0_P to prevent leakage */
    PC->OFFD |= (1 << 6) << GPIO_OFFD_OFFD_Pos;
}

void UART_Init(void)
{
    /* Word length is 8 bits; 1 stop bit; no parity bit. */
    UART0->LCR = UART_LCR_WLS_Msk;
    /* Using mode 2 calculation: UART bit rate = UART peripheral clock rate / (BRD setting + 2) */
    /* UART peripheral clock rate 12MHz; UART bit rate 115200 bps. */
    /* 12000000 / 115200 bps ~= 104 */
    /* 104 - 2 = 0x66. */
    UART0->BAUD = UART_BAUD_DIV_X_EN_Msk | UART_BAUD_DIV_X_ONE_Msk | (0x66);
}

void PowerDownFunction(void)
{
    printf("\nSystem enter power-down mode ... ");

    /* To check if all the debug messages are finished */
    while(IsDebugFifoEmpty() == 0);

    /* Deep sleep mode is selected */
    SCB->SCR = SCB_SCR_SLEEPDEEP_Msk;

    /* To program PWRCTL register, it needs to disable register protection first. */
    CLK->PWRCON &= ~(CLK_PWRCON_PD_WAIT_CPU_Msk | CLK_PWRCON_PWR_DOWN_EN_Msk);
    CLK->PWRCON |= (CLK_PWRCON_PD_WAIT_CPU_Msk | CLK_PWRCON_PWR_DOWN_EN_Msk | CLK_PWRCON_PD_WU_INT_EN_Msk);

    __WFI();
}

/*** (C) COPYRIGHT 2014 Nuvoton Technology Corp. ***/


