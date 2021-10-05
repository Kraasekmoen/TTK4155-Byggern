#include "OLED_Driver.h"

/*
	Smart Address Space:
	0x1000-0x11FF:	Data Register
	0x1200-0x13FF:	Command Register
*/

	#define CMD_OFFSET 0x1200;	
	#define DAT_OFFSET 0x1000;
	
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
		oled_cmdreg_write(0x02);
		oled_cmdreg_write(0xdb);	// VCOM deselect level mode
		oled_cmdreg_write(0x30);
		oled_cmdreg_write(0xad);	// Master configuration 
		oled_cmdreg_write(0x00);	
		oled_cmdreg_write(0xa4);	// Display follows RAM content
		oled_cmdreg_write(0xa6);	// Normal display
		oled_cmdreg_write(0xaf);	// Display ON
	}

