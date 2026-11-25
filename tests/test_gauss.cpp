#include "test_helpers.hpp"

TEST_CASE("gauss: the inverse of the identity is the identity", "[gauss]") {
  const auto inv = gauss(identity<4>());
  REQUIRE(inv.has_value());
  CHECK_MATRIX_EQ(*inv, identity<4>());
}

TEST_CASE("gauss: known triangular matrix is invertible", "[gauss]") {
  const bit_matrix<4, 4> m{{0b0001, 0b0011, 0b0111, 0b1111}};
  const auto inv = gauss(m);

  REQUIRE(inv.has_value());
  CHECK_MATRIX_EQ(m * *inv, identity<4>());
  CHECK_MATRIX_EQ(*inv * m, identity<4>());
}

TEST_CASE("gauss: singular matrices return nullopt", "[gauss]") {
  SECTION("zero matrix") { CHECK_FALSE(gauss(bit_matrix<4, 4>{}).has_value()); }
  SECTION("two equal rows") {
    const bit_matrix<4, 4> m{{0b0011, 0b0011, 0b1100, 0b1001}};
    CHECK_FALSE(gauss(m).has_value());
  }
  SECTION("the sum of all rows is zero") {
    // 0011 ^ 0110 ^ 1100 ^ 1001 == 0000
    const bit_matrix<4, 4> m{{0b0011, 0b0110, 0b1100, 0b1001}};
    CHECK_FALSE(gauss(m).has_value());
  }
  SECTION("a whole column is zero") {
    const bit_matrix<4, 4> m{{0b0001, 0b0010, 0b1000, 0b1011}};
    CHECK_FALSE(gauss(m).has_value());
  }
}

TEST_CASE("gauss: 1x1 cases", "[gauss]") {
  CHECK(gauss(bit_matrix<1, 1>{{0b1}}).has_value());
  CHECK_FALSE(gauss(bit_matrix<1, 1>{{0b0}}).has_value());
}

TEST_CASE("gauss: exhaustive over all 65536 4x4 matrices", "[gauss]") {
  // There are exactly 20160 invertible 4x4 matrices over GF(2).
  std::size_t invertible = 0;
  std::size_t wrong_inverse = 0;
  const auto id = identity<4>();

  for (std::uint32_t v = 0; v < (1u << 16); ++v) {
    const auto m = matrix4_from_bits(v);
    const auto inv = gauss(m);
    if (!inv)
      continue;

    ++invertible;
    if (!equal(m * *inv, id) || !equal(*inv * m, id))
      ++wrong_inverse;
  }

  CHECK(invertible == 20160);
  CHECK(wrong_inverse == 0);
}

TEST_CASE("gauss: the inverse of the inverse is the original", "[gauss]") {
  splitmix64 rng{7};
  int checked = 0;

  while (checked < 100) {
    const auto m = random_matrix<4, 4>(rng);
    const auto inv = gauss(m);
    if (!inv)
      continue;

    const auto back = gauss(*inv);
    REQUIRE(back.has_value());
    CHECK_MATRIX_EQ(*back, m);
    ++checked;
  }
}

TEST_CASE("gauss: works with other sizes (7x7 rotation)", "[gauss]") {
  // Permutation matrix: its inverse is its transpose.
  bit_matrix<7, 7> rot{};
  for (std::size_t i = 0; i < 7; ++i)
    rot.data[i].set((i + 1) % 7);

  const auto inv = gauss(rot);
  REQUIRE(inv.has_value());
  CHECK_MATRIX_EQ(*inv, rot.transpose());
  CHECK_MATRIX_EQ(rot * *inv, identity<7>());
}
