#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "pcs/pcs_mem.h"
#include "arg.h"

#ifndef NULL
#define NULL ((void *)0)
#endif

/*�ҵ�str��һ�γ���ch��λ��*/
static inline char *findchar(char *str, int ch)
{
	char *p = str;
	while (*p && ((int)((unsigned int)(*p))) != ch) p++;
	return p;
}

static inline char *arg_strdup(const char *str)
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

/*�ӳ���·�����ҵ��ļ�����ʼ��λ�ã����ؿ�ʼλ�õ�ָ��*/
static inline char *filename(char *path)
{
	char *p;
	p = path;
	p += strlen(p);
	while (p > path && *p != '/' && *p != '\\') p--;
	if (*p == '/' || *p == '\\') p++;
	return p;
}

#pragma region ��������

/*�����ͷ�struct arg��opts��ϣ���е�ֵ�ֶ�*/
static void free_opt(void *p)
{
	if (p) pcs_free(p);
}

/*��������*/
int parse_arg(struct args *arg, int argc, char *argv[])
{
	int i, j = 0;
	char *p, *op, *val;
	arg->name = filename(argv[0]);
	for (i = 1; i < argc; ++i) {
		p = argv[i];
		if (*p == '-') {
			arg->optc++;
		}
		else {
			arg->argc++;
		}
	}
	if (arg->argc > 0) arg->argc--;
	if (arg->optc > 0) arg->opts = ht_create((int)((float)arg->optc * HASH_EXTEND_MULTIPLIER), 0, &free_opt);
	if (arg->argc > 0) arg->argv = (char **)pcs_malloc(arg->argc * sizeof(char *));
	for (i = 1; i < argc; ++i) {
		p = argv[i];
		if (*p == '-') {
			p++;
			if (*p == '-') {
				p++;
				val = findchar(p, '=');
				if (ht_has(arg->opts, p, val - p)) return -1; /*�ظ�ָ������*/
				if (ht_add(arg->opts, p, val - p, (*val) == '=' ? arg_strdup(val + 1) : NULL))
					return -1; /*��ӵ���ϣ����ʧ��*/
			}
			else {
				op = p;
				while (*op) {
					if (ht_has(arg->opts, op, 1)) return -1; /*�ظ�ָ������*/
					if (ht_add(arg->opts, op, 1, NULL))
						return -1; /*��ӵ���ϣ����ʧ��*/
					op++;
				}
			}
		}
		else if (!arg->cmd) {
			arg->cmd = p;
		}
		else {
			arg->argv[j++] = p;
		}
	}
	return 0;
}

void free_args(struct args *arg)
{
	if (arg->opts) ht_destroy(arg->opts);
	if (arg->argv) pcs_free(arg->argv);
}

/*�ж��Ƿ����optѡ��
������ڵĻ�������1,���򷵻�0*/
int has_opt(struct args *arg, const char *opt)
{
	if (arg->opts) {
		return ht_has(arg->opts, opt, -1);
	}
	return 0;
}

/*�ж��Ƿ����optѡ�������ڵĻ�������ֵ���� ��*pValue���У�������1��
��������ڵĻ��������� (*pValue) = NULL, ������0*/
int has_optEx(struct args *arg, const char *opt, char **pValue)
{
	if (arg->opts) {
		HashtableNode *node = ht_get_node(arg->opts, opt, -1);
		if (node) {
			if (pValue) (*pValue) = node->value;
			return 1;
		}
	}
	if (pValue) (*pValue) = NULL;
	return 0;
}

/*
����arg�п�ѡ���Ƿ���ȷ��
  arg     -
  ...     - ֧�ֵ����п�ѡ���NULL����
�������ͨ���򷵻�0�����򷵻ط���ֵ
����
  if (test_arg(arg, 0, 1, "d", "e", "r", "config", NULL)) {
    printf("Wrong Arguments\n");
    return -1;
  }
*/
int test_opts(struct args *arg, ...)
{
	int co = 0;
	const char *opt;
	va_list ap;
	if (!arg->opts) return 0;
	va_start(ap, arg);
	while ((opt = va_arg(ap, const char *)) != NULL) {
		if (ht_has(arg->opts, opt, -1))
			co++;
	}
	va_end(ap);
	return arg->opts->count - co;
}

/*
ɾ��һ��ѡ�
ɾ���ɹ�����0�����򷵻ط�0ֵ��
ִ�гɹ����� pValue��ΪNULL����ѱ�ɾ�����ֵд�� (*pValue)��ע��ֵ��Ҫ����pcs_free()
*/
int remove_opt(struct args *arg, const char *opt, char **pValue)
{
	char *val = NULL;
	if (!arg->opts) return -1;
	int rc = ht_remove(arg->opts, opt, -1, (void *)(pValue ? pValue : &val));
	if (rc == 0) {
		if (val) pcs_free(val);
		arg->optc--;
	}
	return rc;
}

/*
�ж��Ƿ����optѡ��
  ap - ����ѯ�����п�ѡ���NULL����
���ȫ���������򷵻�0�����򷵻��м�������
����
  if (has_vopt(arg, "d", "e", "r", "config", NULL)) {
    printf("One of the options exists.\n");
  } else {
    printf("All of the options not exists.\n");
  }
*/
int has_vopts(struct args *arg, va_list ap)
{
	int co = 0;
	const char *opt;
	if (!arg->opts) return 0;
	while ((opt = va_arg(ap, const char *)) != NULL) {
		if (ht_has(arg->opts, opt, -1))
			co++;
	}
	return co;
}

/*
�ж��Ƿ����optѡ��
  ap - ����ѯ�����п�ѡ���NULL����
���ȫ���������򷵻�0�����򷵻��м�������
����
  if (has_vopt(arg, "d", "e", "r", "config", NULL)) {
    printf("One of the options exists.\n");
  } else {
    printf("All of the options not exists.\n");
  }
*/
int has_opts(struct args *arg, ...)
{
	int rc = 0;
	va_list ap;
	va_start(ap, arg);
	rc = has_vopts(arg, ap);
	va_end(ap);
	return rc;
}

/*
����arg�Ƿ���ȷ��
arg     -
minArgc - ��������ٲ���
maxArgc - �����������
ap      - ֧�ֵ����п�ѡ���NULL����
�������ͨ���򷵻�0�����򷵻ط���ֵ
����
  if (test_arg(arg, 0, 1, "d", "e", "r", "config", NULL)) {
    printf("Wrong Arguments\n");
    return -1;
  }
*/
int test_varg(struct args *arg, int minArgc, int maxArgc, va_list ap)
{
	int co = 0;
	const char *opt;
	if (arg->argc < minArgc || arg->argc > maxArgc) return -1;
	if (!arg->opts) return 0;
	while ((opt = va_arg(ap, const char *)) != NULL) {
		if (ht_has(arg->opts, opt, -1))
			co++;
	}
	return arg->opts->count - co;
}

/*
����arg�Ƿ���ȷ��
 arg     -
 minArgc - ��������ٲ���
 maxArgc - �����������
 ...     - ֧�ֵ����п�ѡ���NULL����
�������ͨ���򷵻�0�����򷵻ط���ֵ
����
   if (test_arg(arg, 0, 1, "d", "e", "r", "config", NULL)) {
        printf("Wrong Arguments\n");
		return -1;
   }
*/
int test_arg(struct args *arg, int minArgc, int maxArgc, ...)
{
	int rc = 0;
	va_list ap;
	va_start(ap, maxArgc);
	rc = test_varg(arg, minArgc, maxArgc, ap);
	va_end(ap);
	return rc;
}

#pragma endregion
