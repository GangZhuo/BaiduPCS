#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../pcs/pcs_utils.h"
#include "shell.h"
#include "daemon.h"

const char *get_app_name(const char *app_name);
void print_usage(const char *app_name);

int main(int argc, char *argv[])
{
	int res, i;
	if (argc < 2) {
		print_usage(get_app_name(argv[0]));
	}
	for (i = 1; i < argc; i++) {
		if (pcs_utils_streq(argv[i], "--config=", 9)) {
			return start_daemon(argc, argv);
		}
	}
	return shell(argc, argv);
}

const char *get_app_name(const char *app_name)
{
	const char *p;
	p = app_name + strlen(app_name);
	while (p > app_name) {
		if (*p == '/' || *p == '\\') {
			p++;
			break;
		}
		p--;
	}
	return p;
}

void print_usage(const char *app_name)
{
	printf("USAGE: %s <command> <options>\n", app_name);
	printf("Command:\n", app_name);
	printf("   %s quota    \n", app_name);
	printf("   %s list     \n", app_name);
	printf("   %s rename   \n", app_name);
	printf("   %s delete   \n", app_name);
	printf("   %s download \n", app_name);
	printf("   %s upload   \n", app_name);
	printf("   %s move     \n", app_name);
	printf("   %s mkdir    \n", app_name);
	printf("   %s echo     \n", app_name);
	printf("   %s cat      \n", app_name);
	printf("   %s search   \n", app_name);
	printf("   %s copy     \n", app_name);
	printf("   %s meta     \n", app_name);
	printf("   %s svc      \n", app_name);
	printf("   %s reset    \n", app_name);
	printf("   %s update   \n", app_name);
	printf("   %s backup   \n", app_name);
	printf("   %s restore  \n", app_name);
	printf("   %s combin   \n", app_name);
	printf("   %s compare  \n", app_name);
}
