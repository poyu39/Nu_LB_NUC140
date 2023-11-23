#ifndef __Seven_Segment_H__
#define __Seven_Segment_H__

extern uint8_t SEG_LOOP;

extern int8_t seven_segment_buffer[4];

extern void init_seven_segment(uint8_t user_timer, uint8_t timer_hz);

extern void show_seven_segment(uint8_t no, int8_t sn);

extern void close_seven_segment(void);
#endif
