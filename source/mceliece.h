#pragma once
#include <stddef.h>
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
void keygen(PublicKey* public_key, PrivateKey* secret_key);

uint8_t encode(const uint8_t block, const uint8_t sgp[MATRIX_SGP_ROWS]);

uint8_t decode(const uint8_t block, const uint8_t s_inv[MATRIX_S_DIM],
               const uint8_t p[MATRIX_P_DIM], const uint8_t h[MATRIX_H_ROWS],
               const uint8_t g[MATRIX_G_ROWS]);

int encrypt(const PublicKey* public_key, const uint8_t* in,
            const size_t in_len, uint8_t** out, size_t* out_len);

int decrypt(const PrivateKey* private_key, const uint8_t* cipher,
            const size_t cipher_len, uint8_t** out, size_t* out_len);
