#include <OLED_Driver.h>
#include "fonts.h"
#include <globals.h>


/*
	Smart Address Space:
	0x1000-0x11FF:	Data Register
	0x1200-0x13FF:	Command Register
*/

	#define CMD_OFFSET 0x1200;	
	#define DAT_OFFSET 0x1000;
	
	//enum addr_mode{Horizontal=0x00, Vertical=0x01, Page=0x02};

	
	void oled_cmdreg_write(uint8_t data){
		volatile uint8_t *memspace_pointer = CMD_OFFSET;
		*memspace_pointer = data;
	}
	
	void oled_init(){
		oled_cmdreg_write(0xae);	// Display off
		oled_cmdreg_write(0xa1);	// Segment remap
		oled_cmdreg_write(0xda);	// Common pads hardware; 
		oled_cmdreg_write(0x12);		// Alternative COM pin config
		oled_cmdreg_write(0xc8);	// Common output scan direction:com63~com0
		oled_cmdreg_write(0xa8);	// Multiplex ration mode:63
		oled_cmdreg_write(0x3f);		
		oled_cmdreg_write(0xd5);	// Display  divide ratio/osc. freq. mode
		oled_cmdreg_write(0x80);
		oled_cmdreg_write(0x81);	// Contrast control   
		oled_cmdreg_write(0x50);
		oled_cmdreg_write(0xd9);	// Set pre-charge period
		oled_cmdreg_write(0x21);
		oled_cmdreg_write(0x20);	// Set Memory Addressing Mode
		oled_cmdreg_write(0x00);		// Horizontal address mode
		oled_cmdreg_write(0xdb);	// VCOM deselect level mode
		oled_cmdreg_write(0x30);
		oled_cmdreg_write(0xad);	// Master configuration 
		oled_cmdreg_write(0x00);	
		oled_cmdreg_write(0xa4);	// Display follows RAM content
		oled_cmdreg_write(0xa6);	// Normal display
		oled_cmdreg_write(0xaf);	// Display ON
	}
	
	void oled_print_arrow(){
		oled_gddram_write(0b00011000);
		oled_gddram_write(0b00011000);
		oled_gddram_write(0b01111110);
		oled_gddram_write(0b00111100);
		oled_gddram_write(0b00011000);
	}
	
	void oled_gddram_write(uint8_t data){
		volatile uint8_t *memspace_pointer = DAT_OFFSET;
		*memspace_pointer = data;
	}
	
	void set_addressing_mode(addr_mode mode){
		oled_cmdreg_write(0x20);	// Set Memory Addressing Mode
		oled_cmdreg_write(mode);
	}
	
	// Only works in horizontal or vertical address modes. To reset, send 0d and 127d
	void set_column_address_range(uint8_t start, uint8_t end){
		if (start < end)
		{
			oled_cmdreg_write(0x21);
			oled_cmdreg_write(start);
			oled_cmdreg_write(end);
		}
		else {
			printf("Error, column address setting: Start position is after end position \n");
		}
		
	}
	// Only works in horizontal or vertical address modes. To reset, send 0d and 7d
	void set_page_address_range(uint8_t start, uint8_t end){
		if (start < end)
		{
			oled_cmdreg_write(0x22);
			oled_cmdreg_write(start);
			oled_cmdreg_write(end);
		}
		else {
			printf("Error, page address setting: Start position is after end position \n");
		}
	}

	void address_range_reset(){
		set_column_address_range(0,127);
		set_page_address_range(0,7);
	}
	
	void testPrint_font(uint8_t ch){
		for(int i=0; i < sizeof(font8[ch]); i++){
			printf("Element of index %d = %d:\n", i, font8[1][i]);
		}
	}
	
