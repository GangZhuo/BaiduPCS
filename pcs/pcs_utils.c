#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#ifdef WIN32
# include <malloc.h>
# define snprintf _snprintf
#else
# include <alloca.h>
#endif

#include "pcs_mem.h"
#include "pcs_utils.h"
#include "openssl_md5.h"

PCS_API PcsBool pcs_isLittleEndian()
{
	union w {
		int a;
		char b;
	} c;
	c.a = 1;
	return (c.b == 1);
}

PCS_API PcsBool pcs_isBigEndian()
{
	union w {
		int a;
		char b;
	} c;
	c.a = 1;
	return (c.b == 0);
}

PCS_API char *pcs_utils_strdup(const char *str)
{
	char *res = 0;
	if (str) {
		res = (char *)pcs_malloc(strlen(str) + 1);
		if (!res)
			return 0;
		strcpy(res, str);
	}
	return res;
}

PCS_API char *pcs_utils_vsprintf(const char *fmt, va_list ap)
{
    int cnt, sz=0;
    char *buf;
	va_list ap_try;

    sz = 128;
    buf = (char*)pcs_malloc(sz);
	va_copy(ap_try, ap);
	cnt = vsnprintf(buf, sz, fmt, ap_try);
	va_end(ap_try);
    if (cnt >= sz) {
		pcs_free(buf);
        buf = (char*)pcs_malloc(cnt + 1);
        sz = cnt + 1;
		cnt = vsnprintf(buf, sz, fmt, ap);
    }
	buf[cnt] = '\0';
	return buf;
}

PCS_API char *pcs_utils_sprintf(const char *fmt, ...)
{
    char *res;
    va_list args;

    va_start(args, fmt);

    res = pcs_utils_vsprintf(fmt, args);

    va_end(args);
    return res;
}

/* Human-readable file size */
PCS_API char* pcs_utils_readable_size(double size/*in bytes*/, char *buf, int buf_size, char *sp)
{
	int i = 0;
	const char* units[] = {"B", "kB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
	while (size > 1024) {
		size /= 1024;
		i++;
	}
	memset(buf, 0, buf_size);
	snprintf(buf, buf_size, "%.*f%s%s", i, size, sp ? sp : "", units[i]);
	return buf;
}

PCS_API char *pcs_utils_basedir(const char *path)
{
	char *dir, *p;
	int i;
	i = strlen(path);
	dir = (char *) pcs_malloc(i + 1);
	if (!dir)
		return NULL;
	strcpy(dir, path);
	p = dir + i - 1;
	while(p >= dir) {
		if (*p == '/')
			break;
		p--;
	}
	if (p > dir) {
		*p = '\0';
	}
	else {
		*p = '/';
		p++;
		*p = '\0';
	}
	return dir;
}

PCS_API char *pcs_utils_filename(const char *path)
{
	char *name;
	const char *p;
	int i;
	i = strlen(path);
	p = path + i - 1;
	while(p >= path) {
		if (*p == '/')
			break;
		p--;
	}
	if (*p == '/')
		p++;
	i = strlen(p);
	name = (char *) pcs_malloc(i + 1);
	if (!name)
		return NULL;
	strcpy(name, p);
	return name;
}

static inline int chcmpi(char a, char b)
{
	if((a == b) 
		|| ((a >= 'A' || a <= 'Z') && b == a + ('a' - 'A'))
		|| ((a >= 'a' || a <= 'z') && b == a - ('a' - 'A')))
		return 0;
	return a - b;
}

PCS_API int pcs_utils_strcmpi(const char *str1, const char *str2)
{
	int r;
	const char *p1 = str1,
		*p2 = str2;
	while((*p1) && (*p2)) {
		r = chcmpi(*p1, *p2);
		if (r) return r;
		p1++;
		p2++;
	}
	return (*p1) - (*p2);
}

PCS_API PcsBool pcs_utils_streq(const char *str1, const char *str2, int len)
{
	int i = 0;
	const char *p1 = str1,
		*p2 = str2;
	while((*p1) && (*p2) && (len == -1 || i < len)) {
		if ((*p1) != (*p2)) return PcsFalse;
		p1++;
		p2++;
	}
	return PcsTrue;
}


/**
* ×Ö·û´®md5
*/
PCS_API const char *md5_string(const char *str)
{
	static char tmp[33] = { '\0' };
	unsigned char md[16];
	int i;
	MD5((const unsigned char*)str, strlen(str), md);
	for (i = 0; i<16; i++){
		sprintf(&tmp[i * 2], "%02x", md[i]);
	}
	return tmp;
}

/**
* ÎÄ¼þ md5
*/
PCS_API const char *md5_file(const char *file_name)
{
	static char tmp[33] = { '\0' };
	MD5_CTX md5;
	unsigned char md[16];
	int length, i;
	char buffer[1024];
	FILE *file;
	MD5_Init(&md5);
	file = fopen(file_name, "rb");
	if (!file) {
		printf("%s can't be openedn", file_name);
		return 0;
	}
	while (length = fread(buffer, 1, 1024, file))
		MD5_Update(&md5, buffer, length);
	MD5_Final(md, &md5);
	fclose(file);
	for (i = 0; i<16; i++){
		sprintf(&tmp[i * 2], "%02x", md[i]);
	}
	return tmp;
}
