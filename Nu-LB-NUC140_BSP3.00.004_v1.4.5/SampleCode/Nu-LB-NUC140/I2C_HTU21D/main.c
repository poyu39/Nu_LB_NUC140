//
// I2C_HTU21D : using I2C to read humidity & temperature from sensor
//
// EVB : Nu-LB-NUC140
// MCU : NUC140VE3CN
// HTU21D : Digital Relative Humidity Sensor with Temperature Output

// HTU21D connections
// Vdd : 3.3V
// Gnd : GND
// DA  : to I2C0_SDA/PA8
// CL  : to I2C0_SCL/PA9

#include <stdio.h>
#include <math.h>
#include <string.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "LCD.h"
#include "NVT_I2C.h"
#include "HTU21D.h"	

int32_t main()
{
	char Text[16];
  float    Humidity, Temperature;
	
  SYS_Init();
	init_LCD();
	clear_LCD();
	
	print_Line(0, "HTU21DF");	
  NVT_I2C_Init(I2C0_CLOCK_FREQUENCY); // max.=400KHz
	
  HTU21DF_begin();	
  HTU21DF_reset();
	
  while(1) {
    Humidity    = HTU21DF_Humidity();
    Temperature = HTU21DF_Temperature();
		sprintf(Text, "H =%f", Humidity); 
		print_Line(2, Text);
		sprintf(Text, "T =%f", Temperature);
    print_Line(3, Text);
  }
}
