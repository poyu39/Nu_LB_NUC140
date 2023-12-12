//
// NRF24L01 driver
// 
// SPI port
// SPI1_SS  : GPC8
// SPI1_CLK : GPC9
// SPI1_MISO: GPC10
// SPI1_MOSI: GPC11
// SPI1_CE  : GPC12
//
#include <stdio.h>
#include "NUC100Series.h"
#include "sys.h"
#include "spi.h"
#include "gpio.h"
#include "SYS_init.h"
#include "NRF24L01.h"

#define nRF_CE_Lo PC12=0
#define nRF_CE_Hi PC12=1
 
uint8_t payload = NRF24L01_PAYLOAD;
uint8_t PTX = 0;

void nRF_Init(void)
{
    SPI_Open(NRF24L01_SPI_PORT, SPI_MASTER, SPI_MODE_0, NRF24L01_DATAWIDTH, NRF24L01_SPI_SPEED); //8-bit 1MHz
    SPI_DisableAutoSS(NRF24L01_SPI_PORT);
	  SPI_SET_MSB_FIRST(NRF24L01_SPI_PORT);
	  GPIO_SetMode(PC, BIT12, GPIO_MODE_OUTPUT);
	  PC12=1; // CE
}

void nRF_setRADDR(uint8_t * adr)
{
	nRF_CE_Lo;
	nRF_writeRegister(RX_ADDR_P1,adr,mirf_ADDR_LEN);
	nRF_CE_Hi;	
}

void nRF_setTADDR(uint8_t *adr){	
	nRF_writeRegister(RX_ADDR_P0,adr,mirf_ADDR_LEN); 
	nRF_writeRegister(TX_ADDR,adr,mirf_ADDR_LEN);	
}

void nRF_CSN_Lo(void)
{
	SPI_SET_SS0_LOW(NRF24L01_SPI_PORT);
}

void nRF_CSN_Hi(void)
{
	SPI_SET_SS0_HIGH(NRF24L01_SPI_PORT);
}

void nRF_transfer(uint8_t data)
{	
  SPI_WRITE_TX0(NRF24L01_SPI_PORT, data);
  SPI_TRIGGER(NRF24L01_SPI_PORT);
	while(SPI_IS_BUSY(NRF24L01_SPI_PORT));
}

void nRF_writeRegister(uint8_t reg, uint8_t * value, uint8_t len)
{	
	nRF_CSN_Lo();	
		nRF_transfer(W_REGISTER | (REGISTER_MASK & reg));
		nRF_transmitSync(value,len);	
	nRF_CSN_Hi();
}

void nRF_transmitSync(uint8_t *dataout, uint8_t len)
{	
	int8_t i;	
	for(i = 0;i < len;i++)
		nRF_transfer(dataout[i]);
}

void nRF_setpayload(uint8_t p)
{	
	payload = p;
}

void nRF_configRegister(uint8_t reg, uint8_t value)
// Clocks only one byte into the given MiRF register
{
    nRF_CSN_Lo();
		nRF_transfer(W_REGISTER | (REGISTER_MASK & reg));
		nRF_transfer(value);
    nRF_CSN_Hi();
}

void nRF_config(uint8_t channel, uint8_t payload)
{	
		// Set RF channel
		nRF_configRegister(RF_CH, channel);
    // Set length of incoming payload
	  nRF_configRegister(RX_PW_P0, 0);	
	  nRF_configRegister(RX_PW_P1, payload);
	  nRF_configRegister(RX_PW_P2, 0);
	  nRF_configRegister(RX_PW_P3, 0);
	  nRF_configRegister(RX_PW_P4, 0);
		nRF_configRegister(RX_PW_P5, 0);
    // 1 Mbps, TX gain: 0dbm 
    nRF_configRegister(RF_SETUP, (0<<RF_DR)|((0x03)<<RF_PWR)); 
    // CRC enable, 1 byte CRC length 
    nRF_configRegister(CONFIG,((1<<EN_CRC)|(0<<CRCO))); 
    // Auto Acknowledgment 
    nRF_configRegister(EN_AA,(1<<ENAA_P0)|(1<<ENAA_P1)|(0<<ENAA_P2)|(0<<ENAA_P3)|(0<<ENAA_P4)|(0<<ENAA_P5)); 
    // Enable RX addresses 
    nRF_configRegister(EN_RXADDR,(1<<ERX_P0)|(1<<ERX_P1)|(0<<ERX_P2)|(0<<ERX_P3)|(0<<ERX_P4)|(0<<ERX_P5)); 
    // Auto retransmit delay: 1000 us and Up to 15 retransmit trials 
	  nRF_configRegister(SETUP_RETR,(0x04<<ARD)|(0x0F<<ARC)); 
    // Dynamic length configurations: No dynamic length 
    //nRF_configRegister(DYNPD,(0<<DPL_P0)|(0<<DPL_P1)|(0<<DPL_P2)|(0<<DPL_P3)|(0<<DPL_P4)|(0<<DPL_P5));	
    // Start receiver 
    nRF_powerUpRx();
    nRF_flushRx();
}

void nRF_powerUpRx(void)
{	
	PTX = 0;
	nRF_CE_Lo;
	nRF_configRegister(CONFIG, mirf_CONFIG | ( (1<<PWR_UP) | (1<<PRIM_RX) ) );      //PRIM_RX: RX/TX control
	nRF_configRegister(STATUS,(1 << TX_DS) | (1 << MAX_RT)); 
	nRF_CE_Hi;	
}

void nRF_flushRx(void)
{	
		nRF_CSN_Lo();		
		nRF_transfer(FLUSH_RX);	
    nRF_CSN_Hi();
}


void nRF_send(uint8_t *value)
{
	// Sends a data package to the default address. Be sure to send the correct
  // amount of bytes as configured as payload on the receiver.
	uint8_t status;
	status = nRF_getStatus();	
	while (PTX){
		
		status = nRF_getStatus();
		
		if((status & ((1 << TX_DS)  | (1 << MAX_RT)))){
		    PTX = 0;
		    break;
	    }
		}
		
	nRF_CE_Lo;
	
	nRF_powerUpTx();
	
	nRF_CSN_Lo();
	nRF_transfer( FLUSH_TX );
	nRF_CSN_Hi();
	
	nRF_CSN_Lo();
	nRF_transfer(W_TX_PAYLOAD);
	nRF_transmitSync(value,payload);
	nRF_CSN_Hi();                    // Pull up chip select
		
  nRF_CE_Hi;                     // Start transmission
}

uint8_t nRF_getStatus(void)
{
	uint8_t rv;
	nRF_readRegister(STATUS,&rv,1);
	return rv;
}

void nRF_readRegister(uint8_t reg, uint8_t *value, uint8_t len)
// Reads an array of bytes from the given start position in the MiRF registers.
{
    nRF_CSN_Lo();
    nRF_transfer((REGISTER_MASK & reg));    
    nRF_transferSync(value,value,len);		
    nRF_CSN_Hi();
}

void nRF_transferSync(uint8_t *dataout,uint8_t *datain,uint8_t len)
{	
	uint8_t i;

	for( i = 0; i < len; i++){
		nRF_transfer(0xff);
		datain[i] = NRF24L01_SPI_PORT->RX[0];		
	}
}

void nRF_powerUpTx(void)
{	
	PTX = 1;
	nRF_configRegister(CONFIG, mirf_CONFIG | ( (1<<PWR_UP) | (0<<PRIM_RX) ) );	
}


uint8_t nRF_isSending(void)
{	
	uint8_t status;
	
	if(PTX){
		status = nRF_getStatus();
		//  if sending successful (TX_DS) or max retries exceded (MAX_RT).
		if((status & ((1 << TX_DS)  | (1 << MAX_RT)))){
			nRF_powerUpRx();
			return 0; 
		}
		return 1;
	}
	return 0;
}

uint8_t nRF_dataReady(void)
{	
	// Checks if data is available for reading
	uint8_t status = nRF_getStatus();
	
	// We can short circuit on RX_DR, but if it's not set, we still need
  // to check the FIFO for any pending packets
    if ( status & (1 << RX_DR) ) return 1;
    return !nRF_rxFifoEmpty();
}

uint8_t nRF_rxFifoEmpty()
{
	uint8_t fifoStatus;
	nRF_readRegister(FIFO_STATUS,&fifoStatus,sizeof(fifoStatus));
	return (fifoStatus & (1 << RX_EMPTY));
}

void nRF_getData(uint8_t * data)
{
	nRF_CSN_Lo();
	nRF_transfer(R_RX_PAYLOAD);
	nRF_transferSync(data,data,payload);
	nRF_CSN_Hi();
	nRF_configRegister(STATUS,(1<<RX_DR));
}

void nRF_powerDown(void)
{
	nRF_CE_Lo;
	nRF_configRegister(CONFIG, mirf_CONFIG );
}
