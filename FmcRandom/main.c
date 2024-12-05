#include <stdio.h>
#include <stdlib.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "NewLCD.h"


int main(void) {
    uint32_t random_seed;
    uint32_t random_seed_addr = 0x00009239;
    
    SYS_Init();
    init_lcd(FALSE, SPI3_CLOCK_FREQUENCY);
    
    // 關閉暫存器保護
    SYS_UnlockReg();
    
    // 開啟 FMC ISP (In-System Programming) 功能
    FMC->ISPCON |=  FMC_ISPCON_ISPEN_Msk | FMC_ISPCON_APUEN_Msk | FMC_ISPCON_LDUEN_Msk | FMC_ISPCON_CFGUEN_Msk;
    
    random_seed = FMC_Read(random_seed_addr);
    if (random_seed == 0xFFFFFFFF) random_seed = rand() % 0xFFFFFFFF;
    
    srand(random_seed);
    
    random_seed = rand();
    
    FMC_Erase(random_seed_addr);
    FMC_Write(random_seed_addr, random_seed);
    
    // 關閉 FMC ISP 功能
    FMC->ISPCON &=  ~FMC_ISPCON_ISPEN_Msk;
    
    // 開啟暫存器保護
    SYS_LockReg();
    
    printf_line_in_buffer(0, 8, "%d", rand() % 10000);
    show_lcd_buffer();
    
    while(TRUE);
    return 0;
}
