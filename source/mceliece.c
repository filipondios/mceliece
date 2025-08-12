#include <stdint.h>
#include <stdio.h>
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
    memset(p, 0x0, MATRIX_P_DIM);

    // select one fixed column randomly
    uint8_t fixed = (uint8_t) randombytes_random();
    fixed %= MATRIX_P_DIM;

    // initialize indices array
    for (uint8_t i = 0x0, k = 0x0; i < MATRIX_P_DIM; ++i) {
        if (i != fixed) {
            indices[k++] = i;
        }
    }

    // shuffle indices array
    for (uint8_t i = MATRIX_P_DIM - 2; i > 0; --i) {
        uint8_t j = (uint8_t) randombytes_random();
        j %= (i + 1);

        const uint8_t tmp = indices[i];
        indices[i] = indices[j];
        indices[j] = tmp;
    }

    // Build involutive mapping
    uint8_t map[MATRIX_P_DIM];
    map[fixed] = fixed;

    for (uint8_t i = 0; i < MATRIX_P_DIM - 1; i += 2) {
        uint8_t a = indices[i];
        uint8_t b = indices[i + 1];
        map[a] = b;
        map[b] = a;
    }

    for (uint8_t i = 0; i < MATRIX_P_DIM; ++i) {
        SET(p, i, map[i]);
    }
}

void keygen(PublicKey* publickey, PrivateKey* secretkey) {
    // Transpose of the parity matrix of the (3,2) Hamming code
    // (H) and the generator matrix of the (7,4,3)-code (G)

    const uint8_t h[MATRIX_H_ROWS] = { 0x20, 0x40, 0x60, 0x80, 0xA0, 0xC0, 0xE0 };
    const uint8_t g[MATRIX_G_ROWS] = { 0xE0, 0x98, 0x54, 0xD2 };

    // H is not 'part' of the private key (S,G,P) but is an
    // essential part for the decryption process.
    generate_s(secretkey->s);
    generate_p(secretkey->p);
    memcpy(secretkey->g, g, MATRIX_G_ROWS);
    memcpy(secretkey->ht, h, MATRIX_H_ROWS);

    // Calculate the inverse of S
    gauss(secretkey->s, secretkey->si);

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
               const uint8_t p[MATRIX_P_DIM], const uint8_t h[MATRIX_H_ROWS]) {
    // Given 7 bits of information stored in the MSBs of a 8-bit word,
    // decoding the message means obtaining m1 = block*P, then  m1 * H tells
    // where the error 'e' is introduced and its corrected, producing m2.
    // Then, its obtained a 'v' such v*G = m2. Finally, the decoded word
    // is the result of x*S_inv.  

    uint8_t m1, e, syndrome, m2, x, decoded;
    mult_matrices(&block, 0x1, ENCODED_LEN, p, MATRIX_P_DIM, &m1);   
    mult_matrices(&m1, 0x1, ENCODED_LEN, h, MATRIX_H_COLS, &e);

    syndrome = e >> ERROR_SHIFT;
    m2 = (syndrome)? (m1 ^ (MSB >> (syndrome - 0x1))) : m1;

    // Given an encoded 7-bit vector m2 = {R0, R1, R2, R3, R4, R5, R6}
    // by multiplying a 4-bit input v{x, y, z, t} with the 4x7 generator
    // matrix 'G' defined at the 'keygen' function:
    //
    // The original message bits can be directly recovered from m2:
    // (x = R2, y = R4, z = R5, t = R6)
    // 
    // This works because the generator matrix is in systematic form:
    // the last 4 positions in R store the original input bits directly.

    x = 0x0; // TODO Upgrade
    x |= (GET_BIT(m2, 0x2) ? 0x80 : 0x00);
    x |= (GET_BIT(m2, 0x4) ? 0x40 : 0x00);  
    x |= (GET_BIT(m2, 0x5) ? 0x20 : 0x00);
    x |= (GET_BIT(m2, 0x6) ? 0x10 : 0x00);
    
    mult_matrices(&x, 0x1, DECODED_LEN, s_inv, MATRIX_S_DIM, &decoded);
    return decoded;
}

bool encrypt(const PublicKey* public_key, const uint8_t* in,
            const size_t in_len, uint8_t** out, size_t* out_len) {
    // Encrypt is encoding the 4 MSBs and 4 LSBs of each 8-bit block
    // in a buffer. This produces another buffer, each containing a
    // 7-bit block of encoded information. 

    const size_t len = in_len * 2;
    uint8_t* buffer;

    if (!(buffer = malloc(len))) {
        return false;
    }

    for (size_t i = 0x0; i < in_len; ++i) {
        // Divide a block into MSBs and LSBs
        const uint8_t block = in[i];
        uint8_t msbs = GET_MSB4_BLOCK(block);
        uint8_t lsbs = GET_LSB4_BLOCK(block);
        lsbs <<= LSB4_SHIFT;

        // Encode both parts of the block
        msbs = encode(msbs, public_key->sgp);
        lsbs = encode(lsbs, public_key->sgp);

        // Store the encoded blocks
        const size_t index = i * 2;
        buffer[index] = msbs;
        buffer[index + 0x1] = lsbs; 
    }

    *out_len = len;
    *out = buffer;
    return true;
}

bool decrypt(const PrivateKey* private_key, const uint8_t* in,
            const size_t in_len, uint8_t** out, size_t* out_len) {
    // Decrypt is decoding the 7 MSBs of two consecutive 8-bit blocks
    // in a buffer. This produces two 4-bit blocks (the 4 MSBs and 4
    // LSBs of a initial 8-bit block of plaintext information. 

    const size_t len = in_len / 2;
    uint8_t* buffer;

    if ((in_len % 2) || !(buffer = malloc(len))) {
        return false;
    }

    for (size_t i = 0x0; i < in_len; i += 0x2) {
        uint8_t msbs = GET_MSB7_BLOCK(in[i]);
        uint8_t lsbs = GET_MSB7_BLOCK(in[i + 0x1]);

        // Decode both parts of the ciphertext block
        msbs = decode(msbs, private_key->si, private_key->p, private_key->ht);
        lsbs = decode(lsbs, private_key->si, private_key->p, private_key->ht);     

        uint8_t msb_part = GET_MSB4_BLOCK(msbs);
        uint8_t lsb_part = GET_MSB4_BLOCK(lsbs);
        lsb_part >>= LSB4_SHIFT;
         
        const size_t index = i / 2;
        buffer[index] = msb_part | lsb_part;        
    }

    *out_len = len;
    *out = buffer;
    return true;
}
