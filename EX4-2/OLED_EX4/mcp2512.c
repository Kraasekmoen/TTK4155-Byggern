/*
 * mcp2512.c
 *
 * Created: 26.10.2021 16:42:43
 *  Author: sindrbov
 */ 
#include <mcp2515.h>

//								---									MCP config and operations

void MCP_init(mcp_mode md){
	
	// Initialize SPI for MCU, just to be sure
	SPI_init();
	
	// Reset the MCP, just to be sure, and set the desired mode
	MCP_reset();
	for(int j=0; j<10; j++)
	{
		for(int k=0; k<30000; k++);
	}

	MCP_write_byte(MCP_CANINTE, 0b00000011);				// Only enable interrupts on message reception
	MCP_bit_modify(MCP_CANCTRL, 0b00010000, 0b00010000);	// Request abort for all transmittions

	MCP_set_mode(md);

	// Verify that the MCP is in the correct mode
	uint8_t can_status = MCP_read_byte(MCP_CANSTAT);
	uint8_t can_mode = can_status & MCP_MODE_MASK;			// First 3 bits of CANSTAT indicates the MCPs current mode
	if (can_mode != md) { 
		printf("MCP Error: Init failed!\n");
		MCP_print_diagnostix();
	}
}


void MCP_set_mode(mcp_mode md){
	MCP_bit_modify(MCP_CANCTRL, MCP_MODE_MASK, md);
}

void MCP_bit_modify(uint8_t red_addr, uint8_t mask, uint8_t data){
	SPI_SS_LOW();
	//uint8_t snd[4] = {MCP_BITMOD, red_addr, mask, data};
	//SPI_send(snd);
	
	SPI_send_byte(MCP_BITMOD);
	SPI_send_byte(red_addr);
	SPI_send_byte(mask);
	SPI_send_byte(data);
	
	SPI_SS_HIGH();
}

void MCP_reset(){
	SPI_SS_LOW();
	SPI_send_byte(MCP_RESET);
	SPI_SS_HIGH();
}

uint8_t MCP_read_status(){
	SPI_SS_LOW();
	SPI_send_byte(MCP_READ_STATUS);
	uint8_t rec = SPI_read();
	SPI_SS_HIGH();
	return rec;
}

void MCP_print_diagnostix(){
	printf("Diagnostix..\n");
	uint8_t CANSTAT = MCP_read_byte(MCP_CANSTAT);
	uint8_t MODE	= CANSTAT &= 0b11100000;
	uint8_t FLAGS	= CANSTAT &= 0b00001110;
	
	printf("CANSTAT: 0x%02X MODE: 0x%01X FLAGS: 0x%01X\n", CANSTAT, MODE, FLAGS);
	uint8_t RDSTAT = MCP_read_status();
	uint8_t CANCTRL = MCP_read_byte(MCP_CANCTRL);
	printf("RDSTAT: 0x%02X CANCTRL: 0x%02X\n\n", RDSTAT, CANCTRL);
}


//								---									MCP SPI interfacing

uint8_t MCP_read_byte(uint8_t address){
	// Set SS low
	// Send READ instruction
	// Send 8-bit address (MSB first)
	// Read data off SO shift register
	// Raise SS

	SPI_SS_LOW();
	SPI_send_byte(MCP_READ);
	SPI_send_byte(address);
	uint8_t rec = SPI_read();
	SPI_SS_HIGH();
	
	return rec;
}

void MCP_write_byte(uint8_t address, uint8_t data){
	SPI_SS_LOW();
	SPI_send_byte(MCP_WRITE);
	SPI_send_byte(address);
	SPI_send_byte(data);
	SPI_SS_HIGH();
	
	uint8_t check = MCP_read_byte(address);																				// !!! 
	printf("Written: %d Read: %d\n", data, check);
}

void MCP_write(uint8_t start_address, uint8_t *data){
	SPI_SS_LOW();
	SPI_send_byte(MCP_WRITE);
	SPI_send_byte(start_address);
	SPI_send(data);
	SPI_SS_HIGH();
}

//								---									MCP CAN control functions

void MCP_request_to_send(unsigned int bfr){
	SPI_SS_LOW();
	SPI_send_byte(bfr);
	SPI_SS_HIGH();
}