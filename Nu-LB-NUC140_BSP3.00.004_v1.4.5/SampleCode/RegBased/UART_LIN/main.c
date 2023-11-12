/****************************************************************************
 * @file     main.c
 * @version  V2.00
 * $Revision: 3 $
 * $Date: 15/07/14 5:23p $
 * @brief    Transmit LIN Frame including header and response in UART LIN mode.
 * @note
 * Copyright (C) 2011 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/
#include <stdio.h>
#include "NUC100Series.h"


#define PLLCON_SETTING  CLK_PLLCON_50MHz_HXT
#define PLL_CLOCK       50000000


/* CheckSum Method */
#define MODE_CLASSIC    2
#define MODE_ENHANCED   1


/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
volatile int32_t g_i32pointer;
uint8_t g_u8SendData[12] ;


/*---------------------------------------------------------------------------------------------------------*/
/* Define functions prototype                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
extern char GetChar(void);
void LIN_FunctionTest(void);
void LIN_FunctionTestUsingLinCtlReg(void);
void LIN_MasterTest(uint32_t u32id, uint32_t u32ModeSel);
void LIN_MasterTestUsingLinCtlReg(uint32_t u32id, uint32_t u32ModeSel);
void LIN_SendHeader(uint32_t u32id);
void LIN_SendHeaderUsingLinCtlReg(uint32_t u32id, uint32_t u32HeaderSel);
void LIN_SendResponse(int32_t checkSumOption, uint32_t *pu32TxBuf);
void LIN_SendResponseWithByteCnt(int32_t checkSumOption, uint32_t *pu32TxBuf, uint32_t u32ByteCnt);
uint32_t GetCheckSumValue(uint8_t *pu8Buf, uint32_t u32ModeSel);
uint8_t ComputeChksumValue(uint8_t *pu8Buf, uint32_t u32ByteCnt);


/*---------------------------------------------------------------------------------------------------------*/
/*  Sample Code Menu                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
void TestItem(void)
{
    printf("+-----------------------------------------------------------+\n");
    printf("|                LIN Sample Program                         |\n");
    printf("+-----------------------------------------------------------+\n");
    printf("| LIN Master function test                            - [1] |\n");
    printf("| LIN Master function test using UA_LIN_CTL register  - [2] |\n");
    printf("+-----------------------------------------------------------+\n");
    printf("| Quit                                              - [ESC] |\n");
    printf("+-----------------------------------------------------------+\n");
    printf("Please Select key (1~2): ");
}

void LIN_TestItem()
{
    printf("\n\n");
    printf("+-----------------------------------------------------------+\n");
    printf("|     LIN Master Function Test                              |\n");
    printf("+-----------------------------------------------------------+\n");
    printf("| [1] Master send header with ID = 0x30                     |\n");
    printf("| [2] Master send header and response with classic checksum |\n");
    printf("| [3] Master send header and response with enhanced checksum|\n");
    printf("|                                                           |\n");
    printf("| To measure UART1_TXD(PB.5) to check waveform ...          |\n");
    printf("+-----------------------------------------------------------+\n");
    printf("| Quit                                              - [ESC] |\n");
    printf("+-----------------------------------------------------------+\n\n");
}

/*---------------------------------------------------------------------------------------------------------*/
/*  LIN Function Test                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
void LIN_FunctionTest()
{
    uint32_t u32Item;

    /* LIN Max Speed is 20K */
    UART1->BAUD = UART_BAUD_MODE2 | UART_BAUD_MODE2_DIVIDER(__HXT, 9600);

    /* === CASE 1====
        The sample code will send a LIN header with a 12-bit break field,
        0x55 sync field and ID field is 0x30. Measurement the UART1 Tx pin to check it.
    */

    /* === CASE 2====
        The sample code will send a LIN header with ID is 0x35 and response field.
        The response field with 8 data bytes and checksum without including ID.
        Measurement the UART1 Tx pin to check it.
    */

    /* === CASE 3====
        The sample code will send a LIN header with ID is 0x12 and response field.
        The response field with 8 data bytes and checksum with including ID.
        Measurement the UART1 Tx pin to check it.
    */

    do
    {
        LIN_TestItem();
        u32Item = getchar();
        printf("%c\n", u32Item);
        switch(u32Item)
        {
            case '1':
                LIN_SendHeader(0x30);
                break;
            case '2':
                LIN_MasterTest(0x35, MODE_CLASSIC);
                break;
            case '3':
                LIN_MasterTest(0x12, MODE_ENHANCED);
                break;
            default:
                break;
        }
    }
    while(u32Item != 27);

    /* Select UART function mode */
    UART1->FUN_SEL = UART_FUNC_SEL_UART;

    printf("\nLIN Sample Code End.\n");

}

/*---------------------------------------------------------------------------------------------------------*/
/*  LIN Function Test                                                                                      */
/*---------------------------------------------------------------------------------------------------------*/
void LIN_FunctionTestUsingLinCtlReg(void)
{
    uint32_t u32Item;

    /* LIN Max Speed is 20K */
    UART1->BAUD = UART_BAUD_MODE2 | UART_BAUD_MODE2_DIVIDER(__HXT, 9600);

    /* === CASE 1====
        The sample code will send a LIN header with a 12-bit break field,
        0x55 sync field and ID field is 0x30. Measurement the UART1 Tx pin to check it.
    */

    /* === CASE 2====
        The sample code will send a LIN header with ID is 0x35 and response field.
        The response field with 8 data bytes and checksum without including ID.
        Measurement the UART1 Tx pin to check it.
    */

    /* === CASE 3====
        The sample code will send a LIN header with ID is 0x12 and response field.
        The response field with 8 data bytes and checksum with including ID.
        Measurement the UART1 Tx pin to check it.
    */

    do
    {
        LIN_TestItem();
        u32Item = getchar();
        printf("%c\n", u32Item);
        switch(u32Item)
        {
            case '1':
                LIN_SendHeaderUsingLinCtlReg(0x30, UART_LIN_CTL_LIN_HEAD_SEL_BREAK_SYNC_ID);
                break;
            case '2':
                LIN_MasterTestUsingLinCtlReg(0x35, MODE_CLASSIC);
                break;
            case '3':
                LIN_MasterTestUsingLinCtlReg(0x12, MODE_ENHANCED);
                break;
            default:
                break;
        }
    }
    while(u32Item != 27);

    /* Clear header select setting */
    UART1->LIN_CTL &= ~UART_LIN_CTL_LIN_HEAD_SEL_Msk;

    /* Select UART function mode */
    UART1->FUN_SEL = UART_FUNC_SEL_UART;

    printf("\nLIN Sample Code End.\n");
}

/*---------------------------------------------------------------------------------------------------------*/
/*  Master send header and response                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
void LIN_MasterTest(uint32_t u32id, uint32_t u32ModeSel)
{
    uint32_t testPattern[8] = {0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8};

    /* Send ID=0x35 Header and Response TestPatten */
    LIN_SendHeader(u32id);
    LIN_SendResponse(u32ModeSel, &testPattern[0]);
}

/*---------------------------------------------------------------------------------------------------------*/
/*  Master send header and response                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
void LIN_MasterTestUsingLinCtlReg(uint32_t u32id, uint32_t u32ModeSel)
{
    uint8_t au8TestPattern[9] = {0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x0}; // 8 data byte + 1 byte checksum
    uint32_t i;

    if(u32ModeSel == MODE_CLASSIC)
    {
        /* Send break+sync+ID */
        LIN_SendHeaderUsingLinCtlReg(u32id, UART_LIN_CTL_LIN_HEAD_SEL_BREAK_SYNC_ID);

        /* Compute checksum without ID and fill checksum value to  au8TestPattern[8] */
        au8TestPattern[8] = ComputeChksumValue(&au8TestPattern[0], 8);

        for(i = 0; i < 9; i++)
        {
            while(!(UART1->FSR & UART_FSR_TE_FLAG_Msk));   /* Wait Tx empty */

            UART1->THR = au8TestPattern[i]; /* Send UART Data from buffer */
        }

    }
    else if(u32ModeSel == MODE_ENHANCED)
    {
        /* Send break+sync+ID and fill ID value to g_u8SendData[0] */
        LIN_SendHeaderUsingLinCtlReg(u32id, UART_LIN_CTL_LIN_HEAD_SEL_BREAK_SYNC);

        /* Fill test pattern to g_u8SendData[1]~ g_u8SendData[8] */
        for(i = 0; i < 8; i++)
            g_u8SendData[g_i32pointer++] = au8TestPattern[i];

        /* Compute checksum value with ID and fill checksum value to g_u8SendData[9] */
        g_u8SendData[g_i32pointer++] = ComputeChksumValue(&g_u8SendData[0], 9) ;

        for(i = 0; i < 9; i++)
        {
            while(!(UART1->FSR & UART_FSR_TE_FLAG_Msk));   /* Wait Tx empty */

            UART1->THR = g_u8SendData[i + 1]; /* Send UART Data from buffer */
        }

    }
}

/*---------------------------------------------------------------------------------------------------------*/
/* Compute Parity Value                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t GetParityValue(uint32_t u32id)
{
    uint32_t u32Res = 0, ID[6], p_Bit[2] , mask = 0;

    for(mask = 0; mask < 6; mask++)
        ID[mask] = (u32id & (1 << mask)) >> mask;

    p_Bit[0] = (ID[0] + ID[1] + ID[2] + ID[4]) % 2;
    p_Bit[1] = (!((ID[1] + ID[3] + ID[4] + ID[5]) % 2));

    u32Res = u32id + (p_Bit[0] << 6) + (p_Bit[1] << 7);
    return u32Res;
}

/*---------------------------------------------------------------------------------------------------------*/
/* Compute CheckSum Value , MODE_CLASSIC:(Not Include ID)    MODE_ENHANCED:(Include ID)                    */
/*---------------------------------------------------------------------------------------------------------*/
uint32_t GetCheckSumValue(uint8_t *pu8Buf, uint32_t u32ModeSel)
{
    uint32_t i, CheckSum = 0;

    for(i = u32ModeSel; i <= 9; i++)
    {
        CheckSum += pu8Buf[i];
        if(CheckSum >= 256)
            CheckSum -= 255;
    }
    return (255 - CheckSum);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Compute CheckSum Value                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t ComputeChksumValue(uint8_t *pu8Buf, uint32_t u32ByteCnt)
{
    uint32_t i, CheckSum = 0;

    for(i = 0 ; i < u32ByteCnt; i++)
    {
        CheckSum += pu8Buf[i];
        if(CheckSum >= 256)
            CheckSum -= 255;
    }
    return (uint8_t)(255 - CheckSum);
}

/*---------------------------------------------------------------------------------------------------------*/
/*  Send LIN Header Field                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
void LIN_SendHeader(uint32_t u32id)
{
    uint32_t u32Count;

    g_i32pointer = 0;

    /* Select LIN function mode */
    UART1->FUN_SEL = UART_FUNC_SEL_LIN;

    /* Set LIN operation mode, Tx mode and break field length is 12 bits */
    UART1->ALT_CSR &= ~(UART_ALT_CSR_LIN_TX_EN_Msk | UART_ALT_CSR_LIN_TX_EN_Msk | UART_ALT_CSR_UA_LIN_BKFL_Msk);
    UART1->ALT_CSR |= (UART_ALT_CSR_LIN_TX_EN_Msk | (11 << UART_ALT_CSR_UA_LIN_BKFL_Pos));

    g_u8SendData[g_i32pointer++] = 0x55 ;                   // SYNC Field
    g_u8SendData[g_i32pointer++] = GetParityValue(u32id);   // ID+Parity Field

    for(u32Count = 0; u32Count < 2; u32Count++)
    {
        while(!(UART1->FSR & UART_FSR_TE_FLAG_Msk));   /* Wait Tx empty */

        UART1->THR = g_u8SendData[u32Count]; /* Send UART Data from buffer */
    }

}

/*-------------------------------------------------------------------------------------------------------------------------------*/
/*  Send LIN Header Field                                                                                                        */
/*  u32HeaderSel =  UART_LIN_CTL_LIN_HEAD_SEL_BREAK/UART_LIN_CTL_LIN_HEAD_SEL_BREAK_SYNC/UART_LIN_CTL_LIN_HEAD_SEL_BREAK_SYNC_ID */
/*-------------------------------------------------------------------------------------------------------------------------------*/
void LIN_SendHeaderUsingLinCtlReg(uint32_t u32id, uint32_t u32HeaderSel)
{
    g_i32pointer = 0 ;

    /* Switch back to LIN Function */
    UART1->FUN_SEL = UART_FUNC_SEL_LIN;

    /* Set LIN 1. PID as 0x30 [UART_LIN_CTL_LIN_LIN_PID(0x30)]
               2. Header select as includes "break field", "sync field" and "frame ID field".[UART_LIN_CTL_LIN_HEAD_SEL_BREAK_SYNC_ID]
               3. Break/Sync Delimiter Length as 1 bit time [UART_LIN_CTL_LIN_BS_LEN(1)]
               4. Break Field Length as 12 bit time [UART_LIN_CTL_LIN_BKFL(12)]
               5. ID Parity Enable. Hardware will calculate and fill P0/P1 automatically  [UART_LIN_CTL_LIN_IDPEN_Msk]
    */
    if(u32HeaderSel == UART_LIN_CTL_LIN_HEAD_SEL_BREAK_SYNC_ID)
    {
        UART1->LIN_CTL = UART_LIN_CTL_LIN_LIN_PID(u32id) | UART_LIN_CTL_LIN_HEAD_SEL_BREAK_SYNC_ID |
                         UART_LIN_CTL_LIN_BS_LEN(1) | UART_LIN_CTL_LIN_BKFL(12) | UART_LIN_CTL_LIN_IDPEN_Msk;
        /* LIN TX Send Header Enable */
        UART1->LIN_CTL |= UART_LIN_CTL_LIN_SHD_Msk;
        /* Wait until break field, sync field and ID field transfer completed */
        while((UART1->LIN_CTL & UART_LIN_CTL_LIN_SHD_Msk) == UART_LIN_CTL_LIN_SHD_Msk);
    }
    /* Set LIN 1. Header select as includes "break field" and "sync field".[UART_LIN_CTL_LIN_HEAD_SEL_BREAK_SYNC]
               2. Break/Sync Delimiter Length as 1 bit time [UART_LIN_CTL_LIN_BS_LEN(1)]
               3. Break Field Length as 12 bit time [UART_LIN_CTL_LIN_BKFL(12)]
    */
    else if(u32HeaderSel == UART_LIN_CTL_LIN_HEAD_SEL_BREAK_SYNC)
    {
        UART1->LIN_CTL = UART_LIN_CTL_LIN_HEAD_SEL_BREAK_SYNC | UART_LIN_CTL_LIN_BS_LEN(1) | UART_LIN_CTL_LIN_BKFL(12);
        /* LIN TX Send Header Enable */
        UART1->LIN_CTL |= UART_LIN_CTL_LIN_SHD_Msk;
        /* Wait until break field and sync field transfer completed */
        while((UART1->LIN_CTL & UART_LIN_CTL_LIN_SHD_Msk) == UART_LIN_CTL_LIN_SHD_Msk);

        /* Send ID field, g_u8SendData[0] is ID+parity field */
        g_u8SendData[g_i32pointer++] = GetParityValue(u32id);   // ID+Parity Field
        UART1->THR = g_u8SendData[0];
    }
    /* Set LIN 1. Header select as includes "break field".[UART_LIN_CTL_LIN_HEAD_SEL_BREAK]
               2. Break/Sync Delimiter Length as 1 bit time [UART_LIN_CTL_LIN_BS_LEN(1)]
               3. Break Field Length as 12 bit time [UART_LIN_CTL_LIN_BKFL(12)]
    */
    else if(u32HeaderSel == UART_LIN_CTL_LIN_HEAD_SEL_BREAK)
    {
        UART1->LIN_CTL = UART_LIN_CTL_LIN_HEAD_SEL_BREAK | UART_LIN_CTL_LIN_BS_LEN(1) | UART_LIN_CTL_LIN_BKFL(12);
        /* LIN TX Send Header Enable */
        UART1->LIN_CTL |= UART_LIN_CTL_LIN_SHD_Msk;
        /* Wait until break field transfer completed */
        while((UART1->LIN_CTL & UART_LIN_CTL_LIN_SHD_Msk) == UART_LIN_CTL_LIN_SHD_Msk);

        /* Send sync field and ID field */
        g_u8SendData[g_i32pointer++] = 0x55 ;                  // SYNC Field
        g_u8SendData[g_i32pointer++] = GetParityValue(u32id);   // ID+Parity Field
        UART1->THR = g_u8SendData[0];
        while(!(UART1->FSR & UART_FSR_TE_FLAG_Msk));
        UART1->THR = g_u8SendData[1];
        while(!(UART1->FSR & UART_FSR_TE_FLAG_Msk));
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/*  Send LIN Response Field                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
void LIN_SendResponse(int32_t checkSumOption, uint32_t *pu32TxBuf)
{
    int32_t i32;

    for(i32 = 0; i32 < 8; i32++)
        g_u8SendData[g_i32pointer++] = pu32TxBuf[i32] ;

    g_u8SendData[g_i32pointer++] = GetCheckSumValue(g_u8SendData, checkSumOption) ; //CheckSum Field

    for(i32 = 0; i32 < 9; i32++)
    {
        while(!(UART1->FSR & UART_FSR_TE_FLAG_Msk));   /* Wait Tx empty */

        UART1->THR = g_u8SendData[i32 + 2]; /* Send UART Data from buffer */
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/*  Send LIN Response Field                                                                                */
/*---------------------------------------------------------------------------------------------------------*/
void LIN_SendResponseWithByteCnt(int32_t checkSumOption, uint32_t *pu32TxBuf, uint32_t u32ByteCnt)
{
    int32_t i32;

    /* Prepare data */
    for(i32 = 0; i32 < u32ByteCnt; i32++)
        g_u8SendData[g_i32pointer++] = pu32TxBuf[i32] ;

    /* Prepare check sum */
    if(checkSumOption == MODE_CLASSIC)
        g_u8SendData[g_i32pointer++] = GetCheckSumValue(&g_u8SendData[2], u32ByteCnt) ;  //CheckSum Field
    else if(checkSumOption == MODE_ENHANCED)
        g_u8SendData[g_i32pointer++] = GetCheckSumValue(&g_u8SendData[1], (u32ByteCnt + 1)) ; //CheckSum Field

    /* Send data and check sum */
    for(i32 = 0; i32 < 9; i32++)
    {
        while(!(UART1->FSR & UART_FSR_TE_FLAG_Msk));   /* Wait Tx empty */

        UART1->THR = g_u8SendData[i32 + 2]; /* Send UART Data from buffer */
    }

}

void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Enable Internal RC 22.1184MHz clock */
    CLK->PWRCON |= CLK_PWRCON_OSC22M_EN_Msk;

    /* Waiting for Internal RC clock ready */
    while(!(CLK->CLKSTATUS & CLK_CLKSTATUS_OSC22M_STB_Msk));

    /* Switch HCLK clock source to Internal RC and HCLK source divide 1 */
    CLK->CLKSEL0 = (CLK->CLKSEL0 & (~CLK_CLKSEL0_HCLK_S_Msk)) | CLK_CLKSEL0_HCLK_S_HIRC;
    CLK->CLKDIV = (CLK->CLKDIV & (~CLK_CLKDIV_HCLK_N_Msk)) | CLK_CLKDIV_HCLK(1);

    /* Set PLL to power down mode and PLL_STB bit in CLKSTATUS register will be cleared by hardware */
    CLK->PLLCON |= CLK_PLLCON_PD_Msk;     
    
    /* Enable external XTAL 12MHz clock */
    CLK->PWRCON |= CLK_PWRCON_XTL12M_EN_Msk;

    /* Waiting for external XTAL clock ready */
    while(!(CLK->CLKSTATUS & CLK_CLKSTATUS_XTL12M_STB_Msk));

    /* Set core clock as PLL_CLOCK from PLL */
    CLK->PLLCON = PLLCON_SETTING;
    while(!(CLK->CLKSTATUS & CLK_CLKSTATUS_PLL_STB_Msk));
    CLK->CLKSEL0 = (CLK->CLKSEL0 & (~CLK_CLKSEL0_HCLK_S_Msk)) | CLK_CLKSEL0_HCLK_S_PLL;

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate PllClock, SystemCoreClock and CycylesPerUs automatically. */
    //SystemCoreClockUpdate();
    PllClock        = PLL_CLOCK;            // PLL
    SystemCoreClock = PLL_CLOCK / 1;        // HCLK
    CyclesPerUs     = PLL_CLOCK / 1000000;  // For CLK_SysTickDelay()

    /* Enable UART module clock */
    CLK->APBCLK |= (CLK_APBCLK_UART0_EN_Msk | CLK_APBCLK_UART1_EN_Msk);

    /* Select UART module clock source */
    CLK->CLKSEL1 = (CLK->CLKSEL1 & (~CLK_CLKSEL1_UART_S_Msk)) | CLK_CLKSEL1_UART_S_HXT;

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Set GPB multi-function pins for UART0 RXD and TXD */
    /* Set GPB multi-function pins for UART1 RXD and TXD */

    SYS->GPB_MFP &= ~(SYS_GPB_MFP_PB0_Msk | SYS_GPB_MFP_PB1_Msk |
                      SYS_GPB_MFP_PB4_Msk | SYS_GPB_MFP_PB5_Msk);

    SYS->GPB_MFP |= (SYS_GPB_MFP_PB0_UART0_RXD | SYS_GPB_MFP_PB1_UART0_TXD |
                     SYS_GPB_MFP_PB4_UART1_RXD | SYS_GPB_MFP_PB5_UART1_TXD);

}

void UART0_Init()
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Reset UART0 */
    SYS->IPRSTC2 |=  SYS_IPRSTC2_UART0_RST_Msk;
    SYS->IPRSTC2 &= ~SYS_IPRSTC2_UART0_RST_Msk;

    /* Configure UART0 and set UART0 Baudrate */
    UART0->BAUD = UART_BAUD_MODE2 | UART_BAUD_MODE2_DIVIDER(__HXT, 115200);
    UART0->LCR = UART_WORD_LEN_8 | UART_PARITY_NONE | UART_STOP_BIT_1;
}

void UART1_Init()
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Reset UART1 */
    SYS->IPRSTC2 |=  SYS_IPRSTC2_UART1_RST_Msk;
    SYS->IPRSTC2 &= ~SYS_IPRSTC2_UART1_RST_Msk;

    /* Configure UART1 and set UART1 Baudrate */
    UART1->BAUD = UART_BAUD_MODE2 | UART_BAUD_MODE2_DIVIDER(__HXT, 115200);
    UART1->LCR = UART_WORD_LEN_8 | UART_PARITY_NONE | UART_STOP_BIT_1;
}

/*---------------------------------------------------------------------------------------------------------*/
/* MAIN function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/

int main(void)
{
    uint32_t u32Item;

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

    /* UART sample LIN function */
    do
    {
        TestItem();
        u32Item = getchar();
        printf("%c\n", u32Item);
        switch(u32Item)
        {
            case '1':
                LIN_FunctionTest();
                break;
            case '2':
                LIN_FunctionTestUsingLinCtlReg();
                break;
            default:
                break;
        }
    }
    while(u32Item != 27);

    while(1);

}

