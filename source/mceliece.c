#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sodium/randombytes.h>
#include "mceliece.h"
#include "matrix.h"
#include "bits.h"


void generate_s(uint8_t s[MATRIX_S_DIM]) {
    for (;;) {
        // S = Random 4x4 invertible matrix
        randombytes_buf(s, MATRIX_S_DIM);
        uint8_t dummy[MATRIX_S_DIM];

        if (gauss(s, dummy))
            break;
    }

    // Clean up unused bits in the S matrix
    // Rows are stored at the 4 MSBs of s[i]
    for (int i = 0; i < MATRIX_S_DIM; ++i) {
        s[i] &= MSB4_BLOCK;
    }
}

void generate_p(uint8_t p[MATRIX_P_DIM]) {
    uint8_t indices[MATRIX_P_DIM];
    const uint8_t fixed = (randombytes_random() % MATRIX_P_DIM);
    memset(p, 0x0, MATRIX_P_DIM);

    for (uint8_t i = 0x0; i < MATRIX_P_DIM; ++i) {
        // indices[row] = active column bit. Here indices
        // is being initialized as the identity matrix.
        indices[i] = i;
    }

    // Fisher-Yates shuffle, except for one position
    for (uint8_t i = MATRIX_P_DIM - 0x1; i > 0x0; --i) {
        if (i == fixed) { continue; }
        uint8_t j;

        do {
            randombytes_buf((void*) &j, sizeof(uint8_t));
            j %= (i + 1);
        } while (j == fixed);

        const uint8_t tmp = indices[i];
        indices[i] = indices[j];
        indices[j] = tmp;
    }

    // Build the shuffled identity matrix
    for (uint8_t i = 0; i < MATRIX_P_DIM; ++i) {
        SET(p, i, indices[i]);
    }
}

void keygen(PublicKey* publickey, PrivateKey* secretkey) {
    // Transpose of the parity matrix of the (3,2) Hamming code
    // (H) and the generator matrix of the (7,4,3)-code (G)
    const uint8_t h[MATRIX_H_ROWS] = { 0x20, 0x40, 0x60, 0x80, 0xa0, 0xc0, 0xe0 };
    const uint8_t g[MATRIX_G_ROWS] = { 0xe0, 0x98, 0x54, 0xd2 };

    // H is not 'part' of the private key (S,G,P) but is an
    // essential part for the decryption process.
    generate_s(secretkey->s);
    generate_p(secretkey->p);
    memcpy(secretkey->g, g, MATRIX_G_ROWS);
    memcpy(secretkey->h, h, MATRIX_H_ROWS);

    // Generate the public key S*G*P
    uint8_t sg[MATRIX_SG_ROWS];
    mult_matrices(secretkey->s, MATRIX_S_DIM, MATRIX_S_DIM, secretkey->g, MATRIX_G_COLS, sg);
    mult_matrices(sg, MATRIX_SG_ROWS, MATRIX_SG_COLS, secretkey->p, MATRIX_P_DIM, publickey->sgp);
}

uint8_t encode(const uint8_t block, const uint8_t sgp[MATRIX_SGP_ROWS]) {
    // Given 4 bits of information stored in the 4 MSBs of a 8-bit word,
    // encoding the message means m*G + e, being 'e' a error vector, 'm'
    // the message and G = S*G*P. The encoded message has 7 bits.
    
    uint8_t m_sgp;
    mult_matrices(&block, 0x1, DECODED_LEN, sgp, MATRIX_SGP_COLS, &m_sgp);
    const uint8_t rand = randombytes_random() % ENCODED_LEN;
    return m_sgp ^ (MSB >> rand);
}

uint8_t decode(const uint8_t block, const uint8_t s_inv[MATRIX_S_DIM],
               const uint8_t p[MATRIX_P_DIM], const uint8_t h[MATRIX_H_ROWS],
               const uint8_t g[MATRIX_G_ROWS]) {
    return 0;
}

int encrypt(const PublicKey* public_key, const uint8_t* in,
            const size_t in_len, uint8_t** out, size_t* out_len) {
    // Encrypt is encoding the 4 MSBs and 4 LSBs of each 8-bit block
    // in a buffer. This produces another buffer, each containing a
    // 7-bit block of encoded information. 

    uint8_t* buffer;

    if (!(buffer = malloc(in_len << 0x1))) {
        return EXIT_FAILURE;
    }

    for (int i = 0; i < in_len; ++i) {
        const uint8_t block = in[i];
        uint8_t msbs = MSB4_BLOCK & block;
        uint8_t lsbs = (LSB4_BLOCK & block) << LSB4_SHIFT;

        msbs = encode(msbs, public_key->sgp);
        lsbs = encode(lsbs, public_key->sgp);
        
        const uint8_t index = i << 0x2;
        buffer[index] = msbs;
        buffer[index + 0x1] = lsbs; 
    }

    *out_len = in_len << 0x1;
    *out = buffer;
    return EXIT_SUCCESS;
}

int decrypt(const PrivateKey* private_key, const uint8_t* cipher,
            const size_t cipher_len, uint8_t** out, size_t* out_len) {
    return 0;
}
