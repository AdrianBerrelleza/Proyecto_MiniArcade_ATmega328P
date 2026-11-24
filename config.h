#ifndef CONFIG_H
#define CONFIG_H

#define F_CPU 16000000UL

#include <avr/io.h>
#include <stdint.h>

/* Paleta RGB565 */
#define BLACK  0x0000
#define WHITE  0xFFFF
#define CYAN   0x07FF
#define GREEN  0x07E0
#define RED    0xF800
#define YELLOW 0xFFE0

/* Tiempo por fotograma en ms */
#define FRAME_MS 55

/* Botones (máscaras tras leer PIND2..PIND4) */
#define BTN_D2    (1 << 0)
#define BTN_D3    (1 << 1)
#define BTN_START (1 << 2)

#endif /* CONFIG_H */