#ifndef _UTF8_H
#define _UTF8_H

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif


/* is c the start of a utf8 sequence? */
#define isutf(c) (((c)&0xC0)!=0x80)

/* convert UTF-8 data to wide character */
int u8_toucs(wchar_t *dest, int sz, const char *src, int srcsz);

/* return the min size that can store the wide character, not include '\0'-terminated. mean: return_value = wcslen() * sizeof(wchar_t) */
int u8_wc_size(const char *src, int srcsz);

/* the opposite conversion */
int u8_toutf8(char *dest, int sz, const wchar_t *src, int srcsz);

/* return the min size that can store the UTF-8 codes, not include '\0'-terminated. mean: return_value = strlen(UTF-8 codes) */
int u8_size(const wchar_t *src, int srcsz);

/* single character to UTF-8 */
int u8_wc_toutf8(char *dest, wchar_t ch);

/* character number to byte offset 找到第charnum个字符的开始位置，str是utf8编码的一串字符，往后移动charnum个字符，实际移动的字节数量即是返回值 */
int u8_offset(const char *str, int charnum);

/* byte offset to character number 找到offset字节中包含的字符数量，s是utf8编码的一串字符，往后移动offset个字节，实际移动的字符数量即是返回值 */
int u8_charnum(const char *s, int offset);

/* return next character, updating an index variable */
wchar_t u8_nextchar(const char *s, int *i);

/* move to next character */
void u8_inc(const char *s, int *i);

/* move to previous character */
void u8_dec(const char *s, int *i);

/* returns length of next utf-8 sequence */
int u8_seqlen(const char *s);

/* assuming src points to the character after a backslash, read an
   escape sequence, storing the result in dest and returning the number of
   input characters processed */
int u8_read_escape_sequence(const char *src, wchar_t *dest);

/* given a wide character, convert it to an ASCII escape sequence stored in
   buf, where buf is "sz" bytes. returns the number of characters output. */
int u8_escape_wchar(char *buf, int sz, wchar_t ch);

/* convert a string "src" containing escape sequences to UTF-8 */
int u8_unescape(char *buf, int sz, const char *src);

/* convert UTF-8 "src" to ASCII with escape sequences.
   if escape_quotes is nonzero, quote characters will be preceded by
   backslashes as well. */
int u8_escape(char *buf, int sz, const char *src, int escape_quotes);

/* utility predicates used by the above */
int octal_digit(char c);
int hex_digit(char c);

/* return a pointer to the first occurrence of ch in s, or NULL if not
   found. character index of found character returned in *charn. */
char *u8_strchr(const char *s, wchar_t ch, int *charn);

/* same as the above, but searches a buffer of a given size instead of
   a NUL-terminated string. */
char *u8_memchr(const char *s, wchar_t ch, int sz, int *charn);

/* count the number of characters in a UTF-8 string */
int u8_strlen(const char *s);

int u8_is_locale_utf8(const char *locale);

/* printf where the format string and arguments may be in UTF-8.
   you can avoid this function and just use ordinary printf() if the current
   locale is UTF-8. */
int u8_vprintf(const char *fmt, va_list ap);
int u8_printf(const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif
