#include <check.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "bits.h"
#include "matrix.h"
#include "mceliece.h"

START_TEST(test_s_generation) {
    uint8_t inverse[MATRIX_S_DIM];
    uint8_t s[MATRIX_S_DIM];

    // this is already checked lol
    generate_s(s);
    ck_assert(gauss(s, inverse));
} END_TEST

START_TEST(test_p_generation) {   
    for (int count = 0; count < 500; ++count) {
        // P*P must produce the identity matrix
        uint8_t result[MATRIX_P_DIM];
        uint8_t p[MATRIX_P_DIM];

        generate_p(p);
        mult_matrices(p, MATRIX_P_DIM, MATRIX_P_DIM, p, MATRIX_P_DIM, result);

        for (int i = 0; i < MATRIX_P_DIM; ++i) {
            ck_assert(result[i] == (MSB >> i));
        }
    } 
} END_TEST

START_TEST(test_encrypt_decrypt) {   
    PrivateKey private_key;
    PublicKey public_key;

    const uint8_t plaintext[] = {'h','e','l','l','o'}; 
    const size_t plaintext_len = strlen((char*) plaintext);
    uint8_t *cipher = NULL, *recovered = NULL;
    size_t cipher_len, recovered_len;

    // generate keypair (1)
    keygen(&public_key, &private_key);

    // generate cipher text and recover plain text. check allocations (2)
    ck_assert(encrypt(&public_key, plaintext, plaintext_len, &cipher, &cipher_len));
    ck_assert(decrypt(&private_key, cipher, cipher_len, &recovered, &recovered_len));
    ck_assert(cipher != NULL);
    ck_assert(recovered != NULL);

    // check cipher and recovered plaintext length (3)
    ck_assert(cipher_len == (plaintext_len * 2));
    ck_assert(recovered_len == plaintext_len);

    // the recovered plaintext must be the same as the
    // original plaintext, duhh (4)
    ck_assert(!memcmp(plaintext, recovered, plaintext_len));
} END_TEST

int test_mceliece(void) {
    Suite* suite = suite_create("mceliece");
  
    TCase* tc_encrypt = tcase_create("encrypt");
    tcase_add_test(tc_encrypt, test_s_generation);
    tcase_add_test(tc_encrypt, test_p_generation);
    tcase_add_test(tc_encrypt, test_encrypt_decrypt);
    suite_add_tcase(suite, tc_encrypt);
 
    SRunner* sr = srunner_create(suite);
    srunner_run_all(sr, CK_NORMAL);
    int failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return failed;
}
