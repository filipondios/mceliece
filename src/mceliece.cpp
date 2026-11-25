#include <array>
#include <mceliece.hpp>
#include <numeric>
#include <sodium/core.h>
#include <sodium/randombytes.h>
#include <sodium/utils.h>
#include <stdexcept>

namespace mceliece::hc743 {

// generation of the matrix S(4,4) and its inverse
// via rejection sampling and Gauss Jordan
[[nodiscard]] s_pair generate_s() {
  for (;;) {
    bit_matrix<s_dim, s_dim> s{};
    for (auto &row : s.data)
      row = std::bitset<s_dim>{randombytes_uniform(1u << s_dim)};

    if (auto si = gauss(s))
      return {s, *si};
  }
}

// generation of the permutation matrix P(7,7) via
// Fisher-Yates: The 7! permutations are equally probable
[[nodiscard]] bit_matrix<p_dim, p_dim> generate_p() {
  std::array<std::size_t, p_dim> perm;
  std::iota(perm.begin(), perm.end(), std::size_t{0});

  for (std::size_t i = p_dim - 1; i > 0; --i) {
    const auto j = static_cast<std::size_t>(
        randombytes_uniform(static_cast<std::uint32_t>(i + 1)));
    std::swap(perm[i], perm[j]);
  }

  bit_matrix<p_dim, p_dim> p{};
  for (std::size_t i = 0; i < p_dim; ++i)
    p.data[i].set(perm[i]);
  sodium_memzero(perm.data(), sizeof perm);
  return p;
}

[[nodiscard]] key_pair keygen() {
  if (sodium_init() < 0)
    throw std::runtime_error("sodium_init failed");

  const auto [s, si] = generate_s();
  const auto p = generate_p();

  return {.pub = (s * hamming_g) * p,
          .sec = {.s = s, .si = si, .p = p, .pi = p.transpose()}};
}

template <std::size_t N>
// converts a uint8_t to a vector (bit_matrix<1,N>)
[[nodiscard]] bit_matrix<1, N> to_row(std::uint8_t v) noexcept {
  bit_matrix<1, N> out{};
  out.data[0] = std::bitset<N>{v};
  return out;
}

template <std::size_t N>
// converts a vector (bit_matrix<1,N>) to a uint8_t
[[nodiscard]] std::uint8_t from_row(const bit_matrix<1, N> &m) noexcept {
  return static_cast<std::uint8_t>(m.data[0].to_ulong());
}

// y = m * (S*G*P) + e, with wt(e) = 1 at a random position
[[nodiscard]] std::uint8_t encode(std::uint8_t word, const public_key &pk) {
  auto c = to_row<g_rows>(word) * pk;
  c.data[0].flip(randombytes_uniform(static_cast<std::uint32_t>(p_dim)));
  return from_row(c);
}

// y*P^-1 = m*S*G + e*P^-1, then fix 1 error & extract m*S -> *S^-1
[[nodiscard]] std::uint8_t decode(std::uint8_t word, const secret_key &sk) {
  // undo the permutation: P^-1 = P^T
  // then syndrome s = r * Ht, (1,7)x(7,3)
  auto r = to_row<p_dim>(word) * sk.pi;
  const auto s = r * hamming_ht;

  // The row of Ht equal to the syndrome marks the error position.
  // Syndrome 0 does not match any row, so nothing is corrected.
  for (std::size_t i = 0; i < h_rows; ++i)
    r.data[0][i] = r.data[0][i] ^ (s.data[0] == hamming_ht.data[i]);

  // m*S = r * R, (1,7)x(7,4)
  // finally m = (m*S) * S^-1
  bit_matrix<1, g_rows> ms{};
  ms.data[0][0] = r.data[0][2];
  ms.data[0][1] = r.data[0][4];
  ms.data[0][2] = r.data[0][5];
  ms.data[0][3] = r.data[0][6];
  return from_row(ms * sk.si);
}

// encrypts each input byte as two encoded 4-bit nibbles (high
// then low), returning a vector twice the input size.
[[nodiscard]] std::vector<std::uint8_t>
encrypt(const public_key &pk, std::span<const std::uint8_t> cipher) {
  std::vector<std::uint8_t> out;
  out.reserve(cipher.size() * 2);

  for (const auto b : cipher) {
    // all bytes must be divided into nibbles, and then be encoded
    out.push_back(encode(static_cast<std::uint8_t>(b >> 4), pk));
    out.push_back(encode(static_cast<std::uint8_t>(b & 0x0F), pk));
  }
  return out;
}

// decrypts pairs of bytes holding 7-bit codewords back into original
// bytes, returning nullopt if input length is odd or any byte sets bit 7.
[[nodiscard]] std::optional<std::vector<std::uint8_t>>
decrypt(const secret_key &sk, std::span<const std::uint8_t> cipher) {
  if (cipher.size() % 2 != 0)
    // @encrypt to understand!
    return std::nullopt;

  std::vector<std::uint8_t> out;
  out.reserve(cipher.size() / 2);

  for (std::size_t i = 0; i < cipher.size(); i += 2) {
    // a codeword is 7 bits, and bit 7 must be 0
    if ((cipher[i] | cipher[i + 1]) & 0x80)
      return std::nullopt;

    const auto hi = decode(cipher[i], sk);
    const auto lo = decode(cipher[i + 1], sk);
    out.push_back(static_cast<std::uint8_t>((hi << 4) | lo));
  }
  return out;
}

} // namespace mceliece::hc743
