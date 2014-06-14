#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pcs/pcs_mem.h"
#include "pcs/pcs_utils.h"
#include "pcs/pcs.h"
#include "shell.h"

static void init_context(ShellContext *context, int argc, char *argv[])
{
	memset(context, 0, sizeof(ShellContext));
	context->program = pcs_utils_strdup(argv[0]);



}

static void free_context(ShellContext *context)
{

}

static void usage(ShellContext *context)
{

}

int main(int argc, char *argv[])
{
	ShellContext context;
	int rc = 0;
	init_context(&context, argc, argv);


	free_context(&context);
	pcs_mem_print_leak();
	return rc;
}
