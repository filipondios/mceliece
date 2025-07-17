#pragma once

// Bit operations over a matrix position (i, j)
#define GET(m, i, j)   (((m[i]) >> (j)) & 0x1) 
#define SET(m, i, j)   (m[i] |= (0x1 << (j)))
#define UNSET(m, i, j) (m[i] &= ~(0x1 << (j)))
