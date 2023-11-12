//
// GPIO_IRQ : GPIO interrupt triggered by buttons
//
// EVB : Nu-LB-NUC140
// MCU : NUC140VE3CN

// wire PB12,13,14 to Ground will trigger interrupt
// KEY1 connected to PB12 
// KEY2 connected to PB13 
// KEY3 connected to PB14 

#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"

volatile uint8_t KEY1_Flag = 0;
volatile uint8_t KEY2_Flag = 0;
volatile uint8_t KEY3_Flag = 0;

void GPAB_IRQHandler(void)
{
    if (PB->ISRC & BIT12) {        // check if PB12 interrupt occurred
        PB->ISRC |= BIT12;         // clear PB12 interrupt status
			  KEY1_Flag=1;               // set a flag for PB12(KEY1)
    } else if (PB->ISRC & BIT13) { // check if Pb13 interrupt occurred
        PB->ISRC |= BIT13;         // clear PB13 interrupt status  
        KEY2_Flag=1;	              // set a flag for PB13(KEY2)
    } else if (PB->ISRC & BIT14) { // check if PB14 interrupt occurred
        PB->ISRC |= BIT14;         // clear PB14 interrupt status  
        KEY3_Flag=1;	              // set a flag for PB14(KEY3)			
    } else {                      // else it is unexpected interrupts
        PB->ISRC = PB->ISRC;	      // clear all GPB pins
    }
}

void Init_KEY(void)
{
    GPIO_SetMode(PB, (BIT12 | BIT13 | BIT14), GPIO_MODE_INPUT);
    GPIO_EnableInt(PB, 12, GPIO_INT_FALLING);
    GPIO_EnableInt(PB, 13, GPIO_INT_FALLING);
    GPIO_EnableInt(PB, 14, GPIO_INT_FALLING);		
    NVIC_EnableIRQ(GPAB_IRQn);   
    GPIO_SET_DEBOUNCE_TIME(GPIO_DBCLKSRC_LIRC, GPIO_DBCLKSEL_64);
    GPIO_ENABLE_DEBOUNCE(PB, (BIT12 | BIT13 | BIT14));	
}

int32_t main()
{	
    SYS_Init();	
	  Init_KEY();
	  printf("Testing KEY1/2/3 IRQ generation:\n");

    while(1) {	
			  if (KEY1_Flag) {
			        printf("KEY1/PB12 Interrupt!\n");
					    KEY1_Flag=0;
				}
			  if (KEY2_Flag) {
			        printf("KEY2/PB13 Interrupt!\n");
					    KEY2_Flag=0;
				}		
				if (KEY3_Flag) {
			        printf("KEY3/PB14 Interrupt!\n");
					    KEY3_Flag=0;
				}
		}
}
