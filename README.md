# McEliece

## About

En este repositorio encontraras la implementacion del
<a href="https://en.wikipedia.org/wiki/McEliece_cryptosystem">criptosistema de McEliece</a> con codigos Hamming.
En el mismo repositorio, se encuetra una implementacion del codigo Hamming (7,4,3) que se usa en el criptosistema.
Please note that this code is for demonstration purposes only and not for other use.

## Considerations

This code is not recommended for use in document or text encryption of relevance, since, among many reasons, Hamming
codes are not the best option to implement in the McEliece criptosistema. Por ejemplo, los codigos de Goppa o 
Reed solomon ofrecen mayor complejidad y por tanto, mayor seguridad.

## Structure

The content of the different files of the repository will be described below:

- <ins>hamming/</ins>: Carpeta con funciones de codificacion y decodificacion de codigos Hamming (7,4,3).
- <ins>test/</ins>: Carpeta con unos tests para comprobar si McEliece funciona.
- <ins>matrix.c</ins>: Funciones para operaciones entre matrices y vectores.
- <ins>mceliece.c</ins>: Todas las funciones del criptosistema.
- <ins>mceliece.h/</ins>: Funciones para encriptar y desencriptar informacion con McEliece.

## Test

En la carpeta `hamming` hay pruebas para el codigo Hamming (7,4,3) implementado y en la carpeta
`test` estan los test para todo McEliece. Puedes probar ambos ejecutando sus makefile:

```bash
# Probar Hamming
cd hamming/
make test

# Probar McEliece
cd test/
make test
```
