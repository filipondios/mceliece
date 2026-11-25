#include "matrix.hpp"
#include <cstdint>
#include <optional>
#include <span>
#include <vector>

namespace mceliece::hc743 {

struct secret_key {
  // private key secrets:
  // S(4,4) and S inverse
  // P(7,7) permutation
  // G(4,7) generator
  // H(7,3) hamming transpose
  bit_matrix<s_dim, s_dim> s;
  bit_matrix<s_dim, s_dim> si;
  bit_matrix<p_dim, p_dim> p;
  bit_matrix<g_rows, g_cols> g;
  bit_matrix<h_cols, h_rows> ht;
};

struct public_key {
  // S(4,4) * G(4,7) * P(7,7)
  bit_matrix<g_rows, g_cols> sgp;
};

struct key_pair {
  public_key pk;
  secret_key sk;
};

[[nodiscard]] key_pair keygen();
[[nodiscard]] bit_matrix<s_dim, s_dim> generate_s();
[[nodiscard]] bit_matrix<p_dim, p_dim> generate_p();

[[nodiscard]] std::uint8_t encode(std::uint8_t word, const public_key &pk);
[[nodiscard]] std::uint8_t decode(std::uint8_t word, const public_key &sk);

[[nodiscard]] std::vector<std::uint8_t>
encrypt(const public_key &pk, std::span<const std::uint8_t> cipher);

[[nodiscard]] std::optional<std::vector<std::uint8_t>>
decrypt(const secret_key &sk, std::span<const std::uint8_t> cipher);

} // namespace mceliece::hc743
