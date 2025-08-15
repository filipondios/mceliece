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
only the most significant bits are used, with the MSB as the first position. The 
[previous version](https://github.com/filipondios/mceliece/tree/1.0.0) stored each bit separately, 
wasting space and reducing access efficiency.

## Building with CMake

First of all, after cloning this project's repository, you need to initialize the git submodules, which in
this case are the dependencies [check 0.15.2](https://github.com/libcheck/check/tree/455005dc29dc6727de7ee36fee4b49a13b39f73f)
(for tests) and [libsodium 1.0.20](https://github.com/jedisct1/libsodium/tree/9511c982fb1d046470a8b42aa36556cdb7da15de)
(for RNG).

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

> [!IMPORTANT]
> After running build commands (either in Windows or Linux) with a preset `<preset>`, you should
> find the application static library file at `out/build/<preset>/` and the tests executable file at
> `out/build/<preset>/tests/`. The application binary must be named `libmceliece.a` (Linux) or `libmceliece.lib`
> (Windows) and the tests binary
> `mceliece_tests`.

# Version Upgrades

In this new release, following [v1.0.0](https://github.com/filipondios/mceliece/tree/1.0.0), a significant number 
of improvements have been introduced, increasing both the efficiency and overall quality of the project. Among the
most notable changes are: 

### **Matrix Representation**
As mentioned in the introduction, the representation of the cryptosystem matrices has been changed. Previously,
matrices were stored as `uint8_t[][]`. They are now stored as `uint8_t[]`, taking advantage of the fact that the 
cryptosystem matrices are binary and never exceed 8 columns. This allows each row to be stored in a single 
`uint8_t` value, where each bit represents a position in the row.

This change results in **faster access** and **more compact storage**, as shown below:

| Matrix                      | Previous Size         | Current Size |
|-----------------------------|-----------------------|--------------|
| `P`                         | `(7 × 7)` bytes       | `7` bytes    |
| `S`                         | `(4 × 4)` bytes       | `4` bytes    |
| Gaussian extended `S`       | `(4 × 8)` bytes       | `4` bytes    |
| `G`, `S*G`, `S*G*P`         | `(4 × 7)` bytes       | `4` bytes    |
| `Ht`                        | `(7 × 3)` bytes       | `7` bytes    |

### **Ciphertext Size**
In the previous implementation, the encrypted text representation was highly inefficient, reaching a size of  
`plaintext_length × 14`.  
It is now simply `plaintext_length × 2`.

### **Function Reuse**
The same function is now used to compute the inverse of matrix `S` **and** to check if it is invertible via Gaussian elimination, instead of first calculating the determinant to verify invertibility.

### **Other Improvements**
- Cleaner, more readable, and thoroughly tested code.
- CMake is now used instead of Make, enabling cross-platform compilation and easier integration of external libraries.
- Clearer organization of source code and directories.

