#pragma once
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PrivateKey PrivateKey;
typedef struct PublicKey  PublicKey;

bool mceliece_keygen(PublicKey** public_key, PrivateKey** secret_key);

bool mceliece_encrypt(const PublicKey* public_key, const uint8_t* in,
             const size_t in_len, uint8_t** out, size_t* out_len);

bool mceliece_decrypt(const PrivateKey* private_key, const uint8_t* cipher,
             const size_t cipher_len, uint8_t** out, size_t* out_len);

void mceliece_public_key_free(PublicKey* public_key);
void mceliece_private_key_free(PrivateKey* private_key);

#ifdef __cplusplus
}
#endif
