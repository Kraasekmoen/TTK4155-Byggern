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

//#include <OLED_Driver.h>
#include <USART_Driver.h>
//#include <ADC_Driver.h>
#include <SPI.h>
#include <mcp2515.h>
#include <CAN_Driver.h>


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
	
	DDRB &= ~(1<<PB2) | ~(1<<PB3);			// Configure pins PB2 and PB3 to act as inputs (for the USB slider buttons)
}
void Init_pwm(void)
{
	TCCR0 |= (1<<COM00) | (1<<WGM01) | (1<<CS00);
}
void set_configs(){
	SPI_init();								// Enable/initiate Serial Peripheral Interface
	
	// Interrupt Config
	MCUCR |= (1 << ISC11) | (0 << ISC10);	// Configure INT1 such that falling edge triggers interrupt (MCP INT is active low, remains low until intrp is cleared)
	sei();									// Set Global Interrupt Enable in SREG
	GICR |= (1 << INT1);					// Enable external interrupts on Pin PD3
}

// Interrupt handler
volatile uint8_t EXT_INT_FLAG = 0;
ISR(INT1_vect){ 
	EXT_INT_FLAG = 1; 
	printf("External interrupt received on INT1; flag set to %d \n", EXT_INT_FLAG);
}

/*
void ADC_test_1(uint8_t *joy_origins){
		uint8_t *sampled_value;
		
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

void SPI_test_1(){
	uint8_t rec;
	for (uint8_t a=0; a<10; a++){
		SPI_SS_LOW();
		SPI_send_byte(a);
		printf("Sent %d over SPI\n", a);
		
		rec = SPI_read();
		printf("Received %d from SPI slave\n", rec);
		SPI_SS_HIGH();
	}
}

void SPI_READ_STATUS_TEST(){
	SPI_SS_LOW();
	printf("Sent READ_STATUS instruction to MCP...\n");
	SPI_send_byte(MCP_READ_STATUS);
	uint8_t rec = SPI_read();
	printf("Received: "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(rec));
	printf("\n");
	SPI_SS_HIGH();
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

/*
void Exercise_3_Demo(){
	// Auto-calibrate joystick ADC outputs
	uint8_t *joy_origins;
	joy_origins = adc_joystick_autocalibrate(10);
	printf("Sampled average origin (Joy1) = %d \n", joy_origins[0]);
	printf("Sampled average origin (Joy2) = %d \n", joy_origins[1]);

	for (uint8_t i = 0; i<10; i++)
	{
		// Read each channel on ADC in sequence (hard-wired mode)
		ADC_test_1(joy_origins);
		
	}
}
void Exercise_4_Demo(){
	// Initialize OLED screen on USB board
	oled_init();
	// || OLED TESTS ||
		
	printf("Attempting to clear OLED screen...\n");
	oled_cmdreg_write(0xa5);
	
	oled_reset();
	// Delay
	for(int j=0; j<10; j++)
	{
		for(int k=0; k<30000; k++);
	}
	
	for(int j=0; j<10; j++){
		testPrint_font(j);
	}
}
*/
void Exercise_5_Demo(){
	// Initialize MCP in loopback mode
	MCP_init(LOOPBACK);
	
	// Send a message to the MCP over SPI interface, command it to transmit it over CAN bus
	CANMSG message;
	message.ID_high = 0b1101;
	message.ID_low = 0b1001;
	message.data_length = 4;
	for (uint8_t i = 0; i < message.data_length; i++){
		message.data[i] = i;
	}
	printf("Attempting to send message over CAN: \n");
	CAN_print_message(&message);
	
	while (!CAN_transmit_message(&message)){
		printf("Unable to send\n");
		for(int j=0; j<10; j++)
		{
			for(int k=0; k<30000; k++);
		}
	}
	printf("..sent!\n");
		
	for(int j=0; j<10; j++)
	{
		for(int k=0; k<30000; k++);
	}
		
	// Read a received message from the CAN bus, demonstrate that it is the same as the one sent
	if (EXT_INT_FLAG == 1) {EXT_INT_FLAG=0;}
	
	printf("Checking CANINTF rx flags..\n");
	uint8_t INTFs = MCP_read_byte(MCP_CANINTF);
	uint8_t RXnFs = INTFs & 0b00000011;
	CANMSG rec;
	switch (RXnFs)
	{
		case 0b00000001:
			printf("Msg flag for RX0 discovered\n");
			rec = CAN_read_rx_buffer(0);
			if (rec.data_length == 9){
				printf("Msg data length 9 - invalid message RX0!\n");
			}
			else{
				CAN_print_message(&rec);
			}
			break;
			
		case 0b00000010:
			printf("Message flag for RX1 discovered\n");
			rec = CAN_read_rx_buffer(1);
			if (rec.data_length == 9){
				printf("Msg data length 9 - invalid message RX1!\n");
			}
			else{
				CAN_print_message(&rec);
			}
			break;
		
		default:
			printf("No message flags discovered\n");
			break;
	}
	
}


int main(void)
{
	// Initialize USART transmission drivers, as well as MCU ports and external memory
	USART_init(MYUBRR);
	stdout = &mystdout;
	xmem_init();
	Init_ports();
	set_configs();
	Init_pwm();
	SPI_init();
	
	// Test SRAM integrity
	SRAM_test();
	
	Exercise_5_Demo();
		
	
}


