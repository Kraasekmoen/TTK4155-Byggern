


#ifndef SPI_H_
#define SPI_H_

	#include <avr/io.h>
	#include <stdio.h>
	#include <stdint.h>
	#include <stdbool.h>
	#include <stdlib.h>
	#include <avr/interrupt.h>
	
	void SPI_send(uint8_t ch);
	uint8_t SPI_read();
	void SPI_init();	
	
	void SPI_SS_HIGH();
	void SPI_SS_LOW();
	
#endif