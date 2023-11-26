#ifndef __Seven_Segment_H__
#define __Seven_Segment_H__

extern uint8_t SEG_LOOP;

extern int8_t seg_buffer[4];

extern void init_seg(uint8_t user_timer, uint8_t timer_hz);

extern void clear_seg_buffer(void);

extern void show_one_seg(uint8_t no, int8_t sn);

extern void set_seg_buffer_number(uint16_t number, uint8_t fill_zero);

extern void close_seg(void);
#endif
