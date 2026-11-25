#include "game_core.h"
#include "ili9341.h"
#include "hal.h"
#include "ranking.h"

GameContext game;
static uint16_t rng = 0xACE1;

void game_seed(uint16_t seed) {
	/* Un LFSR nunca debe tener semilla 0 */
	rng = seed ? seed : 0xACE1;
}

uint16_t game_random16(void) {
	rng = (rng >> 1) ^ ((0 - (rng & 1)) & 0xB400);
	return rng;
}
/* ... el resto de game_core.c permanece igual ... */