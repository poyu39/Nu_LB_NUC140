//
// USBD_HID_Gamepad : buttons using keypad 3x3
//                    joystick-left = ADC6, joystick-right = ADC7
// EVB : Nu-LB-NUC140
// MCU : NUC140VE3CN
//
#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "hid_gamepad.h"
#include "Scankey.h"
#include "LCD.h"

volatile uint8_t u8ADF;

void ADC_IRQHandler(void)
{
    uint32_t u32Flag;

    // Get ADC conversion finish interrupt flag
    u32Flag = ADC_GET_INT_FLAG(ADC, ADC_ADF_INT);

    if(u32Flag & ADC_ADF_INT) u8ADF = 1;

    ADC_CLR_INT_FLAG(ADC, u32Flag);
}

void Init_ADC(void)
{
    ADC_Open(ADC, ADC_INPUT_MODE, ADC_OPERATION_MODE, ADC_CHANNEL_MASK); // set ADC modes & channels
    ADC_POWER_ON(ADC);                          // Power on ADC
    ADC_EnableInt(ADC, ADC_ADF_INT);            // Enable ADC ADC_IF interrupt
    NVIC_EnableIRQ(ADC_IRQn);	                  // Enable CPU NVIC
}

int32_t main(void)
{
    SYS_Init();
	  init_LCD();
	  clear_LCD();
	  print_Line(0,"USB_Gamepad");
	
    OpenKeyPad(); 
    Init_ADC();
	
    USBD_Open(&gsInfo, HID_ClassRequest, NULL);

    /* Endpoint configuration */
    HID_Init();
    USBD_Start();
    NVIC_EnableIRQ(USBD_IRQn);

    while(1)
    {
        HID_UpdateData();
    }
}

