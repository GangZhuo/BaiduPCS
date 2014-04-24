#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../pcs/pcs_mem.h"
#include "../pcs/pcs_utils.h"
#include "shell_args.h"
#include "shell.h"
#include "daemon.h"

int main(int argc, char *argv[])
{
	int rc;
	struct params *params = shell_args_create_params();

	printf("%s\n", program_full_name);
	shell_args_parse(argc, argv, params);

	if (params->is_fail) {
		shell_args_destroy_params(params);
		pcs_mem_print_leak();
		return -1;
	}

	switch (params->action) {
	case ACTION_SVC:
	case ACTION_RESET:
	case ACTION_UPDATE:
	case ACTION_BACKUP:
	case ACTION_RESTORE:
	case ACTION_COMBIN:
	case ACTION_COMPARE:
	case ACTION_TIME:
		rc = start_daemon(params);
		break;
	default:
		rc = shell(params);
		break;
	}
	shell_args_destroy_params(params);
	pcs_mem_print_leak();
	return rc;
}
