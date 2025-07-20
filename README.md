# McEliece

In this repository you will find the implementation of the
[McEliece cryptosystem](https://en.wikipedia.org/wiki/McEliece_cryptosystem)
using the common ``(7,4,3)`` [Hamming codes](https://en.wikipedia.org/wiki/Hamming_code)
combination. Please note that this code is for demonstration purposes only and not for other use. 

> [!NOTE]
> This code is not recommended for use in document or text encryption of relevance,
> since, among many reasons, Hamming codes are not the best option to implement in
> the McEliece criptosistem. For example, [Goppa](https://en.wikipedia.org/wiki/Binary_Goppa_code)
> or [Reed Solomon](https://tomverbeure.github.io/2022/08/07/Reed-Solomon.html) codes
> offer greater complexity and therefore greater security.

> [!NOTE]
> In this version, the cryptosystem matrices are stored as arrays of 8-bit words, each representing
> a row of up to 8 bits. Since matrices `S=(4×4)`, `P=(7×7)`, and `G=(4×7)` don’t use all 8 bits,
> only the most significant bits are used, with the MSB as the first position. The previous version
> stored each bit separately, wasting space and reducing access efficiency.

## Building with CMake

First of all, after cloning this project's repository, you need to initialize the git submodules, which in
this case are the dependencies [check 0.15.2](https://github.com/libcheck/check/tree/455005dc29dc6727de7ee36fee4b49a13b39f73f)
and [libsodium 1.0.20](https://github.com/jedisct1/libsodium/tree/9511c982fb1d046470a8b42aa36556cdb7da15de).

```bash
git clone https://github.com/filipondios/mceliece
git submodule update --init --recursive
```

### Windows (Visual Studio)

You can open the project folder directly in **Visual Studio** on Windows without any modifications.
Visual Studio automatically detects the `CMakePresets.json` file. It supports building and running
in all configurations (x64, x86, Debug and Release).

### Linux
Similar to Windows, there are existing presets for compiling the project (you can see them in
[CMakePresets.json](CMakePresets.json)). Once you choose a preset, you can compile the project.
For example:

```sh
# Compile the project for x64 Release mode
cmake --preset x64-release-linux
cmake --build --preset x64-release-linux

# Compile the project for x86 Debug mode
cmake --preset x86-debug-linux
cmake --build --preset x86-debug-linux
```

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

> [!NOTE]
> After running build commands (either in Windows or Linux) with a preset `<preset>`, you should
> find the application executable file at `out/build/<preset>/` and the tests executable file at
> `out/build/<preset>/tests/`. The application binary must be named `mceliece` and the tests binary
> `mceliece_tests`.
