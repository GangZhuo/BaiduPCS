#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#ifdef WIN32
# include <malloc.h>
# include <pcre/pcre.h> 
#else
# include <alloca.h>
# include <pcre.h> 
#endif

#include "pcs_mem.h"
#include "utf8.h"
#include "pcs_utils.h"

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

PCS_API char *pcs_utils_strdup_utf8(const char *str)
{
	char *res = 0;
	if (str) {
		char *p;
		size_t sz = strlen(str),
			tmpsz = u8_tombs_size(str, sz);
		if (tmpsz < 1)
			return 0;
		p = (char *)alloca(tmpsz + 1);
		p[tmpsz] = '\0';
		sz = u8_tombs(p, tmpsz, str, sz);
		if (tmpsz < 1)
			return 0;
		sz = strlen(p);
		res = (char *)pcs_malloc(sz + 1);
		if (!res)
			return 0;
		strcpy(res, p);
	}
	return res;
}

PCS_API char *pcs_utils_vsprintf(const char *fmt, va_list ap)
{
    int cnt, sz=0;
    char *buf;
    char *res;

    sz = 512;
    buf = (char*)pcs_malloc(sz);
 try_print:
    cnt = vsnprintf(buf, sz, fmt, ap);
    if (cnt >= sz) {
		pcs_free(buf);
        buf = (char*)pcs_malloc(cnt + 1);
        sz = cnt + 1;
        goto try_print;
    }
	res = (char *)pcs_malloc(cnt + 1);
	memcpy(res, buf, cnt);
	res[cnt] = '\0';
	pcs_free(buf);
	return res;
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

/*
 * 正则匹配
 * input  用于匹配的原始字符串
 * input_size 原始字符串长度
 * pattern 匹配模式
 * indies  匹配模式中输出到outputs中的索引数组
 * outputs 用于接收匹配结果的字符串数组,匹配完成后，将按照indies数组中指定的索引按顺序填充到outputs中
 * output_count indies 或 outputs 的元素个数
*/
PCS_API PcsBool pcs_regex(const char *input, int input_size, const char *pattern, int *indies, PcsSList **pslist)
{
#define REGEX_OVECCOUNT		30 /* should be a multiple of 3 */
#define REGEX_PATTERN_LEN 1024 

	pcre  *re;
	const char *error, *substring_start;
	int  erroffset;
	int  ovector[REGEX_OVECCOUNT];
	int  rc, substring_length;
	int  *indx;
	PcsSList *slist = NULL, 
		*pitem = NULL;
	
	if (!input || !pattern || !indies || !pslist)
		return PcsFalse;
	if (input_size == -1)
		input_size = strlen(input);
	re = pcre_compile(pattern,  // pattern, 输入参数，将要被编译的字符串形式的正则表达式
		0,						// options, 输入参数，用来指定编译时的一些选项
		&error,					// errptr, 输出参数，用来输出错误信息
		&erroffset,				// erroffset, 输出参数，pattern中出错位置的偏移量
		NULL);					// tableptr, 输入参数，用来指定字符表，一般情况用NULL
	if (re == NULL) {                 //如果编译失败，返回错误信息
		return PcsFalse;
	}
	rc = pcre_exec(re,				// code, 输入参数，用pcre_compile编译好的正则表达结构的指针
		NULL,						// extra, 输入参数，用来向pcre_exec传一些额外的数据信息的结构的指针
		input,				// subject, 输入参数，要被用来匹配的字符串
		input_size,			// length, 输入参数， 要被用来匹配的字符串的指针
		0,							// startoffset, 输入参数，用来指定subject从什么位置开始被匹配的偏移量
		0,							// options, 输入参数， 用来指定匹配过程中的一些选项
		ovector,					// ovector, 输出参数，用来返回匹配位置偏移量的数组
		REGEX_OVECCOUNT);					// ovecsize, 输入参数， 用来返回匹配位置偏移量的数组的最大大小
	// 返回值：匹配成功返回非负数，没有匹配返回负数
	if (rc < 0) {                     //如果没有匹配，返回错误信息
		if (rc == PCRE_ERROR_NOMATCH) {
			pcre_free(re);
			return PcsFalse;
		}
		else {
			pcre_free(re);
			return PcsFalse;
		}
	}

	indx = indies;
	while(*indx != -1) {
		if (rc <= *indx)
			break;
		substring_start = input + ovector[2 * *indx];
		substring_length = ovector[2 * *indx + 1] - ovector[2 * *indx];
		if (!slist) {
			slist = pcs_slist_create_ex(substring_start, substring_length);
			if (!slist)
				return PcsFalse;
		}
		else {
			if (!pcs_slist_add_ex(slist, substring_start, substring_length)) {
				pcs_slist_destroy(slist);
				return PcsFalse;
			}
		}
		indx++;
	}
	pcre_free(re);                     // 编译正则表达式re 释放内存
	*pslist = slist;
	return PcsTrue;
}
