extern volatile uint8_t RX_FLAG;

extern void init_p_uart(uint32_t baud_rate);

extern int32_t get_rx_int(void);

extern void p_send_int(int32_t data);
