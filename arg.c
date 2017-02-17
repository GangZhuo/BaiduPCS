#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "pcs/pcs_mem.h"
#include "arg.h"

#ifndef NULL
#define NULL ((void *)0)
#endif

/*找到str第一次出现ch的位置*/
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

/*从程序路径中找到文件名开始的位置，返回开始位置的指针*/
static inline char *filename(char *path)
{
	char *p;
	p = path;
	p += strlen(p);
	while (p > path && *p != '/' && *p != '\\') p--;
	if (*p == '/' || *p == '\\') p++;
	return p;
}

#pragma region 解析参数

/*用于释放struct arg中opts哈希表中的值字段*/
static void free_opt(void *p)
{
	if (p) pcs_free(p);
}

/*解析参数*/
int parse_arg(struct args *arg, int argc, char *argv[], char *(*s2utf8)(const char *s))
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
				if (ht_has(arg->opts, p, val - p)) return -1; /*重复指定参数*/
				if (ht_add(arg->opts, p, val - p, (*val) == '=' ? (s2utf8 ? s2utf8(val + 1) : arg_strdup(val + 1)) : NULL))
					return -1; /*添加到哈希表中失败*/
			}
			else {
				op = p;
				while (*op) {
					if (ht_has(arg->opts, op, 1)) return -1; /*重复指定参数*/
					if (ht_add(arg->opts, op, 1, NULL))
						return -1; /*添加到哈希表中失败*/
					op++;
				}
			}
		}
		else if (!arg->cmd) {
			arg->cmd = p;
		}
		else {
			arg->argv[j++] = s2utf8 ? s2utf8(p) : arg_strdup(p);
		}
	}
	return 0;
}

void free_args(struct args *arg)
{
	if (arg->opts) ht_destroy(arg->opts);
	if (arg->argv) {
		int i;
		for (i = 0; i < arg->argc; i++) {
			char *p = arg->argv[i];
			if (p) pcs_free(p);
		}
		pcs_free(arg->argv);
	}
}

/*判断是否存在opt选项
如果存在的话，返回1,否则返回0*/
int has_opt(struct args *arg, const char *opt)
{
	if (arg->opts) {
		return ht_has(arg->opts, opt, -1);
	}
	return 0;
}

/*判断是否存在opt选项，如果存在的话，把其值填入 （*pValue）中，并返回1，
如果不存在的话，则设置 (*pValue) = NULL, 并返回0*/
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
测试arg中可选项是否正确。
  arg     -
  ...     - 支持的所有可选项，以NULL结束
如果测试通过则返回0，否则返回非零值
例：
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
删除一个选项。
删除成功返回0，否则返回非0值。
执行成功，且 pValue不为NULL，则把被删除项的值写入 (*pValue)。注意值需要调用pcs_free()
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
判断是否存在opt选项
  ap - 待查询的所有可选项，以NULL结束
如果全部不存在则返回0，否则返回有几个存在
例：
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
判断是否存在opt选项
  ap - 待查询的所有可选项，以NULL结束
如果全部不存在则返回0，否则返回有几个存在
例：
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
测试arg是否正确。
arg     -
minArgc - 允许的最少参数
maxArgc - 允许的最多参数
ap      - 支持的所有可选项，以NULL结束
如果测试通过则返回0，否则返回非零值
例：
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
测试arg是否正确。
 arg     -
 minArgc - 允许的最少参数
 maxArgc - 允许的最多参数
 ...     - 支持的所有可选项，以NULL结束
如果测试通过则返回0，否则返回非零值
例：
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
