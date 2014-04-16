#include <stdio.h>

#include "shell.h"

int main(int argc, char *argv[])
{
	int res;
	res = shell(argc, argv);
#if defined(WIN32) && defined(_DEBUG)
	system("pause");
#endif
	return res;
}
