#include <sodium/randombytes.h>

#define getbit(word, bit) ((0x80 >> (bit)) & (word))
#define norm(word)        ((word) ? 1 : 0)
#define get(m, i, j)      (getbit((m)[i], (j)))
#define nget(m, i, j)     (norm(getbit((m)[i], (j))))

//static uint8_t P [7] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02 };
static const uint8_t H [3] = { 0x1E, 0x66, 0xAA };
static const uint8_t G [4] = { 0xE0, 0x98, 0x54, 0xD2 };

static void generate_s(const uint8_t s[4]) {
  for (;;) {
    // Fill with randomness (4x4)
    randombytes_buf(s, sizeof(s));

    // Ensure the matrix is invertible, where determinant = 0
    const uint8_t a00 = get(s,0,0), a01 = get(s,0,1), a02 = get(s,0,2), a03 = get(s,0,3);
    const uint8_t a10 = get(s,1,0), a11 = get(s,1,1), a12 = get(s,1,2), a13 = get(s,1,3);
    const uint8_t a20 = get(s,2,0), a21 = get(s,2,1), a22 = get(s,2,2), a23 = get(s,2,3);
    const uint8_t a30 = get(s,3,0), a31 = get(s,3,1), a32 = get(s,3,2), a33 = get(s,3,3);

    const uint8_t s00 = (a11 & (a22 & a33 ^ a23 & a32))
      ^ (a12 & (a21 & a33 ^ a23 & a31))
      ^ (a13 & (a21 & a32 ^ a22 & a31));

    const uint8_t s01 = (a10 & (a22 & a33 ^ a23 & a32))
      ^ (a12 & (a20 & a33 ^ a23 & a30))
      ^ (a13 & (a20 & a32 ^ a22 & a30));

    const uint8_t s02 = (a10 & (a21 & a33 ^ a23 & a31))
      ^ (a11 & (a20 & a33 ^ a23 & a30))
      ^ (a13 & (a20 & a31 ^ a21 & a30));

    const uint8_t s03 = (a10 & (a21 & a32 ^ a22 & a31))
      ^ (a11 & (a20 & a32 ^ a22 & a30))
      ^ (a12 & (a20 & a31 ^ a21 & a30));

    if ((a00 & s00) ^ (a01 & s01) ^ (a02 & s02) ^ (a03 & s03))
      break;
  }
}
