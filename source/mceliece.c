#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <sodium/randombytes.h>
#include "mceliece.h"
#include "matrix.h"
#include "bits.h"


void generate_s(uint8_t s[MATRIX_S_DIM]) {
    for (;;) {
        // S = Random 4x4 invertible matrix
        randombytes_buf(s, MATRIX_S_DIM);
        uint8_t dummy[MATRIX_S_DIM];

        if (gauss(s, dummy))
            break;
    }

    // Clean up unused bits in the S matrix
    // Rows are stored at the 4 MSBs of s[i]
    for (int i = 0; i < MATRIX_S_DIM; ++i) {
        s[i] &= 0xf0;
    }
}

void generate_p(uint8_t p[MATRIX_P_DIM]) {
    uint8_t indices[MATRIX_P_DIM];
    const uint8_t fixed = (randombytes_random() % MATRIX_P_DIM);
    memset(p, 0x0, MATRIX_P_DIM);

    for (uint8_t i = 0x0; i < MATRIX_P_DIM; ++i) {
        // indices[row] = active column bit. Here indices
        // is being initialized as the identity matrix.
        indices[i] = i;
    }

    // Fisher-Yates shuffle, except for one position
    for (uint8_t i = MATRIX_P_DIM - 0x1; i > 0x0; --i) {
        if (i == fixed) { continue; }
        uint8_t j;

        do {
            randombytes_buf((void*) &j, sizeof(uint8_t));
            j %= (i + 1);
        } while (j == fixed);

        const uint8_t tmp = indices[i];
        indices[i] = indices[j];
        indices[j] = tmp;
    }

    // Build the shuffled identity matrix
    for (uint8_t i = 0; i < MATRIX_P_DIM; ++i) {
        SET(p, i, indices[i]);
    }
}

void keygen(PublicKey* publickey, PrivateKey* secretkey) {
    const uint8_t g[MATRIX_G_ROWS] = { 0x70, 0x4c, 0x2a, 0x69 };
    generate_s(secretkey->s);
    generate_p(secretkey->p);
    memcpy(secretkey->g, g, MATRIX_G_ROWS);

    uint8_t sg[MATRIX_SG_ROWS];
    mult_matrices(secretkey->s, MATRIX_S_DIM, MATRIX_S_DIM, secretkey->g, MATRIX_G_COLS, sg);
    mult_matrices(sg, MATRIX_SG_ROWS, MATRIX_SG_COLS, secretkey->p, MATRIX_P_DIM, publickey->sgp);
}
