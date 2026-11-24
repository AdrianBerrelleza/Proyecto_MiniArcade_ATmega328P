#ifndef RANKING_H
#define RANKING_H

#include "config.h"

#define RANK_GAMES 3  /* Breakout, Invaders, Esquivar */
#define RANK_SLOTS 3  /* Mejores 3 puntajes por juego */

void ranking_init(void);                          /* Carga el ranking desde la EEPROM */
void ranking_clear(void);                         /* Borra el ranking (EEPROM y cache) */
uint16_t ranking_get(uint8_t game, uint8_t pos);  /* Puntaje de la posicion pos (0 = primero) */
int8_t ranking_submit(uint8_t game, uint16_t score); /* Posicion lograda (0..2) o -1 si no entra */

#endif /* RANKING_H */