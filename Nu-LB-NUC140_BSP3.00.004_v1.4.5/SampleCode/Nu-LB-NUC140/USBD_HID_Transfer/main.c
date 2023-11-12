//
// USB_HID_Transfer
//        This sample code demonstrate how to use HID interface to transfer data
//        between PC and USB device.
//        A demo window tool are also included in "WindowsTool" directory with this
//        sample code. User can use it to test data transfer with this sample code.
//
// EVB : Nu-LB-NUC140
// MCU : NUC140VE3CN
// 
#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "hid_transfer.h"

int32_t main(void)
{
    SYS_Init();

    USBD_Open(&gsInfo, HID_ClassRequest, NULL);
    
    HID_Init(); //Init Endpoint configuration for HID

    USBD_Start(); // Start USB device

    NVIC_EnableIRQ(USBD_IRQn); // enable USB device Interrupt

    while(SYS->PDID);
}
