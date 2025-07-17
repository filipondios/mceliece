#include <sodium/randombytes.h>
#include <check.h>
#include <stdint.h>
#include <stdio.h>
#include "matrix.h"

START_TEST(test_gauss_invertible) {
  uint8_t dummy [MATRIX_S_DIM];

  // Identity matrix no.1
  uint8_t m1[MATRIX_S_DIM] = {
    0b1000,
    0b0100,
    0b0010,
    0b0001,
  };

  // Identity matrix no.2
  uint8_t m2[MATRIX_S_DIM] = {
    0b0001,
    0b0010,
    0b0100,
    0b1000,
  };

  ck_assert(gauss(m1, dummy));
  ck_assert(gauss(m2, dummy));
} END_TEST


START_TEST(test_gauss_non_invertible) {
  uint8_t dummy [MATRIX_S_DIM];

  // row 3 = row 4
  uint8_t m1[MATRIX_S_DIM] = {
    0b1000,
    0b0100,
    0b0001,
    0b0001,
  };

  // row 2 = all cero
  uint8_t m2[MATRIX_S_DIM] = {
    0b0001,
    0b0000,
    0b0100,
    0b1000,
  };
  
  // row 3 = XOR(row 1, row 2) 
  uint8_t m3[MATRIX_S_DIM] = {
    0b1010,
    0b0101,
    0b1111,
    0b0011
  };

  // All the rows are the same
  uint8_t m4[4] = {
    0b1010,
    0b1010,
    0b1010,
    0b1010
  };

  ck_assert(!gauss(m1, dummy));
  ck_assert(!gauss(m2, dummy));
  ck_assert(!gauss(m3, dummy));
  ck_assert(!gauss(m4, dummy));
} END_TEST


START_TEST(test_gauss_invert_identity) {  
  uint8_t result[MATRIX_S_DIM];
  uint8_t m1[MATRIX_S_DIM] = {
    0b0001,
    0b0010,
    0b0100,
    0b1000,
  };

  gauss(m1, result);

  for (int i = 0; i < MATRIX_S_DIM; ++i) {
    const uint8_t row = result[i] & 0xf0;
    ck_assert(m1[i] == (row >> 0x4));
  }
   
  uint8_t m2[MATRIX_S_DIM] = {
    0b1000,
    0b0100,
    0b0010,
    0b0001,
  };

  gauss(m2, result);

  for (int i = 0; i < MATRIX_S_DIM; ++i) {
    const uint8_t row = result[i] & 0xf0;
    ck_assert(m2[i] == (row >> 0x4));
  } 
} END_TEST


START_TEST(test_gauss_invert_normal) {
  uint8_t out[MATRIX_S_DIM];
  uint8_t m1[MATRIX_S_DIM] = {
    0b1011,
    0b1110,
    0b1010,
    0b1001,
  };
  
  uint8_t expected1[MATRIX_S_DIM] = {
    0b0101,
    0b1001,
    0b0110,
    0b1101
  };

  gauss(m1, out);

  for (int i = 0; i < MATRIX_S_DIM; ++i) {
    const uint8_t row = out[i] & 0xf0;
    ck_assert(expected1[i] = (row >> 0x4));
  }
   
  uint8_t m2[MATRIX_S_DIM] = {
    0b1011,
    0b0001,
    0b1100,
    0b1000,
  };
  
  uint8_t expected2[MATRIX_S_DIM] = {
    0b0010,
    0b1011,
    0b1100,
    0b1000,
  };

  gauss(m2, out);

  for (int i = 0; i < MATRIX_S_DIM; ++i) {
    const uint8_t row = out[i] & 0xf0;
    ck_assert(expected2[i] = (row >> 0x4));
  }
} END_TEST


START_TEST(test_inverse_mult) {
  uint8_t out[MATRIX_S_DIM];
  uint8_t m1[MATRIX_S_DIM] = {
    0b1011,
    0b1110,
    0b1010,
    0b1001,
  };
  
  uint8_t m2[MATRIX_S_DIM] = {
    0b0101,
    0b1001,
    0b0110,
    0b1101
  };

  mult_matrices(m1, MATRIX_S_DIM, MATRIX_S_DIM, m2, MATRIX_S_DIM, out);
  for (int i = 0; i < MATRIX_S_DIM; ++i)
    ck_assert((out[i] & 0xf) == (0x1 << i));

  uint8_t m3[MATRIX_S_DIM] = {
    0b1011,
    0b0001,
    0b1100,
    0b1000,
  };
  
  uint8_t m4[MATRIX_S_DIM] = {
    0b0010,
    0b1011,
    0b1100,
    0b1000,
  };
  
  mult_matrices(m3, MATRIX_S_DIM, MATRIX_S_DIM, m4, MATRIX_S_DIM, out);
  for (int i = 0; i < MATRIX_S_DIM; ++i)
    ck_assert((out[i] & 0xf) == (0x1 << i));

} END_TEST



int main(void) {
  Suite* suite = suite_create("matrix");
  
  TCase* tc_gauss = tcase_create("gauss");
  tcase_add_test(tc_gauss, test_gauss_invertible);
  tcase_add_test(tc_gauss, test_gauss_non_invertible);
  tcase_add_test(tc_gauss, test_gauss_invert_identity);
  tcase_add_test(tc_gauss, test_gauss_invert_normal);
  suite_add_tcase(suite, tc_gauss);
 
  TCase* tc_mult = tcase_create("matrix_mult");
  tcase_add_test(tc_mult, test_inverse_mult);
  suite_add_tcase(suite, tc_gauss);
  suite_add_tcase(suite, tc_mult);

  SRunner* sr = srunner_create(suite);
  srunner_run_all(sr, CK_NORMAL);
  int failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  
  return failed;
}
