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

static void print_time(const char *format, UInt64 time)
{
	struct tm *tm;
	time_t t = time;
	char tmp[64];

	tm = localtime(&t);

	if (tm) {
		sprintf(tmp, "%d-%02d-%02d %02d:%02d:%02d", 
			1900 + tm->tm_year, 
			tm->tm_mon + 1, 
			tm->tm_mday,
			tm->tm_hour, 
			tm->tm_min,
			tm->tm_sec);
		printf(format, tmp);
	}
	else {
		printf(format, "0000-00-00 00:00:00");
	}
}

static void print_size(const char *format, UInt64 size)
{
	char tmp[64];
	tmp[63] = '\0';
	pcs_utils_readable_size(size, tmp, 63, NULL);
	printf(format, tmp);
}

static void print_fileinfo(PcsFileInfo *f, const char *prex)
{
	if(!prex) prex = "";
	printf("%sCategory:\t%d\n", prex, f->category);
	printf("%sPath:\t\t%s\n", prex, f->path);
	printf("%sFilename:\t%s\n", prex, f->server_filename);
	printf("%s", prex); 
	print_time("Create time:\t%s\n", f->server_ctime);
	printf("%s", prex); 
	print_time("Modify time:\t%s\n", f->server_mtime);
	printf("%sIs Dir:\t%s\n", prex, f->isdir ? "yes" : "no");
	if (f->isdir) {
		printf("%sEmpty Dir:\t%s\n", prex, f->dir_empty ? "yes" : "no");
		//printf("%sHas Sub Dir:\t%s\n", prex, f->ifhassubdir ? "no" : "yes");
	}
	else {
		printf("%s", prex); 
		print_size("Size:\t%s\n", f->size);
		printf("%smd5:\t%s\n", prex, f->md5);
		printf("%sdlink:\t%s\n", prex, f->dlink);
	}
}

static const char *size_tostr(UInt64 size, int *fix_width, char ch)
{
	static char str[128], *p;
	UInt64 i;
	int j, cn, mod;

	if (size == 0) {
		i = 0;
		if (*fix_width > 0) {
			for(; i < *fix_width - 1; i++) {
				str[i] = ch;
			}
		} 
		str[i] = '0';
		str[i + 1] = '\0';
		if (*fix_width < 0)
			*fix_width = 1;
		return str;
	}

	i = size;
	j = 127;
	str[j] = '\0';
	cn = 0;
	while (i != 0) {
		mod = i % 10;
		i = i / 10;
		str[--j] = (char)('0' + mod);
		cn++;
	}

	i = 0;
	if (*fix_width > 0) {
		for(; i < *fix_width - cn; i++) {
			str[i] = ch;
		}
	} 
	p = &str[j];
	while(*p){
		str[i++] = *p++;
	}
	str[i] = '\0';
	if (*fix_width < 0)
		*fix_width = i;
	return str;
}

static void print_filelist_head(int size_width)
{
	int i;
	putchar('D');
	putchar(' ');
	for(i = 0; i < size_width - 4; i++)
		putchar(' ');
	printf("Size");
	putchar(' ');
	printf("Modify Date Time");
	putchar(' ');
	putchar(' ');
	putchar(' ');
	putchar(' ');
	puts("File Name");
}

static void print_filelist_row(PcsFileInfo *f, int size_width)
{
	const char *p;

	if (f->isdir)
		putchar('d');
	else
		putchar('-');
	putchar(' ');

	p = size_tostr(f->size, &size_width, ' ');
	while (*p) {
		putchar(*p++);
	}
	putchar(' ');
	print_time("%s", f->server_mtime);
	putchar(' ');
	puts(f->path);
}

static void print_filelist(PcsFileInfoList *list)
{
	char tmp[64] = {0};
	int cnt_file = 0,
		cnt_dir = 0,
		size_width = 1,
		w;
	PcsFileInfo *file = NULL;
	size_t total = 0;
	PcsFileInfoListIterater iterater;

	pcs_filist_iterater_init(list, &iterater);
	while(pcs_filist_iterater_next(&iterater)) {
		file = iterater.current;
		w = -1;
		size_tostr(file->size, &w, ' ');
		if (size_width < w)
			size_width = w;
		total += file->size;
		if (file->isdir)
			cnt_dir++;
		else
			cnt_file++;
	}

	if (size_width < 4)
		size_width = 4;
	putchar('\n');
	print_filelist_head(size_width);
	puts("------------------------------------------------------------------------------");
	pcs_filist_iterater_init(list, &iterater);
	while(pcs_filist_iterater_next(&iterater)) {
		file = iterater.current;
		print_filelist_row(file, size_width);
	}
	puts("------------------------------------------------------------------------------");
	pcs_utils_readable_size(total, tmp, 63, NULL);
	tmp[63] = '\0';
	printf("Total: %s, File Count: %d, Directory Count: %d\n", tmp, cnt_file, cnt_dir);
	putchar('\n');
}

static void print_pcs_pan_api_res(PcsPanApiRes *res)
{
	int cnt_suc = 0, cnt_fail = 0;
	PcsPanApiResInfoList *info;
	
	info = res->info_list;
	while(info) {
		if (info->info.error) {
			printf("[FAIL] %s\n", info->info.path);
			cnt_fail++;
		}
		else {
			printf("[SUCC] %s\n", info->info.path);
			cnt_suc++;
		}
		info = info->next;
	}
	printf("\n%d Success, %d Failed.\n", cnt_suc, cnt_fail);
}

static PcsFileInfoList *get_file_list(Pcs pcs, const char *dir, const char *sort, PcsBool desc, PcsBool recursion)
{
	PcsFileInfoList *reslist = NULL,
		*list = NULL,
		*sublist = NULL,
		*item = NULL;
	PcsFileInfo *info = NULL;
	int page_index = 1,
		page_size = 100;
	int cnt = 0, cnt_total = 0;
	while(1) {
		printf("List %s, Got %d Files           \r", dir, cnt_total);
		fflush(stdout);
		list = pcs_list(pcs, dir, 
			page_index, page_size,
			sort ? sort : "name", 
			sort ? desc : PcsFalse);
		if (!list) {
			if (pcs_strerror(pcs, PCS_NONE))
				printf("Cannot list %s: %s\n", dir, pcs_strerror(pcs, PCS_NONE));
			return NULL;
		}
		cnt = list->count;
		cnt_total += cnt;
		if (recursion) {
			PcsFileInfoListItem *listitem;
			PcsFileInfoListIterater iterater;
			reslist = pcs_filist_create();
			if (!reslist) {
				printf("Cannot create the PcsFileInfoList object for %s.\n", dir);
				pcs_filist_destroy(list);
				return NULL;
			}
			pcs_filist_iterater_init(list, &iterater);
			while(pcs_filist_iterater_next(&iterater)) {
				info = iterater.current;
				listitem = iterater.cursor;
				pcs_filist_remove(list, listitem, &iterater);
				pcs_filist_add(reslist, listitem);
				if (info->isdir) {
					sublist = get_file_list(pcs, info->path, sort, desc, recursion);
					if (sublist) {
						pcs_filist_combin(reslist, sublist);
						pcs_filist_destroy(sublist);
					}
				}
				else {
					/*
					 * 因为百度网盘返回的都是文件夹在前，文件在后，
					 * 因此遇到第一个文件，则后边的都是文件。
					 * 所以可以跳过检查，直接合并
					*/
					pcs_filist_combin(reslist, list);
					break;
				}
			}
			pcs_filist_destroy(list);
		}
		else {
			reslist = list;
		}
		if (cnt > 0) {
			printf("List %s, Got %d Files           \r", dir, cnt_total);
			fflush(stdout);
		}
		if (cnt < page_size) {
			break;
		}
		page_index++;
	}
	return reslist;
}

static void exec_meta(Pcs pcs, struct params *params)
{
	char str[32] = {0};
	PcsFileInfo *fi;
	printf("\nGet meta %s\n", params->args[0]);
	fi = pcs_meta(pcs, params->args[0]);
	if (!fi) {
		printf("Failed: %s\n", pcs_strerror(pcs, PCS_NONE));
		return;
	}
	print_fileinfo(fi, " ");
	pcs_fileinfo_destroy(fi);
}

static void exec_list(Pcs pcs, struct params *params)
{
	PcsFileInfoList *list;
	printf("\n%sList %s\n", params->is_recursion ? "Recursive " : "", params->args[0]);
	list = get_file_list(pcs, params->args[0], params->sort, params->is_desc, params->is_recursion);
	if (list) {
		printf("Got %d Files\n", list->count);
		fflush(stdout);
		print_filelist(list);
		pcs_filist_destroy(list);
	}
	else {
		printf("Got 0 Files\n");
		fflush(stdout);
		print_filelist_head(4);
	}
}

static void exec_rename(Pcs pcs, struct params *params)
{
	PcsPanApiRes *res;
	PcsSList2 slist;
	slist.string1 = params->args[0]; /* path */
	slist.string2 = params->args[1]; /* new_name */
	slist.next = NULL;
	printf("\nRename %s to %s\n", slist.string1, slist.string1);
	res = pcs_rename(pcs, &slist);
	if (!res) {
		printf("Rename failed: %s\n", pcs_strerror(pcs, PCS_NONE));
		return;
	}
	putchar('\n');
	print_pcs_pan_api_res(res);
	pcs_pan_api_res_destroy(res);
}

static void exec_move(Pcs pcs, struct params *params)
{
	PcsPanApiRes *res;
	PcsSList2 slist;
	slist.string1 = params->args[0]; /* path */
	slist.string2 = params->args[1]; /* new_name */
	slist.next = NULL;

	printf("\nMove %s to %s\n", slist.string1, slist.string2);
	res = pcs_move(pcs, &slist);
	if (!res) {
		printf("Move failed: %s\n", pcs_strerror(pcs, PCS_NONE));
		return;
	}
	putchar('\n');
	print_pcs_pan_api_res(res);
	pcs_pan_api_res_destroy(res);
}

static void exec_copy(Pcs pcs, struct params *params)
{
	PcsPanApiRes *res;
	PcsSList2 slist;
	slist.string1 = params->args[0]; /* path */
	slist.string2 = params->args[1]; /* new_name */
	slist.next = NULL;

	printf("\nCopy %s to %s\n", slist.string1, slist.string2);
	res = pcs_copy(pcs, &slist);
	if (!res) {
		printf("Copy failed: %s\n", pcs_strerror(pcs, PCS_NONE));
		return;
	}
	putchar('\n');
	print_pcs_pan_api_res(res);
	pcs_pan_api_res_destroy(res);
}

static void exec_mkdir(Pcs pcs, struct params *params)
{
	PcsRes res;

	printf("\nCreate folder %s\n", params->args[0]);
	res = pcs_mkdir(pcs, params->args[0]);
	if (res != PCS_OK) {
		printf("Create folder failed: %s\n", pcs_strerror(pcs, res));
		return;
	}
	printf("Create %s success.\n", params->args[0]);
}

static void exec_delete(Pcs pcs, struct params *params)
{
	PcsPanApiRes *res;
	PcsSList *slist = NULL;
	int i;

	printf("\nDelete");
	for (i = 0; i < params->args_count; i++) {
		printf(" %s", params->args[i]);
		if (!slist) {
			slist = pcs_slist_create_ex(params->args[i], -1);
			if (!slist) {
				printf("Cannot create slist\n");
				return;
			}
		}
		else if (!pcs_slist_add_ex(slist, params->args[i], -1)) {
			printf("\nCannot create slist\n");
			pcs_slist_destroy(slist);
			return;
		}
	}
	putchar('\n');

	res = pcs_delete(pcs, slist);
	pcs_slist_destroy(slist);
	if (!res) {
		printf("Delete failed. %s\n", pcs_strerror(pcs, PCS_NONE));
		return;
	}
	putchar('\n');
	print_pcs_pan_api_res(res);
	pcs_pan_api_res_destroy(res);
}

static void exec_cat(Pcs pcs, struct params *params)
{
	const char *res;

	printf("\nCat %s\n", params->args[0]);
	res = pcs_cat(pcs, params->args[0]);
	if (res == NULL) {
		printf("Cat failed: %s\n", pcs_strerror(pcs, PCS_NONE));
		return;
	}
	printf(">>>\n%s\n<<<\n", res);
}

static void exec_search(Pcs pcs, struct params *params)
{
	PcsFileInfoList *list;
	printf("\n%sSearch %s in %s\n", params->is_recursion ? "Recursive " : "", params->args[1], params->args[0]);
	list = pcs_search(pcs, params->args[0], params->args[1], params->is_recursion);
	putchar('\n');
	if (list) {
		print_filelist(list);
		pcs_filist_destroy(list);
	}
	else {
		print_filelist_head(4);
	}
}

static void exec_cmd(Pcs pcs, struct params *params)
{
	switch (params->action)
	{
	case ACTION_QUOTA:
		break;
	case ACTION_META:
		exec_meta(pcs, params);
		break;
	case ACTION_LIST:
		exec_list(pcs, params);
		break;
	case ACTION_RENAME:
		exec_rename(pcs, params);
		break;
	case ACTION_MOVE:
		exec_move(pcs, params);
		break;
	case ACTION_COPY:
		exec_copy(pcs, params);
		break;
	case ACTION_MKDIR:
		exec_mkdir(pcs, params);
		break;
	case ACTION_DELETE:
		exec_delete(pcs, params);
		break;
	case ACTION_CAT:
		exec_cat(pcs, params);
		break;
	case ACTION_ECHO:
		//exec_echo(pcs, params);
		break;
	case ACTION_SEARCH:
		//exec_search(pcs, params);
		break;
	case ACTION_DOWNLOAD:
		//exec_download(pcs, params);
		break;
	case ACTION_UPLOAD:
		//exec_upload(pcs, params);
		break;
	default:
		printf("Unknown command, use `--help` to view help.\n");
		break;
	}
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
	exec_cmd(pcs, params);

main_exit:
	pcs_destroy(pcs);
	main_args_destroy_params(params);
	pcs_mem_print_leak();
#if defined(WIN32) && defined(_DEBUG)
	system("pause");
#endif
	return 0;
}
