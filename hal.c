#include "hal.h"
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <util/delay.h>

static volatile uint32_t ticks = 0;
static volatile uint16_t sound_ms = 0;

ISR(TIMER0_COMPA_vect) {
	ticks++;
	if (sound_ms && !--sound_ms) {
		TIMSK1 = 0;
		PORTD &= ~_BV(PD6);
	}
}

ISR(TIMER1_COMPA_vect) {
	PIND = _BV(PD6);
}

void hal_init(void) {
	/* Configuración de puertos */
	DDRB = _BV(PB0) | _BV(PB1) | _BV(PB2) | _BV(PB3) | _BV(PB5);
	PORTB = _BV(PB0) | _BV(PB1) | _BV(PB2);
	DDRD = _BV(PD6);
	PORTD = _BV(PD2) | _BV(PD3) | _BV(PD4) | _BV(PD5);
	DDRC = _BV(PC1);
	PORTC = 0;
	_delay_ms(30);
	PORTC |= _BV(PC1);
	_delay_ms(10);

	/* ADC (Prescaler 128 -> 125 kHz) */
	ADCSRA = _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);

	/* Timer 0: Base de tiempo de 1 ms (CTC) */
	TCCR0A = _BV(WGM01);
	TCCR0B = _BV(CS01) | _BV(CS00);
	OCR0A = 249;
	TIMSK0 = _BV(OCIE0A);

	/* Timer 1: Generador de tonos en PD6 (CTC) */
	TCCR1A = 0;
	TCCR1B = _BV(WGM12) | _BV(CS11);
}

uint32_t hal_now(void) {
	uint32_t n;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		n = ticks;
	}
	return n;
}

uint16_t hal_adc_read(uint8_t ch) {
	ADMUX = _BV(REFS0) | ch;
	ADCSRA |= _BV(ADSC);
	while (ADCSRA & _BV(ADSC)) {
	}
	return ADC;
}

void hal_beep(uint16_t hz, uint16_t ms) {
	if (hal_adc_read(0) < 80) {
		return;
	}
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		TIMSK1 = 0;
		OCR1A = (uint16_t)(F_CPU / 8UL / 2 / hz - 1);
		TCNT1 = 0;
		TIFR1 = _BV(OCF1A);
		sound_ms = ms;
		TIMSK1 = _BV(OCIE1A);
	}
}