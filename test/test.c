#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "../matrix.h"
#include "../mceliece.h"

void printMatrix(uint8_t n, uint8_t m, uint8_t mat[n][m], char* message);
void printVector(uint8_t n, uint8_t mat[n], char* message);

uint8_t S[4][4];
uint8_t P[7][7];

int main(void) {
  // Mensaje original
  uint8_t mensaje[] = "pepe";
  size_t mensaje_len = sizeof(mensaje);

  // Mensaje cifrado
  size_t cipher_len;
  uint8_t* cipher;

  // Mensaje recuperado
  uint8_t* recovered;
  size_t recovered_len;

  // Obtener las longitudes del mensaje encriptado
  // y del texto desencriptado.
  cipher_len = getCipherLen(mensaje_len);
  recovered_len = getPlainTextLen(cipher_len);

  // Resercar memoria para el mensaje encriptado
  // y del texto desencriptado.
  cipher = malloc(sizeof(uint8_t)*cipher_len);
  recovered = malloc(sizeof(uint8_t)*recovered_len);

  // Encriptar
  init_encCtx(S, P);
  encrypt(mensaje, mensaje_len, cipher);
  printVector(mensaje_len, mensaje, "Mensaje original");  
  printVector(cipher_len, cipher, "Mensaje encriptado");  

  // Desencriptar
  init_decCtx(S, P);
  decrypt(cipher, cipher_len, recovered);
  free(cipher);
  
  printVector(recovered_len, recovered, "Mensaje recuperado");  
  free(recovered);
  return 0;
}

///
/// @brief Prints a matrix along with a descriptive message.
///
void printMatrix(uint8_t n, uint8_t m, uint8_t mat[n][m], char* message) {
  uint8_t i, j;
  printf("Matrix '%s'\n", message);
  for (i = 0; i < n; i++) {
    for (j = 0; j < m; j++)
      printf("%u ", mat[i][j]);
    printf("\n");
  }printf("\n");
}

///
/// @brief Prints a vector along with a descriptive message.
///
void printVector(uint8_t n, uint8_t mat[n], char* message) {
  uint8_t i;
  printf("Vector '%s'\n", message);
  for (i = 0; i < n; i++) {
    printf("%u ", mat[i]);
  }printf("\n\n");
}