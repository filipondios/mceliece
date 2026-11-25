#include "mceliece.hpp"
#include "test_helpers.hpp"
#include <bit>
#include <catch2/generators/catch_generators_all.hpp>
#include <set>
#include <string>
#include <vector>

static_assert(s_dim == 4);
static_assert(p_dim == 7);
static_assert(g_rows == 4 && g_cols == 7);
static_assert(h_rows == 7 && h_cols == 3);

TEST_CASE("hamming: G * H^T == 0", "[hamming]") {
  CHECK(is_zero(hamming_g * hamming_ht));
}

TEST_CASE("hamming: every codeword has zero syndrome", "[hamming]") {
  const auto nibble = static_cast<unsigned>(GENERATE(range(0, 16)));
  const auto codeword = row_vector<4>(nibble) * hamming_g;
  CHECK(is_zero(codeword * hamming_ht));
}

TEST_CASE("hamming: each single error gives a distinct, non-zero syndrome",
          "[hamming]") {
  std::set<unsigned long> syndromes;

  for (unsigned k = 0; k < 7; ++k) {
    const auto syndrome = row_vector<7>(1ull << k) * hamming_ht;
    CHECK(syndrome.data[0].any());
    syndromes.insert(syndrome.data[0].to_ulong());
  }
  CHECK(syndromes.size() == 7);
}

TEST_CASE("hamming: the code's minimum distance is 3", "[hamming]") {
  std::vector<unsigned long> codewords;
  for (unsigned m = 0; m < 16; ++m)
    codewords.push_back((row_vector<4>(m) * hamming_g).data[0].to_ulong());

  int min_distance = 7;
  for (std::size_t a = 0; a < codewords.size(); ++a)
    for (std::size_t b = a + 1; b < codewords.size(); ++b)
      min_distance =
          std::min(min_distance, std::popcount(codewords[a] ^ codewords[b]));
  CHECK(min_distance == 3);
}

TEST_CASE("generate_s: is invertible and si is its inverse", "[keygen]") {
  init_sodium();

  for (int n = 0; n < 200; ++n) {
    const auto sp = generate_s();
    CHECK(equal(sp.s * sp.si, identity<s_dim>()));
    CHECK(equal(sp.si * sp.s, identity<s_dim>()));
  }
}

TEST_CASE("generate_s: does not always return the same matrix", "[keygen]") {
  init_sodium();

  std::set<std::string> seen;
  for (int n = 0; n < 100; ++n)
    seen.insert(generate_s().s.to_string());

  CHECK(seen.size() > 10);
}

TEST_CASE("generate_p: is a permutation matrix", "[keygen]") {
  init_sodium();

  for (int n = 0; n < 200; ++n) {
    const auto p = generate_p();

    // exactly one 1 per row
    for (const auto &row : p.data)
      CHECK(row.count() == 1);

    // exactly one 1 per column
    for (const auto &col : p.transpose().data)
      CHECK(col.count() == 1);

    // inverse == transpose
    CHECK(equal(p * p.transpose(), identity<p_dim>()));
  }
}

TEST_CASE("generate_p: covers general permutations (not only involutions)",
          "[keygen]") {
  init_sodium();
  std::set<std::string> seen;
  bool found_non_involution = false;

  for (int n = 0; n < 500; ++n) {
    const auto p = generate_p();
    seen.insert(p.to_string());
    if (!equal(p, p.transpose()))
      found_non_involution = true;
  }

  // 7! = 5040 possible permutations: 500 samples
  // will include many distinct ones.
  CHECK(seen.size() > 200);
  CHECK(found_non_involution);
}

TEST_CASE("keygen: the public key is S*G*P", "[keygen]") {
  for (int n = 0; n < 50; ++n) {
    const auto kp = keygen();
    CHECK_MATRIX_EQ(kp.pub, kp.sec.s * hamming_g * kp.sec.p);
  }
}

TEST_CASE("keygen: the private key is consistent", "[keygen]") {
  for (int n = 0; n < 50; ++n) {
    const auto kp = keygen();
    CHECK(equal(kp.sec.s * kp.sec.si, identity<s_dim>()));
    CHECK(equal(kp.sec.p * kp.sec.pi, identity<p_dim>()));
    CHECK(equal(kp.sec.pi, kp.sec.p.transpose()));
  }
}

TEST_CASE("keygen: S*G*P is still a code with the same redundancy",
          "[keygen]") {
  // pub * P^-1 * H^T = S*G*H^T = 0
  for (int n = 0; n < 50; ++n) {
    const auto kp = keygen();
    CHECK(is_zero(kp.pub * kp.sec.pi * hamming_ht));
  }
}

TEST_CASE("keygen: generates distinct keys", "[keygen]") {
  std::set<std::string> seen;
  for (int n = 0; n < 20; ++n) {
    const auto kp = keygen();
    seen.insert(kp.sec.s.to_string() + kp.sec.p.to_string());
  }
  CHECK(seen.size() > 1);
}
