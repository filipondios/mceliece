#include "matrix.hpp"
#include <algorithm>

namespace mceliece::hc743 {

// S(4,4) matrix reduction via Gauss and check if S(4,4)
// is invertible (via optional = no inversion)
[[nodiscard]] std::optional<bit_matrix<s_dim, s_dim>>
gauss(const bit_matrix<s_dim, s_dim> &m) noexcept {
  std::array<std::bitset<s_dim>, s_dim> a = m.data;
  bit_matrix<s_dim, s_dim> inv{};

  for (std::size_t i = 0; i < s_dim; ++i)
    inv.data[i].set(i);

  for (std::size_t col = 0; col < s_dim; ++col) {
    // (1) search the pivot
    std::size_t pivot = col;
    while (pivot < s_dim && !a[pivot][col])
      ++pivot;

    if (pivot == s_dim)
      // there is no pivot
      return std::nullopt;

    // (2) put the pivot in the diagonal
    if (pivot != col) {
      std::swap(a[col], a[pivot]);
      std::swap(inv.data[col], inv.data[pivot]);
    }

    // (3) delete that column
    for (std::size_t row = 0; row < s_dim; ++row) {
      if (row != col && a[row][col]) {
        a[row] ^= a[col];
        inv.data[row] ^= inv.data[col];
      }
    }
  }
  return inv;
}

#include <version>
#if defined(__cpp_lib_ranges_zip)
// tienes std::views::zip (C++23)
#endif

} // namespace mceliece::hc743
