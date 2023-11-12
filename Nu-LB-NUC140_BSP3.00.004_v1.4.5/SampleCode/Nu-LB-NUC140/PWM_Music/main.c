//
// PWM_Music : PWM0 Ch0 (PA12) output music "Fur Elise"
//
// EVB : Nu-LB-NUC140
// MCU : NUC140VE3CN  (LQPF-100)
// Buzzer: used as speaker

// PA12/PWM0 connected to PB11
#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "Note_Freq.h"

#define  P125ms 125000
#define  P250ms 250000
#define  P500ms 500000
#define  P1S   1000000

int32_t main (void)
{
  uint8_t i;
  
  uint16_t music[72] = {
	E6 ,D6u,E6 ,D6u,E6 ,B5 ,D6 ,C6 ,A5 ,A5 , 0 , 0 ,
	C5 ,E5 ,A5 ,B5 ,B5 , 0 ,C5 ,A5 ,B5 ,C6 ,C6 , 0 ,
	E6 ,D6u,E6 ,D6u,E6 ,B5 ,D6 ,C6 ,A5 ,A5 , 0 , 0 ,
	C5 ,E5 ,A5 ,B5 ,B5 , 0 ,E5 ,C6 ,B5 ,A5 ,A5 , 0 ,
	B5 ,C6 ,D6 ,E6 ,E6 , 0 ,G5 ,F6 ,E6 ,D6 ,D6 , 0 ,
	F5 ,E6 ,D6 ,C6 ,C6 , 0 ,E5 ,D6 ,C6 ,B5 ,B5 , 0 };
	
  uint32_t pitch[72] = {
	P250ms, P250ms, P250ms, P250ms,	P250ms, P250ms, P250ms, P250ms,	P250ms, P250ms, P250ms, P250ms,
	P250ms, P250ms, P250ms, P250ms,	P250ms, P250ms, P250ms, P250ms,	P250ms, P250ms, P250ms, P250ms,
	P250ms, P250ms, P250ms, P250ms,	P250ms, P250ms, P250ms, P250ms,	P250ms, P250ms, P250ms, P250ms,
	P500ms, P500ms, P500ms, P500ms,	P500ms, P500ms, P500ms, P500ms,	P500ms, P500ms, P500ms, P500ms,
	P500ms, P500ms, P500ms, P500ms,	P500ms, P500ms, P500ms, P500ms,	P500ms, P500ms, P500ms, P500ms,
	P500ms, P500ms, P500ms, P500ms,	P500ms, P500ms, P500ms, P500ms,	P500ms, P500ms, P500ms, P500ms
	};

    SYS_Init();

    PWM_EnableOutput(PWM0, PWM_CH_0_MASK);
    PWM_Start(PWM0, PWM_CH_0_MASK);	

	  for (i=0; i<72; i++) {
			PWM_ConfigOutputChannel(PWM0, PWM_CH0, music[i], 90); // 0=Buzzer ON
			if (music[i]!=0) PWM_EnableOutput(PWM0, PWM_CH_0_MASK);
			else             PWM_DisableOutput(PWM0, PWM_CH_0_MASK);
		  CLK_SysTickDelay(pitch[i]);
    }
}
