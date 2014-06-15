#ifndef _BAIDUPCS_SHELL_H
#define _BAIDUPCS_SHELL_H

#include <time.h>
#include "pcs/pcs.h"

#define SORT_DIRECTION_ASC	0 /*����*/
#define SORT_DIRECTION_DESC 1 /*����*/


/* shell �������� */
typedef struct ShellContext
{
	char		*name; /*��������*/
	char		*cookiefile; /*Cookie�ļ�·��*/
	char		*captchafile; /*��֤��ͼƬ·��*/
	char		*workdir; /*��ǰ����Ŀ¼*/
	Pcs			pcs;

	int			list_page_size; /*ִ��list����ʱ��ÿҳ��С*/
	char		*list_sort_name; /*ִ��list����ʱ�������ֶΣ���ѡֵ��name|time|size*/
	char		*list_sort_direction; /*ִ��list����ʱ�������ֶΣ���ѡֵ��asc|desc*/

	char		*secure_method; /*���ܷ�������ѡֵ��plaintext|aes-cbc-128|aes-cbc-192|aes-cbc-256*/
	char		*secure_key;    /*����ʱ��KEY*/
	int			secure_enable;  /*�Ƿ����ü���*/

} ShellContext;

#endif
