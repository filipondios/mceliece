#include "hamming.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

unsigned char mensaje[] = "test";
int len = sizeof(mensaje);
int cipher_len = sizeof(mensaje)*14;

int main(void) {
  // Mensaje cifrado y recuperado
  unsigned char cipher[cipher_len];
  unsigned char recovered[len];

  unsigned char word;
  unsigned char bits[4];
  unsigned char encoded[7];
  int index = 0;

  // Codificar el mensaje anadiendo 
  // 1 error por cada 4 bits.
  for(int i=0; i<len; i++) {
    if(mensaje[i] == '\0') break;
    word = mensaje[i];

    // Primeros 4 bits
    bits[0] = (word & 0b10000000)>>7;
    bits[1] = (word & 0b01000000)>>6;
    bits[2] = (word & 0b00100000)>>5;
    bits[3] = (word & 0b00010000)>>4;

    encode(bits, encoded);
    add_error(encoded);
    for (int i = index, j = 0; j<7; i++, j++)
      cipher[i] = encoded[j];
    index += 7;

    // Ultimos 4 bits
    bits[0] = (word & 0b00001000)>>3;
    bits[1] = (word & 0b00000100)>>2;
    bits[2] = (word & 0b00000010)>>1;
    bits[3] = (word & 0b00000001);

    encode(bits, encoded);
    add_error(encoded);
    for (int i = index, j = 0; j<7; i++, j++)
      cipher[i] = encoded[j];
    index += 7;
  }

  printf("Encoded message:");
  for (int i = 0; i<cipher_len; i++) {
    printf("%u ", cipher[i]);
  }printf("\n");
  
  // Decodificar el mensaje
  index = 0;
  for(int i=0; i<len; i++) {
    word = 0;

    for (int i = index, j = 0; j<7; i++, j++)
      encoded[j] = cipher[i];
    index += 7;

    decode(encoded, bits);
    word |= (bits[0])<<7;
    word |= (bits[1])<<6;
    word |= (bits[2])<<5;
    word |= (bits[3])<<4;

    for (int i = index, j = 0; j<7; i++, j++)
      encoded[j] = cipher[i];
    index += 7;

    decode(encoded, bits);
    word |= (bits[0])<<3;
    word |= (bits[1])<<2;
    word |= (bits[2])<<1;
    word |= (bits[3]);
    recovered[i] = word;
  }

  printf("Decoded message: ");
  for (int i = 0; i<len; i++) {
    printf("%c", recovered[i]);
  }printf("\n");
  return 0;
}