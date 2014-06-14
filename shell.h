#ifndef _BAIDUPCS_SHELL_H
#define _BAIDUPCS_SHELL_H

#include "pcs/pcs.h"

/* shell 的上下文 */
typedef struct ShellContext
{
	char		*program;

	Pcs			pcs;

	char		*workdir;


} ShellContext;

#endif
