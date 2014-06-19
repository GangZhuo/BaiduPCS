#ifndef _OPENSSL_MD5_H
#define _OPENSSL_MD5_H

#include <stddef.h>

#ifdef  __cplusplus
extern "C" {
#endif

#ifdef OPENSSL_NO_MD5
#error MD5 is disabled.
#endif

	/*
	* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	* ! MD5_LONG has to be at least 32 bits wide. If it's wider, then !
	* ! MD5_LONG_LOG2 has to be defined along.			   !
	* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	*/

#if defined(__LP32__)
#define MD5_LONG unsigned long
#elif defined(OPENSSL_SYS_CRAY) || defined(__ILP64__)
#define MD5_LONG unsigned long
#define MD5_LONG_LOG2 3
	/*
	* _CRAY note. I could declare short, but I have no idea what impact
	* does it have on performance on none-T3E machines. I could declare
	* int, but at least on C90 sizeof(int) can be chosen at compile time.
	* So I've chosen long...
	*					<appro@fy.chalmers.se>
	*/
#else
#define MD5_LONG unsigned int
#endif

#define MD5_CBLOCK	64
#define MD5_LBLOCK	(MD5_CBLOCK/4)
#define MD5_DIGEST_LENGTH 16

	typedef struct MD5state_st
	{
		MD5_LONG A, B, C, D;
		MD5_LONG Nl, Nh;
		MD5_LONG data[MD5_LBLOCK];
		unsigned int num;
	} MD5_CTX;

#ifdef OPENSSL_FIPS
	int private_MD5_Init(MD5_CTX *c);
#endif
	int MD5_Init(MD5_CTX *c);
	int MD5_Update(MD5_CTX *c, const void *data, size_t len);
	int MD5_Final(unsigned char *md, MD5_CTX *c);
	unsigned char *MD5(const unsigned char *d, size_t n, unsigned char *md);
	void MD5_Transform(MD5_CTX *c, const unsigned char *b);
#ifdef  __cplusplus
}
#endif

#endif
