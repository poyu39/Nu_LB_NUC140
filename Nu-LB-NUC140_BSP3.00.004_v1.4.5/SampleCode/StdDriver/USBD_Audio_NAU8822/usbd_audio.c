/******************************************************************************
 * @file     usbd_audio.c
 * @brief    December series USBD driver Sample file
 * @version  1.0.0
 * @date     23, December, 2013
 *
 * @note
 * Copyright (C) 2013 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/

/*!<Includes */
#include <string.h>
#include <stdio.h>
#include "NUC100Series.h"
#include "usbd_audio.h"


/*--------------------------------------------------------------------------*/
/* Global variables for Audio class */
volatile uint32_t g_usbd_UsbAudioState = 0;

volatile uint8_t g_usbd_RecMute       = 0x01;     /* Record MUTE control. 0 = normal. 1 = MUTE */
volatile int16_t g_usbd_RecVolumeL    = 0x1000;   /* Record left channel volume. Range is -32768 ~ 32767 */
volatile int16_t g_usbd_RecVolumeR    = 0x1000;   /* Record right channel volume. Range is -32768 ~ 32767 */
volatile int16_t g_usbd_RecMaxVolume  = 0x7FFF;
volatile int16_t g_usbd_RecMinVolume  = 0x8000;
volatile int16_t g_usbd_RecResVolume  = 0x400;

volatile uint8_t g_usbd_PlayMute      = 0x01;     /* Play MUTE control. 0 = normal. 1 = MUTE */
volatile int16_t g_usbd_PlayVolumeL   = 0x1000;   /* Play left channel volume. Range is -32768 ~ 32767 */
volatile int16_t g_usbd_PlayVolumeR   = 0x1000;   /* PLay right channel volume. Range is -32768 ~ 32767 */
volatile int16_t g_usbd_PlayMaxVolume = 0x7FFF;
volatile int16_t g_usbd_PlayMinVolume = 0x8000;
volatile int16_t g_usbd_PlayResVolume = 0x400;

static volatile uint8_t g_u8RecEn = 0;
static volatile uint8_t g_u8PlayEn = 0;      /* To indicate data is output to I2S */
static volatile int32_t g_i32AdjFlag = 0;    /* To indicate current I2S frequency adjustment status */


/*******************************************************************/
typedef enum
{
    E_RS_NONE,          // no resampling
    E_RS_UP,            // up sampling
    E_RS_DOWN           // down sampling
} RESAMPLE_STATE_T;

/* Temp buffer for play and record */
uint32_t g_au32UsbTmpBuf[((PLAY_RATE > REC_RATE) ? PLAY_RATE : REC_RATE) / 2000 * ((PLAY_CHANNELS > REC_CHANNELS) ? PLAY_CHANNELS : REC_CHANNELS)] = {0};

/* Recoder Buffer and its pointer */
uint32_t g_au32PcmRecBuf[96] = {0};
volatile uint32_t g_u32RecPos = 0;

/* Player Buffer and its pointer */
uint32_t g_au32PcmPlayBuf[BUF_LEN] = {0};
volatile uint32_t g_u32PlayPos_Out = 0;
volatile uint32_t g_u32PlayPos_In = 0;



uint32_t GetSamplesInBuf(void)
{
    int32_t i32Tmp;

    i32Tmp = g_u32PlayPos_In;
    i32Tmp -= g_u32PlayPos_Out;
    if(i32Tmp < 0)
        i32Tmp += BUF_LEN;

    return (uint32_t)i32Tmp;
}

/*--------------------------------------------------------------------------*/


/**
 * @brief       USBD Interrupt Service Routine
 *
 * @param[in]   None
 *
 * @return      None
 *
 * @details     This function is the USBD ISR
 */
void USBD_IRQHandler(void)
{
    uint32_t u32IntSts = USBD_GET_INT_FLAG();
    uint32_t u32State = USBD_GET_BUS_STATE();

//------------------------------------------------------------------
    if(u32IntSts & USBD_INTSTS_FLDET)
    {
        // Floating detect
        USBD_CLR_INT_FLAG(USBD_INTSTS_FLDET);

        if(USBD_IS_ATTACHED())
        {
            /* USB Plug In */
            USBD_ENABLE_USB();
        }
        else
        {
            /* USB Un-plug */
            USBD_DISABLE_USB();
        }
    }

//------------------------------------------------------------------
    if(u32IntSts & USBD_INTSTS_BUS)
    {
        /* Clear event flag */
        USBD_CLR_INT_FLAG(USBD_INTSTS_BUS);

        if(u32State & USBD_STATE_USBRST)
        {
            /* Bus reset */
            USBD_ENABLE_USB();
            USBD_SwReset();
        }
        if(u32State & USBD_STATE_SUSPEND)
        {
            /* Enable USB but disable PHY */
            USBD_DISABLE_PHY();
        }
        if(u32State & USBD_STATE_RESUME)
        {
            /* Enable USB and enable PHY */
            USBD_ENABLE_USB();
        }
    }

//------------------------------------------------------------------
    if(u32IntSts & USBD_INTSTS_USB)
    {
        // USB event
        if(u32IntSts & USBD_INTSTS_SETUP)
        {
            // Setup packet
            /* Clear event flag */
            USBD_CLR_INT_FLAG(USBD_INTSTS_SETUP);

            /* Clear the data IN/OUT ready flag of control end-points */
            USBD_STOP_TRANSACTION(EP0);
            USBD_STOP_TRANSACTION(EP1);

            USBD_ProcessSetupPacket();
        }

        // EP events
        if(u32IntSts & USBD_INTSTS_EP0)
        {
            /* Clear event flag */
            USBD_CLR_INT_FLAG(USBD_INTSTS_EP0);

            // control IN
            USBD_CtrlIn();
        }

        if(u32IntSts & USBD_INTSTS_EP1)
        {
            /* Clear event flag */
            USBD_CLR_INT_FLAG(USBD_INTSTS_EP1);

            // control OUT
            USBD_CtrlOut();
        }

        if(u32IntSts & USBD_INTSTS_EP2)
        {
            /* Clear event flag */
            USBD_CLR_INT_FLAG(USBD_INTSTS_EP2);

            // Isochronous IN
            EP2_Handler();
        }

        if(u32IntSts & USBD_INTSTS_EP3)
        {
            /* Clear event flag */
            USBD_CLR_INT_FLAG(USBD_INTSTS_EP3);

            // Isochronous OUT
            EP3_Handler();
        }

        if(u32IntSts & USBD_INTSTS_EP4)
        {
            /* Clear event flag */
            USBD_CLR_INT_FLAG(USBD_INTSTS_EP4);
        }

        if(u32IntSts & USBD_INTSTS_EP5)
        {
            /* Clear event flag */
            USBD_CLR_INT_FLAG(USBD_INTSTS_EP5);
        }

    }

}

/**
 * @brief       EP2 Handler
 *
 * @param[in]   None
 *
 * @return      None
 *
 * @details     This function is used to process EP2 event for recording.
 */

void EP2_Handler(void)
{
    /* ISO IN transfer ACK */
    if(g_usbd_UsbAudioState == UAC_START_AUDIO_RECORD)
    {
        UAC_DeviceEnable(UAC_MICROPHONE);
        g_usbd_UsbAudioState = UAC_PROCESSING_AUDIO_RECORD;
    }
    else if(g_usbd_UsbAudioState == UAC_PROCESSING_AUDIO_RECORD)
        g_usbd_UsbAudioState = UAC_BUSY_AUDIO_RECORD;

    if(g_usbd_UsbAudioState == UAC_BUSY_AUDIO_RECORD)
        UAC_SendRecData();
    else
        USBD_SET_PAYLOAD_LEN(EP2, 0);

}

/**
 * @brief       EP3 Handler (ISO OUT interrupt handler)
 *
 * @param[in]   None
 *
 * @return      None
 *
 * @details     This function is used to process EP3 event (ISO OUT transfer ACK) for play audio.
 */
void EP3_Handler(void)
{

    uint32_t u32Len;
    int32_t i;
    uint8_t *pu8Buf;
    uint8_t *pu8Src;
    uint32_t u32Idx;

    /* Get the address in USB buffer */
    pu8Src = (uint8_t *)((uint32_t)USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP3));

    /* Prepare for nex OUT packet */
    USBD_SET_PAYLOAD_LEN(EP3, EP3_MAX_PKT_SIZE);

    /* Get the temp buffer */
    pu8Buf = (uint8_t *)g_au32UsbTmpBuf;

    /* Calculate byte size of play data */
    u32Len = PLAY_RATE / 1000 * PLAY_CHANNELS * 2;

    /* Copy all data from USB buffer to SRAM buffer */
    /* We assume the source data are 4 bytes alignment. */
    for(i = 0; i < u32Len; i += 4)
    {
        pu8Buf[i] = pu8Src[i];
        pu8Buf[i + 1] = pu8Src[i + 1];
        pu8Buf[i + 2] = pu8Src[i + 2];
        pu8Buf[i + 3] = pu8Src[i + 3];
    }

    /* Calculate word length */
    u32Len = u32Len >> 2;
    for(i = 0; i < u32Len; i++)
    {
        /* Check ring buffer turn around */
        u32Idx = g_u32PlayPos_In + 1;
        if(u32Idx >= BUF_LEN)
            u32Idx = 0;

        /* Check if buffer full */
        if(u32Idx != g_u32PlayPos_Out)
        {
            /* Update play ring buffer only when it is not full */
            g_au32PcmPlayBuf[u32Idx] = g_au32UsbTmpBuf[i];

            /* Update IN index */
            g_u32PlayPos_In = u32Idx;
        }
    }

    if(g_u8PlayEn == 0)
    {
        /* Start play data output through I2S only when we have enough data in buffer */
        if(GetSamplesInBuf() > BUF_LEN / 2)
            g_u8PlayEn = 1;
    }


}



/*--------------------------------------------------------------------------*/
/**
 * @brief       UAC Class Initial
 *
 * @param[in]   None
 *
 * @return      None
 *
 * @details     This function is used to configure endpoints for UAC class
 */
void UAC_Init(void)
{
    /* Init setup packet buffer */
    /* Buffer for setup packet -> [0 ~ 0x7] */
    USBD->STBUFSEG = SETUP_BUF_BASE;

    /*****************************************************/
    /* EP0 ==> control IN endpoint, address 0 */
    USBD_CONFIG_EP(EP0, USBD_CFG_CSTALL | USBD_CFG_EPMODE_IN | 0);
    /* Buffer range for EP0 */
    USBD_SET_EP_BUF_ADDR(EP0, EP0_BUF_BASE);

    /* EP1 ==> control OUT endpoint, address 0 */
    USBD_CONFIG_EP(EP1, USBD_CFG_CSTALL | USBD_CFG_EPMODE_OUT | 0);
    /* Buffer range for EP1 */
    USBD_SET_EP_BUF_ADDR(EP1, EP1_BUF_BASE);

    /*****************************************************/
    /* EP2 ==> Isochronous IN endpoint, address 1 */
    USBD_CONFIG_EP(EP2, USBD_CFG_EPMODE_IN | ISO_IN_EP_NUM | USBD_CFG_TYPE_ISO);
    /* Buffer offset for EP2 */
    USBD_SET_EP_BUF_ADDR(EP2, EP2_BUF_BASE);

    /*****************************************************/
    /* EP3 ==> Isochronous OUT endpoint, address 2 */
    USBD_CONFIG_EP(EP3, USBD_CFG_EPMODE_OUT | ISO_OUT_EP_NUM | USBD_CFG_TYPE_ISO);
    /* Buffer offset for EP3 */
    USBD_SET_EP_BUF_ADDR(EP3, EP3_BUF_BASE);
    /* trigger receive OUT data */
    USBD_SET_PAYLOAD_LEN(EP3, EP3_MAX_PKT_SIZE);
}


/**
 * @brief       UAC class request
 *
 * @param[in]   None
 *
 * @return      None
 *
 * @details     This function is used to process UAC class requests
 */
void UAC_ClassRequest(void)
{
    uint8_t buf[8];

    USBD_GetSetupPacket(buf);

    if(buf[0] & 0x80)    /* request data transfer direction */
    {
        // Device to host
        switch(buf[1])
        {
            case UAC_GET_CUR:
            {
                switch(buf[3])
                {
                    case MUTE_CONTROL:
                    {
                        if(REC_FEATURE_UNITID == buf[5])
                            M8(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP0)) = g_usbd_RecMute;
                        else if(PLAY_FEATURE_UNITID == buf[5])
                            M8(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP0)) = g_usbd_PlayMute;

                        /* Data stage */
                        USBD_SET_DATA1(EP0);
                        USBD_SET_PAYLOAD_LEN(EP0, 1);
                        break;
                    }
                    case VOLUME_CONTROL:
                    {
                        if(REC_FEATURE_UNITID == buf[5])
                        {
                            /* Left or right channel */
                            if(buf[2] == 1)
                            {
                                M8(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP0)) = g_usbd_RecVolumeL;
                                M8(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP0) + 1) = g_usbd_RecVolumeL >> 8;
                            }
                            else
                            {
                                M8(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP0)) = g_usbd_RecVolumeR;
                                M8(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP0) + 1) = g_usbd_RecVolumeR >> 8;
                            }

                        }
                        else if(PLAY_FEATURE_UNITID == buf[5])
                        {
                            /* Left or right channel */
                            if(buf[2] == 1)
                            {
                                M8(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP0)) = g_usbd_PlayVolumeL;
                                M8(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP0) + 1) = g_usbd_PlayVolumeL >> 8;
                            }
                            else
                            {
                                M8(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP0)) = g_usbd_PlayVolumeR;
                                M8(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP0) + 1) = g_usbd_PlayVolumeR >> 8;
                            }
                        }
                        /* Data stage */
                        USBD_SET_DATA1(EP0);
                        USBD_SET_PAYLOAD_LEN(EP0, 2);
                        break;
                    }
                    default:
                    {
                        /* Setup error, stall the device */
                        USBD_SetStall(0);
                    }
                }
                // Trigger next Control Out DATA1 Transaction.
                /* Status stage */
                USBD_PrepareCtrlOut(0, 0);
                break;
            }

            case UAC_GET_MIN:
            {
                switch(buf[3])
                {
                    case VOLUME_CONTROL:
                    {
                        if(REC_FEATURE_UNITID == buf[5])
                        {
                            M8(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP0)) = g_usbd_RecMinVolume;
                            M8(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP0) + 1) = g_usbd_RecMinVolume >> 8;
                        }
                        else if(PLAY_FEATURE_UNITID == buf[5])
                        {
                            M8(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP0)) = g_usbd_PlayMinVolume;
                            M8(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP0) + 1) = g_usbd_PlayMinVolume >> 8;
                        }
                        /* Data stage */
                        USBD_SET_DATA1(EP0);
                        USBD_SET_PAYLOAD_LEN(EP0, 2);
                        break;
                    }
                    default:
                        /* STALL control pipe */
                        USBD_SetStall(0);
                }
                // Trigger next Control Out DATA1 Transaction.
                /* Status stage */
                USBD_PrepareCtrlOut(0, 0);
                break;
            }

            case UAC_GET_MAX:
            {
                switch(buf[3])
                {
                    case VOLUME_CONTROL:
                    {
                        if(REC_FEATURE_UNITID == buf[5])
                        {
                            M8(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP0)) = g_usbd_RecMaxVolume;
                            M8(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP0) + 1) = g_usbd_RecMaxVolume >> 8;
                        }
                        else if(PLAY_FEATURE_UNITID == buf[5])
                        {
                            M8(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP0)) = g_usbd_PlayMaxVolume;
                            M8(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP0) + 1) = g_usbd_PlayMaxVolume >> 8;
                        }
                        /* Data stage */
                        USBD_SET_DATA1(EP0);
                        USBD_SET_PAYLOAD_LEN(EP0, 2);
                        break;
                    }
                    default:
                        /* STALL control pipe */
                        USBD_SetStall(0);
                }
                // Trigger next Control Out DATA1 Transaction.
                /* Status stage */
                USBD_PrepareCtrlOut(0, 0);
                break;
            }

            case UAC_GET_RES:
            {
                switch(buf[3])
                {
                    case VOLUME_CONTROL:
                    {
                        if(REC_FEATURE_UNITID == buf[5])
                        {
                            M8(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP0)) = g_usbd_RecResVolume;
                            M8(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP0) + 1) = g_usbd_RecResVolume >> 8;
                        }
                        else if(PLAY_FEATURE_UNITID == buf[5])
                        {
                            M8(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP0)) = g_usbd_PlayResVolume;
                            M8(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP0) + 1) = g_usbd_PlayResVolume >> 8;
                        }
                        /* Data stage */
                        USBD_SET_DATA1(EP0);
                        USBD_SET_PAYLOAD_LEN(EP0, 2);
                        break;
                    }
                    default:
                        /* STALL control pipe */
                        USBD_SetStall(0);
                }
                // Trigger next Control Out DATA1 Transaction.
                /* Status stage */
                USBD_PrepareCtrlOut(0, 0);
                break;
            }

            default:
            {
                /* Setup error, stall the device */
                USBD_SetStall(0);
            }
        }
    }
    else
    {
        // Host to device
        switch(buf[1])
        {
            case UAC_SET_CUR:
            {
                switch(buf[3])
                {
                    case MUTE_CONTROL:
                        if(REC_FEATURE_UNITID == buf[5])
                            USBD_PrepareCtrlOut((uint8_t *)&g_usbd_RecMute, buf[6]);
                        else if(PLAY_FEATURE_UNITID == buf[5])
                        {
                            USBD_PrepareCtrlOut((uint8_t *)&g_usbd_PlayMute, buf[6]);
                        }
                        /* Status stage */
                        USBD_SET_DATA1(EP0);
                        USBD_SET_PAYLOAD_LEN(EP0, 0);
                        break;

                    case VOLUME_CONTROL:
                        if(REC_FEATURE_UNITID == buf[5])
                        {
                            if(buf[2] == 1)
                            {
                                /* Prepare the buffer for new record volume of left channel */
                                USBD_PrepareCtrlOut((uint8_t *)&g_usbd_RecVolumeL, buf[6]);
                            }
                            else
                            {
                                /* Prepare the buffer for new record volume of right channel */
                                USBD_PrepareCtrlOut((uint8_t *)&g_usbd_RecVolumeR, buf[6]);
                            }
                        }
                        else if(PLAY_FEATURE_UNITID == buf[5])
                        {
                            if(buf[2] == 1)
                            {
                                /* Prepare the buffer for new play volume of left channel */
                                USBD_PrepareCtrlOut((uint8_t *)&g_usbd_PlayVolumeL, buf[6]);
                            }
                            else
                            {
                                /* Prepare the buffer for new play volume of right channel */
                                USBD_PrepareCtrlOut((uint8_t *)&g_usbd_PlayVolumeR, buf[6]);
                            }
                        }
                        /* Status stage */
                        USBD_SET_DATA1(EP0);
                        USBD_SET_PAYLOAD_LEN(EP0, 0);
                        break;

                    default:
                        /* STALL control pipe */
                        USBD_SetStall(0);
                        break;
                }
                break;
            }

            default:
            {
                /* Setup error, stall the device */
                USBD_SetStall(0);
                break;
            }
        }
    }
}

/**
 * @brief       Set Interface standard request
 *
 * @param[in]   u32AltInterface Interface
 *
 * @return      None
 *
 * @details     This function is used to set UAC Class relative setting
 */
void UAC_SetInterface(void)
{
    uint8_t buf[8];
    uint32_t u32AltInterface;

    USBD_GetSetupPacket(buf);

    u32AltInterface = buf[2];

    if(buf[4] == 1)
    {
        /* Audio Iso IN interface */
        if(u32AltInterface == 1)
        {
            g_usbd_UsbAudioState = UAC_START_AUDIO_RECORD;
            USBD_SET_DATA1(EP2);
            USBD_SET_PAYLOAD_LEN(EP2, 0);
            UAC_DeviceEnable(UAC_MICROPHONE);

        }
        else if(u32AltInterface == 0)
        {
            UAC_DeviceDisable(UAC_MICROPHONE);
            USBD_SET_DATA1(EP2);
            USBD_SET_PAYLOAD_LEN(EP2, 0);
            g_usbd_UsbAudioState = UAC_STOP_AUDIO_RECORD;
        }
    }
    else if(buf[4] == 2)
    {
        /* Audio Iso OUT interface */
        if(u32AltInterface == 1)
        {
            USBD_SET_PAYLOAD_LEN(EP3, EP3_MAX_PKT_SIZE);
            UAC_DeviceEnable(UAC_SPEAKER);
        }
        else
            UAC_DeviceDisable(UAC_SPEAKER);
    }
}


/*---------------------------------------------------------------------------------------------------------*/
/*  Write 9-bit data to 7-bit address register of WAU8822 with I2C0                                        */
/*---------------------------------------------------------------------------------------------------------*/
void I2C_WriteWAU8822(uint8_t u8addr, uint16_t u16data)
{
    /* Send START */
    I2C_START(I2C0);
    I2C_WAIT_READY(I2C0);

    /* Send device address */
    I2C_SET_DATA(I2C0, 0x1A << 1);
    I2C_SET_CONTROL_REG(I2C0, I2C_I2CON_SI);
    I2C_WAIT_READY(I2C0);

    /* Send register number and MSB of data */
    I2C_SET_DATA(I2C0, (uint8_t)((u8addr << 1) | (u16data >> 8)));
    I2C_SET_CONTROL_REG(I2C0, I2C_I2CON_SI);
    I2C_WAIT_READY(I2C0);

    /* Send data */
    I2C_SET_DATA(I2C0, (uint8_t)(u16data & 0x00FF));
    I2C_SET_CONTROL_REG(I2C0, I2C_I2CON_SI);
    I2C_WAIT_READY(I2C0);

    /* Send STOP */
    I2C_STOP(I2C0);

}


void WAU8822_Setup(void)
{
    I2C_WriteWAU8822(0,  0x000);   /* Reset all registers */
    CLK_SysTickDelay(10000);

    I2C_WriteWAU8822(1,  0x0EF);
    I2C_WriteWAU8822(2,  0x1BF);   /* Enable L/R Headphone, ADC Mix/Boost, ADC */
    I2C_WriteWAU8822(3,  0x07F);   /* Enable L/R main mixer, DAC */
    I2C_WriteWAU8822(4,  0x010);   /* 16-bit word length, I2S format, Stereo */
    I2C_WriteWAU8822(5,  0x000);   /* Companding control and loop back mode (all disable) */

#if(PLAY_RATE == 48000)
    I2C_WriteWAU8822(6,  0x14D);   /* Divide by 2, 48K */
    I2C_WriteWAU8822(7,  0x000);   /* 48K for internal filter coefficients */
#elif(PLAY_RATE == 32000)
    I2C_WriteWAU8822(6,  0x16D);   /* Divide by 3, 32K */
    I2C_WriteWAU8822(7,  0x002);   /* 32K for internal filter coefficients */
#elif(PLAY_RATE == 16000)
    I2C_WriteWAU8822(6,  0x1AD);   /* Divide by 6, 16K */
    I2C_WriteWAU8822(7,  0x006);   /* 16K for internal filter coefficients */
#else
    I2C_WriteWAU8822(6,  0x1ED);   /* Divide by 12, 8K */
    I2C_WriteWAU8822(7,  0x00A);   /* 8K for internal filter coefficients */
#endif

    I2C_WriteWAU8822(10, 0x008);   /* DAC soft mute is disabled, DAC oversampling rate is 128x */
    I2C_WriteWAU8822(14, 0x108);   /* ADC HP filter is disabled, ADC oversampling rate is 128x */
    I2C_WriteWAU8822(15, 0x0FF);   /* ADC left digital volume control */
    I2C_WriteWAU8822(16, 0x1FF);   /* ADC right digital volume control */

    I2C_WriteWAU8822(44, 0x000);   /* LLIN/RLIN is not connected to PGA */
    I2C_WriteWAU8822(45, 0x090);   /* LAUXIN connected, and its Gain value is 0dB */
    I2C_WriteWAU8822(46, 0x190);   /* RAUXIN connected, and its Gain value is 0dB */
    I2C_WriteWAU8822(47, 0x005);   /* LAUXIN connected, and its Gain value is 0dB */
    I2C_WriteWAU8822(48, 0x005);   /* RAUXIN connected, and its Gain value is 0dB */
    I2C_WriteWAU8822(50, 0x001);   /* Left DAC connected to LMIX */
    I2C_WriteWAU8822(51, 0x001);   /* Right DAC connected to RMIX */

    I2C_WriteWAU8822(52, 0x039);   /* HP Volume */
    I2C_WriteWAU8822(53, 0x139);   /* HP Volume */

    I2C_WriteWAU8822(54, 0x140);   /* LSPKOUT Volume */
    I2C_WriteWAU8822(55, 0x140);   /* RSPKOUT Volume */

    GPIO_SetMode(PE, (1 << 14), GPIO_PMD_OUTPUT);
    PE14 = 0;

}



void I2S_IRQHandler(void)
{
    uint32_t u32I2SIntFlag;
    uint32_t i, u32Idx;

    u32I2SIntFlag = I2S->STATUS;//  I2S_GET_INT_FLAG(I2S, I2S_STATUS_I2STXINT_Msk | I2S_STATUS_I2SRXINT_Msk);

    if(u32I2SIntFlag & I2S_STATUS_TXTHF_Msk)
    {

        /* Fill 4 word data when it is Tx threshold interrupt */
        for(i = 0; i < 4; i++)
        {
            /* Check buffer empty */
            if((g_u32PlayPos_Out != g_u32PlayPos_In) && g_u8PlayEn)
            {

                /* Check ring buffer trun around */
                u32Idx = g_u32PlayPos_Out + 1;
                if(u32Idx >= BUF_LEN)
                    u32Idx = 0;

                /* Play to I2S */
                I2S_WRITE_TX_FIFO(I2S, g_au32PcmPlayBuf[u32Idx]);

                /* Update OUT index */
                g_u32PlayPos_Out = u32Idx;
            }
            else
            {
                /* Fill 0x0 when buffer is empty */
                I2S_WRITE_TX_FIFO(I2S, 0x00);

                /* Buffer underrun. Disable play */
                g_u8PlayEn = 0;
            }
        }

    }

    if(u32I2SIntFlag & I2S_STATUS_RXTHF_Msk)
    {
        static int16_t i16Cnt = 0;
        if((g_u32RecPos < 96) && g_u8RecEn)
        {

            g_au32PcmRecBuf[g_u32RecPos  ] = I2S_READ_RX_FIFO(I2S);
            g_au32PcmRecBuf[g_u32RecPos + 1] = I2S_READ_RX_FIFO(I2S);
            g_au32PcmRecBuf[g_u32RecPos + 2] = I2S_READ_RX_FIFO(I2S);
            g_au32PcmRecBuf[g_u32RecPos + 3] = I2S_READ_RX_FIFO(I2S);
            g_u32RecPos += 4;
            if(i16Cnt > 512) i16Cnt = -512;
        }
        else
        {
            I2S_READ_RX_FIFO(I2S);
            I2S_READ_RX_FIFO(I2S);
            I2S_READ_RX_FIFO(I2S);
            I2S_READ_RX_FIFO(I2S);
        }
    }

}



/**
  * @brief  SendRecData, prepare the record data for next ISO transfer.
  * @param  None.
  * @retval None.
  */
void UAC_SendRecData(void)
{
    uint8_t *pu8Buf;
    uint32_t u32Size;
    int32_t i;

    /* Get the address in USB buffer */
    pu8Buf = (uint8_t *)((uint32_t)USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP2));

    /* Prepare the data to USB IN buffer */
    u32Size = g_u32RecPos * 4;
    if(u32Size > EP2_MAX_PKT_SIZE) u32Size = EP2_MAX_PKT_SIZE;
    USBD_MemCopy(pu8Buf, (uint8_t *)g_au32PcmRecBuf, u32Size);

    /* Trigger ISO IN */
    USBD_SET_PAYLOAD_LEN(EP2, u32Size);

    __set_PRIMASK(1);
    /*g_au32PcmRecBuf is not ring buffer. We need to copy remained data to buffer front */
    g_u32RecPos -= (u32Size / 4);
    if(g_u32RecPos)
    {
        for(i = 0; i < g_u32RecPos; i++)
            g_au32PcmRecBuf[i] = g_au32PcmRecBuf[i + u32Size / 4];
    }
    __set_PRIMASK(0);

}


/**
  * @brief  UAC_DeviceEnable. To enable the device to play or record audio data.
  * @param  u8Object: To select the device, UAC_MICROPHONE or UAC_SPEAKER.
  * @retval None.
  */
void UAC_DeviceEnable(uint8_t u8Object)
{
    if(u8Object == UAC_MICROPHONE)
    {
        /* Enable record hardware */
        g_u8RecEn = 1;

        if(g_u8RecEn == 0)
        {
            /* Reset record buffer */
            memset(g_au32PcmRecBuf, 0, sizeof(g_au32PcmRecBuf));
            g_u32RecPos = 0;
        }

    }
    else
    {
        /* Eanble play hardware */

        /* Reset Play buffer */
        if(g_u8PlayEn == 0)
        {
            /* Fill 0x0 to buffer before playing for buffer operation smooth */
            memset(g_au32PcmPlayBuf, 0, sizeof(g_au32PcmPlayBuf));
            g_u32PlayPos_In = BUF_LEN / 2;
            g_u32PlayPos_Out = 0;
            g_u8PlayEn = 1;
        }

    }
}


/**
  * @brief  UAC_DeviceDisable. To disable the device to play or record audio data.
  * @param  u8Object: To select the device, UAC_MICROPHONE or UAC_SPEAKER.
  * @retval None.
  */
void UAC_DeviceDisable(uint8_t u8Object)
{
    if(u8Object ==  UAC_MICROPHONE)
    {
        /* Disable record hardware/stop record */
        g_u8RecEn = 0;
    }
    else
    {
        /* Disable play hardware/stop play */
        g_u8PlayEn = 0;
    }
}



void AdjustCodecPll(RESAMPLE_STATE_T r)
{
    static uint16_t tb[3][3] = {{0x00C, 0x093, 0x0E9}, // 8.192
        {0x00E, 0x1D2, 0x1E3},  // * 1.005 = 8.233
        {0x009, 0x153, 0x1EF}
    }; // * .995 = 8.151
    static RESAMPLE_STATE_T current = E_RS_NONE;
    int i, s;

    if(r == current)
        return;
    else
        current = r;
    switch(r)
    {
        case E_RS_UP:
            s = 1;
            break;
        case E_RS_DOWN:
            s = 2;
            break;
        case E_RS_NONE:
        default:
            s = 0;
    }

    for(i = 0; i < 3; i++)
        I2C_WriteWAU8822(37 + i, tb[s][i]);
}



void AdjFreq(void)
{
    uint32_t u32Size;
    static int32_t i32PreFlag = 0;
    static int32_t i32Cnt = 0;

    /* Only adjust the frequency when play data */
    if(g_u8PlayEn == 0)
        return;

    /* Get sample size in play buffer */
    u32Size = GetSamplesInBuf();


    if(g_i32AdjFlag == 0)
    {
        /* Check if we need to adjust the frequency when we didn't in adjusting state */
        if(u32Size > (BUF_LEN * 3 / 4))
        {
            /* USB rate > I2S rate. So we increase I2S rate here */
            AdjustCodecPll(E_RS_UP);
            g_i32AdjFlag = -1;
        }
        else if(u32Size < (BUF_LEN * 1 / 4))
        {
            /* USB rate < I2S rate. So we decrease I2S rate here */
            AdjustCodecPll(E_RS_DOWN);
            g_i32AdjFlag = 1;
        }
    }
    else
    {
        /* Check if we need to stop adjust the frequency when we are in adjusting state */
        if((g_i32AdjFlag > 0) && (u32Size > BUF_LEN / 2))
        {
            AdjustCodecPll(E_RS_NONE);
            g_i32AdjFlag = 0;
        }

        if((g_i32AdjFlag < 0) && (u32Size < BUF_LEN / 2))
        {
            AdjustCodecPll(E_RS_NONE);
            g_i32AdjFlag = 0;
        }
    }

    /* Show adjustment, buffer, volume status */
    if(i32PreFlag != g_i32AdjFlag || (i32Cnt++ > 40000))
    {
        i32PreFlag = g_i32AdjFlag;
        printf("%d %d %d %d\n", g_i32AdjFlag, u32Size, g_usbd_PlayVolumeL, g_usbd_RecVolumeL);
        i32Cnt = 0;
    }

}

void VolumnControl(void)
{
    static uint8_t u8PrePlayMute = 0;
    static uint8_t u8PreRecMute = 0;
    static int16_t i16PrePlayVolumeL = 0;
    static int16_t i16PrePlayVolumeR = 0;
    static int16_t i16PreRecVolumeL = 0;
    static int16_t i16PreRecVolumeR = 0;
    uint8_t IsChange = 0;
    uint32_t u32R52, u32R53;
    uint32_t u32R15, u32R16;

    /*
        g_usbd_PlayMute is used for MUTE control. 0 = not MUTE. 1 = MUTE.
        g_usbd_PlayVolumeL is volume of left channel. Range is -32768 ~ 32767
        g_usbd_PlayVolumeR is volume of right channel. Range is -32768 ~ 32767

        NAU8822 LHPGAIN (R52) = -57dB ~ +6dB. Code is 0x0 ~ 0x3F.
        NAU8822 RHPGAIN (R53) = -57dB ~ +6dB. Code is 0x0 ~ 0x3F.
        Play volume mapping to code will be (Volume >> 10)+32

        NAU8822 LADCGAIN (R15) = MUTE, -127dB ~ 0dB. Code is 0x0, 0x1 ~ 0xFF.
        NAU8822 RADCGAIN (R16) = MUTE, -127dB ~ 0dB. Code is 0x0, 0x1 ~ 0xFF.
        Record volume mapping to code will be (Volume >> 8)+128
    */

    u32R52 = 0;
    u32R53 = 0;

    /* Update MUTE and volume to u32R52/53 when MUTE changed for play */
    if(u8PrePlayMute != g_usbd_PlayMute)
    {
        u8PrePlayMute = g_usbd_PlayMute;
        u32R52 = u32R52 | (g_usbd_PlayMute << 6);
        u32R53 = u32R53 | (g_usbd_PlayMute << 6);
        i16PrePlayVolumeL = g_usbd_PlayVolumeL;
        u32R52 |= (g_usbd_PlayVolumeL >> 10) + 32;
        i16PrePlayVolumeR = g_usbd_PlayVolumeR;
        u32R53 |= (g_usbd_PlayVolumeL >> 10) + 32;
        IsChange |= 3;
    }

    /* Update left volume to u32R52 when left volume changed for play */
    if(i16PrePlayVolumeL != g_usbd_PlayVolumeL)
    {
        i16PrePlayVolumeL = g_usbd_PlayVolumeL;
        u32R52 |= (g_usbd_PlayVolumeL >> 10) + 32;
        IsChange |= 1;
    }

    /* Update right volume to u32R53 when left volume changed for play */
    if(i16PrePlayVolumeR != g_usbd_PlayVolumeR)
    {
        i16PrePlayVolumeR = g_usbd_PlayVolumeR;
        u32R53 |= (g_usbd_PlayVolumeR >> 10) + 32;
        IsChange |= 2;
    }

    u32R15 = 0;
    u32R16 = 0;

    /* Update MUTE and volume to u32R15/16 when MUTE changed for record */
    if(u8PreRecMute != g_usbd_RecMute)
    {
        u8PreRecMute = g_usbd_RecMute;

        if(!g_usbd_RecMute)
        {
            i16PreRecVolumeL = g_usbd_RecVolumeL;
            i16PreRecVolumeR = g_usbd_RecVolumeR;
            u32R15 |= (g_usbd_RecVolumeL >> 8) + 128;
            u32R16 |= (g_usbd_RecVolumeR >> 8) + 128;
        }

        IsChange |= 0xc;
    }

    /* Update left volume to u32R15 when left volume changed for record */
    if(i16PreRecVolumeL != g_usbd_RecVolumeL)
    {
        i16PreRecVolumeL = g_usbd_RecVolumeL;
        u32R15 |= (g_usbd_RecVolumeL >> 8) + 128;
        IsChange |= 4;
    }

    /* Update right volume to u32R16 when left volume changed for record */
    if(i16PreRecVolumeR != g_usbd_RecVolumeR)
    {
        i16PreRecVolumeR = g_usbd_RecVolumeR;
        u32R16 |= (g_usbd_RecVolumeR >> 8) + 128;
        IsChange |= 8;
    }

    /* Update R52, R53 when MUTE or volume changed */
    if((IsChange & 3) == 3)
    {
        /* Both channels need to be changed */
        I2C_WriteWAU8822(52, u32R52);
        I2C_WriteWAU8822(53, u32R53 | 0x100);
        IsChange ^= 3;
    }
    else if(IsChange & 1)
    {
        /* Only change left channel */
        I2C_WriteWAU8822(52, u32R52 | 0x100);
        IsChange ^= 1;
    }
    else if(IsChange & 2)
    {
        /* Only change right channel */
        I2C_WriteWAU8822(53, u32R53 | 0x100);
        IsChange ^= 2;
    }

    /* Update R15, R16 when MUTE or volume changed */
    if((IsChange & 0xc) == 0xc)
    {
        /* Both channels need to be changed */
        I2C_WriteWAU8822(15, u32R15);
        I2C_WriteWAU8822(16, u32R16 | 0x100);
        IsChange ^= 0xc;
    }
    else if(IsChange & 4)
    {
        /* Only change left channel */
        I2C_WriteWAU8822(15, u32R15 | 0x100);
        IsChange ^= 4;
    }
    else if(IsChange & 8)
    {
        /* Only change right channel */
        I2C_WriteWAU8822(16, u32R16 | 0x100);
        IsChange ^= 8;
    }

}




