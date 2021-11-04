/*
 * mcp2512.c
 *
 * Created: 26.10.2021 16:42:43
 *  Author: sindrbov
 */ 
#include <mcp2515.h>

//								---									MCP config and operations

void MCP_set_mode_loopback(){
	SPI_SS_LOW();						// Set slave select low
	
	SPI_send_byte(MCP_BITMOD);			// Send bit modify instruction to MCP
	SPI_send_byte(MCP_CANCTRL);			// Send address of data to be modified
	SPI_send_byte(0b11100000);			// Send modification mask
	SPI_send_byte(MODE_LOOPBACK);		// Data byte; write to equates to 'loopback mode' to the CAN control register
	
	SPI_SS_HIGH();
}

void MCP_set_mode(mcp_mode md){
	SPI_SS_LOW();						
	SPI_send_byte(MCP_BITMOD);			
	SPI_send_byte(MCP_CANCTRL);			
	SPI_send_byte(0b11100000);			
	SPI_send_byte(md);					
	SPI_SS_HIGH();
}

void MCP_bit_modify(uint8_t red_addr, uint8_t mask, uint8_t data){
	SPI_SS_LOW();
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