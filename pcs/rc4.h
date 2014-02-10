#ifndef _RC4_H_
#define _RC4_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  unsigned char sbox[256];
  unsigned char i, j;
} RC4_CTX;

/*
 * Assumes you have not yet initialized the context, but have allocated it.
 * @ctx Pointer to an RC4_CTX object.
 * @keybytes Number of bytes to key.
 * @key 
*/
void RC4_set_key(RC4_CTX *ctx, size_t keybytes, unsigned char *key);

/*
 * Assumes you have not yet initialized the context, but have allocated it.
 * @ctx Pointer to an RC4_CTX object.
 * @n   Number of bytes to encrypt.
 * @in  Buffer to encrypt.
 * @out Output buffer.
*/
void RC4(RC4_CTX *ctx, size_t n, unsigned char *in, unsigned char *out);

/*
 * Assumes you have not yet initialized the context, but have allocated it.
 * @ctx Pointer to an RC4_CTX object.
 * @key key should be end with '\0'
*/
void secure_rc4_setup(RC4_CTX *ctx, char *key);

#ifdef __cplusplus
}
#endif



#endif
