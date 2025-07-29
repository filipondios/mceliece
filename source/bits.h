#pragma once

#define BLOCK_BITS  0x8
#define BLOCK_BYTES 0x1
#define MSB  0x80
#define MSB4 0x08
#define LSB4 0x01
#define MSB4_BLOCK 0xf0
#define LSB4_BLOCK 0x0f
#define LSB4_SHIFT 0x04

#define ENCODED_LEN 0x7
#define DECODED_LEN 0x4

#define GET(m, i, j)   ((m[i]) &   (MSB >> j))
#define SET(m, i, j)   ((m[i]) |=  (MSB >> j))
#define UNSET(m, i, j) ((m[i]) &= ~(MSB >> j))
