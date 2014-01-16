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
 * ����ƥ��
 * input  ����ƥ���ԭʼ�ַ���
 * input_size ԭʼ�ַ�������
 * pattern ƥ��ģʽ
 * indies  ƥ��ģʽ�������outputs�е���������
 * outputs ���ڽ���ƥ�������ַ�������,ƥ����ɺ󣬽�����indies������ָ����������˳����䵽outputs��
 * output_count indies �� outputs ��Ԫ�ظ���
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
	re = pcre_compile(pattern,  // pattern, �����������Ҫ��������ַ�����ʽ��������ʽ
		0,						// options, �������������ָ������ʱ��һЩѡ��
		&error,					// errptr, ����������������������Ϣ
		&erroffset,				// erroffset, ���������pattern�г���λ�õ�ƫ����
		NULL);					// tableptr, �������������ָ���ַ���һ�������NULL
	if (re == NULL) {                 //�������ʧ�ܣ����ش�����Ϣ
		return PcsFalse;
	}
	rc = pcre_exec(re,				// code, �����������pcre_compile����õ�������ṹ��ָ��
		NULL,						// extra, ���������������pcre_exec��һЩ�����������Ϣ�Ľṹ��ָ��
		input,				// subject, ���������Ҫ������ƥ����ַ���
		input_size,			// length, ��������� Ҫ������ƥ����ַ�����ָ��
		0,							// startoffset, �������������ָ��subject��ʲôλ�ÿ�ʼ��ƥ���ƫ����
		0,							// options, ��������� ����ָ��ƥ������е�һЩѡ��
		ovector,					// ovector, �����������������ƥ��λ��ƫ����������
		REGEX_OVECCOUNT);					// ovecsize, ��������� ��������ƥ��λ��ƫ���������������С
	// ����ֵ��ƥ��ɹ����طǸ�����û��ƥ�䷵�ظ���
	if (rc < 0) {                     //���û��ƥ�䣬���ش�����Ϣ
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
	pcre_free(re);                     // ����������ʽre �ͷ��ڴ�
	*pslist = slist;
	return PcsTrue;
}
