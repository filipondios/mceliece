#pragma once
#include <stdint.h>

void cpyMatrix(uint8_t m, uint8_t n, const uint8_t A[m][n], uint8_t B[m][n]);

void multMat(uint8_t m, uint8_t n, const uint8_t A[m][n], uint8_t p,
  const uint8_t B[n][p], uint8_t C[m][p]);

void multVector(uint8_t n, const uint8_t A[n], uint8_t m,
  const uint8_t B[n][m], uint8_t C[m]);

void inverse(int N, uint8_t matriz[N][N]);

void printMatrix(uint8_t n, uint8_t m, uint8_t mat[n][m], char* message);

void printVector(uint8_t n, uint8_t mat[n], char* message);