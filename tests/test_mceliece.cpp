#include "mceliece.hpp"
#include "test_helpers.hpp"
#include <algorithm>
#include <bit>
#include <catch2/generators/catch_generators_all.hpp>
#include <cstdint>
#include <optional>
#include <set>
#include <vector>

namespace {

// Palabra de codigo sin error para un nibble: m * (S*G*P).
std::uint8_t clean_codeword(unsigned nibble, const public_key &pk) {
  return static_cast<std::uint8_t>(
      (row_vector<4>(nibble) * pk).data[0].to_ulong());
}

std::vector<std::uint8_t> all_bytes() {
  std::vector<std::uint8_t> v(256);
  for (std::size_t i = 0; i < v.size(); ++i)
    v[i] = static_cast<std::uint8_t>(i);
  return v;
}

} // namespace

TEST_CASE("decode: corrige cualquier error simple en cualquier posicion",
          "[decode]") {
  for (int key = 0; key < 10; ++key) {
    const auto kp = keygen();

    for (unsigned nibble = 0; nibble < 16; ++nibble) {
      const auto cw = clean_codeword(nibble, kp.pub);
      CHECK(decode(cw, kp.sec) == nibble);

      // error en cada una de las 7 posiciones
      for (unsigned k = 0; k < 7; ++k) {
        const auto noisy = static_cast<std::uint8_t>(cw ^ (1u << k));
        INFO("nibble=" << nibble << " error en posicion " << k);
        CHECK(decode(noisy, kp.sec) == nibble);
      }
    }
  }
}

TEST_CASE("encode: añade exactamente un error a la palabra de codigo",
          "[encode]") {
  const auto kp = keygen();

  for (unsigned nibble = 0; nibble < 16; ++nibble) {
    const auto cw = clean_codeword(nibble, kp.pub);

    for (int n = 0; n < 50; ++n) {
      const auto word = encode(static_cast<std::uint8_t>(nibble), kp.pub);
      CHECK((word & 0x80u) == 0); // solo 7 bits
      CHECK(std::popcount(static_cast<unsigned>(word ^ cw)) == 1);
    }
  }
}

TEST_CASE("encode: el error aparece en las 7 posiciones", "[encode]") {
  const auto kp = keygen();
  const auto cw = clean_codeword(0b1010, kp.pub);

  std::set<unsigned> positions;
  for (int n = 0; n < 1000; ++n) {
    const auto word = encode(0b1010, kp.pub);
    positions.insert(static_cast<unsigned>(
        std::countr_zero(static_cast<unsigned>(word ^ cw))));
  }

  CHECK(positions.size() == 7);
}

TEST_CASE("encode/decode: ida y vuelta de los 16 nibbles", "[roundtrip]") {
  for (int key = 0; key < 10; ++key) {
    const auto kp = keygen();

    for (unsigned nibble = 0; nibble < 16; ++nibble) {
      for (int n = 0; n < 30; ++n) {
        const auto word = encode(static_cast<std::uint8_t>(nibble), kp.pub);
        INFO("nibble=" << nibble);
        CHECK(decode(word, kp.sec) == nibble);
      }
    }
  }
}

TEST_CASE("encrypt/decrypt: ida y vuelta con los 256 valores de byte",
          "[encrypt]") {
  const auto kp = keygen();
  const auto plain = all_bytes();
  const auto cipher = encrypt(kp.pub, plain);
  const auto back = decrypt(kp.sec, cipher);
  REQUIRE(back.has_value());
  CHECK(*back == plain);
}

TEST_CASE("encrypt/decrypt: mensaje vacio", "[encrypt]") {
  const auto kp = keygen();
  const std::vector<std::uint8_t> empty;
  const auto cipher = encrypt(kp.pub, empty);
  CHECK(cipher.empty());
  const auto back = decrypt(kp.sec, cipher);
  REQUIRE(back.has_value());
  CHECK(back->empty());
}

TEST_CASE("encrypt: el cifrado ocupa el doble y usa palabras de 7 bits",
          "[encrypt]") {
  const auto kp = keygen();
  const auto plain = all_bytes();
  const auto cipher = encrypt(kp.pub, plain);
  CHECK(cipher.size() == plain.size() * 2);
  CHECK(std::ranges::all_of(cipher,
                            [](std::uint8_t b) { return (b & 0x80u) == 0; }));
}

TEST_CASE("encrypt: es probabilistico (mismo mensaje, distinto cifrado)",
          "[encrypt]") {
  const auto kp = keygen();
  const auto plain = all_bytes();
  const auto c1 = encrypt(kp.pub, plain);
  const auto c2 = encrypt(kp.pub, plain);
  CHECK(c1 != c2);

  // ...pero ambos descifran al mismo mensaje
  CHECK(decrypt(kp.sec, c1) == std::optional{plain});
  CHECK(decrypt(kp.sec, c2) == std::optional{plain});
}

TEST_CASE("decrypt: tamano impar devuelve nullopt", "[decrypt]") {
  const auto kp = keygen();
  const std::vector<std::uint8_t> odd(5, 0);
  CHECK_FALSE(decrypt(kp.sec, odd).has_value());
}

TEST_CASE("decrypt: una clave distinta no recupera el mensaje", "[decrypt]") {
  const auto kp1 = keygen();
  const auto kp2 = keygen();
  const auto plain = all_bytes();
  const auto cipher = encrypt(kp1.pub, plain);
  const auto wrong = decrypt(kp2.sec, cipher);

  // Si por casualidad kp1 == kp2 el test no tiene sentido: se omite.
  if (kp1.sec.s.data == kp2.sec.s.data && kp1.sec.p.data == kp2.sec.p.data)
    SKIP("claves identicas por azar");

  REQUIRE(wrong.has_value());
  CHECK(*wrong != plain);
}
