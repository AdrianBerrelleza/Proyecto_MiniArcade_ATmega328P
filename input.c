#include "input.h"
#include "hal.h"

static uint8_t keys = 0, press = 0, raw_old = 0, stable = 0;
static uint32_t changed = 0;
static int16_t center_x = 512;

void input_init(void) {
	uint32_t sum = 0;
	for (uint8_t i = 0; i < 16; i++) {
		sum += hal_adc_read(2);
	}
	center_x = sum / 16;
}

void input_update(void) {
	uint8_t r = (~PIND >> 2) & 7;
	uint32_t t = hal_now();
	press = 0;

	if (r != raw_old) {
		raw_old = r;
		changed = t;
	}
	if (t - changed >= 25 && r != stable) {
		press = r & ~stable;
		stable = r;
	}
	keys = stable;
}

uint8_t input_keys(void) {
	return keys;
}

uint8_t input_pressed(void) {
	return press;
}

int8_t input_axis_x(void) {
	int16_t a = (int16_t)hal_adc_read(2) - center_x;
	return a > 160 ? 1 : (a < -160 ? -1 : 0);
}

int8_t input_axis_y(void) {
	int16_t y = 512 - (int16_t)hal_adc_read(3);
	return y > 180 ? 1 : (y < -180 ? -1 : 0);
}