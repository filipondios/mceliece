#include <string.h>
#include <stdbool.h>
#include <sodium/randombytes.h>
#include "mceliece.h"
#include "matrix.h"


void generate_s(uint8_t s[MATRIX_S_DIM]) {
    for (;;) {
        randombytes_buf(s, MATRIX_S_DIM);
        uint8_t dummy[MATRIX_S_DIM];

        if (gauss(s, dummy))
            break;
    }
}

void generate_p(uint8_t p[MATRIX_P_DIM]) {
    uint8_t idxs[MATRIX_P_DIM] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 };
    const uint8_t id [7] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40 };
    memcpy(p, id, MATRIX_P_DIM);

    // Fisher-Yates shuffle
    for (uint8_t i = MATRIX_P_DIM - 1; i > 0; --i) {
        const uint8_t j = randombytes_random() % (i + 1);
        const uint8_t tmp = idxs[i];
        idxs[i] = idxs[j];
        idxs[j] = tmp;
    }

    for (uint8_t i = 0; i < MATRIX_P_DIM; ++i)
        p[i] = id[idxs[i]];
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
