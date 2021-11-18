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
	MCP_write_byte(MCP_CANINTE, MCP_RD_INT_MASK);					// Only enable interrupts on message reception
	MCP_bit_modify(MCP_CANCTRL, MCP_TXABRT_MASK, MCP_TXABRT_MASK);	// Request abort for all transmittions; necessary for setting mode
	
	// Set CAN timing settings
	MCP_write_byte(MCP_CNF1, 0b11000010);	// SJW:4 BRP:3
	MCP_write_byte(MCP_CNF2, 0b11001001);	// BTLMODE:1 SAM:1 PS1:2 PROPSEG:2
	MCP_write_byte(MCP_CNF3, 0b00000010);	// SOF:x WAKFIL:x PS2:3
	
	MCP_set_mode(md);
	// Verify that the MCP is in the correct mode
	uint8_t can_status = MCP_read_byte(MCP_CANSTAT);
	uint8_t can_mode = can_status &= MCP_MODE_MASK;					// First 3 bits of CANSTAT indicates the MCPs current mode
	if (can_mode != md) { 
		printf("MCP Error: Init failed!\n");
		MCP_print_diagnostix();
	}
}


void MCP_set_mode(mcp_mode md){
	MCP_bit_modify(MCP_CANCTRL, MCP_MODE_MASK, md);
}

void MCP_bit_modify(uint8_t red_addr, uint8_t mask, uint8_t data){
	// uint8_t send_array[4] = {MCP_BITMOD, red_addr, mask, data};
		
	SPI_SS_LOW();
	
	//SPI_send(send_array);
	
	SPI_send(MCP_BITMOD);
	SPI_send(red_addr);
	SPI_send(mask);
	SPI_send(data);
	
	SPI_SS_HIGH();
}

void MCP_reset(){
	SPI_SS_LOW();
	SPI_send(MCP_RESET);
	SPI_SS_HIGH();
}

uint8_t MCP_read_status(){
	SPI_SS_LOW();
	SPI_send(MCP_READ_STATUS);
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
	SPI_send(MCP_READ);
	SPI_send(address);
	uint8_t rec = SPI_read();
	SPI_SS_HIGH();
	
	return rec;
}

uint8_t * MCP_read(uint8_t address, uint8_t length){
	SPI_SS_LOW();
	SPI_send(MCP_READ);
	SPI_send(address);
	uint8_t rec[length];
	
	for (int i = 0; i<length; i++){
		rec[i] = SPI_read();
	}
	
	SPI_SS_HIGH();
	
	return rec;
}

void MCP_write_byte(uint8_t address, uint8_t data){
	SPI_SS_LOW();
	SPI_send(MCP_WRITE);
	SPI_send(address);
	SPI_send(data);
	SPI_SS_HIGH();
	
	//uint8_t check = MCP_read_byte(address);																				// !!! 
	//printf("Written: %d Read: %d\n", data, check);
}

void MCP_write(uint8_t start_address, uint8_t *data, uint8_t data_length){
	SPI_SS_LOW();
	SPI_send(MCP_WRITE);
	SPI_send(start_address);
	for (uint8_t i = 0; i<data_length; i++){
		SPI_send(data[i]);
		//printf("MCP_W: %d,%d\n",i, data[i]);
	}
	SPI_SS_HIGH();
}

//								---									MCP CAN control functions

void MCP_request_to_send(uint8_t bfr){
	SPI_SS_LOW();
	SPI_send(bfr);			// MCP_RTS_TXn counts as both command and address
	SPI_SS_HIGH();
}