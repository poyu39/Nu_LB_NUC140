//
// LCD: display Text on LCD
//
// EVB : Nu-LB-NUC140
// MCU : NUC140VE3CN  (LQPF-100)
#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "LCD.h"

int main(void)
{
	SYS_Init();
	init_LCD();
	clear_LCD();
	
  print_Line(0, "Nu-LB-NUC140    ");
  print_Line(1, "Cortex-M0 @50MHz");
  print_Line(2, "128KB Flash ROM ");
  print_Line(3, "32KB  SRAM      ");	
}
