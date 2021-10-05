/*
 * ADC_READ_3.c
 *
 * Created: 05.10.2021 11:17:49
 * Author : santialf
 */ 

#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/interrupt.h>

#include <OLED_Driver.h>

#define OFFSET 0x1000

#define F_CPU 4915200UL

#define FOSC F_CPU// Clock Speed
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1
#define loop_until_bit_is_set(sfr, bit) do { } while (bit_is_clear(sfr, bit))

static int uart_putchar(char c, FILE *stream);
static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

int uart_putchar(char c, FILE *stream)
{
	if (c == '\n')
	uart_putchar('\r', stream);
	loop_until_bit_is_set(UCSR0A, UDRE0);
	UDR0 = c;
	return 0;
}

void USART_Transmit(unsigned char data)
{
	/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE0)) );
	
	/* Put data into buffer, sends the data */
	UDR0 = data;
}

unsigned char USART_Receive(void)
{
	/* Wait for data to be received */
	while ( !(UCSR0A & (1<<RXC0)) )
	;
	/* Get and return received data from buffer */
	return UDR0;
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
	fdevopen(USART_Transmit, USART_Receive);
}

void xmem_init(void)
{
	MCUCR |= (1<<SRE);
	SFIOR |= (1<<XMM2);
	DDRC |= 0xFF;
	PORTC = 0x00;
}


void Init_ports(void)
{
	DDRD |= (1<<PD6);
	PORTD |= (1<<PD6);
	DDRB |= (1<<PB0);
	DDRB &= ~(1<<PB1);
	
	DDRB &= ~(1<<PB2) | ~(1<<PB3); // Configure pins PB2 and PB3 to act as inputs (for the USB slider buttons)
}

void Init_pwm(void)
{
	TCCR0 |= (1<<COM00) | (1<<WGM01) | (1<<CS00);
}

void adc_init(void)
{
	// CS high, WR high, RD high (pointer RAM ???))
	
	
	
}

/*
uint8_t adc_read(uint8_t channel)
{
	// CS low, WR low
	volatile char *ext_adc = (char *) 0x1400;
	*ext_adc = channel;
	*ext_adc &= ~(1<<2) &~ (1<<3) &~ (1<<5) &~ (1<<6);
	*ext_adc |= (1<<4) | (1<<7);
	
	/*PORTC = channel;
	PORTC &= ~(1<<2) &~ (1<<3) &~ (1<<5) &~ (1<<6);
	PORTC |= (1<<4) | (1<<7);
	*|
	
	// CS high, WR high
	
	while(!(PORTB & (1<<PB0)));
	// CS low, RD low
	uint8_t result = *ext_adc;
	// RD high, CS high
}
*/


uint8_t * adc_read()
{
	volatile char *adc = (char *) 0x1400;
	uint8_t command = 3;
	adc[0] = command;

	//_delay_us(200);
	for(int j=0; j<10; j++)
	{
		for(int k=0; k<6000; k++);
	}
	
	static uint8_t value[4];
	value[0] = adc[0];
	value[1] = adc[0];
	value[2] = adc[0];
	value[3] = adc[0];
	return value;
}

int8_t adc_formatter(uint8_t reading, uint8_t offset){		// Takes reading from ADC [00-FF] and returns number between -128 and 127
	int8_t reading_conv;
	if (reading <= offset) {
		reading_conv = -(128 - ((reading * 128) / offset));
	} else {
		reading_conv = reading - offset;
		reading_conv = (reading_conv * 127) / (255-offset);
	}
	
	return reading_conv;
}

uint8_t array_average(uint8_t *array, uint8_t size){		// Broken; doesnt work for some reason
	int sum, loop;
	uint8_t avg;
	sum = avg = 0;
	
	for(loop = 0; loop < size; loop++) {
		sum = sum + array[loop];
	}
	
	avg = sum / loop;
	return avg;
}

uint8_t * adc_joystick_calibrate(uint8_t samples){		// Takes a number of samples from both joysticks and returns pointer to array of averages [Doesnt work]
	uint8_t *sampled_value;
	uint8_t joy1_samples[samples];
	uint8_t joy2_samples[samples];
	uint8_t joy_origins[2];
	
	for(int i=0; i<samples; i++){
		sampled_value = adc_read();
		printf("Sample from Joy1 = %d \n", sampled_value[0]);
		joy1_samples[i] = sampled_value[0];
		joy2_samples[i] = sampled_value[1];		
	}
	
	joy_origins[0] = array_average(joy1_samples, samples);
	joy_origins[1] = array_average(joy2_samples, samples);
	
	return joy_origins;
}


void pos_read (void)
// uso variabili globali
{
	// calculate the position of the joystick
}

void SRAM_test(void)
{
	volatile char *ext_ram = (char *) 0x1800; // Start address for the SRAM
	uint16_t ext_ram_size = 0x800;
	uint16_t write_errors = 0;
	uint16_t retrieval_errors = 0;
	printf("Starting SRAM test...\r\n");
	// rand() stores some internal state, so calling this function in a loop will
	// yield different seeds each time (unless srand() is called before this function)
	uint16_t seed = rand();
	// Write phase: Immediately check that the correct value was stored
	srand(seed);
	for (uint16_t i = 0; i < ext_ram_size; i++) {
		uint8_t some_value = rand();
		ext_ram[i] = some_value;
		uint8_t retreived_value = ext_ram[i];
		if (retreived_value != some_value) {
			printf("Write phase error: ext_ram[%4d] = %02X (should be %02X)\r\n", i, retreived_value, some_value);
			write_errors++;
		}
	}
	// Retrieval phase: Check that no values were changed during or after the write phase
	srand(seed);
	// reset the PRNG to the state it had before the write phase
	for (uint16_t i = 0; i < ext_ram_size; i++) {
		uint8_t some_value = rand();
		uint8_t retreived_value = ext_ram[i];
		if (retreived_value != some_value) {
			printf("Retrieval phase error: ext_ram[%4d] = %02X (should be %02X)\r\n", i, retreived_value, some_value);
			retrieval_errors++;
		}
	}
	printf("SRAM test completed with \r\n%4d errors in write phase and \r\n%4d errors in retrieval phase\r\n\r\n", write_errors, retrieval_errors);
}

int main(void)
{
	USART_init(MYUBRR);
	stdout = &mystdout;
	printf("Program started\n");
	xmem_init();
	Init_ports();
	Init_pwm();
	printf("XMEM Init completed\n");
	
	oled_init();
	
	oled_cmdreg_write(0xa7);	// Inverts display
	for(int j=0; j<10; j++)
	{
		for(int k=0; k<30000; k++);
	}

    SRAM_test();

	uint8_t *joy_origins;
	joy_origins = adc_read();
	printf("Sampled average origin (Joy1) = %d \n", joy_origins[0]);
	printf("Sampled average origin (Joy2) = %d \n", joy_origins[1]);
	
	
	while(1)
	{
		uint8_t *sampled_value;
		sampled_value = adc_read();
		printf("Sampled value channel 0 (Joy1) = %d \n", adc_formatter(sampled_value[0], 149));
		printf("Sampled value channel 1 (Joy2) = %d \n", adc_formatter(sampled_value[1], 165));
		printf("Sampled value channel 2 (SliL) = %d \n", (sampled_value[2]));
		printf("Sampled value channel 3 (SliR) = %d \n", (sampled_value[3]));			// Doesnt seem to work well; only* outputs 00 or FF
		for(int j=0; j<10; j++)
		{
			for(int k=0; k<30000; k++);
		}
		printf("Restart from the first channel \n");
	}
	
}


