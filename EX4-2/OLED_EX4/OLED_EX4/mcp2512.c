/*
 * mcp2512.c
 *
 * Created: 26.10.2021 16:42:43
 *  Author: sindrbov
 */ 
#include <mcp2515.h>

void MCP_set_mode_loopback(){
	SPI_SS_LOW();					// Set slave select low
	
	SPI_send(MCP_BITMOD);			// Send bit modify instruction to MCP
	SPI_send(MCP_CANCTRL);			// Send address of data to be modified
	SPI_send(0b11100000);			// Send modification mask
	SPI_send(MODE_LOOPBACK);		// Data byte; write to equates to 'loopback mode' to the CAN control register
	
	SPI_SS_HIGH();
}
