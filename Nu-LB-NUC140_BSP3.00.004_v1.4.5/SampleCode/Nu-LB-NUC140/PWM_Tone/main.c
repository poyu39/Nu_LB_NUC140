//
// PWM_Tone : PWM0 Channel0 output single tone to speaker (Buzzer)
//
// EVB : Nu-LB-NUC140
// MCU : NUC140VE3CN
// Buzzer: low-active, 2.4KHz beep
//
// PA12/PWM0 connected to PB11 (Buzzer)
//
#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"

#define Sound_Freq 1000
#define Sound_Duration 1000000

void Init_PWM(void)
{
    PWM_EnableOutput(PWM0, PWM_CH_0_MASK);
    PWM_Start(PWM0, PWM_CH_0_MASK);
}

void Sound(uint32_t frequency, uint32_t duration)
{
    PWM_ConfigOutputChannel(PWM0, PWM_CH0, frequency, 90); // duty cycle = 90%
    CLK_SysTickDelay(duration);
}

int32_t main (void)
{
    SYS_Init();
    Init_PWM();
	
    while(1) {
       Sound(Sound_Freq, Sound_Duration);	
    }
}

