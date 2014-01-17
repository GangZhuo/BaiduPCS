#ifndef _PCS_UTILS_H
#define _PCS_UTILS_H

#include <stdarg.h>

#include "pcs_defs.h"
#include "pcs_slist.h"

PCS_API char *pcs_utils_strdup(const char *str);
PCS_API char *pcs_utils_strdup_utf8(const char *str);
PCS_API char *pcs_utils_vsprintf(const char *fmt, va_list ap);
PCS_API char *pcs_utils_sprintf(const char *fmt, ...);
PCS_API char *pcs_utils_readable_size(double size/*in bytes*/, char *buf, int buf_size, char *sp);
PCS_API char *pcs_utils_basedir(const char *path);
PCS_API char *pcs_utils_filename(const char *path);
PCS_API int pcs_utils_strcmpi(const char *str1, const char *str2);

/*
 * 正则匹配
 * input  用于匹配的原始字符串
 * input_size 原始字符串长度
 * pattern 匹配模式
 * indies  匹配模式中输出到outputs中的索引数组,以-1结尾
 * pslist  用于接收匹配结果的字符串链表,匹配完成后，将按照indies数组中指定的索引按顺序填充到outputs中
*/
PCS_API PcsBool pcs_regex(const char *input, int input_size, const char *pattern, int *indies, PcsSList **pslist);

#endif
