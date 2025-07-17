#include <string.h>
#include "matrix.h"
#include "bits.h"


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


void mult_matrices(const uint8_t* a, const int a_rows, const int a_cols,
    const uint8_t* b, const int b_cols, uint8_t* c) {

    for (int i = 0; i < a_rows; ++i) {
        uint8_t row = 0x0;

        for (int j = 0; j < b_cols; ++j) {
            uint8_t acc = 0x0;
            
            for (int k = 0; k < a_cols; ++k) {
                const uint8_t bit_a = GET(a, i, k);
                const uint8_t bit_b = GET(b, k, j);
                acc ^= (bit_a & bit_b);
            }
            row |= (acc << j);
        }
        c[i] = row;
    }
}
