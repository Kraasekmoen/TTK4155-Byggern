

void oled_cmdreg_write(uint8_t data);

void oled_init();
void oled_reset();
void oled_home();
void oled_goto_line();
void oled_goto_comlumn();
void oled_clear_line();
void oled_pos(uint8_t row,uint8_t column);
void oled_print(uint8_t *char);