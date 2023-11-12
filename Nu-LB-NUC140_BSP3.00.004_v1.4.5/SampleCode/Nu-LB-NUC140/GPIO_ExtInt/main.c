//
// GPIO_EXTINT : External Interrupt Pins to interrupt MCU
//
// EVB : Nu-LB-NUC140
// MCU : NUC140VE3CN

// INT0 /PB14 : NUC140 LQFP100 pin4
// INT1 /PB15 : NUC140 LQFP100 pin91

#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"


void EINT0_IRQHandler(void)
{
    GPIO_CLR_INT_FLAG(PB, BIT14);  // Clear GPIO interrupt flag
    printf("PB14 EINT0 occurred.\n");
}

void EINT1_IRQHandler(void)
{
    GPIO_CLR_INT_FLAG(PB, BIT15);	// Clear GPIO interrupt flag
    printf("PB15 EINT1 occurred.\n");
}

void Init_EXTINT(void)
{
    // Configure EINT0 pin and enable interrupt by falling edge trigger
    GPIO_SetMode(PB, BIT14, GPIO_MODE_INPUT);
    GPIO_EnableEINT0(PB, 14, GPIO_INT_FALLING);
    NVIC_EnableIRQ(EINT0_IRQn);

    // Configure EINT1 pin and enable interrupt by rising and falling edge trigger
    GPIO_SetMode(PB, BIT15, GPIO_MODE_INPUT);
    GPIO_EnableEINT1(PB, 15, GPIO_INT_RISING); // RISING, FALLING, BOTH_EDGE, HIGH, LOW
    NVIC_EnableIRQ(EINT1_IRQn);

    // Enable interrupt de-bounce function and select de-bounce sampling cycle time
    GPIO_SET_DEBOUNCE_TIME(GPIO_DBCLKSRC_LIRC, GPIO_DBCLKSEL_64);
    GPIO_ENABLE_DEBOUNCE(PB, BIT14);
    GPIO_ENABLE_DEBOUNCE(PB, BIT15);
}

int32_t main()
{	
    SYS_Init();
    Init_EXTINT();

    while(1);
}
