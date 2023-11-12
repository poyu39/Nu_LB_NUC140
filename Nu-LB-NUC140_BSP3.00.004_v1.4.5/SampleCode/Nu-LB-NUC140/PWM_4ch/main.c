//
// PWM_4ch : 4 channel PWM outputs
//
// EVB : Nu-LB-NUC140
// MCU : NUC140VE3CN  (LQPF-100)

#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"

int32_t main(void)
{
    SYS_Init();

    PWM_ConfigOutputChannel(PWM0, PWM_CH0, 370, 44);
    PWM_ConfigOutputChannel(PWM0, PWM_CH1, 370, 44);
    PWM_ConfigOutputChannel(PWM0, PWM_CH2, 370, 44);
    PWM_ConfigOutputChannel(PWM0, PWM_CH3, 370, 44);	
    PWM_EnableOutput(PWM0, 0xF);
    PWM_Start(PWM0, 0xF);
    while(1) {
		};
}
