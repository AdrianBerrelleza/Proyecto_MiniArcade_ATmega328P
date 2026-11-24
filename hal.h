#ifndef HAL_H
#define HAL_H

#include "config.h"

void hal_init(void);
uint32_t hal_now(void);
uint16_t hal_adc_read(uint8_t ch);
void hal_beep(uint16_t hz, uint16_t ms);

#endif /* HAL_H */