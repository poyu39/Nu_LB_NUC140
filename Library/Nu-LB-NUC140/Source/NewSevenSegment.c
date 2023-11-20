#include <stdio.h>
#include "NUC100Series.h"
#include "GPIO.h"
#include "SYS.h"
#include "Seven_Segment.h"
#define SEG_N0 0x82
#define SEG_N1 0xEE
#define SEG_N2 0x07
#define SEG_N3 0x46
#define SEG_N4 0x6A
#define SEG_N5 0x52
#define SEG_N6 0x12
#define SEG_N7 0xE6
#define SEG_N8 0x02
#define SEG_N9 0x62
#define SEG_N10 0x22
#define SEG_N11 0x1A
#define SEG_N12 0x93
#define SEG_N13 0x0E
#define SEG_N14 0x13
#define SEG_N15 0x33

uint8_t SEG_BUF[17] = {SEG_N0, SEG_N1, SEG_N2, SEG_N3, SEG_N4, SEG_N5, SEG_N6, SEG_N7, SEG_N8, SEG_N9, SEG_N10, SEG_N11, SEG_N12, SEG_N13, SEG_N14, SEG_N15, 0xFF};

void init_seven_segment(void) {
    GPIO_SetMode(PC, (BIT4 | BIT5 | BIT6 | BIT7), GPIO_PMD_OUTPUT);
    PC4 = 0;
    PC5 = 0;
    PC6 = 0;
    PC7 = 0;
    GPIO_SetMode(PE, (BIT0 | BIT1 | BIT2 | BIT3 | BIT4 | BIT5 | BIT6 | BIT7), GPIO_PMD_QUASI);
    PE0 = 0;
    PE1 = 0;
    PE2 = 0;
    PE3 = 0;
    PE4 = 0;
    PE5 = 0;
    PE6 = 0;
    PE7 = 0;
}

void show_seven_segment(uint8_t no, uint8_t number) {
    PE->DOUT = SEG_BUF[number];
    switch (no) {
    case 0:
        PC4 = 1;
        break;
    case 1:
        PC5 = 1;
        break;
    case 2:
        PC6 = 1;
        break;
    case 3:
        PC7 = 1;
        break;
    }
}

void close_seven_segment(void) {
    PC4 = 0;
    PC5 = 0;
    PC6 = 0;
    PC7 = 0;
}
