#include <stdio.h>
#include <string.h>
#include <sys/random.h>
#include <stdint.h>
#include "matrix.h"
#include "hamming.h"

static const uint8_t Ht[7][3] = {
  { 0, 0, 1 },
  { 0, 1, 0 },
  { 0, 1, 1 },
  { 1, 0, 0 },
  { 1, 0, 1 },
  { 1, 1, 0 },
  { 1, 1, 1 },
};

static const uint8_t G[4][7] = {
  { 1, 1, 1, 0, 0, 0, 0 },
  { 1, 0, 0, 1, 1, 0, 0 },
  { 0, 1, 0, 1, 0, 1, 0 },
  { 1, 1, 0, 1, 0, 0, 1 }
};

void add_error(uint8_t c[7]) {
  uint8_t rnd;
  getrandom(&rnd, 1, 0);
  rnd %= 7;
  c[rnd] ^= 1;
}

void encode(uint8_t m[4], uint8_t c[7]) {
  multVector(4, m, 7, G, c);
}

void decode(uint8_t c[7], uint8_t m[4]) {
  uint8_t error[3];
  uint8_t cpy[7];
  uint8_t ierr;

  memcpy(cpy, c, 7);
  multVector(7, c, 3, Ht, error);

  // Si el sindrome es (0,0,0), no hay error
  if(error[0]==0 && error[1]==0 && error[2]==0)
    goto SYS; 

  // Obtener el indice del error
  c[(error[0]<<2 | error[1]<<1 | error[2])-1] ^= 1;
  
  // Obtener un x0 tal que 
  // x0 * G = c (x0 = m)
  SYS:
    m[0] = c[2];
    m[1] = c[4];
    m[2] = c[5];
    m[3] = c[6];
}
