/*
 * usart.c
 *
 * Created: 31.08.2021 16:49:28
 * Author : thoam
 */ 

#include <avr/io.h>

#define F_CPU 4915200UL

#define FOSC F_CPU// Clock Speed
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1

void USART_init(unsigned int ubrr);
void USART_Transmit(unsigned char data );
unsigned char USART_Receive( void );


int main(void)
{
	USART_init(MYUBRR);
    /* Replace with your application code */
	unsigned char var = 8;
	unsigned int i=0,j=0;
	USART_Transmit(var);
	for(i=0;i<1000;i++)
	{
		for(j=0;j<60000;j++);
	}
	USART_Receive();
	    
}

void USART_init(unsigned int ubrr)
{
	/* Set baud rate */
	UBRR0H = (unsigned char) (ubrr>>8);
	UBRR0L = (unsigned char) (ubrr);
	
	/* Enable receiver and transmitter */
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	
	/* Set frame format: 8data, 2stop bit */
	UCSR0C = (1 << URSEL0) | (1 << USBS0) | (3 << UCSZ00);
}

void USART_Transmit( unsigned char data )
{
	/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE0)) );
	
	/* Put data into buffer, sends the data */
	UDR0 = data;
}

unsigned char USART_Receive( void )
{
	/* Wait for data to be received */
	while ( !(UCSR0A & (1<<RXC0)) )
	;
	/* Get and return received data from buffer */
	return UDR0;
}

fdevopen(USART_Transmit, USART_Receive);