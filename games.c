#include "games.h"
#include "game_core.h"
#include "ili9341.h"
#include "input.h"
#include "hal.h"

static void draw_bricks(void) {
	for (uint8_t i = 0; i < 24; i++) {
		if (game.bricks[i]) {
			lcd_rect(4 + (i % 6) * 39, 38 + (i / 6) * 16, 35, 12,
			(i / 6) & 1 ? CYAN : YELLOW);
		}
	}
}

static void draw_fleet(uint16_t color) {
	for (uint8_t i = 0; i < 15; i++) {
		if (game.aliens[i]) {
			int16_t x = game.ex + (i % 5) * 40;
			int16_t y = game.ey + (i / 5) * 23;
			lcd_rect(x, y, 20, 12, color);
			if (color) {
				lcd_rect(x + 4, y + 3, 3, 3, BLACK);
				lcd_rect(x + 13, y + 3, 3, 3, BLACK);
			}
		}
	}
}

static void reset_ball(void) {
	game.launch = 0;
	game.bx = game.player + 19;
	game.by = 280;
	game.vx = 2;
	game.vy = -3;
	lcd_rect(game.bx, game.by, 5, 5, WHITE);
}

void game_start(void) {
	game.mode = game.selected + 1;
	game.score = 0;
	game.lives = 3;
	game.player = 100;
	game.frame = 0;
	game.enemyclock = 0;
	game.paused = 0;
	game.over = 0;
	game.shoty = -20;
	game.enemyy = -20;
	game.ex = 15;
	game.ey = 40;
	game.vx = 2;

	lcd_rect(0, 0, 240, 320, BLACK);
	ui_hud();

	if (game.mode == 1) {
		for (uint8_t i = 0; i < 24; i++) {
			game.bricks[i] = 1;
		}
		draw_bricks();
		lcd_rect(game.player, 290, 42, 7, CYAN);
		reset_ball();
		} else if (game.mode == 2) {
		for (uint8_t i = 0; i < 15; i++) {
			game.aliens[i] = 1;
		}
		draw_fleet(GREEN);
		lcd_rect(game.player, 290, 22, 12, CYAN);
		} else if (game.mode == 3) {
		for (uint8_t i = 0; i < 3; i++) {
			game.ox[i] = 20 + game_random16() % 180;
			game.oy[i] = -100 - i * 95;
		}
		lcd_rect(2, 24, 2, 296, WHITE);
		lcd_rect(236, 24, 2, 296, WHITE);
		lcd_rect(game.player, 285, 16, 24, CYAN);
		lcd_rect(game.player + 3, 289, 10, 5, WHITE);
	}
}

void game_breakout_update(void) {
	int16_t old_player = game.player;
	int16_t old_bx = game.bx;
	int16_t old_by = game.by;

	game.player += input_axis_x() * 5;
	if (game.player < 0) game.player = 0;
	if (game.player > 198) game.player = 198;

	if (!game.launch) {
		game.bx = game.player + 19;
		game.by = 280;
		if (input_pressed() & BTN_D2) {
			game.launch = 1;
			hal_beep(800, 40);
		}
		} else {
		int16_t oldy = game.by;
		game.bx += game.vx;
		game.by += game.vy;

		if (game.bx < 0) { game.bx = 0; game.vx = -game.vx; }
		if (game.bx > 235) { game.bx = 235; game.vx = -game.vx; }
		if (game.by < 25) { game.by = 25; game.vy = -game.vy; }

		if (game.vy > 0 && oldy + 5 <= 290 &&
		game_hit(game.bx, game.by, 5, 5, game.player, 290, 42, 7)) {
			game.by = 285;
			game.vy = -3;
			game.vx = game.bx < game.player + 20 ? -2 : 2;
			hal_beep(600, 25);
		}

		for (uint8_t i = 0; i < 24; i++) {
			if (game.bricks[i] &&
			game_hit(game.bx, game.by, 5, 5, 4 + (i % 6) * 39, 38 + (i / 6) * 16, 35, 12)) {
				game.bricks[i] = 0;
				lcd_rect(4 + (i % 6) * 39, 38 + (i / 6) * 16, 35, 12, BLACK);
				game.vy = -game.vy;
				game.score += 10;
				ui_hud();
				hal_beep(1000, 25);
				break;
			}
		}

		if (game.by > 315) {
			if (!--game.lives) {
				ui_finish(0);
				return;
			}
			ui_hud();
			lcd_rect(old_bx, old_by, 5, 5, BLACK);
			reset_ball();
			old_bx = game.bx;
			old_by = game.by;
		}
		if (game.score == 240) {
			ui_finish(1);
			return;
		}
	}

	if (game.player != old_player) {
		lcd_rect(old_player, 290, 42, 7, BLACK);
		lcd_rect(game.player, 290, 42, 7, CYAN);
	}
	if (game.bx != old_bx || game.by != old_by) {
		lcd_rect(old_bx, old_by, 5, 5, BLACK);
		lcd_rect(game.bx, game.by, 5, 5, WHITE);
	}
}

void game_invaders_update(void) {
	int16_t old_player = game.player;
	int16_t old_shotx = game.shotx, old_shoty = game.shoty;
	int16_t old_enemyx = game.enemyx, old_enemyy = game.enemyy;

	game.player += input_axis_x() * 5;
	if (game.player < 0) game.player = 0;
	if (game.player > 218) game.player = 218;

	if ((input_keys() & BTN_D2) && game.shoty < 0) {
		game.shotx = game.player + 10;
		game.shoty = 280;
		hal_beep(900, 20);
	}
	if (game.shoty >= 0) {
		game.shoty -= 8;
	}

	if (++game.frame >= 6) {
		game.frame = 0;
		draw_fleet(BLACK);
		game.ex += game.vx;
		if (game.ex < 2 || game.ex > 58) {
			game.vx = -game.vx;
			game.ex += game.vx;
			game.ey += 10;
		}
		draw_fleet(GREEN);
	}

	uint8_t alive = 0;
	for (uint8_t i = 0; i < 15; i++) {
		if (game.aliens[i]) {
			alive++;
			int16_t x = game.ex + (i % 5) * 40;
			int16_t y = game.ey + (i / 5) * 23;

			if (game.shoty >= 0 && game_hit(game.shotx, game.shoty, 3, 8, x, y, 20, 12)) {
				game.aliens[i] = 0;
				lcd_rect(x, y, 20, 12, BLACK);
				game.shoty = -20;
				game.score += 10;
				ui_hud();
				hal_beep(1300, 30);
			}
			if (y + 12 >= 290) {
				ui_finish(0);
				return;
			}
		}
	}

	if (!alive) {
		ui_finish(1);
		return;
	}

	if (game.enemyy < 0 && ++game.enemyclock >= 18) {
		game.enemyclock = 0;
		uint8_t j = game_random16() % 15;
		for (uint8_t k = 0; k < 15; k++) {
			uint8_t i = (j + k) % 15;
			if (game.aliens[i]) {
				game.enemyx = game.ex + (i % 5) * 40 + 8;
				game.enemyy = game.ey + (i / 5) * 23 + 12;
				break;
			}
		}
	}

	if (game.enemyy >= 0) {
		game.enemyy += 5;
		if (game_hit(game.enemyx, game.enemyy, 4, 8, game.player, 290, 22, 12)) {
			game.enemyy = -20;
			hal_beep(180, 100);
			if (!--game.lives) {
				ui_finish(0);
				return;
			}
			ui_hud();
			} else if (game.enemyy > 319) {
			game.enemyy = -20;
		}
	}

	if (old_shoty >= 22) lcd_rect(old_shotx, old_shoty, 3, 8, BLACK);
	if (game.shoty >= 22) lcd_rect(game.shotx, game.shoty, 3, 8, YELLOW);

	if (old_enemyy >= 22) {
		lcd_rect(old_enemyx, old_enemyy, 4, 8, BLACK);
		if (game.frame != 0) {
			draw_fleet(GREEN);
		}
	}
	if (game.enemyy >= 22) lcd_rect(game.enemyx, game.enemyy, 4, 8, RED);

	if (game.player != old_player) {
		lcd_rect(old_player, 290, 22, 12, BLACK);
		lcd_rect(game.player, 290, 22, 12, CYAN);
		} else if (old_enemyy >= 280) {
		lcd_rect(game.player, 290, 22, 12, CYAN);
	}
}

void game_dodge_update(void) {
	int16_t old_player = game.player;
	uint8_t k = input_keys();

	int8_t a = input_axis_x();
	if (k & BTN_D2) a = -1;
	if (k & BTN_D3) a = 1;

	game.player += a * 5;
	if (game.player < 8) game.player = 8;
	if (game.player > 216) game.player = 216;

	if (game.player != old_player) {
		lcd_rect(old_player, 285, 16, 24, BLACK);
		lcd_rect(game.player, 285, 16, 24, CYAN);
		lcd_rect(game.player + 3, 289, 10, 5, WHITE);
	}

	for (uint8_t i = 0; i < 3; i++) {
		int16_t old_oy = game.oy[i];
		game.oy[i] += 3 + (game.score / 15 > 4 ? 4 : game.score / 15);

		if (game.oy[i] > 320) {
			if (old_oy >= 24) lcd_rect(game.ox[i], old_oy, 22, 25, BLACK);
			game.oy[i] = -40;
			game.ox[i] = 8 + game_random16() % 203;
			game.score++;
			ui_hud();
			} else if (game_hit(game.player, 285, 16, 24, game.ox[i], game.oy[i], 22, 25)) {
			if (old_oy >= 24) lcd_rect(game.ox[i], old_oy, 22, 25, BLACK);
			lcd_rect(game.player, 285, 16, 24, CYAN);
			lcd_rect(game.player + 3, 289, 10, 5, WHITE);
			game.oy[i] = -80;
			game.ox[i] = 8 + game_random16() % 203;
			hal_beep(160, 100);
			if (!--game.lives) {
				ui_finish(0);
				return;
			}
			ui_hud();
			} else {
			if (old_oy >= 24) lcd_rect(game.ox[i], old_oy, 22, 25, BLACK);
			if (game.oy[i] >= 24) lcd_rect(game.ox[i], game.oy[i], 22, 25, RED);
		}
	}
}