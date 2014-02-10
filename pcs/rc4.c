#include <stdlib.h>
#include "pcs_mem.h"
#include "md5.h"
#include "rc4.h"

void RC4_set_key(RC4_CTX *c, size_t keybytes, unsigned char *key)
{
	int i, j;
	unsigned char keyarr[256], swap;

	c->i = c->j = 0;
	for (i = j = 0;  i < 256;  i++, j = (j + 1) % keybytes) {
		c->sbox[i] = i;
		keyarr[i] = key[j];
	}
	for (i = j = 0;  i < 256;  i++) {
		j += c->sbox[i] + keyarr[i];
		j %= 256;
		swap = c->sbox[i];
		c->sbox[i] = c->sbox[j];
		c->sbox[j] = swap;
	}
}

/*
 * @c Pointer to an RC4_CTX object.
 * @n Number of bytes to encrypt.
 * @in Buffer to encrypt.
 * @out Output buffer.
*/
void RC4(RC4_CTX *c, size_t n, unsigned char *in, unsigned char *out)
{
	unsigned char swap;

	while (n--) {
		c->j += c->sbox[++c->i];
		swap = c->sbox[c->i];
		c->sbox[c->i] = c->sbox[c->j];
		c->sbox[c->j] = swap;
		swap = c->sbox[c->i] + c->sbox[c->j];
		*out++ = *in++ ^ c->sbox[swap];
	}
}

/* Assumes you have not yet initialized the context, but have allocated it. */
void secure_rc4_setup(RC4_CTX *ctx, char *key)
{
	char res[33]; /* 32 is the size in bytes of the resulting MD5 digest. */
   
	MDString(key, res);
	RC4_set_key(ctx, 32, (unsigned char *)res);
}
