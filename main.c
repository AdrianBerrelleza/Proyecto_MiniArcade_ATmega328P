#include "config.h"
#include "hal.h"
#include "ili9341.h"
#include "input.h"
#include "game_core.h"
#include "games.h"
#include "ranking.h"
#include <avr/interrupt.h>

int main(void) {
	hal_init();
	sei();

	lcd_init();
	input_init();

	/* Carga el ranking; D2 + D3 presionados al encender lo borran */
	ranking_init();
	if (((~PIND >> 2) & (BTN_D2 | BTN_D3)) == (BTN_D2 | BTN_D3)) {
		ranking_clear();
	}
	ui_menu();

	uint32_t last = hal_now();
	uint32_t nav = 0;
	uint8_t navheld = 0;

	for (;;) {
		input_update();
		uint32_t t = hal_now();
		uint8_t press = input_pressed();

		if (!game.mode) {
			int8_t d = input_axis_y();
			if (press & BTN_D2) d = -1;
			if (press & BTN_D3) d = 1;

			if (d && (!navheld || t - nav > 250)) {
				game.selected = (game.selected + (d > 0 ? 1 : 2)) % 3;
				ui_menu();
				nav = t;
			}
			navheld = (d != 0);

			if (press & BTN_START) {
				game_start();
				last = t;
			}
			continue;
		}

		if (game.over) {
			if (press & BTN_START) {
				ui_menu();
			}
			continue;
		}

		if (press & BTN_START) {
			game.paused = !game.paused;
			lcd_rect(0, 307, 240, 13, BLACK);
			if (game.paused) {
				lcd_text(8, 309, "PAUSA  B2 PARA MENU", YELLOW, 2);
			}
			last = t;
		}

		if (game.paused) {
			if (press & BTN_D3) {
				ui_menu();
			}
			continue;
		}

		/* Conserva el flanco de lanzamiento entre fotogramas */
		if (game.mode == 1 && !game.launch && (press & BTN_D2)) {
			game.launch = 1;
			hal_beep(800, 40);
		}

		if (t - last >= FRAME_MS) {
			last = t;
			if (game.mode == 1) {
				game_breakout_update();
				} else if (game.mode == 2) {
				game_invaders_update();
				} else {
				game_dodge_update();
			}
		}
	}
}