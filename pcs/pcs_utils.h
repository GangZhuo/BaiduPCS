#ifndef _PCS_UTILS_H
#define _PCS_UTILS_H

#include <stdarg.h>

#include "pcs_defs.h"
#include "pcs_slist.h"

/*返回当前处理器是否是大端模式*/
PCS_API PcsBool pcs_isLittleEndian();
/*返回当前处理器是否是小端模式*/
PCS_API PcsBool pcs_isBigEndian();

/*原因复制一份字符串。返回值需要调用pcs_free()*/
PCS_API char *pcs_utils_strdup(const char *str);
/*pcs_utils_sprintf的矢量模式*/
PCS_API char *pcs_utils_vsprintf(const char *fmt, va_list ap);
/*输出字符串到内存，并返回该内存。*/
PCS_API char *pcs_utils_sprintf(const char *fmt, ...);
/*格式化size为容易读写模式*/
PCS_API char *pcs_utils_readable_size(double size/*in bytes*/, char *buf, int buf_size, char *sp);
/*获取path的父目录*/
PCS_API char *pcs_utils_basedir(const char *path);
/*获取path中文件名部分*/
PCS_API char *pcs_utils_filename(const char *path);
/*忽略大小写比较两个字符串。 str1 < str2时返回负数；str1 == str2时返回0；str1 > str2时返回正数。*/
PCS_API int pcs_utils_strcmpi(const char *str1, const char *str2);
/*判断两字符串是否相等。相等返回PcsTrue，否则返回PcsFalse。只比较len指定的长度。*/
PCS_API PcsBool pcs_utils_streq(const char *str1, const char *str2, int len);

#endif
