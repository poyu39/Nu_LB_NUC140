//
// I2C_MLX90614 : using I2C to read ambient/object temperature
//
// EVB : Nu-LB-NUC140
// MCU : NUC140VE3CN
// MLX90614 : Infra-Red Thermometer (SMBus : 10K ~ 100KHz)
//            
// MLX90614 connections
// Vin : 5V
// Gnd : GND
// SCL : to I2C0_SCL/PA9
// SDA : to I2C0_SDA/PA8

#include <stdio.h>
#include <math.h>
#include <string.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "LCD.h"
#include "MLX90614.h"	

char Text[16];
float ObjectT, AmbientT;
	
int32_t main()
{	
  SYS_Init();

	init_LCD();
	clear_LCD();
  print_Line(0, "MLX90614");
	
  I2C_Open(I2C0, I2C0_CLOCK_FREQUENCY);

  while(1) {		
    ObjectT  = MLX90614_readObjectTempC();
		sprintf(Text, "Obj=%f", ObjectT); 
		print_Line(2, Text);
		
    AmbientT = MLX90614_readAmbientTempC();		
		sprintf(Text, "Amb=%f", AmbientT); 
		print_Line(3, Text);
  }
}
