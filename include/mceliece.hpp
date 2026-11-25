#pragma once
#include "matrix.hpp"
#include <cstdint>
#include <optional>
#include <span>
#include <vector>

namespace mceliece::hc743 {

// Hamming codes H(3,7) matrix transpose Ht(7,3)
inline constexpr bit_matrix<h_rows, h_cols> hamming_ht{
    {0b100, 0b010, 0b110, 0b001, 0b101, 0b011, 0b111}};

// Hamming codes G(4,7) generator matrix
inline constexpr bit_matrix<g_rows, g_cols> hamming_g{
    {0b0000111, 0b0011001, 0b0101010, 0b1001011}};

struct secret_key {
  // private key secrets:
  // S(4,4) and its inverse
  // P(7,7) and its transpose
  bit_matrix<s_dim, s_dim> s;
  bit_matrix<s_dim, s_dim> si;
  bit_matrix<p_dim, p_dim> p;
  bit_matrix<p_dim, p_dim> pi;
};

// public key = S(4,4) * G(4,7) * P(7,7)
using public_key = bit_matrix<g_rows, g_cols>;

struct key_pair {
  public_key pub;
  secret_key sec;
};

struct s_pair {
  // matrix S(4,4) S-1(4,4)
  bit_matrix<s_dim, s_dim> s;
  bit_matrix<s_dim, s_dim> si;
};

[[nodiscard]] s_pair generate_s();
[[nodiscard]] bit_matrix<p_dim, p_dim> generate_p();
[[nodiscard]] key_pair keygen();

[[nodiscard]] std::uint8_t encode(std::uint8_t word, const public_key &pk);
[[nodiscard]] std::uint8_t decode(std::uint8_t word, const secret_key &sk);

[[nodiscard]] std::vector<std::uint8_t>
encrypt(const public_key &pk, std::span<const std::uint8_t> cipher);

[[nodiscard]] std::optional<std::vector<std::uint8_t>>
decrypt(const secret_key &sk, std::span<const std::uint8_t> cipher);

} // namespace mceliece::hc743
