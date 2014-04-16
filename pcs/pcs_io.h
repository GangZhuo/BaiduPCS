#ifndef _PCS_IO_H
#define _PCS_IO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef WIN32
#  include <malloc.h>
#  include <conio.h>
#  include <direct.h>
#else
#  include <alloca.h>
#  include <inttypes.h>
#  include <termios.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*�ӱ�׼�����л�ȡһ���ַ����ַ����Ե���׼���*/
void get_string_from_std_input(char *str, int size);
/*�ӱ�׼�����л�ȡһ���ַ��������ַ������ԡ�*/
void get_password_from_std_input(char *password, int size);

#ifdef __cplusplus
}
#endif


#endif
