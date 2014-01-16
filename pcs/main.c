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

void cb_pcs_http_response(unsigned char *ptr, size_t size, void *state)
{
	printf("\n<<<\n");
	if (ptr) {
		//char ch = ptr[size - 1];
		//ptr[size - 1] = '\0';
		printf("%s\n", ptr);
		//ptr[size - 1] = ch;
	}
	printf(">>>\n\n");
}

static PcsBool cb_get_verify_code(unsigned char *ptr, size_t size, char *captcha, size_t captchaSize, void *state)
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
	get_string_from_std_input(captcha, captchaSize);
	//printf("\n");
	return PcsTrue;
}

static size_t cb_get_verify_code_byurlc_curl_write(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	char **html = (char **) userdata;
	char *p;
	size_t sz, ptrsz;

	ptrsz = size * nmemb;
	if (ptrsz == 0)
		return ptrsz;

	if (*html)
		sz = strlen(*html);
	else
		sz = 0;
	size = sz + ptrsz;
	p = (char *) pcs_malloc(size + 1);
	if (!p)
		return 0;
	if (*html) {
		memcpy(p, *html, sz);
		pcs_free(*html);
	}
	memcpy(&p[sz], ptr, ptrsz);
	p[size] = '\0';
	*html = p;
	return ptrsz;
}

static PcsBool cb_get_verify_code_byurlc(unsigned char *ptr, size_t size, char *captcha, size_t captchaSize, void *state)
{
	CURL *curl;
	CURLcode res;
	struct curl_httppost *formpost = 0;
    struct curl_httppost *lastptr  = 0;
	char *html = NULL;

	curl = curl_easy_init();
	if (!curl) {
		puts("Cannot init the libcurl.");
		return PcsFalse;
	}
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
#   define USAGE "Mozilla/5.0 (Windows NT 5.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/31.0.1650.57 Safari/537.36"
	curl_easy_setopt(curl, CURLOPT_USERAGENT, USAGE);
	/* tell libcurl to follow redirection */
	//res = curl_easy_setopt(pcs->curl, CURLOPT_COOKIEFILE, cookie_folder);
	//curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");
	
	curl_formadd(&formpost, &lastptr, 
		CURLFORM_PTRNAME, "photofile", 
		CURLFORM_BUFFER, "verify_code.gif",
		CURLFORM_BUFFERPTR, ptr, 
		CURLFORM_BUFFERLENGTH, (long)size,
		CURLFORM_END);
	curl_easy_setopt(curl, CURLOPT_URL, "http://urlc.cn/g/upload.php");
    curl_easy_setopt(curl, CURLOPT_POST, 1);
	//curl_easy_setopt(curl, CURLOPT_COOKIE, "");
	curl_easy_setopt(curl, CURLOPT_HEADER , 1);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &cb_get_verify_code_byurlc_curl_write);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &html);
	curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost); 
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_REFERER , "http://urlc.cn/g/");

	res = curl_easy_perform(curl);
	if(res != CURLE_OK) {
		printf("Cannot upload the image to http://urlc.cn/g/\n%s\n", curl_easy_strerror(res));
		if (html)
			pcs_free(html);
		curl_formfree(formpost);
		curl_easy_cleanup(curl);
		return PcsFalse;
	}
	curl_formfree(formpost);
	curl_easy_cleanup(curl);

	if (!html) {
		printf("Cannot get the response from http://urlc.cn/g/\n");
		return PcsFalse;
	}

	printf("\n%s\n\n", html);
	pcs_free(html);
	printf("You can access the verify code image from the url that can find from above html, please input the text in the image: ");
	get_string_from_std_input(captcha, captchaSize);
	return PcsTrue;
}

static const char *get_default_cookie_file(const char *username)
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
		cookie_file = get_default_cookie_file(params->username);

	pcs = pcs_create(cookie_file);

	if (params->use_urlc) {
		pcs_setopt(pcs, PCS_OPTION_CAPTCHA_FUNCTION, cb_get_verify_code_byurlc);
	}
	else {
		pcs_setopt(pcs, PCS_OPTION_CAPTCHA_FUNCTION, cb_get_verify_code);
	}
	if (params->is_verbose) {
		pcs_setopt(pcs, PCS_OPTION_HTTP_RESPONSE_FUNCTION, cb_pcs_http_response);
	}

	if ((pcsres = pcs_islogin(pcs)) != PCS_LOGIN) {
		if (!params->username) {
			printf("Your session is time out, please restart with -u option\n");
			goto main_exit;
		}
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
		if (params->username && strcmpi(pcs_sysUID(pcs), params->username) != 0) {
			char flag[8] = {0};
			printf("You have been logged in with %s, but you specified %s,\ncontinue?(yes|no): \n", pcs_sysUID(pcs), params->username);
			get_string_from_std_input(flag, 4);
			if (strcmpi(flag, "yes") && strcmpi(flag, "y")) {
				goto main_exit;
			}
		}
	}
	printf("UID: %s\n", pcs_sysUID(pcs));
	show_quota(pcs);

	//show_quota(pcs);
	//exec_list(pcs);
	//exec_search(pcs);
	exec_meta(pcs);

main_exit:
	pcs_destroy(pcs);
	main_args_destroy_params(params);
	pcs_mem_print_leak();
#if defined(WIN32) && defined(_DEBUG)
	system("pause");
#endif
	return 0;
}
