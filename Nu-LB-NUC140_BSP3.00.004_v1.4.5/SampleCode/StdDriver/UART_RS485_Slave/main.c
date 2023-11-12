/****************************************************************************
 * @file     main.c
 * @version  V1.00
 * $Revision: 2 $
 * $Date: 15/04/14 9:31a $
 * @brief
 *           Transmit and receive data in UART RS485 mode.
 *           This sample code needs to work with UART_RS485_Master.
 * @note
 * Copyright (C) 2011 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/
#include <stdio.h>
#include "NUC100Series.h"


#define PLL_CLOCK 50000000


#define IS_USE_RS485NMM   1      //1:Select NMM_Mode , 0:Select AAD_Mode
#define MATCH_ADDRSS1     0xC0
#define MATCH_ADDRSS2     0xA2
#define UNMATCH_ADDRSS1   0xB1
#define UNMATCH_ADDRSS2   0xD3


/*---------------------------------------------------------------------------------------------------------*/
/* Define functions prototype                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
extern char GetChar(void);
void RS485_HANDLE(void);
void RS485_9bitModeSlave(void);
void RS485_FunctionTest(void);


/*---------------------------------------------------------------------------------------------------------*/
/* ISR to handle UART Channel 1 interrupt event                                                            */
/*---------------------------------------------------------------------------------------------------------*/
void UART1_IRQHandler(void)
{
    RS485_HANDLE();
}

/*---------------------------------------------------------------------------------------------------------*/
/* RS485 Callback function                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/
void RS485_HANDLE()
{
    volatile uint32_t addr = 0;
    volatile uint32_t regRX = 0xFF;
    volatile uint32_t u32IntSts = UART1->ISR;;

    if(UART_GET_INT_FLAG(UART1, UART_ISR_RLS_INT_Msk) && UART_GET_INT_FLAG(UART1, UART_ISR_RDA_INT_Msk))      /* RLS INT & RDA INT */
    {
        if(UART_RS485_GET_ADDR_FLAG(UART1))     /* ADD_IF, RS485 mode */
        {
            addr = UART_READ(UART1);
            UART_RS485_CLEAR_ADDR_FLAG(UART1);         /* clear ADD_IF flag */
            printf("\nAddr=0x%x,Get:", addr);

#if (IS_USE_RS485NMM ==1) //RS485_NMM
            /* if address match, enable RX to receive data, otherwise to disable RX. */
            /* In NMM mode,user can decide multi-address filter. In AAD mode,only one address can set */
            if((addr == MATCH_ADDRSS1) || (addr == MATCH_ADDRSS2))
            {
                UART1->FCR &= ~ UART_FCR_RX_DIS_Msk;  /* Enable RS485 RX */
            }
            else
            {
                printf("\n");
                UART1->FCR |= UART_FCR_RX_DIS_Msk;    /* Disable RS485 RX */
                UART1->FCR |= UART_FCR_RFR_Msk;       /* Clear data from RX FIFO */
            }
#endif
        }
    }
    else if(UART_GET_INT_FLAG(UART1, UART_ISR_RDA_INT_Msk) || UART_GET_INT_FLAG(UART1, UART_ISR_TOUT_INT_Msk))       /* Rx Ready or Time-out INT */
    {
        /* Handle received data */
        printf("%d,", UART1->RBR);
    }

    else if(UART_GET_INT_FLAG(UART1, UART_ISR_BUF_ERR_INT_Msk))       /* Buffer Error INT */
    {
        printf("\nBuffer Error...\n");
        UART_ClearIntFlag(UART1, UART_ISR_BUF_ERR_INT_Msk);
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/*  RS485 Receive Test  (IS_USE_RS485NMM: 0:AAD  1:NMM)                                                    */
/*---------------------------------------------------------------------------------------------------------*/
void RS485_9bitModeSlave()
{
    /* Set Data Format, only need parity enable whatever parity ODD/EVEN */
    UART_SetLine_Config(UART1, 0, UART_WORD_LEN_8, UART_PARITY_EVEN, UART_STOP_BIT_1);

    /* Set RTS pin active level as high level active */
    UART1->MCR &= ~UART_MCR_LEV_RTS_Msk;
    UART1->MCR |= UART_RTS_IS_HIGH_LEV_ACTIVE;

#if(IS_USE_RS485NMM == 1)
    printf("+-----------------------------------------------------------+\n");
    printf("|    Normal Multidrop Operation Mode                        |\n");
    printf("+-----------------------------------------------------------+\n");
    printf("| The function is used to test 9-bit slave mode.            |\n");
    printf("| Only Address %x and %x,data can receive                   |\n", MATCH_ADDRSS1, MATCH_ADDRSS2);
    printf("+-----------------------------------------------------------+\n");

    /* Set RX_DIS enable before set RS485-NMM mode */
    UART1->FCR |= UART_FCR_RX_DIS_Msk;

    /* Set RS485-NMM Mode */
    UART_SelectRS485Mode(UART1, UART_ALT_CSR_RS485_NMM_Msk | UART_ALT_CSR_RS485_AUD_Msk, 0);

    /* Set RS485 address detection enable */
    UART1->ALT_CSR |= UART_ALT_CSR_RS485_ADD_EN_Msk;

#else
    printf("Auto Address Match Operation Mode\n");
    printf("+-----------------------------------------------------------+\n");
    printf("| The function is used to test 9-bit slave mode.            |\n");
    printf("|    Auto Address Match Operation Mode                      |\n");
    printf("+-----------------------------------------------------------+\n");
    printf("|Only Address %x,data can receive                           |\n", MATCH_ADDRSS1);
    printf("+-----------------------------------------------------------+\n");

    /* Set RS485-AAD Mode and address match is 0xC0 */
    UART_SelectRS485Mode(UART1, UART_ALT_CSR_RS485_AAD_Msk | UART_ALT_CSR_RS485_AUD_Msk, MATCH_ADDRSS1);

    /* Set RS485 address detection enable */
    UART1->ALT_CSR |= UART_ALT_CSR_RS485_ADD_EN_Msk;

#endif

    /* Enable RDA\RLS\Time-out Interrupt */
    UART_ENABLE_INT(UART1, (UART_IER_RDA_IEN_Msk | UART_IER_RLS_IEN_Msk | UART_IER_TOUT_IEN_Msk));

    /* Enable UART1 interrupt */
    NVIC_EnableIRQ(UART1_IRQn);

    printf("Ready to receive data...(Press any key to stop test)\n");
    GetChar();

    /* Flush FIFO */
    while(UART_GET_RX_EMPTY(UART1) == 0)
    {
        UART_READ(UART1);
    }

    /* Disable RDA/RLS/RTO interrupt */
    UART_DISABLE_INT(UART1, (UART_IER_RDA_IEN_Msk | UART_IER_RLS_IEN_Msk | UART_IER_TOUT_IEN_Msk));

    /* Set UART Function */
    UART_Open(UART1, 115200);

    printf("\n\nEnd test\n");
}


/*---------------------------------------------------------------------------------------------------------*/
/*  RS485 Function Test                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
void RS485_FunctionTest()
{
    printf("\n\n");
    printf("+-----------------------------------------------------------+\n");
    printf("|      RS485 Function Test IO Setting                       |\n");
    printf("+-----------------------------------------------------------+\n");
    printf("|  ______                                            _____  |\n");
    printf("| |      |                                          |     | |\n");
    printf("| |Master|--UART1_TXD(PB.5)  <==>  UART1_RXD(PB.4)--|Slave| |\n");
    printf("| |      |--UART1_nRTS(PB.6) <==> UART1_nRTS(PB.6)--|     | |\n");
    printf("| |______|                                          |_____| |\n");
    printf("|                                                           |\n");
    printf("+-----------------------------------------------------------+\n");


    /*
        The sample code is used to test RS485 9-bit mode and needs
        two Module test board to complete the test.
        Master:
            1.Set AUD mode and HW will control RTS pin. LEV_RTS is set to '0'.
            2.Master will send four different address with 10 bytes data to test Slave.
            3.Address bytes : the parity bit should be '1'. (Set UA_LCR = 0x2B)
            4.Data bytes : the parity bit should be '0'. (Set UA_LCR = 0x3B)
            5.RTS pin is low in idle state. When master is sending,
              RTS pin will be pull high.

        Slave:
            1.Set AAD and AUD mode firstly. LEV_RTS is set to '0'.
            2.The received byte, parity bit is '1' , is considered "ADDRESS".
            3.The received byte, parity bit is '0' , is considered "DATA".  (Default)
            4.AAD: The slave will ignore any data until ADDRESS match ADDR_MATCH value.
              When RLS and RDA interrupt is happened,it means the ADDRESS is received.
              Check if RS485_ADD_DETF is set and read UA_RBR to clear ADDRESS stored in rx_fifo.

              NMM: The slave will ignore data byte until disable RX_DIS.
              When RLS and RDA interrupt is happened,it means the ADDRESS is received.
              Check the ADDRESS is match or not by user in UART_IRQHandler.
              If the ADDRESS is match,clear RX_DIS bit to receive data byte.
              If the ADDRESS is not match,set RX_DIS bit to avoid data byte stored in FIFO.
    */

    RS485_9bitModeSlave();

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

    /* Enable UART module clock */
    CLK_EnableModuleClock(UART0_MODULE);
    CLK_EnableModuleClock(UART1_MODULE);

    /* Select UART module clock source */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART_S_HXT, CLK_CLKDIV_UART(1));
    CLK_SetModuleClock(UART1_MODULE, CLK_CLKSEL1_UART_S_HXT, CLK_CLKDIV_UART(1));

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Set GPB multi-function pins for UART0 RXD and TXD */
    /* Set GPB multi-function pins for UART1 RXD, TXD, nRTS and nCTS */

    SYS->GPB_MFP &= ~(SYS_GPB_MFP_PB0_Msk | SYS_GPB_MFP_PB1_Msk |
                      SYS_GPB_MFP_PB4_Msk | SYS_GPB_MFP_PB5_Msk |
                      SYS_GPB_MFP_PB6_Msk | SYS_GPB_MFP_PB7_Msk);

    SYS->GPB_MFP |= (SYS_GPB_MFP_PB0_UART0_RXD | SYS_GPB_MFP_PB1_UART0_TXD |
                     SYS_GPB_MFP_PB4_UART1_RXD | SYS_GPB_MFP_PB5_UART1_TXD |
                     SYS_GPB_MFP_PB6_UART1_nRTS | SYS_GPB_MFP_PB7_UART1_nCTS);

}

void UART0_Init()
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Reset UART0 */
    SYS_ResetModule(UART0_RST);

    /* Configure UART0 and set UART0 Baudrate */
    UART_Open(UART0, 115200);
}

void UART1_Init()
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Reset UART1 */
    SYS_ResetModule(UART1_RST);

    /* Configure UART1 and set UART1 Baudrate */
    UART_Open(UART1, 115200);
}

/*---------------------------------------------------------------------------------------------------------*/
/* MAIN function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main(void)
{

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();

    /* Init UART0 for printf */
    UART0_Init();

    /* Init UART1 for testing */
    UART1_Init();

    /*---------------------------------------------------------------------------------------------------------*/
    /* SAMPLE CODE                                                                                             */
    /*---------------------------------------------------------------------------------------------------------*/

    printf("\n\nCPU @ %dHz\n", SystemCoreClock);

    printf("\n\nUART Sample Program\n");

    /* UART RS485 sample slave function */
    RS485_FunctionTest();

    while(1);

}
