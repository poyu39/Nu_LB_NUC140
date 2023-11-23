#include <stdio.h>
#include <stdlib.h>
#include "NUC100Series.h"
#include "MCU_init.h"
#include "SYS_init.h"
#include "NewSevenSegment.h"

int main(void) {
    SYS_Init();
    init_seven_segment(TRUE, 200);
    seven_segment_buffer[0] = 9;
    seven_segment_buffer[1] = 3;
    seven_segment_buffer[2] = 2;
    seven_segment_buffer[3] = 9;
    while (TRUE);
}
