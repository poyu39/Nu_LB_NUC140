//
// I2C_MC5883L : using I2C to read HMC5883 magnentometer (GY271)
//
// EVB    : Nu-LB-NUC140
// MCU    : NUC140VE3CN (LQFP100)
// Sensor : HMC5883 magnentometer (GY271)
//
// HMC5883 Connections :
// VCC : to VCC33 (3.3V)
// GND : to Gnd
// SCL : to I2C0-SCL / PA9
// SDA : to I2C0-SDA / PA8

#include <stdio.h>
#include <math.h>
#include <string.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "LCD.h"
#include "HMC5883L.h"

int32_t main (void)
{
  char Text[16];
	VecInt16 m;
	
  SYS_Init();

  init_LCD(); 
  clear_LCD();
  print_Line(0,"Magnetometer");

  NVT_I2C_Init(I2C0_CLOCK_FREQUENCY);
	HMC5883L_begin();
		
  while(1) {	
		m=HMC5883L_readRaw();

		sprintf(Text, "mx: %6d", m.X);
    print_Line(1,Text);
    sprintf(Text, "my: %6d", m.Y);
    print_Line(2,Text);
    sprintf(Text, "mz: %6d", m.Z);
    print_Line(3,Text);	
  }
}
