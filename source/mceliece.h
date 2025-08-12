#pragma once
#include <stddef.h>
#include <stdint.h>
#include "matrix.h"

typedef struct {
    /** Matrix 'S' (4x4) **/
    uint8_t s[MATRIX_S_DIM];
    /** Matrix 'S' inverse **/
    uint8_t si[MATRIX_S_DIM];
    /** Matrix 'P' (7x7) **/
    uint8_t p[MATRIX_P_DIM];
    /** Matrix 'G' (4x7) **/
    uint8_t g[MATRIX_G_ROWS];
    /** Matrix 'H' traspose (7x3) **/
    uint8_t ht[MATRIX_H_ROWS];
} PrivateKey;

typedef struct {
    /** Matrix S*G*P (4x7) **/
    uint8_t sgp[MATRIX_SGP_ROWS];
} PublicKey;


void generate_s(uint8_t s[MATRIX_S_DIM]);
void generate_p(uint8_t p[MATRIX_P_DIM]);
void keygen(PublicKey* public_key, PrivateKey* secret_key);

uint8_t encode(const uint8_t block, const uint8_t sgp[MATRIX_SGP_ROWS]);

uint8_t decode(const uint8_t block, const uint8_t s_inv[MATRIX_S_DIM],
               const uint8_t p[MATRIX_P_DIM], const uint8_t h[MATRIX_H_ROWS]);

bool encrypt(const PublicKey* public_key, const uint8_t* in,
             const size_t in_len, uint8_t** out, size_t* out_len);

bool decrypt(const PrivateKey* private_key, const uint8_t* cipher,
             const size_t cipher_len, uint8_t** out, size_t* out_len);
