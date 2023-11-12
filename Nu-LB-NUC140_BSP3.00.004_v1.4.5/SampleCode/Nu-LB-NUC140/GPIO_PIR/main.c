//
// GPIO_PIR : GPIO input to read sensors (Infrared sensor, or button)
//
// Passive InfraRed Sensor connection:
// pin 1 : Vcc (+5V)
// pin 2 : Output connected to PA0 ( 1 = detected, 0 = no detection )
// pin 3 : Gnd

#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"

int main(void)
{
    SYS_Init();
    GPIO_SetMode(PA, BIT0, GPIO_MODE_INPUT);

    while(1) {
      if (PA0==1) printf("PIR detected!\n");
      else        printf("PIR no detection!\n");			
    }
}
