#pragma once
#include <stddef.h>
#include <stdint.h>

// Obtener la longitud del vector resultante al encriptar
// un vector de longitud 'pt_len'.
#define getCipherLen(pt_len) (pt_len*14)

// Obtener la longitud del vector resultante al desencriptar
// un vector de longitud 'ct_len'.
#define getPlainTextLen(ct_len) (ct_len/14)

///
/// @brief Inicia el contexto de encriptacion. Genera las 
/// matrices S y P del criptosistema.
///
void init_encCtx(uint8_t Sin[4][4], uint8_t Pin[7][7]);

///
/// @brief Dado un vector de informacion 'pt' de longitud
/// pt_len, encriptarlo y obtener un vector de 0s y 1s con
/// el codig generado de longitud ct.
/// 
/// Tener en cuenta que ya se han introducido errores en
/// dicho vector y que anadir mas, solo hara que falle 
/// la desencriptacion.
///
void encrypt(uint8_t* pt, size_t pt_len, uint8_t* ct);

///
/// @brief Inicia el contexto de desencriptacion. Obtiene las 
/// matrices S y P del criptosistema y genera la inversa de
/// la matriz S.
///
void init_decCtx(uint8_t Sin[4][4], uint8_t Pin[7][7]);

///
/// @brief Dado un vector de informacion 'ct' de longitud
/// ct_len, desencriptarlo y obtener el contenido original
/// que se mando a 'encrypt' como 'pt'.
/// 
/// Tener en cuenta que ya se han corregido errores en
/// dicho vector.
///
void decrypt(uint8_t* ct, size_t ct_len, uint8_t* pt);