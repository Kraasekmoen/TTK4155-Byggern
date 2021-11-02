


#include "SPI.h"
#include "avr\iom162.h"

void SPI_init()
{
	//Set MOSI and SCK output, all others input 
		// "DDR_SPI = (1<<DD_MOSI)|(1<<DD_SCK)"
		// Since MOSI is on Pin PB5, the actuals are set accordingly (see page 159 of manual)
	DDRB = (1<<DDB5)|(1<<DDB7);
	
	// Enable SPI, Master, set clock rate fck/16
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
}

// Reads the master's SPDR register, not the slave register! For that, send a dummy write first
uint8_t SPI_read(){	return SPDR;}
//uint8_t SPI_read_slave(){ SPI_send(0);	return SPI_read();}

void SPI_send(uint8_t ch){ SPDR = ch;}
	
void SPI_SS_LOW() { PORTB &= ~(1<<PB4);}
void SPI_SS_HIGH(){ PORTB |= (1<<PB4);}