#include "ranking.h"
#include <util/atomic.h>

/*
 * Mapa de la EEPROM (19 bytes de los 1024 disponibles):
 *   0x00      : firma 0xA5 (indica que el ranking ya fue inicializado)
 *   0x01-0x12 : 9 puntajes de 16 bits (little endian), 3 por juego,
 *               ordenados de mayor a menor.
 */
#define EE_MAGIC_ADDR 0
#define EE_MAGIC      0xA5
#define EE_DATA_ADDR  1

/* Copia en SRAM (18 bytes) para no leer la EEPROM al dibujar pantallas */
static uint16_t table[RANK_GAMES][RANK_SLOTS];

/* Lectura de un byte (acceso directo a registros EEAR, EECR y EEDR) */
static uint8_t ee_read(uint16_t addr) {
	while (EECR & _BV(EEPE)) {
	}
	EEAR = addr;
	EECR |= _BV(EERE);
	return EEDR;
}

/* Escritura de un byte solo si cambia (ahorra tiempo y ciclos de escritura) */
static void ee_update(uint16_t addr, uint8_t data) {
	if (ee_read(addr) == data) {
		return;
	}
	while (EECR & _BV(EEPE)) {
	}
	EEAR = addr;
	EEDR = data;
	/* EEPE debe activarse a lo mas 4 ciclos despues de EEMPE */
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		EECR |= _BV(EEMPE);
		EECR |= _BV(EEPE);
	}
}

static uint16_t slot_addr(uint8_t game, uint8_t pos) {
	return EE_DATA_ADDR + (game * RANK_SLOTS + pos) * 2;
}

static uint16_t ee_read_word(uint16_t addr) {
	return ee_read(addr) | ((uint16_t)ee_read(addr + 1) << 8);
}

static void ee_update_word(uint16_t addr, uint16_t v) {
	ee_update(addr, (uint8_t)v);
	ee_update(addr + 1, (uint8_t)(v >> 8));
}

void ranking_clear(void) {
	for (uint8_t g = 0; g < RANK_GAMES; g++) {
		for (uint8_t p = 0; p < RANK_SLOTS; p++) {
			table[g][p] = 0;
			ee_update_word(slot_addr(g, p), 0);
		}
	}
	/* La firma se escribe al final: si se corta la energia, se reinicia de nuevo */
	ee_update(EE_MAGIC_ADDR, EE_MAGIC);
}

void ranking_init(void) {
	if (ee_read(EE_MAGIC_ADDR) != EE_MAGIC) {
		ranking_clear(); /* EEPROM nueva (0xFF) o dañada */
		return;
	}
	for (uint8_t g = 0; g < RANK_GAMES; g++) {
		for (uint8_t p = 0; p < RANK_SLOTS; p++) {
			table[g][p] = ee_read_word(slot_addr(g, p));
		}
	}
}

uint16_t ranking_get(uint8_t game, uint8_t pos) {
	if (game >= RANK_GAMES || pos >= RANK_SLOTS) {
		return 0;
	}
	return table[game][pos];
}

int8_t ranking_submit(uint8_t game, uint16_t score) {
	if (game >= RANK_GAMES || score == 0) {
		return -1;
	}

	int8_t pos = -1;
	for (uint8_t p = 0; p < RANK_SLOTS; p++) {
		if (score > table[game][p]) { /* en empate queda debajo del existente */
			pos = (int8_t)p;
			break;
		}
	}
	if (pos < 0) {
		return -1;
	}

	for (uint8_t p = RANK_SLOTS - 1; p > (uint8_t)pos; p--) {
		table[game][p] = table[game][p - 1];
	}
	table[game][pos] = score;

	for (uint8_t p = (uint8_t)pos; p < RANK_SLOTS; p++) {
		ee_update_word(slot_addr(game, p), table[game][p]);
	}
	return pos;
}