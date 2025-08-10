#pragma once
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// Square matrices (S, P)
#define MATRIX_S_DIM 4
#define MATRIX_P_DIM 7
// Hamming code *transpose* matrix
#define MATRIX_H_ROWS 7
#define MATRIX_H_COLS 3
// Code generator matrix
#define MATRIX_G_ROWS 4
#define MATRIX_G_COLS 7
// SxG Matrix result
#define MATRIX_SG_ROWS 4
#define MATRIX_SG_COLS 7
// McEliece SxGxP Matrix result
#define MATRIX_SGP_ROWS MATRIX_SG_ROWS
#define MATRIX_SGP_COLS MATRIX_SG_COLS


bool gauss(const uint8_t m[MATRIX_S_DIM], uint8_t b[MATRIX_S_DIM]);

void mult_matrices(const uint8_t* a, const size_t a_rows, const size_t a_cols,
                   const uint8_t* b, const size_t b_cols, uint8_t* c);

void print_matrix(const char* title, const uint8_t* matrix, const size_t rows, const size_t cols);
