#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "matrix.h"
#include "bits.h"

bool gauss(const uint8_t m[MATRIX_S_DIM], uint8_t b[MATRIX_S_DIM]) {
    memcpy(b, m, MATRIX_S_DIM);

    for (uint8_t row = 0x0; row < MATRIX_S_DIM; ++row) {
        const uint8_t hi = GET_MSB4_BLOCK(b[row]);
        const uint8_t lo = (uint8_t) (MSB >> (row + LSB4_SHIFT));
        b[row] = hi | lo;
    }

    for (int col = 0x0; col < MATRIX_S_DIM; ++col) {
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

        for (int row = 0x0; row < MATRIX_S_DIM; ++row) {
            if (row != col && GET(b, row, col))
                b[row] ^= b[col];
        }
    }

    for (int i = 0; i < MATRIX_S_DIM; ++i) {
        uint8_t row = GET_LSB4_BLOCK(b[i]);
        row <<= LSB4_SHIFT; 
        b[i] = row;
    }
    return true;
}

void mult_matrices(const uint8_t* a, const size_t a_rows, const size_t a_cols,
    const uint8_t* b, const size_t b_cols, uint8_t* c) {

    for (size_t i = 0; i < a_rows; ++i) {
        uint8_t row = 0x0;

        for (size_t j = 0; j < b_cols; ++j) {
            uint8_t acc = 0x0;
            
            for (size_t k = 0; k < a_cols; ++k) {
                const uint8_t bit_a = GET(a, i, k);
                const uint8_t bit_b = GET(b, k, j);
                acc ^= (bit_a && bit_b);
            }

            if (acc)
                row |= MSB >> j;
        }
        c[i] = row;
    }
}

void print_matrix(const char* title, const uint8_t* matrix, const size_t rows,
    const size_t cols){
    
    printf("%s = {", title);

    for (size_t row = 0; row < rows; ++row) {
        const uint8_t row_vals = matrix[row];
        
        for (size_t col = 0; col < cols; ++col) {
            const uint8_t bit = GET_BIT(row_vals, col);
            const char num = bit? '1' : '0';
            char sep;

            if (row == rows - 1) { sep = (col == cols - 1)? ' ' : ','; }
            else { sep = (col == cols - 1)? ';' : ','; }

            printf("%c%c", num, sep);
        }
    }    
    printf("}\n");
}
