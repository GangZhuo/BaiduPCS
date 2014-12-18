#ifndef _OPENSSL_RSA_H
#define _OPENSSL_RSA_H

#include <stddef.h>

#ifdef  __cplusplus
extern "C" {
#endif

#ifdef OPENSSL_NO_RSA
#error RSA is disabled.
#endif

typedef struct bio_st BIO;

typedef struct rsa_st RSA;

#define RSA_NO_PADDING		3

BIO *BIO_new_mem_buf(void *buf, int len);

typedef int pem_password_cb(char *buf, int size, int rwflag, void *userdata);

RSA *PEM_read_bio_RSA_PUBKEY(BIO *bp, RSA **x, pem_password_cb *cb, void *u);

RSA *PEM_read_bio_RSAPublicKey(BIO *bp, RSA **x, pem_password_cb *cb, void *u);
int	RSA_public_encrypt(int flen, const unsigned char *from,
	unsigned char *to, RSA *rsa,int padding);
void	RSA_free(RSA *r);

void ERR_print_errors_fp(FILE *fp);



typedef struct evp_Encode_Ctx_st
{
	int num;	/* number saved in a partial encode/decode */
	int length;	/* The length is either the output line length
				* (in input bytes) or the shortest input line
				* length that is ok.  Once decoding begins,
				* the length is adjusted up each time a longer
				* line is decoded */
	unsigned char enc_data[80];	/* data to encode */
	int line_num;	/* number read on current line */
	int expect_nl;
} EVP_ENCODE_CTX;

void	EVP_EncodeInit(EVP_ENCODE_CTX *ctx);
void	EVP_EncodeUpdate(EVP_ENCODE_CTX *ctx,unsigned char *out,int *outl,
	const unsigned char *in,int inl);
void	EVP_EncodeFinal(EVP_ENCODE_CTX *ctx, unsigned char *out, int *outl);

#ifdef  __cplusplus
}
#endif

#endif
