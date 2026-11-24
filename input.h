#ifndef INPUT_H
#define INPUT_H

#include "config.h"

void input_init(void);
void input_update(void);
uint8_t input_keys(void);
uint8_t input_pressed(void);
int8_t input_axis_x(void);
int8_t input_axis_y(void);

#endif /* INPUT_H */