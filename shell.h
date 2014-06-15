#ifndef _BAIDUPCS_SHELL_H
#define _BAIDUPCS_SHELL_H

#include "pcs/pcs.h"

/* shell 的上下文 */
typedef struct ShellContext
{
	char		*name; /*程序名字*/
	char		*cookiefile; /*Cookie文件路径*/
	char		*captchafile; /*验证码图片路径*/
	char		*workdir; /*当前工作目录*/
	Pcs			pcs;



} ShellContext;

#endif
