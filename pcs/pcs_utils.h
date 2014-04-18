#ifndef _PCS_UTILS_H
#define _PCS_UTILS_H

#include <stdarg.h>

#include "pcs_defs.h"
#include "pcs_slist.h"

/*���ص�ǰ�������Ƿ��Ǵ��ģʽ*/
PCS_API PcsBool pcs_isLittleEndian();
/*���ص�ǰ�������Ƿ���С��ģʽ*/
PCS_API PcsBool pcs_isBigEndian();

/*ԭ����һ���ַ���������ֵ��Ҫ����pcs_free()*/
PCS_API char *pcs_utils_strdup(const char *str);
/*pcs_utils_sprintf��ʸ��ģʽ*/
PCS_API char *pcs_utils_vsprintf(const char *fmt, va_list ap);
/*����ַ������ڴ棬�����ظ��ڴ档*/
PCS_API char *pcs_utils_sprintf(const char *fmt, ...);
/*��ʽ��sizeΪ���׶�дģʽ*/
PCS_API char *pcs_utils_readable_size(double size/*in bytes*/, char *buf, int buf_size, char *sp);
/*��ȡpath�ĸ�Ŀ¼*/
PCS_API char *pcs_utils_basedir(const char *path);
/*��ȡpath���ļ�������*/
PCS_API char *pcs_utils_filename(const char *path);
/*���Դ�Сд�Ƚ������ַ����� str1 < str2ʱ���ظ�����str1 == str2ʱ����0��str1 > str2ʱ����������*/
PCS_API int pcs_utils_strcmpi(const char *str1, const char *str2);
/*�ж����ַ����Ƿ���ȡ���ȷ���PcsTrue�����򷵻�PcsFalse��ֻ�Ƚ�lenָ���ĳ��ȡ�*/
PCS_API PcsBool pcs_utils_streq(const char *str1, const char *str2, int len);

#endif
