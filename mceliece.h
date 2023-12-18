#pragma once
#include <stdint.h>

void init_encCtx(void);
void encrypt(uint8_t mensaje[4], uint8_t cipher[7]);

void init_decCtx(uint8_t Sin[4][4], uint8_t Pin[7][7]);
void decrypt(uint8_t cipher[7], uint8_t mensaje[4]);
