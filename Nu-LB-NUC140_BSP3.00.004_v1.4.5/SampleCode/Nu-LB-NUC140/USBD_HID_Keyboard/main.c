//
// USB_HID_Keyboard : emulate USB Keyboard using 3x3 keypad
//
// EVB : Nu-LB-NUC140
// MCU : NUC140VE3CN#include <stdio.h>
// Keypad : 1~9 will send key 'a'~'i' to USB

#include "NUC100Series.h"
#include "hid_kb.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "Scankey.h"
/*--------------------------------------------------------------------------*/
uint8_t volatile g_u8EP2Ready = 0;



/*--------------------------------------------------------------------------*/

void EnableCLKO(uint32_t u32ClkSrc, uint32_t u32ClkDiv)
{
    /* CLKO = clock source / 2^(u32ClkDiv + 1) */
    CLK->FRQDIV = CLK_FRQDIV_DIVIDER_EN_Msk | u32ClkDiv;

    /* Enable CLKO clock source */
    CLK->APBCLK |= CLK_APBCLK_FDIV_EN_Msk;

    /* Select CLKO clock source */
    CLK->CLKSEL2 = (CLK->CLKSEL2 & (~CLK_CLKSEL2_FRQDIV_S_Msk)) | u32ClkSrc;
}

void HID_UpdateKbData(void)
{
    int32_t i;
    uint8_t *buf;
    uint32_t key = 0xF;
    static uint32_t preKey;

    if(g_u8EP2Ready)
    {
        buf = (uint8_t *)(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP2));

        /* If GPB15 = 0, just report it is key 'a' */
        // key = (PB->PIN & (1 << 15)) ? 0 : 1;
			  key = ScanKey();

        if(key == 0)
        {
            for(i = 0; i < 8; i++)
            {
                buf[i] = 0;
            }

            if(key != preKey)
            {
                /* Trigger to note key release */
                USBD_SET_PAYLOAD_LEN(EP2, 8);
            }
        }
        else
        {
            preKey = key;
            buf[2] = 0x03 + key; /* Key = a~i for 3x3 keypad = 1~9 */
            USBD_SET_PAYLOAD_LEN(EP2, 8);
        }
    }
}

int32_t main(void)
{
    SYS_Init();
    USBD_Open(&gsInfo, HID_ClassRequest, NULL);
	  OpenKeyPad();

    /* Endpoint configuration */
    HID_Init();
    USBD_Start();
    NVIC_EnableIRQ(USBD_IRQn);

    /* start to IN data */
    g_u8EP2Ready = 1;

    while(1)
    {
        HID_UpdateKbData();
    }
}

