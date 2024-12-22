extern void init_buzzer(void);

extern void buzzer_play(uint16_t freq, uint16_t t);

extern void buzzer_play_song(uint16_t *freq, uint16_t *t, uint16_t note_delay, uint8_t size);

extern void buzzer_stop(void);

extern uint8_t buzzer_is_playing(void);
