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

## Building with CMake

First of all, after cloning this project's repository, you need to initialize the git submodules, which in
this case are the dependencies [check 0.15.2](https://github.com/libcheck/check/tree/455005dc29dc6727de7ee36fee4b49a13b39f73f)
(for tests) and [libsodium 1.0.20](https://github.com/jedisct1/libsodium/tree/9511c982fb1d046470a8b42aa36556cdb7da15de)
(for RNG).

```bash
git clone https://github.com/filipondios/mceliece
git submodule update --init --recursive
```

The `CMakePresets.json` file supports building for `x64` and `x86` architectures, in any mode `debug` or `release` for 
Linux and Windows. The compilation process is very simple: first you must choose a preset and the compile that preset.
These are some examples:

```sh
# Compile the project for x64 linux release mode
cmake --preset x64-release-linux
cmake --build --preset x64-release-linux

# Compile the project for x86 windows debug mode
cmake --preset x86-debug-windows
cmake --build --preset x86-debug-windows
```

> [!IMPORTANT]
> After running build commands (either in Windows or Linux) with a preset `<preset>`, you should
> find the application static library file at `out/build/<preset>/` and the tests executable file at
> `out/build/<preset>/tests/`. The application binary must be named `libmceliece.a` (Linux) or `libmceliece.lib`
> (Windows) and the tests binary
> `mceliece_tests`.

Once you run the command `cmake --preset <preset>`, the `out/build/<preset>/compile_commands.json`
file will be created. This file is used by the [clangd](https://github.com/clangd/clangd) LSP
to provide C/C++ IDE features to many editors. However, this file needs to be in the
root of the project. The best option is to create a symlink to the file:

```sh
cd /path/to/mceliece
PRESET="x64-release-linux"
cmake --preset $PRESET
ln -s out/build/$PRESET/compile_commands.json compile_commands.json
```
