#pragma once

#define BLOCK_BITS  0x08
#define BLOCK_BYTES 0x01
#define MSB         0x80
#define MSB4        0x08
#define LSB4        0x01
#define MSB4_BLOCK  0xf0
#define LSB4_BLOCK  0x0f
#define LSB4_SHIFT  0x04
#define MSB7_BLOCK  0xfe
#define ENCODED_LEN 0x07
#define DECODED_LEN 0x04
#define ERROR_SHIFT 0x05

#define GET_BIT(b, i)     ((b) &  (MSB >> i))
#define SET_BIT(b, i)     ((b) |= (MSB >> i))
#define GET(m, i, j)      (GET_BIT(m[i], j))
#define SET(m, i, j)      (SET_BIT(m[i], j))
#define GET_MSB4_BLOCK(b) (b & MSB4_BLOCK)
#define GET_LSB4_BLOCK(b) (b & LSB4_BLOCK)
#define GET_MSB7_BLOCK(b) (b & MSB7_BLOCK)
