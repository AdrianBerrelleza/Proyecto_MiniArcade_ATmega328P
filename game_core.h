#ifndef GAME_CORE_H
#define GAME_CORE_H

#include "config.h"

typedef struct {
	uint8_t mode;       /* 0=Menu, 1=Breakout, 2=Invaders, 3=Esquivar */
	uint8_t selected;
	uint8_t paused;
	uint8_t over;
	uint8_t lives;
	uint16_t score;
	int16_t player;

	/* Variables compartidas entre minijuegos */
	int16_t bx, by, vx, vy;
	int16_t shotx, shoty, ex, ey, enemyx, enemyy;
	uint8_t bricks[24];
	uint8_t aliens[15];
	uint8_t launch;
	uint8_t frame;
	uint8_t enemyclock;
	int16_t ox[3], oy[3];
} GameContext;

extern GameContext game;

uint16_t game_random16(void);
uint8_t game_hit(int16_t x, int16_t y, int16_t w, int16_t h,
int16_t a, int16_t b, int16_t c, int16_t d);

void ui_hud(void);
void ui_menu(void);
void ui_finish(uint8_t win);

#endif /* GAME_CORE_H */