//
// PWM_DeadZone : PWM output with Deadzone protection
//
// EVB : Nu-LB-NUC140
// MCU : NUC140VE3CN  (LQPF-100)
//
// Function: 
// 1. Enable two pairs of PWM 
//    PWM0-ch0 & 1 (PA12 & PA13)
//    PWM0-ch2 & 3 (PA14 & PA15)
// 2. Enter PWM0 interrupt service routine @100Hz
//    out disable every other second
#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"

void PWM0_IRQHandler(void)
{
    static uint32_t cnt;
    static uint32_t out;

    // Channel 0 frequency is 100Hz, every 1 second enter this IRQ handler 100 times.
    if(++cnt == 100)
    {
        if(out)
            PWM_EnableOutput(PWM0, PWM_CH_0_MASK | PWM_CH_1_MASK | PWM_CH_2_MASK |PWM_CH_3_MASK);
        else
            PWM_DisableOutput(PWM0, PWM_CH_0_MASK | PWM_CH_1_MASK | PWM_CH_2_MASK |PWM_CH_3_MASK);
        out ^= 1; // out = out xor 1
        cnt = 0;
    }
    // Clear channel 0 period interrupt flag
    PWM_ClearPeriodIntFlag(PWM0, 0);
}

void Init_PWM(void)
{
    // PWM0 frequency is 100Hz, duty 30%,
    PWM_ConfigOutputChannel(PWM0, PWM_CH0, 100, 30);
    PWM_EnableDeadZone(PWM0, PWM_CH0, 400);

    // PWM2 frequency is 300Hz, duty 50%
    PWM_ConfigOutputChannel(PWM0, PWM_CH2, 300, 50);
    PWM_EnableDeadZone(PWM0, PWM_CH2, 200);

    // Enable output of all PWM0 channels
    PWM_EnableOutput(PWM0, PWM_CH_0_MASK | PWM_CH_1_MASK | PWM_CH_2_MASK |PWM_CH_3_MASK);

    // Enable PWM0 channel 0 period interrupt, use channel 0 to measure time.
    PWM_EnablePeriodInt(PWM0, PWM_CH0, 0);
    NVIC_EnableIRQ(PWM0_IRQn);

    // Start
    PWM_Start(PWM0, PWM_CH_0_MASK | PWM_CH_1_MASK | PWM_CH_2_MASK |PWM_CH_3_MASK);
}

int main(void)
{
    SYS_Init();
    Init_PWM();

    while(1);
}

