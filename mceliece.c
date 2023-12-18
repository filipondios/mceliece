#include <sys/random.h>
#include <string.h>
#include <stdint.h>
#include "matrix.h"
#include "mceliece.h"

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

static uint8_t (*S)[4];
static uint8_t (*P)[7];
static uint8_t SGP[4][7];

uint8_t isSInvertible(const uint8_t mat[4][4]) {
  uint8_t e00 = mat[0][0], e01 = mat[0][1], e02 = mat[0][2], e03 = mat[0][3];
  uint8_t e10 = mat[1][0], e11 = mat[1][1], e12 = mat[1][2], e13 = mat[1][3];
  uint8_t e20 = mat[2][0], e21 = mat[2][1], e22 = mat[2][2], e23 = mat[2][3];
  uint8_t e30 = mat[3][0], e31 = mat[3][1], e32 = mat[3][2], e33 = mat[3][3];
  
  return (e00 & (e11 & (e22&e33 ^ e23&e32) ^ e12&(e21&e33 ^ e23&e31) ^ e13&(e21&e32 ^ e22&e31)) 
    ^ e01 & (e10&(e22&e33 ^ e23&e32) ^ e12&(e20&e33 ^ e23&e30) ^ e13&(e20&e32 ^ e22&e30))
    ^ e02 & (e10&(e21&e33 ^ e23&e31) ^ e11&(e20&e33 ^ e23&e30) ^ e13&(e20&e31 ^ e21&e30))
    ^ e03 & (e10&(e21&e32 ^ e22&e31) ^ e11&(e20&e32 ^ e22&e30) ^ e12&(e20&e31 ^ e21&e30)));
}

void generateS() {
  uint8_t rand;
  do {
    for (uint8_t i = 0; i<4; i++) {
      for (uint8_t j = 0; j<4; j++) {
        getrandom(&rand, 1, 0);
        S[i][j] = rand%2;
    }}} while(!isSInvertible(S));
}

void generatePi() {
  uint8_t i, j;
  for (uint8_t i = 0; i<7; i++)
    for (uint8_t j = 0; j<7; j++)
      P[i][j] = (i==j)? 1 : 0;
}

void shuffleP() {
  uint8_t used_i[7];
  uint8_t tmp_col[7];
  uint8_t fixed;
  uint8_t rn;

  memset(used_i, 0, 7);
  getrandom(&fixed, 1, 0);
  fixed %= 7;

  for (uint8_t i = 0; i < 7; i++) {
    if(i == fixed || used_i[i] == 1) continue;
    do {
      getrandom(&rn, 1, 0);
      rn %= 7;
    } while (rn == fixed || used_i[rn]);
    used_i[rn] = 1;
    used_i[i] = 1;

    for (uint8_t j = 0; j < 7; j++)
      tmp_col[j] = P[j][i];
    for (uint8_t j = 0; j < 7; j++)
      P[j][i] = P[j][rn];
    for (uint8_t j = 0; j < 7; j++)
      P[j][rn] = tmp_col[j];
  }
}

void init_encCtx(uint8_t Sin[4][4], uint8_t Pin[7][7]) {
  // Generar S y P
  S = Sin;
  P = Pin;
  generateS();
  generatePi();
  shuffleP();

  // Obtener SGP 
  uint8_t SG[4][7];
  multMat(4, 4, S, 7, G, SG);
  multMat(4, 7, SG, 7, P, SGP);
}

void init_decCtx(uint8_t Sin[4][4], uint8_t Pin[7][7]) {
  S = Sin;
  P = Pin;
  getSinverse(S);
}

void encryptSemiWord(uint8_t mensaje[4], uint8_t cipher[7]) {
  // Codificar el mensaje
  multVector(4, mensaje, 7, SGP, cipher);

  // Anadir un error
  uint8_t rnd;
  getrandom(&rnd, 1, 0);
  rnd %= 7;
  cipher[rnd] ^= 1;
}

void decryptSemiWord(uint8_t cipher[7], uint8_t mensaje[4]) {
  // Obtener cipher * P^-1
  uint8_t correct[7];
  uint8_t cpy[7];
  multVector(7, cipher, 7, P, correct);

  // Obtener y corregir bit de error
  uint8_t error[3];
  memcpy(cpy, correct, 7);
  multVector(7, correct, 3, Ht, error);
  cpy[(error[0]<<2 | error[1]<<1 | error[2])-1] ^= 1;

  uint8_t x0[4];
  x0[0] = cpy[2];
  x0[1] = cpy[4];
  x0[2] = cpy[5];
  x0[3] = cpy[6];
  
  // obtener el mensaje original 
  // mediante x0 * S^-1
  multVector(4, x0, 4, S, mensaje);
}

void encrypt(uint8_t* pt, size_t pt_len, uint8_t* ct) {
  uint8_t ew[7];
  uint8_t word; 
  uint8_t wb[4];
  size_t index;

  index = 0;
  for (size_t i = 0; i<pt_len; i++) {
    word = pt[i];

    // Extract the first 4 bits
    wb[0] = (word & 0x80)>>7;
    wb[1] = (word & 0x64)>>6;
    wb[2] = (word & 0x20)>>5;
    wb[3] = (word & 0x10)>>4;

    encryptSemiWord(wb, ew);
    for (int i = index, j = 0; j<7; i++, j++)
      ct[i] = ew[j];
    index += 7;

    // Extract the last 4 bits
    wb[0] = (word & 0x8)>>3;
    wb[1] = (word & 0x4)>>2;
    wb[2] = (word & 0x2)>>1;
    wb[3] = (word & 0x1);

    encryptSemiWord(wb, ew);
    for (size_t i = index, j = 0; j<7; i++, j++)
      ct[i] = ew[j];
    index += 7;
  }
} 

void decrypt(uint8_t* ct, size_t ct_len, uint8_t* pt) {
  size_t text_len;
  uint8_t ew[7];
  uint8_t word; 
  uint8_t wb[4];
  size_t index;

  text_len = ct_len/14;
  index = 0;

  for (size_t i = 0; i<text_len; i++) {
    word = 0;

    for (size_t i = index, j = 0; j<7; i++, j++)
      ew[j] = ct[i];
    index += 7;

    // Decrypt the first 7 bits, getting the
    // first 4 bits.
    decryptSemiWord(ew, wb);
    word |= (wb[0])<<7;
    word |= (wb[1])<<6;
    word |= (wb[2])<<5;
    word |= (wb[3])<<4;

    for (size_t i = index, j = 0; j<7; i++, j++)
      ew[j] = ct[i];
    index += 7;

    // Decrypt the first 7 bits, getting the
    // last 4 bits.
    decryptSemiWord(ew, wb);
    word |= (wb[0])<<3;
    word |= (wb[1])<<2;
    word |= (wb[2])<<1;
    word |= (wb[3]);
    pt[i] = word;
  }
}