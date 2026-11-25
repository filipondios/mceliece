# McEliece

In this repository you will find the implementation of the
[McEliece cryptosystem](https://en.wikipedia.org/wiki/McEliece_cryptosystem)
using the common ``(7,4,3)`` [Hamming codes](https://en.wikipedia.org/wiki/Hamming_code)
combination as a static library. Please note that this code is for demonstration purposes
only and not for other use. 

> [!NOTE]
> This code is not recommended for use in document or text encryption of relevance,
> since, among many reasons, Hamming codes are not the best option to implement in
> the McEliece criptosistem. For example, [Goppa](https://en.wikipedia.org/wiki/Binary_Goppa_code)
> or [Reed Solomon](https://tomverbeure.github.io/2022/08/07/Reed-Solomon.html) codes
> offer greater complexity and therefore greater security.

In this new version, the cryptosystem matrices are stored as arrays of 8-bit blocks, each representing
a row of up to 8 bits. Since matrices `S=(4×4)`, `P=(7×7)`, and `G=(4×7)` don’t use all 8 bits,
only the most significant bits are used, with the [MSB](https://en.wikipedia.org/wiki/Bit_numbering#Most_significant_bit)
as the first position. The [previous version v1.0.0](https://github.com/filipondios/mceliece/tree/1.0.0) 
stored each bit separately, wasting space and reducing access efficiency.

## Building with Conan + CMake

Dependencies ([libsodium](https://github.com/jedisct1/libsodium) for the RNG and
[Catch2](https://github.com/catchorg/Catch2) for the tests) are resolved with
[Conan 2](https://conan.io) and the project is built with CMake. Requires CMake
>= 3.23, Ninja and a Conan profile (`conan profile detect`).

```sh
conan install . --output-folder=build --build=missing   # fetch/build dependencies
cmake --preset release                                  # configure
cmake --build --preset release                          # compile
ctest --preset release                                  # run tests
```

The `debug` and `release` presets share the `build/` directory so that
`compile_commands.json` keeps a fixed location for clangd / LSP servers. To
switch build type, re-run `conan install` with the matching setting first:

```sh
conan install . -s build_type=Debug --output-folder=build --build=missing
cmake --preset debug
cmake --build --preset debug
ctest --preset debug
```

> [!IMPORTANT]
> Everything is written to `build/`: the static library `build/libmceliece.a`
> (`libmceliece.lib` on Windows), the test binary `build/tests/mceliece_tests`
> and `build/compile_commands.json`.

The compiler, architecture and build type are taken from the Conan profile, so
the same presets work on other platforms and toolchains by installing a
matching profile.
