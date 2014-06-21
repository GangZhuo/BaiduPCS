#ifndef _PCS_SHELL_ARG_H_
#define _PCS_SHELL_ARG_H_

#include "hashtable.h"

/*��������*/
struct args
{
	char		*name;
	char		*cmd;
	Hashtable	*opts;
	int			optc;
	char		**argv;
	int			argc;
};

/*��������*/
int parse_arg(struct args *arg, int argc, char *argv[]);
void free_args(struct args *arg);
/*
ɾ��һ��ѡ�
ɾ���ɹ�����0�����򷵻ط�0ֵ��
ִ�гɹ����� pValue��ΪNULL����ѱ�ɾ�����ֵд�� (*pValue)��ע��ֵ��Ҫ����pcs_free()
*/
int remove_opt(struct args *arg, const char *opt, char **pValue);

/*�ж��Ƿ����optѡ��
������ڵĻ�������1,���򷵻�0*/
int has_opt(struct args *arg, const char *opt);

/*�ж��Ƿ����optѡ�������ڵĻ�������ֵ���� ��*pValue���У�������1��
��������ڵĻ��������� (*pValue) = NULL, ������0*/
int has_optEx(struct args *arg, const char *opt, char **pValue);

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
int test_opts(struct args *arg, ...);

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
int has_opts(struct args *arg, ...);

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
int test_varg(struct args *arg, int minArgc, int maxArgc, va_list ap);

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
int test_arg(struct args *arg, int minArgc, int maxArgc, ...);


#endif
