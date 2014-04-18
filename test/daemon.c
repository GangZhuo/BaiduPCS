#include <stdio.h>

typedef struct DaemonConfig {
	char	*cookie;
	int		interval;


} DaemonConfig;



int daemon(int argc, char *argv[])
{
	int res, i;
	for (i = 0; i < argc; i++) {
		if (pcs_utils_streq(argv[i], "--config=", 9)) {
			return daemon(argc, argv);
		}
	}
	return 0;
}
