//
// USBD_MassStorage_DataFlash : emulate USB mass storage device using data flash.
//
// EVB : Nu-LB-NUC140
// MCU : NUC140VE3CN
//
#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "massstorage.h"

#define CONFIG_BASE      0x00300000
#define DATA_FLASH_BASE  MASS_STORAGE_OFFSET

int32_t main(void)
{
    uint32_t au32Config[2];
	
    SYS_Init();
    GPIO_SetMode(PC, BIT12, GPIO_MODE_OUTPUT);
	  PC12=1;
    FMC_Open(); // Enable FMC ISP function

    // Check if Data Flash Size is 64K. If not, to re-define Data Flash size and to enable Data Flash function
    if(FMC_ReadConfig(au32Config, 2) < 0)  return -1;

    if(((au32Config[0] & 0x01) == 1) || (au32Config[1] != DATA_FLASH_BASE))
    {
        FMC_EnableConfigUpdate();
        au32Config[0] &= ~0x1;
        au32Config[1] = DATA_FLASH_BASE;
        FMC_Erase(CONFIG_BASE);
        if(FMC_WriteConfig(au32Config, 2) < 0)
            return -1;

        FMC_ReadConfig(au32Config, 2);
        if(((au32Config[0] & 0x01) == 1) || (au32Config[1] != DATA_FLASH_BASE))
        {
            //printf("Error: Program Config Failed!\n");
					  PC12=0; // if Error then turn-on LED
            /* Disable FMC ISP function */
            FMC_Close();
            return -1;
        }

        /* Reset Chip to reload new CONFIG value */
        SYS->IPRSTC1 = SYS_IPRSTC1_CHIP_RST_Msk;
    }

    //printf("NuMicro USB MassStorage Start!\n");

    USBD_Open(&gsInfo, MSC_ClassRequest, NULL);

    USBD_SetConfigCallback(MSC_SetConfig);

    /* Endpoint configuration */
    MSC_Init();
    USBD_Start();
    NVIC_EnableIRQ(USBD_IRQn);

    while(1)
    {
        MSC_ProcessCmd();
#ifdef NONBLOCK_PRINTF
        putchar(0); // Flush the data in software FIFO to UART
#endif
    }
}
