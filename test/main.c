#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../pcs/pcs_utils.h"
#include "shell.h"
#include "daemon.h"

int main(int argc, char *argv[])
{
	int res, i;
	for (i = 0; i < argc; i++) {
		if (pcs_utils_streq(argv[i], "--config=", 9)) {
			return daemon(argc, argv);
		}
	}
	return shell(argc, argv);
}
