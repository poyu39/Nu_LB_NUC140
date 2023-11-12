/****************************************************************************
 * @file     main.c
 * @version  V2.0
 * $Revision: 1 $
 * $Date: 14/12/08 11:47a $
 * @brief    CCID Smart Card reader Sample Code.
 *
 * @note
 * Copyright (C) 2014 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/
#include <stdio.h>
#include "NUC100Series.h"
#include "ccid.h"
#include "ccid_if.h"
#include "sclib.h"

#define PLL_CLOCK       48000000

/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
#define INT_BUFFER_SIZE     64    /* Interrupt message buffer size */
#define BULK_BUFFER_SIZE    512   /* bulk message buffer size */

uint8_t UsbIntMessageBuffer[INT_BUFFER_SIZE];
uint8_t UsbMessageBuffer[BULK_BUFFER_SIZE];

uint8_t volatile gu8IsDeviceReady;
uint8_t volatile gu8AbortRequestFlag;
uint8_t volatile gu8IsBulkOutReady;
uint8_t volatile gu8IsBulkInReady;

uint8_t *pu8IntInBuf;
uint8_t *pUsbMessageBuffer;
uint32_t volatile u32BulkSize;

int32_t volatile gi32UsbdMessageLength;

/*---------------------------------------------------------------------------------------------------------*/
/* The interrupt services routine of smartcard port 0                                                      */
/*---------------------------------------------------------------------------------------------------------*/
void SC012_IRQHandler(void)
{
    // Please don't remove any of the function calls below
    if(SCLIB_CheckCDEvent(0))
    {
        RDR_to_PC_NotifySlotChange();
        USBD_MemCopy((uint8_t *)(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP4)), pu8IntInBuf, 2);
        USBD_SET_PAYLOAD_LEN(EP4, 2);
        return; // Card insert/remove event occurred, no need to check other event...
    }

    SCLIB_CheckTimeOutEvent(0);
    SCLIB_CheckTxRxEvent(0);
    SCLIB_CheckErrorEvent(0);

    return;
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

    /* Waiting for external XTAL clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_XTL12M_STB_Msk);

    /* Set core clock as PLL_CLOCK from PLL */
    CLK_SetCoreClock(PLL_CLOCK);

    /* Select IP clock source */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART_S_PLL, CLK_CLKDIV_UART(1));
    CLK_SetModuleClock(SC0_MODULE, CLK_CLKSEL3_SC0_S_HXT, CLK_CLKDIV1_SC0(3));
    CLK_SetModuleClock(USBD_MODULE, 0, CLK_CLKDIV_USB(1));

    /* Enable IP clock */
    CLK_EnableModuleClock(UART0_MODULE);
    CLK_EnableModuleClock(SC0_MODULE);
    CLK_EnableModuleClock(USBD_MODULE);

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Set GPB multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFP &= ~(SYS_GPB_MFP_PB0_Msk | SYS_GPB_MFP_PB1_Msk);
    SYS->GPB_MFP |= SYS_GPB_MFP_PB0_UART0_RXD | SYS_GPB_MFP_PB1_UART0_TXD;

    /* Set PA.0 ~ PA.3 and PC.6 for SC0 interface */
    SYS->GPA_MFP &= ~(SYS_GPA_MFP_PA0_Msk | SYS_GPA_MFP_PA1_Msk | SYS_GPA_MFP_PA2_Msk | SYS_GPA_MFP_PA3_Msk);
    SYS->GPC_MFP &= ~SYS_GPC_MFP_PC6_Msk;
    SYS->GPA_MFP |= SYS_GPA_MFP_PA0_SC0_PWR | SYS_GPA_MFP_PA1_SC0_RST | SYS_GPA_MFP_PA2_SC0_CLK | SYS_GPA_MFP_PA3_SC0_DAT;
    SYS->GPC_MFP |= SYS_GPC_MFP_PC6_SC0_CD;
    SYS->ALT_MFP1 &= ~(SYS_ALT_MFP1_PA0_Msk | SYS_ALT_MFP1_PA1_Msk | SYS_ALT_MFP1_PA2_Msk | SYS_ALT_MFP1_PA3_Msk);
    SYS->ALT_MFP1 &= ~SYS_ALT_MFP1_PC6_Msk;
    SYS->ALT_MFP1 |= SYS_ALT_MFP1_PA0_SC0_PWR | SYS_ALT_MFP1_PA1_SC0_RST | SYS_ALT_MFP1_PA2_SC0_CLK | SYS_ALT_MFP1_PA3_SC0_DAT;
    SYS->ALT_MFP1 |= SYS_ALT_MFP1_PC6_SC0_CD;
}


/*---------------------------------------------------------------------------------------------------------*/
/* Init UART                                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
void UART0_Init()
{
    /* Reset IP */
    SYS_ResetModule(UART0_RST);

    /* Configure UART0 and set UART0 Baudrate */
    UART_Open(UART0, 115200);
}

/*---------------------------------------------------------------------------------------------------------*/
/* MAIN function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
int main(void)
{
//     SCLIB_CARD_INFO_T s_info;
//     int retval, i;

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, IP clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();

    /* Init UART0 for printf */
    UART0_Init();

    /*---------------------------------------------------------------------------------------------------------*/
    /* SAMPLE CODE                                                                                             */
    /*---------------------------------------------------------------------------------------------------------*/

    printf("NuMicro USB CCID SmartCard Reader\n");

    // Open smartcard interface 0. CD pin state low indicates card insert and PWR pin low raise VCC pin to card
    SC_Open(SC0, SC_PIN_STATE_LOW, SC_PIN_STATE_HIGH);
    NVIC_EnableIRQ(SC012_IRQn);

    SC0->IER = SC_IER_CD_IE_Msk;

    USBD_Open(&gsInfo, CCID_ClassRequest, NULL);

    /* Endpoint configuration */
    CCID_Init();
    //Set priority is a must under current architecture. Otherwise smartcard interrupt will be blocked by USBD interrupt
    NVIC_SetPriority(USBD_IRQn, (1 << __NVIC_PRIO_BITS) - 2);

    USBD_Start();
    NVIC_EnableIRQ(USBD_IRQn);

    while(1);

}
