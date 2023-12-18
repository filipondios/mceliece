#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "matrix.h"

void printMatrix(uint8_t n, uint8_t m, uint8_t mat[n][m], char* message) {
  uint8_t i, j;
  printf("Matriz '%s'\n", message);
  for (i = 0; i < n; i++) {
    for (j = 0; j < m; j++)
      printf("%u ", mat[i][j]);
    printf("\n");
  }printf("\n");
}

void printVector(uint8_t n, uint8_t mat[n], char* message) {
  uint8_t i;
  printf("Vector '%s'\n", message);
  for (i = 0; i < n; i++) {
    printf("%u ", mat[i]);
  }printf("\n\n");
}

void cpyMatrix(uint8_t m, uint8_t n, const uint8_t A[m][n], uint8_t B[m][n]) {
  uint8_t i, j;
  for (i = 0; i < m; i++)
    for (j = 0; j < n; j++)
      B[i][j] = A[i][j];
}

void multMat(uint8_t m, uint8_t n, const uint8_t A[m][n], uint8_t p,
  const uint8_t B[n][p], uint8_t C[m][p]) {
  int i, j, k;
  for (i = 0; i < m; i++)
    for (j = 0; j < p; j++)
      C[i][j] = 0;

  for (i = 0; i < m; i++) {
    for (j = 0; j < p; j++) {
      C[i][j] = 0;
      for (k = 0; k < n; k++) {
        C[i][j] ^= A[i][k] & B[k][j];
      }
    }
  }
}

void multVector(uint8_t n, const uint8_t A[n], uint8_t m,
  const uint8_t B[n][m], uint8_t C[m]){
  uint8_t i, j;
  for(i = 0; i < n; i++) 
    C[i] = 0;

  for (i = 0; i < m; i++) {
    C[i] = 0;
    for(j = 0; j < n; j++) 
      C[i] ^= A[j] & B[j][i];
  }
}

void row_operations(uint8_t N, uint8_t gauss[N][N*2], uint8_t pivot) {
    for (int i = 0; i < N; i++) {
        if (i != pivot) {
            // Realizar operaciones de fila elementales
            int factor = gauss[i][pivot];
            for (int j = 0; j < N*2; j++) {
                gauss[i][j] = (gauss[i][j] + factor * gauss[pivot][j]) % 2;
            }
        }
    }
}

void swap_rows(uint8_t N, uint8_t gauss[N][N*2], uint8_t row1, uint8_t row2) {
    for (int j = 0; j < N*2; j++) {
        uint8_t temp = gauss[row1][j];
        gauss[row1][j] = gauss[row2][j];
        gauss[row2][j] = temp;
    }
}

void inverse(int N, uint8_t matriz[N][N]) {
  
  uint8_t gauss[N][N*2];

  for (uint8_t i =0; i<N; i++) {
    for (uint8_t j=0; j<N; j++) {
      gauss[i][j] = matriz[i][j];
      gauss[i][j+N] = (i==j)? 1 : 0;
    }
  }

  for (int i = 0; i < N; i++) {
    int pivot = -1;
    for (int j = i; j < N; j++) {
      if (gauss[j][i] == 1) {
        pivot = j;
        break;
      }
    }

    if (pivot != -1) {
      swap_rows(N, gauss, i, pivot);
      row_operations(N, gauss, i);
    }
  }

  for (uint8_t i =0; i<N; i++) {
    for (uint8_t j=N; j<N*2; j++) {
      matriz[i][j-N] = gauss[i][j]; 
    }
  }
}

