#pragma once
#include "matrix.hpp"
#include <bitset>
#include <catch2/catch_test_macros.hpp>
#include <cstddef>
#include <cstdint>
#include <sodium.h>

using namespace mceliece::hc743;

// Compara dos matrices y, si falla, imprime ambas (columna 0 a la izquierda).
#define CHECK_MATRIX_EQ(actual, expected)                                      \
  do {                                                                         \
    const auto &m_actual_ = (actual);                                          \
    const auto &m_expected_ = (expected);                                      \
    INFO("actual:\n"                                                           \
         << m_actual_.to_string() << "expected:\n"                             \
         << m_expected_.to_string());                                          \
    CHECK(m_actual_.data == m_expected_.data);                                 \
  } while (false)

template <std::size_t N> [[nodiscard]] bit_matrix<N, N> identity() {
  bit_matrix<N, N> m{};
  for (std::size_t i = 0; i < N; ++i)
    m.data[i].set(i);
  return m;
}

template <std::size_t R, std::size_t C>
[[nodiscard]] bool equal(const bit_matrix<R, C> &a, const bit_matrix<R, C> &b) {
  return a.data == b.data;
}

template <std::size_t R, std::size_t C>
[[nodiscard]] bool is_zero(const bit_matrix<R, C> &m) {
  for (const auto &row : m.data)
    if (row.any())
      return false;
  return true;
}

// Vector fila 1xC a partir de un entero (bit k = columna k).
template <std::size_t C>
[[nodiscard]] bit_matrix<1, C> row_vector(unsigned long long v) {
  bit_matrix<1, C> m{};
  m.data[0] = std::bitset<C>(v);
  return m;
}

// Matriz 4x4 a partir de 16 bits: la fila i son los bits 4i..4i+3.
[[nodiscard]] inline bit_matrix<4, 4> matrix4_from_bits(std::uint32_t v) {
  bit_matrix<4, 4> m{};
  for (std::size_t i = 0; i < 4; ++i)
    m.data[i] = std::bitset<4>((v >> (4 * i)) & 0xFu);
  return m;
}

// PRNG determinista (splitmix64) SOLO para tests reproducibles.
struct splitmix64 {
  std::uint64_t state;
  std::uint64_t next() {
    std::uint64_t z = (state += 0x9E3779B97F4A7C15ULL);
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
    return z ^ (z >> 31);
  }
};

template <std::size_t R, std::size_t C>
[[nodiscard]] bit_matrix<R, C> random_matrix(splitmix64 &rng) {
  bit_matrix<R, C> m{};
  for (auto &row : m.data)
    row = std::bitset<C>(rng.next());
  return m;
}

// libsodium debe estar iniciada antes de llamar a randombytes_* directamente.
inline void init_sodium() { REQUIRE(sodium_init() >= 0); }
