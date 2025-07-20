#pragma once
#include <stdint.h>
#include "matrix.h"

typedef struct {
    uint8_t s[MATRIX_S_DIM];
    uint8_t p[MATRIX_P_DIM];
    uint8_t g[MATRIX_G_ROWS];
    uint8_t h[MATRIX_H_ROWS];
} PrivateKey;

typedef struct {
    uint8_t sgp[MATRIX_SGP_ROWS];
} PublicKey;


void generate_s(uint8_t s[MATRIX_S_DIM]);
void generate_p(uint8_t p[MATRIX_P_DIM]);
void keygen(PublicKey* publickey, PrivateKey* secretkey);
