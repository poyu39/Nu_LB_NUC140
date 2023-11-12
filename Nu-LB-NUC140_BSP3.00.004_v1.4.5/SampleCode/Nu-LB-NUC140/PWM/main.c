//
// PWM : PWM0 Ch0 & Ch1 output 125KHz, duty cycle=50%
//
// EVB : Nu-LB-NUC140
// MCU : NUC140VE3CN  (LQPF-100)

#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"

void Init_PWM(void)
{
    PWM_ConfigOutputChannel(PWM0, PWM_CH0, 125000, 50);
    PWM_ConfigOutputChannel(PWM0, PWM_CH1, 125000, 50);
    PWM_EnableOutput(PWM0, (PWM_CH_0_MASK | PWM_CH_1_MASK));
    PWM_Start(PWM0, (PWM_CH_0_MASK | PWM_CH_1_MASK));
}

int32_t main(void)
{
    SYS_Init();
    Init_PWM();

    while(1);
}
