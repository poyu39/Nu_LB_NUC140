#include <stdio.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "NewTurnButton.h"

int main(void) {
    // init
    SYS_Init();
    init_turn_button();
    while (TRUE) {
        printf("turn button: %d\n", get_turn_button());
    }
}
