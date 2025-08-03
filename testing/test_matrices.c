#include <check.h>
#include <stdint.h>
#include <string.h>
#include "matrix.h"


START_TEST(test_gauss_invertible) {
    uint8_t dummy [MATRIX_S_DIM];

    // Identity matrix no.1 (4x4)
    uint8_t m1[MATRIX_S_DIM] = {
        0b10000000,
        0b01000000,
        0b00100000,
        0b00010000,
    };

    // Identity matrix no.2 (4x4)
    uint8_t m2[MATRIX_S_DIM] = {
        0b00010000,
        0b00100000,
        0b01000000,
        0b10000000,
    };

    // Identity matrices are always
    // invertible due its range
    ck_assert(gauss(m1, dummy));
    ck_assert(gauss(m2, dummy));
} END_TEST


START_TEST(test_gauss_non_invertible) {
    uint8_t dummy [MATRIX_S_DIM];

    // row 3 = row 4
    uint8_t m1[MATRIX_S_DIM] = {
        0b10000000,
        0b01000000,
        0b00010000,
        0b00010000,
    };

    // row 2 = all cero
    uint8_t m2[MATRIX_S_DIM] = {
        0b00010000,
        0b00000000,
        0b01000000,
        0b10000000,
    };
  
    // row 3 = XOR(row 1, row 2) 
    uint8_t m3[MATRIX_S_DIM] = {
        0b10100000,
        0b01010000,
        0b11110000,
        0b00110000,
    };

    // All the rows are the same
    uint8_t m4[4] = {
        0b10100000,
        0b10100000,
        0b10100000,
        0b10100000,
    };

    // All those cases are not
    // invertible due its propierties
    ck_assert(!gauss(m1, dummy));
    ck_assert(!gauss(m2, dummy));
    ck_assert(!gauss(m3, dummy));
    ck_assert(!gauss(m4, dummy));
} END_TEST


START_TEST(test_gauss_invert_identity) {  
    // The inverse of a identity matrix its
    // the same identity matrix.
    //
    // The gauss function stores the rows of
    // the inverse 4x4 matrix at the lowest
    // 4 bits of each position of the result.
    uint8_t out[MATRIX_S_DIM];
    
    // Identity matrix no.1 (4x4)
    uint8_t m1[MATRIX_S_DIM] = {
        0b00010000,
        0b00100000,
        0b01000000,
        0b10000000,
    };
    
    uint8_t expected1[MATRIX_S_DIM] = {
        0b00010000,
        0b00100000,
        0b01000000,
        0b10000000,
    };

    gauss(m1, out);
    ck_assert(!memcmp(expected1, out, MATRIX_S_DIM));
   
    // Identity matrix no.2 (4x4)
    uint8_t m2[MATRIX_S_DIM] = {
        0b10000000,
        0b01000000,
        0b00100000,
        0b00010000,
    };

    uint8_t expected2[MATRIX_S_DIM] = {
        0b10000000,
        0b01000000,
        0b00100000,
        0b00010000,
    };

    gauss(m2, out);
    ck_assert(!memcmp(expected2, out, MATRIX_S_DIM));
} END_TEST


START_TEST(test_gauss_invert_normal) {
    // In this case we try to calculate
    // the inverse of matrices that are
    // invertible and are not identity.

    uint8_t out[MATRIX_S_DIM];
    uint8_t m1[MATRIX_S_DIM] = {
        0b10110000,
        0b11100000,
        0b10100000,
        0b10010000,
    };
  
    uint8_t expected1[MATRIX_S_DIM] = {
        0b10110000,
        0b01100000,
        0b10010000,
        0b10100000,
    };

    gauss(m1, out);    
    ck_assert(!memcmp(expected1, out, MATRIX_S_DIM));
   
    uint8_t m2[MATRIX_S_DIM] = {
        0b10110000,
        0b00010000,
        0b11000000,
        0b10000000,
    };
  
    uint8_t expected2[MATRIX_S_DIM] = {
        0b00010000,
        0b00110000,
        0b11010000,
        0b01000000,
    };

    gauss(m2, out);
    ck_assert(!memcmp(expected2, out, MATRIX_S_DIM));
} END_TEST


START_TEST(test_inverse_mult) {
    // Here we test if multiplication between
    // a matrix and its inverse produce as a
    // result the identity matrix. It must.

    // All the matrices here are the same as
    // the ones used at the test test_gauss_
    // invert_normal
    
    uint8_t out[MATRIX_S_DIM];
    uint8_t m1[MATRIX_S_DIM] = {
        0b10110000,
        0b11100000,
        0b10100000,
        0b10010000,
    };
  
    uint8_t m2[MATRIX_S_DIM] = {
        0b10110000,
        0b01100000,
        0b10010000,
        0b10100000,
    };
    
    uint8_t expected1[MATRIX_S_DIM] = {
        0b10000000,
        0b01000000,
        0b00100000,
        0b00010000,
    };

    mult_matrices(m1, MATRIX_S_DIM, MATRIX_S_DIM, m2, MATRIX_S_DIM, out);    
    ck_assert(!memcmp(out, expected1, MATRIX_S_DIM));

    uint8_t m3[MATRIX_S_DIM] = {
        0b10110000,
        0b00010000,
        0b11000000,
        0b10000000,
    };
  
    uint8_t m4[MATRIX_S_DIM] = {
        0b00010000,
        0b00110000,
        0b11010000,
        0b01000000,
    };
    
    uint8_t expected2[MATRIX_S_DIM] = {
        0b10000000,
        0b01000000,
        0b00100000,
        0b00010000,
    };

    mult_matrices(m3, MATRIX_S_DIM, MATRIX_S_DIM, m4, MATRIX_S_DIM, out);
    ck_assert(!memcmp(out, expected2, MATRIX_S_DIM));
} END_TEST


START_TEST(test_identity_mult) {
    // Here is tested the case where we
    // multiply a matrix with the identity
    // matrix. The result must be the same
    // as the first matrix.
    uint8_t out[MATRIX_S_DIM];

    // Normal matrix (4x4)
    uint8_t m1[MATRIX_S_DIM] = {
        0b10110000,
        0b11100000,
        0b10100000,
        0b10010000,
    };

    // Identity matrix (4x4)
    uint8_t m2[MATRIX_S_DIM] = {
        0b10000000,
        0b01000000,
        0b00100000,
        0b00010000,
    };

    mult_matrices(m1, MATRIX_S_DIM, MATRIX_S_DIM, m2, MATRIX_S_DIM, out);
    ck_assert(!memcmp(out, m1, MATRIX_S_DIM));
} END_TEST


START_TEST(test_normal_mult) {
    // In this test we try to recreate a real
    // case scenario of the McEliece crypto-
    // system. First, we mutiply a matrix S(4x4)
    // with the system matrix G(4x7). The result
    // SG (4x7) is multiplied with a permutation
    // matrix P(7x7).
    uint8_t out[MATRIX_SG_ROWS];

    // Random matrix S(4x4)
    uint8_t s[MATRIX_S_DIM] = {
        0b11110000,
        0b10010000,
        0b11100000,
        0b01010000,
    };

    // System matrix G(4x7)  
    uint8_t g[MATRIX_G_ROWS] = {
        0b11100000,
        0b10011000,
        0b01010100,
        0b11010010,
    };

    // Expected result S*G
    uint8_t sg[MATRIX_SG_ROWS] = {
        0b11111110,
        0b00110010,
        0b00101100,
        0b01001010,
    };

    mult_matrices(s, MATRIX_S_DIM, MATRIX_S_DIM, g, MATRIX_G_COLS, out);
    ck_assert(!memcmp(out, sg, MATRIX_SG_ROWS));
   
    // Permutation matrix P(7x7)
    uint8_t p[MATRIX_P_DIM] = {
        0b00000010,
        0b01000000,
        0b00010000,
        0b00100000,
        0b00001000,
        0b00000100,
        0b10000000,
    };

    // Expected S*G*P matrix (4x7)
    uint8_t sgp[MATRIX_SGP_ROWS] = {
        0b11111110,
        0b10110000,
        0b00011100,
        0b11001000,
    };
    
    mult_matrices(sg, MATRIX_SG_ROWS, MATRIX_SG_COLS, p, MATRIX_P_DIM, out);    
    ck_assert(!memcmp(out, sgp, MATRIX_SGP_ROWS));
} END_TEST


START_TEST(test_vector_and_matrix_mult) { 
    uint8_t block = 0b11000000;
    uint8_t sgp[MATRIX_SGP_ROWS] = {
        0b11111110,
        0b10110000,
        0b00011100,
        0b11001000,
    };

    uint8_t expected = 0b01001110;
    uint8_t result;

    mult_matrices(&block, 1, 4, sgp, MATRIX_SGP_COLS, &result);    
    ck_assert(result == expected);
} END_TEST

int test_matrices(void) {
    Suite* suite = suite_create("matrix");
  
    TCase* tc_gauss = tcase_create("gauss");
    tcase_add_test(tc_gauss, test_gauss_invertible);
    tcase_add_test(tc_gauss, test_gauss_non_invertible);
    tcase_add_test(tc_gauss, test_gauss_invert_identity);
    tcase_add_test(tc_gauss, test_gauss_invert_normal);
    suite_add_tcase(suite, tc_gauss);
 
    TCase* tc_mult = tcase_create("matrix_mult");
    tcase_add_test(tc_mult, test_inverse_mult);    
    tcase_add_test(tc_mult, test_identity_mult);
    tcase_add_test(tc_mult, test_normal_mult);
    tcase_add_test(tc_mult, test_vector_and_matrix_mult);
    suite_add_tcase(suite, tc_mult);

    SRunner* sr = srunner_create(suite);
    srunner_run_all(sr, CK_NORMAL);
    int failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return failed;
}
