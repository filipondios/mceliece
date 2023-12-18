#pragma once
#include <stdint.h>

void add_error(uint8_t c[7]);
void encode(uint8_t m[4], uint8_t c[7]);
void decode(uint8_t c[7], uint8_t m[4]);
