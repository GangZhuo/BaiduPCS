#include "utf8.h"
#include "encode.h"

#ifdef _WIN32

#include <stdio.h>
#include <malloc.h>
#include <stdint.h>
#include <windows.h>
typedef uint32_t u_int32_t;
#define snprintf _snprintf

int encode_is_utf8_sys()
{
	UINT codepage = GetConsoleCP();
	return codepage == 65001;
}

int encode_to_utf8(char *dest, int sz, const char *src, int srcsz)
{
	wchar_t *unicode;
	int wchars, err;

	wchars = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, src, srcsz, NULL, 0);
	if(wchars == 0) {
		fprintf(stderr, "Unicode translation error %d\n", GetLastError());
		return -1;
	}

	unicode = (wchar_t *) alloca((wchars + 1) * sizeof(unsigned short));
	err = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, src, srcsz, unicode, wchars);
	if(err != wchars) {
		fprintf(stderr, "Unicode translation error %d\n", GetLastError());
		return -1;
	}
	unicode[wchars] = L'\0';
	return u8_toutf8(dest, sz, unicode, wchars);
}

int encode_to_utf8_size(const char *src, int srcsz)
{
	wchar_t *unicode;
	int wchars, err;

	wchars = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, src, srcsz, NULL, 0);
	if(wchars == 0) {
		fprintf(stderr, "Unicode translation error %d\n", GetLastError());
		return -1;
	}

	unicode = (wchar_t *) alloca((wchars + 1) * sizeof(unsigned short));
	err = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, src, srcsz, unicode, wchars);
	if(err != wchars) {
		fprintf(stderr, "Unicode translation error %d\n", GetLastError());
		return -1;
	}
	unicode[wchars] = L'\0';
	//wprintf(L"UNICODE: %ls\n", unicode);
	return u8_size(unicode, wchars);
}

int encode_to_mbs(char *dest, int sz, const char *src, int srcsz)
{
	int unicode_size;
    wchar_t *unicode;
	//int unicode_len;
    int /*chars,*/ err;

	unicode_size = u8_wc_size(src, srcsz);
	unicode = (wchar_t *) alloca((unicode_size + 1) * sizeof(unsigned short));
	unicode[unicode_size] = L'\0';
	u8_toucs(unicode, unicode_size, src, srcsz);

    err = WideCharToMultiByte(GetConsoleCP(), WC_COMPOSITECHECK, unicode, unicode_size, dest, sz, NULL, NULL);
    if(err < 1)
    {
        fprintf(stderr, "Unicode translation error %d\n", GetLastError());
        return -1;
    }

    return err;
}

int encode_to_mbs_size(const char *src, int srcsz)
{
	int unicode_size;
    wchar_t *unicode;
	//int unicode_len;
    int /*chars,*/ err;

	unicode_size = u8_wc_size(src, srcsz);
	unicode = (wchar_t *) alloca((unicode_size + 1) * sizeof(unsigned short));
	unicode[unicode_size] = L'\0';
	u8_toucs(unicode, unicode_size, src, srcsz);

    err = WideCharToMultiByte(GetConsoleCP(), WC_COMPOSITECHECK, unicode, unicode_size, NULL, 0, NULL, NULL);
    return err;
}

#else

#include <alloca.h>
#include <iconv.h>

static char *u8_current_charset = 0; /* means "US-ASCII" */

const char *u8_get_sys_charset()
{
	static char sys_charset[64] = {0};
	if (!sys_charset[0]) {
		char *charset = getenv("LANG");
		if (charset && *charset) {
			char *p = charset;
			int i;
			while (*p && *p != '.') p++;
			if (*p == '.')
				p++;
			else
				p = charset;
			i = 0;
			while(*p) {
				if (i >= 63)
					break;
				if (*p >= 'A' && *p <= 'Z')
					sys_charset[i] = *p - 'A' + 'a';
				else
					sys_charset[i] = *p;
				i++;
				p++;
			}
			sys_charset[i] = '\0';
		} else {
			strncpy(sys_charset, "GB2312", 63);
		}
	}
	return sys_charset;
}

inline int encode_chcmpi(char a, char b)
{
	if((a == b) 
		|| ((a >= 'A' || a <= 'Z') && b == a + ('a' - 'A'))
		|| ((a >= 'a' || a <= 'z') && b == a - ('a' - 'A')))
		return 0;
	return a - b;
}

inline int encode_strcmpi(const char *str1, const char *str2)
{
	int r;
	const char *p1 = str1,
		*p2 = str2;
	while((*p1) && (*p2)) {
		r = encode_chcmpi(*p1, *p2);
		if (r) return r;
		p1++;
		p2++;
	}
	return (*p1) - (*p2);
}

int encode_is_utf8_sys()
{
	const char *charset = u8_get_sys_charset();
	return encode_strcmpi(charset, "utf-8") == 0;
}

void u8_convert_set_charset(const char *charset)
{

  if (!charset)
    charset = getenv("CHARSET");

  free(u8_current_charset);
  u8_current_charset = 0;
  if (charset && *charset)
    u8_current_charset = strdup(charset);
}

//代码转换:从一种编码转为另一种编码
int code_convert(const char *from_charset, const char *to_charset,
				 char *dest, int sz, const char *src, int srcsz)
{
	int ret = -1;
	iconv_t cd;
	int rc;
	size_t srcsz2 = srcsz,
		sz2 = sz;
	char **pin = (char **)&src;
	char **pout = &dest;
	cd = iconv_open(to_charset, from_charset);
	if(cd == 0) return -1;
	memset(dest, 0, sz2);
	if((ret = iconv(cd, pin, &srcsz2, pout, &sz2)) == -1) return -1;
	iconv_close(cd);
	return ret;
}

int encode_to_utf8(char *dest, int sz, const char *src, int srcsz)
{
	const char *charset = u8_get_sys_charset();
	if (pcs_utils_strcmpi(charset, "utf-8") == 0) {
		if (srcsz == -1) {
			const char *p = src;
			while(*p) {
				p++;
			}
			srcsz = p - src;
		}
		memcpy(dest, src, srcsz + 1);
		return srcsz;
	}
	else {
		return code_convert(charset, "utf-8", dest, sz, src, srcsz);
	}
}

int encode_to_utf8_size(const char *src, int srcsz)
{
	const char *charset = u8_get_sys_charset();
	if (pcs_utils_strcmpi(charset, "utf-8") == 0) {
		if (srcsz == -1) {
			const char *p = src;
			while(*p) {
				p++;
			}
			srcsz = p - src;
		}
		return srcsz;
	}
	else {
		int sz;
		char *dest;
		sz = srcsz * 4 + 4;
		dest = (char *) alloca(sz);
		return code_convert(charset, "utf-8", dest, sz, src, srcsz);
	}
}

int encode_to_mbs(char *dest, int sz, const char *src, int srcsz)
{
	const char *charset = u8_get_sys_charset();
	if (pcs_utils_strcmpi(charset, "utf-8") == 0) {
		if (srcsz == -1) {
			const char *p = src;
			while(*p) {
				p++;
			}
			srcsz = p - src;
		}
		memcpy(dest, src, srcsz);
		return srcsz;
	}
	else {
		return code_convert("utf-8", charset, dest, sz, src, srcsz);
	}
}

int encode_to_mbs_size(const char *src, int srcsz)
{
	const char *charset = u8_get_sys_charset();
	if (pcs_utils_strcmpi(charset, "utf-8") == 0) {
		if (srcsz == -1) {
			const char *p = src;
			while(*p) {
				p++;
			}
			srcsz = p - src;
		}
		return srcsz;
	}
	else {
		int sz;
		char *dest;
		sz = srcsz * 4 + 4;
		dest = (char *) alloca(sz);
		return code_convert("utf-8", charset, dest, sz, src, srcsz);
	}
}


#endif
