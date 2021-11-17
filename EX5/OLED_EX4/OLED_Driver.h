#ifndef OLED_DRIVER_H
#define OLED_DRIVER_H

	#include <stdio.h>
	#include <stdint.h>
	#include <stdlib.h>


	typedef enum {Horizontal=0x00, Vertical=0x01, Page=0x02}addr_mode;
	typedef struct {uint8_t col, page; } oled_cursor_pos;

	void oled_cmdreg_write(uint8_t data);
	void oled_gddram_write(uint8_t data);

	void oled_init();
	void oled_reset();
	void oled_home();
	void oled_goto_page(uint8_t page);
	void oled_goto_column(uint8_t col);
	void oled_clear_page(uint8_t page);
	void oled_set_cursor_pos(uint8_t row,uint8_t column);
	void oled_print_large(unsigned char character);

	void set_addressing_mode(addr_mode mode);
	
	void set_column_address_range(uint8_t start, uint8_t end);
	void set_page_address_range(uint8_t start, uint8_t end);
	void address_range_reset();

	
	void oled_print_arrow();
	void testPrint_font(uint8_t ch);

void oled_invert_screen();

void oled_screen_normal();

#endif

