


#include "SPI.h"
#include "avr\iom162.h"

void SPI_init()
{
	//Set MOSI and SCK output, all others input 
		// "DDR_SPI = (1<<DD_MOSI)|(1<<DD_SCK)"
		// Since MOSI is on Pin PB5, the actuals are set accordingly (see page 159 of manual)
	DDRB = (1<<DDB4)|(1<<DDB5)|(1<<DDB7);
	
	// Enable SPI, Master, set clock rate fck/16
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
	SPI_SS_HIGH();
}

// Reads the master's SPDR register, not the slave register! For that, send a dummy write first
uint8_t SPI_read(){	SPDR = 0x01; return SPDR;}
//uint8_t SPI_read_master(){ return SPDR;}

void SPI_send_byte(uint8_t ch){ 
	SPDR = ch;
	
	// SPSR: SPI Status Register		SPIF: SPI End of Transmission Flag; flags when full byte is written to SPDR			(see example at p 160)
	loop_until_bit_is_set(SPSR, SPIF);
}
void SPI_send(uint8_t *charray){
	for (uint8_t i = 0; i <= sizeof(charray); i++){	
		SPI_send(charray[i]); 
	}
}
	
void SPI_SS_LOW() { PORTB &= ~(1<<PB4); }
void SPI_SS_HIGH(){ PORTB |= (1<<PB4);}
	
