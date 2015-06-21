#ifndef _PCS_SHELL_ARG_H_
#define _PCS_SHELL_ARG_H_

#include "hashtable.h"

/*参数对象*/
struct args
{
	char		*name;
	char		*cmd;
	Hashtable	*opts;
	int			optc;
	char		**argv;
	int			argc;
};

/*解析参数*/
int parse_arg(struct args *arg, int argc, char *argv[], char *(*utf8)(const char *s));
void free_args(struct args *arg);
/*
删除一个选项。
删除成功返回0，否则返回非0值。
执行成功，且 pValue不为NULL，则把被删除项的值写入 (*pValue)。注意值需要调用pcs_free()
*/
int remove_opt(struct args *arg, const char *opt, char **pValue);

/*判断是否存在opt选项
如果存在的话，返回1,否则返回0*/
int has_opt(struct args *arg, const char *opt);

/*判断是否存在opt选项，如果存在的话，把其值填入 （*pValue）中，并返回1，
如果不存在的话，则设置 (*pValue) = NULL, 并返回0*/
int has_optEx(struct args *arg, const char *opt, char **pValue);

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
int test_opts(struct args *arg, ...);

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
int has_opts(struct args *arg, ...);

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
int test_varg(struct args *arg, int minArgc, int maxArgc, va_list ap);

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
int test_arg(struct args *arg, int minArgc, int maxArgc, ...);


#endif
