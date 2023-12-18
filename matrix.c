#include <stdint.h>
#include <string.h>
#include "matrix.h"

void multMat(uint8_t m, uint8_t n, const uint8_t A[m][n], uint8_t p,
  const uint8_t B[n][p], uint8_t C[m][p]) {
  int i, j, k;

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

  for (i = 0; i < m; i++) {
    C[i] = 0;
    for(j = 0; j < n; j++) 
      C[i] ^= A[j] & B[j][i];
  }
}

void getSinverse(uint8_t S[4][4]) {
  uint8_t i, j, k;
  uint8_t factor;
  int8_t pivot;
  uint8_t temp;
  uint8_t gauss[4][8];

  for (i =0; i<4; i++) {
    for (j=0; j<4; j++) {
      gauss[i][j] = S[i][j];
      gauss[i][j+4] = (i==j)? 1 : 0;
    }
  }

  for (i = 0; i<4; i++) {
    pivot = -1;
    for (j = i; j<4; j++) {
      if (gauss[j][i] == 1) {
        pivot = j;
        break;
      }
    }

    if (pivot != -1) {
      // Swap rows
      for (j = 0; j<8; j++) {
        temp = gauss[i][j];
        gauss[i][j] = gauss[pivot][j];
        gauss[pivot][j] = temp;
      }

      // Row operations
      for (j = 0; j<4; j++) {
        if (j != i) {
          factor = gauss[j][i];
          for (k = 0; k<8; k++) {
            gauss[j][k] = gauss[j][k] ^ (factor & gauss[i][k]);
          }
        }
      }
    }
  }

  for(i =0; i<4; i++) {
    for(j=4; j<8; j++) {
      S[i][j-4] = gauss[i][j]; 
    }
  }
}