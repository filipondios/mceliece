#pragma once
#include <array>
#include <bitset>
#include <cstddef>
#include <optional>
#include <ostream>
#include <string>

namespace mceliece::hc743 {

// square matrices (S, P) dimensions
inline constexpr std::size_t s_dim = 4;
inline constexpr std::size_t p_dim = 7;
// hamming transpose matrix dimensions
inline constexpr std::size_t h_rows = 7;
inline constexpr std::size_t h_cols = 3;
// code generator matrix dimensions
// also valid for results S(4,4) x G(4,7)
// and S(4,4) x G(4,7) x P(7,7)
inline constexpr std::size_t g_rows = 4;
inline constexpr std::size_t g_cols = 7;

// representation of a bit-made matrix (only 1s and 0s) as
// an array of bitsets, composed by Ri rows Ci columns.
template <std::size_t Ri, std::size_t Ci> struct bit_matrix {
  std::array<std::bitset<Ci>, Ri> data;

  template <std::size_t Pi>
  [[nodiscard]] bit_matrix<Ri, Pi>
  // matrix multiplication A(Ri,Ci) * B(Ci,Pi) = C(Ri,Pi)
  operator*(const bit_matrix<Ci, Pi> &rhs) const noexcept {
    bit_matrix<Ri, Pi> out{};

    for (std::size_t i = 0; i < Ri; ++i) {
      std::bitset<Pi> row{};

      for (std::size_t k = 0; k < Ci; ++k)
        if (this->data[i][k])
          row ^= rhs.data[k];
      out.data[i] = row;
    }
    return out;
  }

  // A(Ri,Ci) matrix transpose calculation, A^T(Ci,Ri)
  [[nodiscard]] bit_matrix<Ci, Ri> transpose() const noexcept {
    bit_matrix<Ci, Ri> out{};

    for (std::size_t i = 0; i < Ri; ++i)
      for (std::size_t j = 0; j < Ci; ++j)
        out.data[j][i] = this->data[i][j];
    return out;
  }

  // translate the matrix to a string
  std::string to_string() const {
    std::string out;
    out.reserve(Ri * (Ci + 1));

    for (std::size_t i = 0; i < Ri; ++i) {
      for (std::size_t k = 0; k < Ci; ++k)
        out += data[i][k] ? '1' : '0';
      out += '\n';
    }
    return out;
  }

  // overriding the '<<' operator so you can do "std::cout << matrix"
  friend std::ostream &operator<<(std::ostream &os, const bit_matrix &m) {
    return os << m.to_string();
  }
};

template <std::size_t N>
// M(n,n) matrix reduction via Gauss-Jordan
[[nodiscard]] std::optional<bit_matrix<N, N>>
gauss(const bit_matrix<N, N> &m) noexcept {
  std::array<std::bitset<N>, N> a = m.data;
  bit_matrix<N, N> inv{};

  for (std::size_t i = 0; i < N; ++i)
    inv.data[i].set(i);

  for (std::size_t col = 0; col < N; ++col) {
    // (1) search the pivot
    std::size_t pivot = col;
    while (pivot < N && !a[pivot][col])
      ++pivot;

    if (pivot == N)
      // there is no pivot
      return std::nullopt;

    // (2) put the pivot in the diagonal
    if (pivot != col) {
      std::swap(a[col], a[pivot]);
      std::swap(inv.data[col], inv.data[pivot]);
    }

    // (3) delete that column
    for (std::size_t row = 0; row < N; ++row) {
      if (row != col && a[row][col]) {
        a[row] ^= a[col];
        inv.data[row] ^= inv.data[col];
      }
    }
  }
  return inv;
}

} // namespace mceliece::hc743
