//
// PWM_Siren : PWM0 Channel0 output to speaker for playing Siren
//
// EVB : Nu-LB-NUC140
// MCU : NUC140VE3CN  (LQPF-100)
// Buzzer: used as speaker

// PA12/PWM0 connected to PB11
#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"

#define Ambulance_Siren_Low   650
#define Ambulance_Siren_High  900
#define Ambulance_Siren_L_period 400000 // 0.4 sec
#define Ambulance_Siren_H_period 600000 // 0.6 sec

#define PoliceCar_Siren_Low   650
#define PoliceCar_Siren_High 1450
#define PoliceCar_Siren_L_duration 230000 // 0.23 sec
#define PoliceCar_Siren_H_duration 100000 // 0.1  sec

void Ambulance_Siren(uint32_t siren_no)
{
	uint32_t i;
	PWM_EnableOutput(PWM0, PWM_CH_0_MASK);
	for (i=0; i<siren_no; i++) {
		PWM_ConfigOutputChannel(PWM0, PWM_CH0, Ambulance_Siren_Low, 90);
		CLK_SysTickDelay(Ambulance_Siren_L_period);
		PWM_ConfigOutputChannel(PWM0, PWM_CH0, Ambulance_Siren_High, 90);
		CLK_SysTickDelay(Ambulance_Siren_H_period);	
	}
	PWM_DisableOutput(PWM0, PWM_CH_0_MASK);
}

void PoliceCar_Siren(uint32_t siren_no)
{
	uint32_t i, j;
	uint32_t siren_freq;
	
	PWM_EnableOutput(PWM0, PWM_CH_0_MASK);
	for (i=0; i<siren_no; i++) {
		siren_freq = PoliceCar_Siren_Low;
		for (j=0; j<230; j++) {
			PWM_ConfigOutputChannel(PWM0, PWM_CH0, siren_freq, 90);
		  CLK_SysTickDelay(1000);
			siren_freq = siren_freq + (PoliceCar_Siren_High - PoliceCar_Siren_Low)/230;	
		}		
		siren_freq = PoliceCar_Siren_High;
		for (j=0; j<100; j++) {
			PWM_ConfigOutputChannel(PWM0, PWM_CH0, siren_freq, 90);
		  CLK_SysTickDelay(1000);
			siren_freq = siren_freq - (PoliceCar_Siren_High - PoliceCar_Siren_Low)/100;	
		}			
	}
	PWM_DisableOutput(PWM0, PWM_CH_0_MASK);
}

int32_t main (void)
{
	SYS_Init();
	
  PWM_EnableOutput(PWM0, PWM_CH_0_MASK);
  PWM_Start(PWM0, PWM_CH_0_MASK);
	
	while(1) {
    Ambulance_Siren(5);	
	  PoliceCar_Siren(5);
	}
}
