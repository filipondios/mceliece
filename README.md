# McEliece

## About

In this repository you will find the implementation of the <a href="https://en.wikipedia.org/wiki/McEliece_cryptosystem">McEliece cryptosystem</a>
with Hamming codes. In the same repository, you will find an implementation of the Hamming code (7,4,3) used in the
cryptosystem. Please note that this code is for demonstration purposes only and not for other use.

## Considerations

This code is not recommended for use in document or text encryption of relevance, since, among many reasons, Hamming
codes are not the best option to implement in the McEliece criptosistema. For example, the codes of Goppa or 
Reed Solomon offer greater complexity and therefore greater security

## Structure

The content of the different files of the repository will be described below:

- <ins>hamming/</ins>: Folder with Hamming code encoding and decoding functions (7,4,3).
- <ins>test/</ins>: Folder with some tests to check if McEliece works.
- <ins>matrix.c</ins>: Functions for operations between matrices and vectors.
- <ins>mceliece.c</ins>: All cryptosystem functions.
- <ins>mceliece.h/</ins>: Functions to encrypt and decrypt information with McEliece.

## Test

In the `hamming` folder there are tests for the implemented Hamming code (7,4,3) and in the `test` folder there are 
tests for all McEliece. You can test both by running their makefile:

```bash
# Probar Hamming
cd hamming/
make test

# Probar McEliece
cd test/
make test
```
