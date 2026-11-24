#ifndef ILI9341_H
#define ILI9341_H

#include "config.h"

void lcd_init(void);
void lcd_rect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t c);
void lcd_text(int16_t x, int16_t y, const char *s, uint16_t c, uint8_t z);
void lcd_number(int16_t x, int16_t y, uint16_t v);
void lcd_number_c(int16_t x, int16_t y, uint16_t v, uint16_t color);

#endif /* ILI9341_H */