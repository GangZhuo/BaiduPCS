#include <stdio.h>
#include <stdlib.h>

#include "pcs_io.h"
#include "pcs_mem.h"
#include "pcs_utils.h"
#include "pcs.h"
#include "main_args.h"

#ifdef WIN32
#  define strcmpi  _strcmpi
#  define strncmpi _strnicmp
#  define snprintf _snprintf
#  define mkdir _mkdir
#endif

static PcsBool my_get_verify_code(unsigned char *ptr, size_t size, char *captcha, size_t captchaSize, void *state)
{
	static char filename[1024] = { 0 };
	FILE *pf;

	if (!filename[0]) {

#ifdef WIN32
		strcpy(filename, getenv("UserProfile"));
		strcat(filename, "\\.baidupcs");
		mkdir(filename);
		strcat(filename, "\\verify_code.gif");
#else
		strcpy(filename, getenv("HOME"));
		strcat(filename, "/.baidupcs");
		mkdir(filename);
		strcat(filename, "/verify_code.gif");
#endif

	}

	pf = fopen(filename, "wb");
	if (!pf)
		return PcsFalse;
	fwrite(ptr, 1, size, pf);
	fclose(pf);

	printf("The captcha image saved at %s\nPlease input the verify code: ", filename);
	scanf("%4s", captcha);
	printf("\n");
	return PcsTrue;
}

static const char *my_get_cookie_file(const char *username)
{
	static char filename[1024] = { 0 };

#ifdef WIN32
	strcpy(filename, getenv("UserProfile"));
	strcat(filename, "\\.baidupcs");
	mkdir(filename);
	strcat(filename, "\\");
	if (!username || !username[0]) {
		strcat(filename, "default.cookie");
	}
	else {
		strcat(filename, username);
		strcat(filename, ".cookie");
	}
#else
	strcpy(filename, getenv("HOME"));
	strcat(filename, "/.baidupcs");
	mkdir(filename);
	strcat(filename, "/");
	if (!username || !username[0]) {
		strcat(filename, "default.cookie");
	}
	else {
		strcat(filename, username);
		strcat(filename, ".cookie");
	}
#endif

	return filename;
}

static void exec_meta(Pcs pcs)
{
	char str[32] = {0};
	PcsFileInfo *fi = pcs_meta(pcs, "/temp/temp2.txt");
	if (!fi) {
		printf("Get meta for \"temp/temp2.txt\" failed: %s\n", pcs_strerror(pcs, PCS_NONE));
		return;
	}

	printf("Path: %s%s\n", fi->path, fi->isdir ? "(dir)" : "");
	printf("Size: ");
	pcs_utils_readable_size((double)fi->size, str, 30, NULL);
	printf(str);
	putchar('\n');
	pcs_fileinfo_destroy(fi);
}

static void exec_search(Pcs pcs)
{
	PcsFileInfoList *fi,
		*list = pcs_search(pcs, "/", "temp", PcsTrue);
	if (!list) {
		printf("Search \"temp\" failed: %s\n", pcs_strerror(pcs, PCS_NONE));
		return;
	}

	fi = list;
	while(fi) {
		printf("%s\n", fi->info.path);
		fi = fi->next;
	}
	pcs_filist_destroy(list);
}

static void exec_list(Pcs pcs)
{
	PcsFileInfoList *fi,
		*list = pcs_list(pcs, "/", 1, 100, "name", PcsFalse);
	if (!list) {
		printf("List / failed: %s\n", pcs_strerror(pcs, PCS_NONE));
		return;
	}

	fi = list;
	while(fi) {
		printf("%s\n", fi->info.path);
		fi = fi->next;
	}
	pcs_filist_destroy(list);
}

static void show_quota(Pcs pcs)
{
	PcsRes pcsres;
	UInt64 quota, used;
	char str[32] = {0};
	pcsres = pcs_quota(pcs, &quota, &used);
	if (pcsres != PCS_OK) {
		printf("Get quota failed: %s\n", pcs_strerror(pcs, pcsres));
		return;
	}
	printf("Quota: ");
	pcs_utils_readable_size((double)used, str, 30, NULL);
	printf(str);
	putchar('/');
	pcs_utils_readable_size((double)quota, str, 30, NULL);
	printf(str);
	printf("\n");
}

int main(int argc, char *argv[])
{
	PcsRes pcsres;
	const char *cookie_file;
	Pcs pcs;
	struct params *params = main_args_create_params();

	if (!params) {
		printf("Create Param Object Failed\n");
		return 0;
	}

	main_args_parse(argc, argv, params);

	if (params->is_fail) {
		main_args_destroy_params(params);
		return 0;
	}

	if (params->cookie)
		cookie_file = params->cookie;
	else
		cookie_file = my_get_cookie_file(params->username);

	pcs = pcs_create(cookie_file);

	pcs_setopt(pcs, PCS_OPTION_CAPTCHA_FUNCTION, my_get_verify_code);

	if ((pcsres = pcs_islogin(pcs)) != PCS_LOGIN) {
		pcs_setopt(pcs, PCS_OPTION_USERNAME, params->username);
		if (!params->password) {
			char password[50];
			printf("Password: ");
			get_password_from_std_input(password, 50);
			pcs_setopt(pcs, PCS_OPTION_PASSWORD, password);
		}
		else {
			pcs_setopt(pcs, PCS_OPTION_PASSWORD, params->password);
		}
		if ((pcsres = pcs_login(pcs)) != PCS_OK) {
			printf("Login Failed: %s\n", pcs_strerror(pcs, pcsres));
			goto main_exit;
		}
	}
	else {
		printf("You have been logged in.\n");
	}
	printf("UID: %s\n", pcs_sysUID(pcs));


	//show_quota(pcs);
	//exec_list(pcs);
	//exec_search(pcs);
	exec_meta(pcs);

main_exit:
	pcs_destroy(pcs);
	pcs_mem_print_leak();
#if defined(WIN32) && defined(_DEBUG)
	system("pause");
#endif
	return 0;
}
