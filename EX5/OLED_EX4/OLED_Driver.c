#include <OLED_Driver.h>
#include "fonts.h"
#include <globals.h>


/*
	Smart Address Space:
	0x1000-0x11FF:	Data Register
	0x1200-0x13FF:	Command Register
*/

	#define CMD_OFFSET 0x1000;	
	#define DAT_OFFSET 0x1200;
	
	volatile char *cmd_reg_pointer = CMD_OFFSET;
	volatile char *gddram_pointer = DAT_OFFSET;
	
	volatile oled_cursor_pos Position;

// Fundamental functions
	
	// Write data to the command register
	void oled_cmdreg_write(uint8_t data){
		cmd_reg_pointer[0] = data;
	}
	
	// Write data to the graphic display data ram
	void oled_gddram_write(uint8_t data){
		gddram_pointer[0] = data;
	}
	
	void oled_goto_page(uint8_t page){	//Wrapper for set_page_address_range
		// Check function bounds
		if (page < 0 || page > 7) { printf("OLED Error: Attempting to go to page out of bounds\n"); return;}
		set_page_address_range(page, 7);
		Position.page = page;
	}
	
	void oled_goto_column(uint8_t col){ //Wrapper for set_column_address_range
		// Check function bounds
		if (col < 0 || col > 127) { printf("OLED Error: Attempting to go to column out of bounds\n"); return;}
		set_column_address_range(col, 127);	
		Position.col = col;
	}
	
	void oled_set_cursor_pos(uint8_t row,uint8_t column){ //Wrapper for goto-functions
		oled_goto_page(row);
		oled_goto_column(column);
	}

	// Set all pixels in page to zero
	void oled_clear_page(uint8_t page){
		// Set cursor to beginning of page
		oled_set_cursor_pos(page, 0);
		// Clear each column in page
		for (int column = 0; column <= 127; column++){
			oled_gddram_write(0b00000000);
		}
		// Return cursor to beginning of page
		oled_set_cursor_pos(page, 0);
	}
	
	void oled_cursor_home(){ //Wrapper for set_cursor_pos
		oled_set_cursor_pos(0,0);
	}
	
	// Set all pixels on screen to zero
	void oled_reset(){
		for (uint8_t page = 0; page < 8; page++){
			oled_clear_page(page);
		}
		oled_cursor_home();
	}
	
	void oled_print_large(unsigned char character){
		uint8_t c = character - 32;
		uint8_t fontSize = sizeof(font8[c]);
		
		for (int i = 0; i < fontSize; i++){
			oled_gddram_write(pgm_read_word(&font8[c][i]));
			Position.col += fontSize;
		}
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
	
// Option functions
	
	void set_addressing_mode(addr_mode mode){
		oled_cmdreg_write(0x20);	// Set Memory Addressing Mode
		oled_cmdreg_write(mode);
	}
	
	// Only works in horizontal or vertical address modes. To reset, send 0d and 127d
	void set_column_address_range(uint8_t start, uint8_t end){
		
		// Set address mode to horizontal, just to be sure
		set_addressing_mode(Horizontal);
		
		if (start <= end)
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
		
		// Set address mode to horizontal, just to be sure
		set_addressing_mode(Horizontal);		

		if (start <= end)
		{
			oled_cmdreg_write(0x22);
			oled_cmdreg_write(start);
			oled_cmdreg_write(end);
		}
		else {
			printf("Error, page address setting: Start position is after end position \n");
		}
	}

	void reset_address_range(){
		set_column_address_range(0,127);
		set_page_address_range(0,7);
	}
	



// Test functions below

void oled_invert_screen(){
	oled_cmdreg_write(0xa7);

}
void oled_screen_normal(){
	oled_cmdreg_write(0xa6);
}

	
void oled_print_arrow(){
	oled_gddram_write(0b00011000);
	oled_gddram_write(0b00011000);
	oled_gddram_write(0b01111110);
	oled_gddram_write(0b00111100);
	oled_gddram_write(0b00011000);
}
		
	
void testPrint_font(uint8_t ch){
	for(int i=0; i < sizeof(font8[ch]); i++){
		printf("Element of index %d = %d:\n", i, pgm_read_word(&font8[ch][i]));
	}
}
	
