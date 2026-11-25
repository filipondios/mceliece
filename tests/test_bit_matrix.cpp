#include "test_helpers.hpp"
#include <type_traits>

TEST_CASE("bit_matrix: known product (non-square)", "[bit_matrix]") {
  // Convention: bit k of a row is column k.
  // A(2x3) * B(3x2) over GF(2):
  //   C[0] = B[0] ^ B[1] = 01 ^ 11 = 10
  //   C[1] = B[0] ^ B[2] = 01 ^ 10 = 11
  const bit_matrix<2, 3> a{{0b011, 0b101}};
  const bit_matrix<3, 2> b{{0b01, 0b11, 0b10}};
  const bit_matrix<2, 2> expected{{0b10, 0b11}};
  CHECK_MATRIX_EQ(a * b, expected);
}

TEST_CASE("bit_matrix: identity is the neutral element", "[bit_matrix]") {
  splitmix64 rng{1};

  for (int n = 0; n < 100; ++n) {
    const auto a = random_matrix<4, 7>(rng);
    CHECK_MATRIX_EQ(identity<4>() * a, a);
    CHECK_MATRIX_EQ(a * identity<7>(), a);
  }
}

TEST_CASE("bit_matrix: multiplying by the zero matrix gives zero",
          "[bit_matrix]") {
  splitmix64 rng{2};
  const auto a = random_matrix<4, 7>(rng);
  CHECK(is_zero(bit_matrix<4, 4>{} * a));
  CHECK(is_zero(a * bit_matrix<7, 3>{}));
}

TEST_CASE("bit_matrix: associativity", "[bit_matrix]") {
  splitmix64 rng{3};

  for (int n = 0; n < 100; ++n) {
    const auto a = random_matrix<4, 4>(rng);
    const auto b = random_matrix<4, 7>(rng);
    const auto c = random_matrix<7, 7>(rng);
    CHECK_MATRIX_EQ((a * b) * c, a * (b * c));
  }
}

TEST_CASE("bit_matrix: distributivity over addition (XOR)", "[bit_matrix]") {
  splitmix64 rng{4};

  for (int n = 0; n < 100; ++n) {
    const auto a = random_matrix<4, 4>(rng);
    const auto b = random_matrix<4, 7>(rng);
    const auto c = random_matrix<4, 7>(rng);

    bit_matrix<4, 7> b_plus_c{};
    for (std::size_t i = 0; i < 4; ++i)
      b_plus_c.data[i] = b.data[i] ^ c.data[i];

    const auto ab = a * b;
    const auto ac = a * c;
    bit_matrix<4, 7> ab_plus_ac{};
    for (std::size_t i = 0; i < 4; ++i)
      ab_plus_ac.data[i] = ab.data[i] ^ ac.data[i];
    CHECK_MATRIX_EQ(a * b_plus_c, ab_plus_ac);
  }
}

TEST_CASE("bit_matrix: transpose swaps dimensions and elements",
          "[bit_matrix][transpose]") {
  const bit_matrix<2, 3> a{{0b011, 0b100}};
  const auto t = a.transpose();

  static_assert(
      std::is_same_v<std::remove_cvref_t<decltype(t)>, bit_matrix<3, 2>>);

  for (std::size_t i = 0; i < 2; ++i)
    for (std::size_t j = 0; j < 3; ++j)
      CHECK(t.data[j][i] == a.data[i][j]);
}

TEST_CASE("bit_matrix: transpose is an involution", "[bit_matrix][transpose]") {
  splitmix64 rng{5};

  for (int n = 0; n < 100; ++n) {
    const auto a = random_matrix<4, 7>(rng);
    CHECK_MATRIX_EQ(a.transpose().transpose(), a);
  }
}

TEST_CASE("bit_matrix: (A*B)^T == B^T * A^T", "[bit_matrix][transpose]") {
  splitmix64 rng{6};

  for (int n = 0; n < 100; ++n) {
    const auto a = random_matrix<3, 5>(rng);
    const auto b = random_matrix<5, 6>(rng);
    CHECK_MATRIX_EQ((a * b).transpose(), b.transpose() * a.transpose());
  }
}

TEST_CASE("bit_matrix: to_string prints one row per line, column 0 on the left",
          "[bit_matrix][to_string]") {
  const bit_matrix<2, 3> m{{0b011, 0b100}};
  CHECK(m.to_string() == "110\n001\n");
}
