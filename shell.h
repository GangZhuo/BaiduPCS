#ifndef _BAIDUPCS_SHELL_H
#define _BAIDUPCS_SHELL_H

#include "pcs/pcs.h"

/* shell �������� */
typedef struct ShellContext
{
	char		*name; /*��������*/
	char		*cookiefile; /*Cookie�ļ�·��*/
	char		*captchafile; /*��֤��ͼƬ·��*/
	char		*workdir; /*��ǰ����Ŀ¼*/
	Pcs			pcs;



} ShellContext;

#endif
