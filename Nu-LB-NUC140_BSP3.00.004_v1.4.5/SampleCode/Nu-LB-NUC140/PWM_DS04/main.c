//
// PWM_DS04 : PWM output to drive 360-degree rotation DC servo
//
// EVB : Nu-LB-NUC140
// MCU : NUC140VE3CN  (LQPF-100)

// using PWM to generate 50Hz (20ms) pulse to DC Servo signal pin
// 1.0 ~ 1.5ms forward rotation
// 1.5 ~ 2.0ms reverse rotation
// Model  :DS04-NFC
// Weight : 38g
// Size   : 40.8 x 20 x 39.5 mm
// torque : 5.5kg/cm(at 4.8V)
// speed  : 0.22sec/60 degree(at 4.8V)
// Voltage: 4.8v-6v
// Temperature: :0V-60V
// Current:<1000mA

// DS4 DC servo
// pin1 : signal to PWM0/GPA12 (NUC140 LQFP100 pin65)
// pin2 : Vcc
// pin3 : Gnd

// DS4 DC servo
// pin1 : signal to PWM1/GPA13 (NUC140 LQFP100 pin64)
// pin2 : Vcc
// pin3 : Gnd
#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"

#define  SERVO_CYCTIME     2000 // 20ms = 50Hz
#define  FORWARD_PULSE      100 // minimum Hi width = 1.0ms
#define  REVERSE_PULSE      200 // maximum Hi width = 2.0ms

void Init_PWM(void)
{
    PWM_EnableOutput(PWM0, (PWM_CH_0_MASK | PWM_CH_1_MASK));
    PWM_Start(PWM0, (PWM_CH_0_MASK | PWM_CH_1_MASK));
}

int32_t main(void)
{ 
    int i;
    SYS_Init();
    Init_PWM();
	
    while(1) {
      PWM_ConfigOutputChannel(PWM0, PWM_CH0, 50, 5);	
      PWM_ConfigOutputChannel(PWM0, PWM_CH1, 50,10);
      for (i=0; i<2; i++) CLK_SysTickDelay(500000);
      PWM_ConfigOutputChannel(PWM0, PWM_CH0, 50,10);	
      PWM_ConfigOutputChannel(PWM0, PWM_CH1, 50, 5);
      for (i=0; i<2; i++) CLK_SysTickDelay(500000);			
    }
}
