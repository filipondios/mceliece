#pragma once
#include <stdint.h>

///
/// @brief Multiplica una matriz A de dimension m x n con una B de
/// dimension n x p, guardando el resultado en una matriz C de 
/// dimensiones m x p.
///
void multMat(uint8_t m, uint8_t n, const uint8_t A[m][n], uint8_t p,
  const uint8_t B[n][p], uint8_t C[m][p]);

///
/// @brief Multiplica un vector A de longitud n con una matriz
/// B de dimension n x m, guardando el resultado en un vector C
/// de longitud m. 
///
void multVector(uint8_t n, const uint8_t A[n], uint8_t m,
  const uint8_t B[n][m], uint8_t C[m]);

///
/// @brief Obtiene la inversa de la matriz S del criptosistema
/// de McEliece por medio del metodo de Gauss. 
///
void getSinverse(uint8_t S[4][4]);