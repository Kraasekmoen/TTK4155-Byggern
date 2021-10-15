/*
 * ADC_READ_3.c
 *
 * Created: 05.10.2021 11:17:49
 * Author : santialf
 */ 

#include <avr/io.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <avr/interrupt.h>

#include <OLED_Driver.h>
#include <USART_Driver.h>
#include <ADC_Driver.h>

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
	// Initialize USART transmission drivers, as well as MCU ports and external memory
	USART_init(MYUBRR);
	stdout = &mystdout;
	printf("Program started\n");
	xmem_init();
	Init_ports();
	Init_pwm();
	printf("XMEM Init completed\n");
	
	// Initialize OLED screen on USB board
	oled_init();
	// OLED Tests
	for(int j=0; j<10; j++){
		testPrint_font(j);
	}
		
	printf("Attempting to clear OLED screen...\n");
	oled_reset();
	// Delay
	for(int j=0; j<100; j++)
	{
		for(int k=0; k<30000; k++);
	}
	
	// Test SRAM integrity
	SRAM_test();
		
	// Auto-calibrate joystick ADC outputs
	uint8_t *joy_origins;
	joy_origins = adc_joystick_autocalibrate(10);
	printf("Sampled average origin (Joy1) = %d \n", joy_origins[0]);
	printf("Sampled average origin (Joy2) = %d \n", joy_origins[1]);
	
	uint8_t *sampled_value;

	while(1)
	{
		// Read each channel on ADC in sequence (hard-wired mode)
		sampled_value = adc_read();
		printf("Sampled value channel 0 (Joy1) = %d \n", adc_formatter(sampled_value[0], joy_origins[0]));
		printf("Sampled value channel 1 (Joy2) = %d \n", adc_formatter(sampled_value[1], joy_origins[1]));
		printf("Sampled value channel 2 (SliL) = %d \n", (sampled_value[2]));
		printf("Sampled value channel 3 (SliR) = %d \n", (sampled_value[3]));
		
		// Delay
		for(int j=0; j<10; j++)
		{
			for(int k=0; k<30000; k++);
		}
		printf("Restart from the first channel \n");
	}
	
}


