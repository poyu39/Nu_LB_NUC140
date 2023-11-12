//
// USBD_VCOM : Implement a USB virtual COM port device on UART0
//
#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "cdc_serial.h"

/*--------------------------------------------------------------------------*/
STR_VCOM_LINE_CODING gLineCoding = {115200, 0, 0, 8};   /* Baud rate : 115200    */
/* Stop bit     */
/* parity       */
/* data bits    */
uint16_t gCtrlSignal = 0;     /* BIT0: DTR(Data Terminal Ready) , BIT1: RTS(Request To Send) */

/*--------------------------------------------------------------------------*/
#define RXBUFSIZE           512 /* RX buffer size */
#define TXBUFSIZE           512 /* RX buffer size */

#define TX_FIFO_SIZE        64  /* TX Hardware FIFO size */


/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
/* UART0 */
volatile uint8_t comRbuf[RXBUFSIZE];
volatile uint16_t comRbytes = 0;
volatile uint16_t comRhead = 0;
volatile uint16_t comRtail = 0;

volatile uint8_t comTbuf[TXBUFSIZE];
volatile uint16_t comTbytes = 0;
volatile uint16_t comThead = 0;
volatile uint16_t comTtail = 0;

uint8_t gRxBuf[64] = {0};
volatile uint8_t *gpu8RxBuf = 0;
volatile uint32_t gu32RxSize = 0;
volatile uint32_t gu32TxSize = 0;

volatile int8_t gi8BulkOutReady = 0;


void Init_UART(void)
{
    UART_Open(UART0, 115200);
    UART_ENABLE_INT(UART0, UART_IER_TOUT_IEN_Msk | UART_IER_RDA_IEN_Msk);
    NVIC_EnableIRQ(UART02_IRQn);	
}

void UART02_IRQHandler(void)
{
    uint32_t u32IntStatus;
    uint8_t bInChar;
    int32_t size;

    u32IntStatus = UART0->ISR;

    if((u32IntStatus & 0x1 /* RDAIF */) || (u32IntStatus & 0x10 /* TOUT_IF */))
    {
        /* Receiver FIFO threashold level is reached or Rx time out */

        /* Get all the input characters */
        while((UART0->FSR & UART_FSR_RX_EMPTY_Msk) == 0)
        {
            /* Get the character from UART Buffer */
            bInChar = UART0->DATA;

            /* Check if buffer full */
            if(comRbytes < RXBUFSIZE)
            {
                /* Enqueue the character */
                comRbuf[comRtail++] = bInChar;
                if(comRtail >= RXBUFSIZE)
                    comRtail = 0;
                comRbytes++;
            }
            else
            {
                /* FIFO over run */
            }
        }
    }

    if(u32IntStatus & 0x2 /* THRE_IF */)
    {

        if(comTbytes)
        {
            /* Fill the Tx FIFO */
            size = comTbytes;
            if(size >= TX_FIFO_SIZE)
            {
                size = TX_FIFO_SIZE;
            }

            while(size)
            {
                bInChar = comTbuf[comThead++];
                UART0->DATA = bInChar;
                if(comThead >= TXBUFSIZE)
                    comThead = 0;
                comTbytes--;
                size--;
            }
        }
        else
        {
            /* No more data, just stop Tx (Stop work) */
            UART0->IER &= (~UART_IER_THRE_IEN_Msk);
        }
    }

}

void VCOM_TransferData(void)
{
    int32_t i, i32Len;

    /* Check wether USB is ready for next packet or not*/
    if(gu32TxSize == 0)
    {
        /* Check wether we have new COM Rx data to send to USB or not */
        if(comRbytes)
        {
            i32Len = comRbytes;
            if(i32Len > EP2_MAX_PKT_SIZE)
                i32Len = EP2_MAX_PKT_SIZE;

            for(i = 0; i < i32Len; i++)
            {
                gRxBuf[i] = comRbuf[comRhead++];
                if(comRhead >= RXBUFSIZE)
                    comRhead = 0;
            }

            __set_PRIMASK(1);
            comRbytes -= i32Len;
            __set_PRIMASK(0);

            gu32TxSize = i32Len;
            USBD_MemCopy((uint8_t *)(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP2)), (uint8_t *)gRxBuf, i32Len);
            USBD_SET_PAYLOAD_LEN(EP2, i32Len);
        }
        else
        {
            /* Prepare a zero packet if previous packet size is EP2_MAX_PKT_SIZE and
               no more data to send at this moment to note Host the transfer has been done */
            i32Len = USBD_GET_PAYLOAD_LEN(EP2);
            if(i32Len == EP2_MAX_PKT_SIZE)
                USBD_SET_PAYLOAD_LEN(EP2, 0);
        }
    }

    /* Process the Bulk out data when bulk out data is ready. */
    if(gi8BulkOutReady && (gu32RxSize <= TXBUFSIZE - comTbytes))
    {
        for(i = 0; i < gu32RxSize; i++)
        {
            comTbuf[comTtail++] = gpu8RxBuf[i];
            if(comTtail >= TXBUFSIZE)
                comTtail = 0;
        }

        __set_PRIMASK(1);
        comTbytes += gu32RxSize;
        __set_PRIMASK(0);

        gu32RxSize = 0;
        gi8BulkOutReady = 0; /* Clear bulk out ready flag */

        /* Ready to get next BULK out */
        USBD_SET_PAYLOAD_LEN(EP3, EP3_MAX_PKT_SIZE);
    }

    /* Process the software Tx FIFO */
    if(comTbytes)
    {
        /* Check if Tx is working */
        if((UART0->IER & UART_IER_THRE_IEN_Msk) == 0)
        {
            /* Send one bytes out */
            UART0->DATA = comTbuf[comThead++];
            if(comThead >= TXBUFSIZE)
                comThead = 0;

            __set_PRIMASK(1);
            comTbytes--;
            __set_PRIMASK(0);

            /* Enable Tx Empty Interrupt. (Trigger first one) */
            UART0->IER |= UART_IER_THRE_IEN_Msk;
        }
    }
}

int32_t main(void)
{
    SYS_Init();
    Init_UART();


    USBD_Open(&gsInfo, VCOM_ClassRequest, NULL);

    /* Endpoint configuration */
    VCOM_Init();
    USBD_Start();
    NVIC_EnableIRQ(USBD_IRQn);

    while(1)
    {
        VCOM_TransferData();
    }
}


