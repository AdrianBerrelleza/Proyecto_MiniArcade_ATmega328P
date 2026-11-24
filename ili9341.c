#include "ili9341.h"
#include <avr/pgmspace.h>
#include <util/delay.h>

/* Fuente compacta 3x5 (0-9 y A-Z) almacenada en Flash */
static const uint16_t glyph[] PROGMEM = {
	0x7B6F, 0x2492, 0x73E7, 0x73CF, 0x5BC9, 0x79CF, 0x79EF, 0x7249, 0x7BEF, 0x7BCF,
	0x2BED, 0x6BAE, 0x7927, 0x6B6E, 0x79E7, 0x79E4, 0x796F, 0x5BED, 0x7497, 0x124E,
	0x5BAD, 0x4927, 0x5FED, 0x5F6D, 0x7B6F, 0x7BE4, 0x7B7B, 0x7BAD, 0x79CF, 0x7492,
	0x5B6F, 0x5B6A, 0x5BFD, 0x5AAD, 0x5A92, 0x72A7
};

static void spi_write(uint8_t d) {
	SPDR = d;
	while (!(SPSR & _BV(SPIF))) {
	}
}

static void lcd_cmd(uint8_t c) {
	PORTB &= ~(_BV(PB2) | _BV(PB1));
	spi_write(c);
	PORTB |= _BV(PB1);
}

static void lcd_end(void) {
	PORTB |= _BV(PB2);
}

static void lcd_word(uint16_t n) {
	spi_write(n >> 8);
	spi_write(n);
}

static void lcd_reg1(uint8_t c, uint8_t d) {
	lcd_cmd(c);
	spi_write(d);
	lcd_end();
}

void lcd_init(void) {
	SPCR = _BV(SPE) | _BV(MSTR);
	SPSR = 0; /* 4 MHz SPI */

	PORTB &= ~_BV(PB0);
	_delay_ms(20);
	PORTB |= _BV(PB0);
	_delay_ms(120);

	lcd_cmd(0x01);
	lcd_end();
	_delay_ms(150);

	lcd_reg1(0xC0, 0x23);
	lcd_reg1(0xC1, 0x10);

	lcd_cmd(0xC5);
	spi_write(0x3E);
	spi_write(0x28);
	lcd_end();

	lcd_reg1(0xC7, 0x86);
	lcd_reg1(0x36, 0x48);
	lcd_reg1(0x3A, 0x55);

	lcd_cmd(0xB1);
	spi_write(0);
	spi_write(0x18);
	lcd_end();

	lcd_cmd(0xB6);
	spi_write(8);
	spi_write(0x82);
	spi_write(0x27);
	lcd_end();

	lcd_cmd(0x11);
	lcd_end();
	_delay_ms(120);

	lcd_cmd(0x29);
	lcd_end();

	lcd_rect(0, 0, 240, 320, BLACK);
}

void lcd_rect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t c) {
	if (x < 0) { w += x; x = 0; }
	if (y < 0) { h += y; y = 0; }
	if (x + w > 240) { w = 240 - x; }
	if (y + h > 320) { h = 320 - y; }
	if (w <= 0 || h <= 0) { return; }

	lcd_cmd(0x2A);
	lcd_word(x);
	lcd_word(x + w - 1);
	lcd_end();

	lcd_cmd(0x2B);
	lcd_word(y);
	lcd_word(y + h - 1);
	lcd_end();

	lcd_cmd(0x2C);
	uint32_t n = (uint32_t)w * h;
	while (n--) {
		lcd_word(c);
	}
	lcd_end();
}

void lcd_text(int16_t x, int16_t y, const char *s, uint16_t c, uint8_t z) {
	while (*s) {
		char a = *s++;
		uint16_t bits = 0;

		if (a >= '0' && a <= '9') {
			bits = pgm_read_word(glyph + a - '0');
			} else if (a >= 'A' && a <= 'Z') {
			bits = pgm_read_word(glyph + 10 + a - 'A');
		}

		for (uint8_t r = 0; r < 5; r++) {
			for (uint8_t k = 0; k < 3; k++) {
				if (bits & (1U << (14 - r * 3 - k))) {
					lcd_rect(x + k * z, y + r * z, z, z, c);
				}
			}
		}
		x += 4 * z;
	}
}

void lcd_number_c(int16_t x, int16_t y, uint16_t v, uint16_t color) {
	char b[6];
	uint8_t i = 5;
	b[i] = 0;

	do {
		b[--i] = '0' + v % 10;
		v /= 10;
	} while (v);

	lcd_text(x, y, b + i, color, 2);
}

void lcd_number(int16_t x, int16_t y, uint16_t v) {
	lcd_number_c(x, y, v, WHITE);
}