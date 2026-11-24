#include "game_core.h"
#include "ili9341.h"
#include "hal.h"
#include "ranking.h"

GameContext game;
static uint16_t rng = 0xACE1;

uint16_t game_random16(void) {
	rng = (rng >> 1) ^ ((0 - (rng & 1)) & 0xB400);
	return rng;
}

uint8_t game_hit(int16_t x, int16_t y, int16_t w, int16_t h,
int16_t a, int16_t b, int16_t c, int16_t d) {
	return x < a + c && x + w > a && y < b + d && y + h > b;
}

void ui_hud(void) {
	lcd_rect(0, 0, 240, 22, BLACK);
	lcd_text(4, 5, "PTS", CYAN, 2);
	lcd_number(32, 5, game.score);
	lcd_text(160, 5, "VIDAS", CYAN, 2);
	lcd_number(210, 5, game.lives);
}

void ui_menu(void) {
	game.mode = 0;
	game.paused = 0;
	game.over = 0;
	lcd_rect(0, 0, 240, 320, BLACK);
	lcd_text(24, 25, "MINI ARCADE", CYAN, 4);
	lcd_text(20, 100, "BREAKOUT", game.selected == 0 ? YELLOW : WHITE, 3);
	lcd_text(20, 150, "INVADERS", game.selected == 1 ? YELLOW : WHITE, 3);
	lcd_text(20, 200, "ESQUIVAR", game.selected == 2 ? YELLOW : WHITE, 3);

	/* Mejor puntaje de cada juego (guardado en EEPROM) */
	for (uint8_t g = 0; g < RANK_GAMES; g++) {
		lcd_text(20, 122 + g * 50, "REC", CYAN, 2);
		lcd_number(56, 122 + g * 50, ranking_get(g, 0));
	}
	lcd_text(16, 275, "START PARA JUGAR", GREEN, 2);
}

/* Muestra el top 3 del juego; la posicion nueva (pos) se resalta */
static void ui_ranking(uint8_t g, int8_t pos) {
	lcd_rect(0, 195, 240, 125, BLACK);
	lcd_text(16, 205, "RANKING", CYAN, 2);
	for (uint8_t p = 0; p < RANK_SLOTS; p++) {
		int16_t y = 228 + p * 22;
		uint16_t color = (p == pos) ? YELLOW : WHITE;
		lcd_number_c(16, y, p + 1, color);
		lcd_number_c(48, y, ranking_get(g, p), color);
		if (p == pos) {
			lcd_text(130, y, "NUEVO", GREEN, 2);
		}
	}
}

void ui_finish(uint8_t win) {
	game.over = 1;
	int8_t pos = ranking_submit(game.mode - 1, game.score);
	lcd_rect(0, 130, 240, 60, BLACK);
	lcd_text(20, 140, win ? "GANASTE" : "GAME OVER", YELLOW, 4);
	lcd_text(16, 175, "START PARA MENU", WHITE, 2);
	ui_ranking(game.mode - 1, pos);
	hal_beep(win ? 1200 : 180, 250);
}