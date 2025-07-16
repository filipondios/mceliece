#include <string.h>
#include <stdbool.h>
#include <sodium/randombytes.h>

// Bit operations over a matrix position (i, j)
#define GET(m, i, j)   (((m[i]) >> (j)) & 0x1) 
#define SET(m, i, j)   (m[i] |= (0x1 << (j)))
#define UNSET(m, i, j) (m[i] &= ~(0x1 << (j)))

// System matrices dims
#define MATRIX_S_DIM 4
#define MATRIX_H_DIM 4
#define MATRIX_P_DIM 7
#define MATRIX_G_DIM 4

static uint8_t P [7] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02 };
static const uint8_t H [3] = { 0x1E, 0x66, 0xAA };
static const uint8_t G [4] = { 0xE0, 0x98, 0x54, 0xD2 };


bool gauss(const uint8_t m[MATRIX_S_DIM], uint8_t b[MATRIX_S_DIM]) {
    memcpy(b, m, MATRIX_S_DIM);

    for (int col = 0; col < MATRIX_S_DIM; ++col) {
        int pivot = -1;

        for (int row = col; row < MATRIX_S_DIM; ++row) {
            if (GET(b, row, col)) {
                pivot = row;
                break;
            }
        }

        if (pivot == -1)
            return false;

        if (pivot != col) {
            const uint8_t tmp = b[col];
            b[col] = b[pivot];
            b[pivot] = tmp;
        }

        for (int row = 0; row < MATRIX_S_DIM; ++row) {
            if (row != col && GET(b, row, col))
                b[row] ^= b[col];
        }
    }
    return true;
}


static void generate_s(uint8_t s[MATRIX_S_DIM]) {
  for (;;) {
    randombytes_buf(s, MATRIX_S_DIM);
    uint8_t dummy[MATRIX_S_DIM];

    if (gauss(s, dummy))
        break;
  }
}

static void generate_p(uint8_t p[MATRIX_P_DIM]) {
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




