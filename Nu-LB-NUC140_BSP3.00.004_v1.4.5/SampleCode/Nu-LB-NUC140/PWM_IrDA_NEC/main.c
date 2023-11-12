//
// PWM_IrDA_NEC :  IR emitter to transmit and IR receiver to receive NEC code
//
// EVB : Nu-LB-NUC140
// MCU : NUC140VE3CN  (LQPF-100)
// IRrx: VS1838B

// PWM ch0 connected to PWM0 ch2
// PA12/ PWM0_CH0 to IR Emitter (PWM output 38KHz)
// PA13 /PWM0_CH1 to IR Receiver (VS1838B)

#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "IrDA_NEC.h"

uint32_t u32LEDEanble;

void PWM0_IRQHandler(void)
{
    uint32_t TDR_tmp;
    TDR_tmp = MaxValue - PWM_GET_CAPTURE_FALLING_DATA(PWM0, 1);
    PWM_ClearCaptureIntFlag(PWM0, 1, PWM_CAPTURE_INT_FALLING_LATCH);
    IrDA_NEC_Rx(TDR_tmp);
}


int32_t main (void)
{
    uint8_t au8IR_Code[4];

    SYS_Init();

    IrDA_NEC_TxRx_Init();
    printf("NUC140 IrDA NEC Code \n");
	
    au8IR_Code[0] = 0x00;
    au8IR_Code[1] = ~au8IR_Code[0];
    
    while(1)
    {
      au8IR_Code[2] =0x10;
      au8IR_Code[3] = ~au8IR_Code[2];            
      SendNEC(au8IR_Code);
      CLK_SysTickDelay(300000); 
    }
}

