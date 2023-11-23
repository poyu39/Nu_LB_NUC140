#include <stdio.h>
#include <stdlib.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "NewSevenSegment.h"

int main(void) {
    SYS_Init();
    init_seg(TRUE, 200);
    set_seg_buffer_number(9239, FALSE);
    while (TRUE);
}
