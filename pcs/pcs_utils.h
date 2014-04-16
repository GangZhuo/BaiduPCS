#ifndef _PCS_UTILS_H
#define _PCS_UTILS_H

#include <stdarg.h>

#include "pcs_defs.h"
#include "pcs_slist.h"

PCS_API PcsBool pcs_isLittleEndian();
PCS_API PcsBool pcs_isBigEndian();

PCS_API char *pcs_utils_strdup(const char *str);
PCS_API char *pcs_utils_strdup_utf8(const char *str);
PCS_API char *pcs_utils_vsprintf(const char *fmt, va_list ap);
PCS_API char *pcs_utils_sprintf(const char *fmt, ...);
PCS_API char *pcs_utils_readable_size(double size/*in bytes*/, char *buf, int buf_size, char *sp);
PCS_API char *pcs_utils_basedir(const char *path);
PCS_API char *pcs_utils_filename(const char *path);
PCS_API int pcs_utils_strcmpi(const char *str1, const char *str2);
PCS_API PcsBool pcs_utils_streq(const char *str1, const char *str2, int len);

#endif
