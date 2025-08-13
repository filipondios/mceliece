#include <check.h>
#include <stdio.h>
#include <string.h>
#include <sodium.h>
#include "matrix.h"
#include "mceliece.h"


START_TEST(test_encrypt_decrypt) {
    PrivateKey* private_key;
    PublicKey* public_key;

    const uint8_t plaintext[] = "filipondios"; 
    const size_t plaintext_len = sizeof(plaintext);
    uint8_t *cipher = NULL, *recovered = NULL;
    size_t cipher_len, recovered_len;

    // generate keypair (1)
    mceliece_keygen(&public_key, &private_key);

    printf("---- test mceliece ----\n---- keygen ----\n");
    print_matrix("s", (void*)private_key, MATRIX_S_DIM, MATRIX_S_DIM);
    print_matrix("p", (void*)private_key+4, MATRIX_P_DIM, MATRIX_P_DIM);
    print_matrix("g", (void*)private_key+8, MATRIX_G_ROWS, MATRIX_G_COLS);
    print_matrix("ht", (void*)private_key+15, MATRIX_H_ROWS, MATRIX_H_COLS);
    print_matrix("sgp", (void*)public_key+19, MATRIX_SGP_ROWS, MATRIX_SGP_COLS);

    // generate cipher text and recover plain text. check allocations (2)
    ck_assert(mceliece_encrypt(public_key, plaintext, plaintext_len, &cipher, &cipher_len));
    ck_assert(mceliece_decrypt(private_key, cipher, cipher_len, &recovered, &recovered_len));
    ck_assert(cipher != NULL);
    ck_assert(recovered != NULL);
    
    printf("\n---- encryption results ----\n");
    print_matrix("text (1)", plaintext, sizeof(plaintext), 0x8);
    print_matrix("cipher (2)", cipher, cipher_len, 0x8);
    print_matrix("text (3)", recovered, recovered_len, 0x8);
    
    // check cipher and recovered plaintext length (3)
    ck_assert(cipher_len == (plaintext_len * 2));
    ck_assert(recovered_len == plaintext_len);

    // the recovered plaintext must be the same as the
    // original plaintext, duhh (4)
    ck_assert(!memcmp(plaintext, recovered, plaintext_len));
    free(cipher);
    free(recovered);
} END_TEST

START_TEST(test_encrypt_decrypt_random) {

    uint8_t *plaintext, *cipher, *recovered;
    size_t cipher_len, recovered_len;
    PublicKey* public_key;
    PrivateKey* private_key;

    const size_t plaintext_len = 2500;
    const size_t iterations = 100;

    for (int i = 0; i < iterations; ++i) {
        plaintext = malloc(plaintext_len);

        randombytes(plaintext, plaintext_len);
        mceliece_keygen(&public_key, &private_key);
        mceliece_encrypt(public_key, plaintext, plaintext_len, &cipher, &cipher_len);
        mceliece_decrypt(private_key, cipher, cipher_len, &recovered, &recovered_len);

        ck_assert(cipher_len == (plaintext_len * 2));
        ck_assert(recovered_len == plaintext_len);
        ck_assert(!memcmp(plaintext, recovered, plaintext_len));

        free(public_key);
        free(private_key);
        free(plaintext);
        free(cipher);
        free(recovered);
    }

} END_TEST

int test_mceliece(void) {
    Suite* suite = suite_create("mceliece");
  
    TCase* tc_encrypt = tcase_create("encrypt");
    tcase_add_test(tc_encrypt, test_encrypt_decrypt);
    tcase_add_test(tc_encrypt, test_encrypt_decrypt_random);
    suite_add_tcase(suite, tc_encrypt);
 
    SRunner* sr = srunner_create(suite);
    srunner_run_all(sr, CK_NORMAL);
    int failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return failed;
}
