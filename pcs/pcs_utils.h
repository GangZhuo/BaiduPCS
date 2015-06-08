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
PCS_API char* pcs_utils_readable_left_time(int64_t second, char *buf, int buf_size, char *sp);
/*获取path的父目录*/
PCS_API char *pcs_utils_basedir(const char *path);
/*获取path中文件名部分*/
PCS_API char *pcs_utils_filename(const char *path);
/*忽略大小写比较两个字符串。 str1 < str2时返回负数；str1 == str2时返回0；str1 > str2时返回正数。*/
PCS_API int pcs_utils_strcmpi(const char *str1, const char *str2);
/*判断两字符串是否相等。相等返回PcsTrue，否则返回PcsFalse。只比较len指定的长度。*/
PCS_API PcsBool pcs_utils_streq(const char *str1, const char *str2, int len);

/*计算字节的MD5值*/
PCS_API const unsigned char *pcs_md5_bytes_raw(const unsigned char* bytes, int len);

/*计算字节的MD5值*/
PCS_API const char *pcs_md5_bytes(const unsigned char* bytes, int len);

/**
* 字符串md5
*/
PCS_API const char *pcs_md5_string(const char *str);
/**
* 字符串md5。返回16字节的MD5值
*/
PCS_API const unsigned char *pcs_md5_string_raw(const char *str);
/**
* 文件 md5
*/
PCS_API const char *pcs_md5_file_s(const char *file_name);

/*把32位整数按从高位到低位顺序填充到buf的4个字节中。
* 例：0xF1E2D3C4 填充后 buf[0] = 0xF1, buf[1] = 0xE2, buf[2] = 0xD3, buf[3] = 0xC4.buf中其他项无改动
*/
void int2Buffer(int v, char *buf);

/*int2Buffer的逆过程*/
int readInt(const char *buf);

/*
* 提取出字符 callback({...}) 中的 {...} 部分
*/
char *extract_json_from_callback(char *callback);

PCS_API int pcs_strlen(const char *s);

PCS_API const char *pcs_time2str(time_t time);


#endif
