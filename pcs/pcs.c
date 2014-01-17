#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#ifdef WIN32
# include <malloc.h>
#else
# include <alloca.h>
#endif

#include "pcs_defs.h"
#include "pcs_mem.h"
#include "pcs_utils.h"
#include "pcs_http.h"
#include "cJSON.h"
#include "pcs.h"

#define URL_HOME			"http://www.baidu.com"
#define URL_DISK_HOME		"http://pan.baidu.com/disk/home"
#define URL_PASSPORT_API	"https://passport.baidu.com/v2/api/?"
#define URL_CAPTCHA			"https://passport.baidu.com/cgi-bin/genimage?"
#define URL_PAN_API			"http://pan.baidu.com/api/"
#define URL_PCS_REST		"http://c.pcs.baidu.com/rest/2.0/pcs/file"

struct pcs {
	char		*username;
	char		*password;
	char		*bdstoken;
	char		*bduss;
	char		*sysUID;
	char		*errmsg;

	PcsGetCaptchaFunction		captcha_func;
	void						*captcha_data;

	PcsHttp		http;

};

inline void pcs_clear_error(Pcs handle)
{
	struct pcs *pcs = (struct pcs *)handle;
	if (pcs->errmsg) {
		pcs_free(pcs->errmsg);
		pcs->errmsg = NULL;
	}
}

inline void pcs_set_error(Pcs handle, const char *errmsg)
{
	struct pcs *pcs = (struct pcs *)handle;
	if (pcs->errmsg) {
		pcs_free(pcs->errmsg);
		pcs->errmsg = NULL;
	}
	pcs->errmsg = pcs_utils_strdup(errmsg);
}

inline void pcs_cat_error(Pcs handle, const char *errmsg)
{
	struct pcs *pcs = (struct pcs *)handle;
	char *p;
	size_t sz = 0;
	if (pcs->errmsg) {
		sz = strlen(pcs->errmsg) + strlen(errmsg);
		p = (char *)pcs_malloc(sz + 1);
		if (!p) return;
		strcpy(p, pcs->errmsg);
		pcs_free(pcs->errmsg);
		pcs->errmsg = p;
		strcat(p, errmsg);
	}
	else {
		pcs->errmsg = pcs_utils_strdup(errmsg);
	}
}

inline void pcs_set_errmsg(Pcs handle, PcsRes error)
{
	if (error != PCS_NONE)
		pcs_set_error(handle, pcs_strerror(handle, error));
}

inline PcsRes pcs_get_captcha(Pcs handle, const char *code_string, char *captcha, int captchaSize)
{
	struct pcs *pcs = (struct pcs *)handle;
	char *url, *img;
	size_t imgsz;

	captcha[0] = '\0';
	if (!pcs->captcha_func) {
		return PCS_NO_CAPTCHA_FUNC;
	}
	url = pcs_utils_sprintf(URL_CAPTCHA "%s", code_string);
	img = pcs_http_get_raw(pcs->http, url, PcsTrue, &imgsz); pcs_free(url);
	if (!img)
		return PCS_NETWORK_ERROR;
	if (!(*pcs->captcha_func)((unsigned char *)img, imgsz, captcha, captchaSize, pcs->captcha_data)) {
		return PCS_GET_CAPTCHA_FAIL;
	}
	return PCS_OK;
}

static char *pcs_build_pan_api_url_v(Pcs handle, const char *action, va_list args)
{
	struct pcs *pcs = (struct pcs *)handle;
	char *baseurl, *url, *tt;
	tt = pcs_utils_sprintf("%d", (int)time(0));
	baseurl = (char *)alloca(strlen(URL_PAN_API) + strlen(action) + 1);
	strcpy(baseurl, URL_PAN_API);
	strcat(baseurl, action);
	baseurl = pcs_http_build_url(pcs->http, baseurl,
		"channel", "chunlei",
		"clienttype", "0",
		"web","1", 
		"t", tt,
		"bdstoken", pcs->bdstoken,
		NULL);
	pcs_free(tt);
	if (!baseurl)
		return NULL;
	url = pcs_http_build_url_v(pcs->http, baseurl, args);
	pcs_free(baseurl);
	return url;
}

static char *pcs_build_pan_api_url(Pcs handle, const char *action, ...)
{
	char *url;
	va_list args;
    va_start(args, action);
	url = pcs_build_pan_api_url_v(handle, action, args);
    va_end(args);
	return url;
}

static PcsFileInfo *pcs_parse_fileinfo(cJSON * item)
{
	cJSON *val, *list;
	PcsFileInfo *fi = pcs_fileinfo_create();
	if (!fi) {
		return NULL;
	}
	val = cJSON_GetObjectItem(item, "fs_id");
	if ((val = cJSON_GetObjectItem(item, "fs_id")))
		fi->fs_id = (UInt64)val->valuedouble;

	val = cJSON_GetObjectItem(item, "path");
	if (val)
		fi->path = pcs_utils_strdup_utf8(val->valuestring);

	val = cJSON_GetObjectItem(item, "server_filename");
	if (val)
		fi->server_filename = pcs_utils_strdup_utf8(val->valuestring);

	val = cJSON_GetObjectItem(item, "mtime");
	if (val)
		fi->server_mtime = (UInt64)val->valuedouble;

	val = cJSON_GetObjectItem(item, "ctime");
	if (val)
		fi->server_ctime = (UInt64)val->valuedouble;

	val = cJSON_GetObjectItem(item, "server_mtime");
	if (val)
		fi->server_mtime = (UInt64)val->valuedouble;

	val = cJSON_GetObjectItem(item, "server_ctime");
	if (val)
		fi->server_ctime = (UInt64)val->valuedouble;

	val = cJSON_GetObjectItem(item, "local_mtime");
	if (val)
		fi->local_mtime = (UInt64)val->valuedouble;

	val = cJSON_GetObjectItem(item, "local_ctime");
	if (val)
		fi->local_ctime = (UInt64)val->valuedouble;

	val = cJSON_GetObjectItem(item, "isdir");
	if (val)
		fi->isdir = val->valueint ? PcsTrue : PcsFalse;

	val = cJSON_GetObjectItem(item, "category");
	if (val)
		fi->category = val->valueint;

	val = cJSON_GetObjectItem(item, "size");
	if (val)
		fi->size = (UInt64)val->valuedouble;

	val = cJSON_GetObjectItem(item, "dir_empty");
	if (val)
		fi->dir_empty = val->valueint ? PcsTrue : PcsFalse;

	val = cJSON_GetObjectItem(item, "empty");
	if (val)
		fi->empty = val->valueint ? PcsTrue : PcsFalse;

	val = cJSON_GetObjectItem(item, "ifhassubdir");
	if (val)
		fi->ifhassubdir = val->valueint ? PcsTrue : PcsFalse;

	val = cJSON_GetObjectItem(item, "md5");
	if (val)
		fi->md5 = pcs_utils_strdup_utf8(val->valuestring);

	val = cJSON_GetObjectItem(item, "dlink");
	if (val)
		fi->dlink = pcs_utils_strdup_utf8(val->valuestring);

	list = cJSON_GetObjectItem(item, "block_list");
	if (list) {
		int i, cnt = cJSON_GetArraySize(list);
		if (cnt > 0) {
			fi->block_list = (char **) pcs_malloc((cnt + 1) + sizeof(char *));
			if (!fi->block_list) return fi;
			memset(fi->block_list, 0, (cnt + 1) + sizeof(char *));
			for (i = 0; i < cnt; i++) {
				val = cJSON_GetArrayItem(list, i);
				fi->block_list[i] = pcs_utils_strdup_utf8(val->valuestring);
			}
		}
	}
	return fi;
}

static PcsFileInfoList *pcs_pan_api_1(Pcs handle, const char *action, ...)
{
	struct pcs *pcs = (struct pcs *)handle;
	va_list args;
	cJSON *json, *item, *list;
	char *url, *html;
	int error, cnt, i;
	PcsFileInfoList *filist = NULL;
	PcsFileInfoListItem *filist_item;
	PcsFileInfo *fi;

    va_start(args, action);
	url = pcs_build_pan_api_url_v(handle, action, args);
    va_end(args);
	if (!url) {
		pcs_set_errmsg(handle, PCS_BUILD_URL);
		return NULL;
	}
	html = pcs_http_get(pcs->http, url, PcsTrue);
	pcs_free(url);
	if (!html) {
		pcs_set_errmsg(handle, PCS_NETWORK_ERROR);
		return NULL;
	}
	json = cJSON_Parse(html);
	if (!json){
		printf("%s\n", html);
		pcs_set_errmsg(handle, PCS_WRONG_RESPONSE);
		return NULL;
	}
	item = cJSON_GetObjectItem(json, "errno");
	if (!item) {
		//printf("%s\n", html);
		pcs_set_errmsg(handle, PCS_WRONG_RESPONSE);
		cJSON_Delete(json);
		return NULL;
	}
	error = item->valueint;
	if (error != 0) {
		//printf("%s\n", html);
		pcs_set_errmsg(handle, PCS_FAIL);
		cJSON_Delete(json);
		return NULL;
	}
	list = cJSON_GetObjectItem(json, "list");
	if (!list) {
		//pcs_set_errmsg(handle, PCS_NO_LIST);
		cJSON_Delete(json);
		return NULL;
	}
	cnt = cJSON_GetArraySize(list);
	if (cnt <= 0) {
		//pcs_set_errmsg(handle, PCS_NO_LIST);
		cJSON_Delete(json);
		return NULL;
	}
	filist = pcs_filist_create();
	if (!filist) {
		pcs_set_errmsg(handle, PCS_CREATE_OBJ);
		cJSON_Delete(json);
		return NULL;
	}
	for (i = 0; i < cnt; i++) {
		item = cJSON_GetArrayItem(list, i);
		filist_item = pcs_filistitem_create();
		if (!filist_item) {
			pcs_set_errmsg(handle, PCS_CREATE_OBJ);
			cJSON_Delete(json);
			if (filist) pcs_filist_destroy(filist);
			return NULL;
		}
		fi = pcs_parse_fileinfo(item);
		if (!fi) {
			pcs_set_errmsg(handle, PCS_CREATE_OBJ);
			cJSON_Delete(json);
			pcs_filistitem_destroy(filist_item);
			if (filist) pcs_filist_destroy(filist);
			return NULL;
		}
		filist_item->info = fi;
		pcs_filist_add(filist, filist_item);
	}
	cJSON_Delete(json);
	return filist;
}

static char *pcs_build_filelist_1(Pcs handle, PcsSList *slist)
{
	PcsSList *item;
	char *file_list_string;
	size_t sz;

	if (!slist)
		return NULL;

	sz = 0;
	item = slist;
	while(item) {
		sz += strlen(item->string);
		sz += 3;
		item = item->next;
	}
	sz += 2;
	
	file_list_string = (char *) pcs_malloc(sz);
	if (!file_list_string) {
		pcs_set_errmsg(handle, PCS_ALLOC_MEMORY);
		return NULL;
	}

	strcpy(file_list_string, "[");
	item = slist;
	while(item) {
		strcat(file_list_string, "\"");
		strcat(file_list_string, item->string);
		strcat(file_list_string, "\",");
		item = item->next;
	}
	file_list_string[sz - 2] = ']';
	file_list_string[sz - 1] = '\0';
	return file_list_string;
}

static char *pcs_build_filelist_2(Pcs handle, PcsSList2 *slist)
{
	PcsSList2 *item;
	char *file_list_string, *p;
	size_t sz;

	if (!slist)
		return NULL;

	sz = 0;
	item = slist;
	while(item) {
		sz += strlen(item->string1);
		sz += strlen(item->string2);
		sz += 25;
		item = item->next;
	}
	sz += 2;
	
	file_list_string = (char *) pcs_malloc(sz);
	if (!file_list_string) {
		pcs_set_errmsg(handle, PCS_ALLOC_MEMORY);
		return NULL;
	}

	strcpy(file_list_string, "[");
	p = file_list_string;
	item = slist;
	while(item) {
		while(*p) p++;
		sprintf(p, "{\"path\":\"%s\",\"newname\":\"%s\"},", item->string1, item->string2);
		p += 25;
		item = item->next;
	}
	file_list_string[sz - 2] = ']';
	file_list_string[sz - 1] = '\0';

	return file_list_string;
}

static char *pcs_build_filelist_3(Pcs handle, PcsSList2 *slist)
{
	PcsSList2 *item;
	char *file_list_string, *p, *dir, *filename;
	size_t sz;

	if (!slist)
		return NULL;

	sz = 0;
	item = slist;
	while(item) {
		sz += strlen(item->string1);
		sz += strlen(item->string2);
		sz += 35;
		item = item->next;
	}
	sz += 2;
	
	file_list_string = (char *) pcs_malloc(sz);
	if (!file_list_string) {
		pcs_set_errmsg(handle, PCS_ALLOC_MEMORY);
		return NULL;
	}
	memset(file_list_string, 0, sz);
	strcpy(file_list_string, "[");
	p = file_list_string;
	item = slist;
	while(item) {
		while(*p) p++;
		dir = pcs_utils_basedir(item->string2);
		filename = pcs_utils_filename(item->string2);
		if (!dir || !filename) {
			pcs_free(file_list_string);
			pcs_set_errmsg(handle, PCS_ALLOC_MEMORY);
			return NULL;
		}
		sprintf(p, "{\"path\":\"%s\",\"dest\":\"%s\",\"newname\":\"%s\"},", 
			item->string1, dir, filename);
		pcs_free(dir);
		pcs_free(filename);
		p += 35;
		item = item->next;
	}
	p = file_list_string + sz - 1;
	while (p > file_list_string) {
		if (*p == ',')
			break;
		p--;
	}
	if (*p == ',') {
		*p = ']';
		p++;
		*p = '\0';
	}

	return file_list_string;
}

static PcsPanApiRes *pcs_pan_api_filemanager(Pcs handle, const char *opera, const char *filelist)
{
	struct pcs *pcs = (struct pcs *)handle;
	cJSON *json, *item, *list, *val;
	char *url, *html, *postdata;
	int error, cnt, i;
	PcsPanApiRes *res = NULL;
	PcsPanApiResInfoList *tail, *ri;

	url = pcs_build_pan_api_url(handle, "filemanager",
		"opera", opera,
		NULL);
	if (!url) {
		pcs_set_errmsg(handle, PCS_BUILD_URL);
		return NULL;
	}

	postdata = pcs_http_build_post_data(pcs->http, "filelist", filelist, NULL);
	if (!postdata) {
		pcs_set_errmsg(handle, PCS_BUILD_POST_DATA);
		pcs_free(url);
		return NULL;
	}
	html = pcs_http_post(pcs->http, url, postdata, PcsTrue);
	pcs_free(url);
	pcs_free(postdata);
	if (!html) {
		pcs_set_errmsg(handle, PCS_NETWORK_ERROR);
		return NULL;
	}
	json = cJSON_Parse(html);
	if (!json){
		pcs_set_errmsg(handle, PCS_WRONG_RESPONSE);
		return NULL;
	}
	item = cJSON_GetObjectItem(json, "errno");
	if (!item) {
		pcs_set_errmsg(handle, PCS_WRONG_RESPONSE);
		cJSON_Delete(json);
		return NULL;
	}
	error = item->valueint;
	/*if (error != 0) {
		pcs_set_errmsg(handle, PCS_FAIL);
		cJSON_Delete(json);
		return NULL;
	}*/
	list = cJSON_GetObjectItem(json, "info");
	if (!list) {
		pcs_set_errmsg(handle, PCS_NO_LIST);
		cJSON_Delete(json);
		return NULL;
	}
	cnt = cJSON_GetArraySize(list);
	/*if (cnt < 0) {
		pcs_set_errmsg(handle, PCS_NO_LIST);
		cJSON_Delete(json);
		return NULL;
	}*/
	res = pcs_pan_api_res_create();
	if (!res) {
		pcs_set_errmsg(handle, PCS_ALLOC_MEMORY);
		cJSON_Delete(json);
		return NULL;
	}
	res->error = error;
	for (i = 0; i < cnt; i++) {
		item = cJSON_GetArrayItem(list, i);
		ri = pcs_pan_api_res_infolist_create();
		if (!ri) {
			pcs_set_errmsg(handle, PCS_CREATE_OBJ);
			cJSON_Delete(json);
			if (res) pcs_pan_api_res_destroy(res);
			return NULL;
		}
		val = cJSON_GetObjectItem(item, "path");
		if (val)
			ri->info.path = pcs_utils_strdup_utf8(val->valuestring);
		val = cJSON_GetObjectItem(item, "errno");
		ri->info.error = val->valueint;
		if (!res->info_list) {
			res->info_list = tail = ri;
		}
		else {
			tail->next = ri;
			tail = ri;
		}
	}
	cJSON_Delete(json);
	return res;
}

static int pcs_get_errno_from_api_res(Pcs handle, const char *html)
{
	int res;
	cJSON *json, *item;
	
	json = cJSON_Parse(html);
	if (!json) {
		pcs_set_errmsg(handle, PCS_WRONG_RESPONSE);
		return 1;
	}

	item = cJSON_GetObjectItem(json, "errno");
	if (!item) {
		pcs_set_errmsg(handle, PCS_WRONG_RESPONSE);
		cJSON_Delete(json);
		return 1;
	}
	res = item->valueint;
	cJSON_Delete(json);
	return res;
}

static PcsFileInfo *pcs_upload_form(Pcs handle, const char *path, PcsBool overwrite, PcsHttpForm form)
{
	struct pcs *pcs = (struct pcs *)handle;
	char *url, *html,
		*dir = pcs_utils_basedir(path),
		*filename = pcs_utils_filename(path);
	cJSON *json;
	PcsFileInfo *meta;

	url = pcs_http_build_url(pcs->http, URL_PCS_REST,
		"method", "upload",
		"app_id", "250528",
		"ondup", overwrite ? "overwrite" : "newcopy",
		"dir", dir,
		"filename", filename,
		"BDUSS", pcs->bduss, 
		NULL);
	pcs_free(dir);
	pcs_free(filename);
	if (!url) {
		pcs_set_errmsg(handle, PCS_BUILD_URL);
		return NULL;
	}
	html = pcs_post_httpform(pcs->http, url, form, PcsTrue);
	pcs_free(url);
	if (!html) {
		pcs_set_errmsg(handle, PCS_NETWORK_ERROR);
		return NULL;
	}
	json = cJSON_Parse(html);
	if (!json) {
		pcs_set_errmsg(handle, PCS_WRONG_RESPONSE);
		return NULL;
	}

	meta = pcs_parse_fileinfo(json);
	cJSON_Delete(json);
	if (!meta) {
		pcs_set_errmsg(handle, PCS_ALLOC_MEMORY);
		return NULL;
	}
	return meta;
}

PCS_API Pcs pcs_create(const char *cookie_file)
{
	struct pcs *pcs;

	pcs = (struct pcs *) pcs_malloc(sizeof(struct pcs));
	if (!pcs)
		return NULL;
	memset(pcs, 0, sizeof(struct pcs));
	pcs->http = pcs_http_create(cookie_file);
	if (!pcs->http) {
		pcs_free(pcs);
		return NULL;
	}
	return pcs;
}

PCS_API void pcs_destroy(Pcs handle)
{
	struct pcs *pcs = (struct pcs *)handle;
	if (pcs->http)
		pcs_http_destroy(pcs->http);
	if (pcs->username)
		pcs_free(pcs->username);
	if (pcs->password)
		pcs_free(pcs->password);
	if (pcs->bdstoken)
		pcs_free(pcs->bdstoken);
	if (pcs->bduss)
		pcs_free(pcs->bduss);
	if (pcs->sysUID)
		pcs_free(pcs->sysUID);
	if (pcs->errmsg)
		pcs_free(pcs->errmsg);
	pcs_free(pcs);
}

PCS_API const char *pcs_sysUID(Pcs handle)
{
	struct pcs *pcs = (struct pcs *)handle;
	return pcs->sysUID;
}

PCS_API const char *pcs_strerror(Pcs handle, PcsRes error)
{
	struct pcs *pcs = (struct pcs *)handle;
	switch (error)
	{
	case PCS_NONE:
		return pcs->errmsg;
	case PCS_OK:
		return "Ok";
	case PCS_FAIL:
		return "Server Response Error";
	case PCS_LOGIN:
		return "Logon";
	case PCS_NOT_LOGIN:
		return "NOT LOGIN";
	case PCS_UNKNOWN_OPT:
		return "Unknown Option";
	case PCS_NO_BDSTOKEN:
		return "No bdstoken";
	case PCS_NETWORK_ERROR:
		return "Network Error";
	case PCS_WRONG_RESPONSE:
		return "Wrong Response";
	case PCS_NO_CAPTCHA_FUNC:
		return "No Captcha Function";
	case PCS_GET_CAPTCHA_FAIL:
		return "Get Captcha Failed";
	case PCS_ALLOC_MEMORY:
		return "Alloc Memory Failed";
	case PCS_BUILD_POST_DATA:
		return "Build Post Data Failed";
	case PCS_BUILD_URL:
		return "Build Url Failed";
	case PCS_NO_LIST:
		return "Empty List";
	case PCS_CREATE_OBJ:
		return "Create Object Failed";
	case PCS_NOT_EXIST:
		return "Not Exist";
	case PCS_CLONE_OBJ:
		return "Clone Object Failed";
	case PCS_WRONG_ARGS:
		return "Wrong Arguments";
	}
	return "Unknown Error";
}

PCS_API PcsRes pcs_setopt(Pcs handle, PcsOption opt, void *value)
{
	PcsRes res = PCS_OK;
	struct pcs *pcs = (struct pcs *)handle;
	switch (opt)
	{
	case PCS_OPTION_USERNAME:
		if (pcs->username) pcs_free(pcs->username);
		pcs->username = pcs_utils_strdup((char *)value);
		break;
	case PCS_OPTION_PASSWORD:
		if (pcs->password) pcs_free(pcs->password);
		pcs->password = pcs_utils_strdup((char *)value);
		break;
	case PCS_OPTION_CAPTCHA_FUNCTION:
		pcs->captcha_func = (PcsGetCaptchaFunction)value;
		break;
	case PCS_OPTION_CAPTCHA_FUNCTION_DATA:
		pcs->captcha_data = value;
		break;
	case PCS_OPTION_DOWNLOAD_WRITE_FUNCTION:
		pcs_http_setopt(pcs->http, PCS_HTTP_OPTION_HTTP_WRITE_FUNCTION, value);
		break;
	case PCS_OPTION_DOWNLOAD_WRITE_FUNCTION_DATA:
		pcs_http_setopt(pcs->http, PCS_HTTP_OPTION_HTTP_WRITE_FUNCTION_DATE, value);
		break;
	case PCS_OPTION_HTTP_RESPONSE_FUNCTION:
		pcs_http_setopt(pcs->http, PCS_HTTP_OPTION_HTTP_RESPONSE_FUNCTION, value);
		break;
	case PCS_OPTION_HTTP_RESPONSE_FUNCTION_DATE:
		pcs_http_setopt(pcs->http, PCS_HTTP_OPTION_HTTP_RESPONSE_FUNCTION_DATE, value);
		break;
	case PCS_OPTION_PROGRESS_FUNCTION:
		pcs_http_setopt(pcs->http, PCS_HTTP_OPTION_PROGRESS_FUNCTION, value);
		break;
	case PCS_OPTION_PROGRESS_FUNCTION_DATE:
		pcs_http_setopt(pcs->http, PCS_HTTP_OPTION_PROGRESS_FUNCTION_DATE, value);
		break;
	case PCS_OPTION_PROGRESS:
		pcs_http_setopt(pcs->http, PCS_HTTP_OPTION_PROGRESS, value);
		break;
	default:
		res = PCS_UNKNOWN_OPT;
		break;
	}
	return res;
}

PCS_API PcsRes pcs_setopts(Pcs handle, ...)
{
	PcsRes res = PCS_OK;
	PcsOption opt;
	void *val;
    va_list args;
    va_start(args, handle);
	while((opt = va_arg(args, PcsOption)) != PCS_OPTION_END) {
		val = va_arg(args, void *);
		if ((res = pcs_setopt(handle, opt, val)) != PCS_OK) {
			break;
		}
	}
    va_end(args);
	return res;
}

PCS_API PcsRes pcs_islogin(Pcs handle)
{
	struct pcs *pcs = (struct pcs *)handle;
	const char *html;
	PcsSList *slist;
	int indies[2] = { 1, -1 };

	pcs_clear_error(handle);
	html = pcs_http_get(pcs->http, URL_DISK_HOME, PcsTrue);
	if (!html)
		return PCS_NETWORK_ERROR;
	
	if (!pcs_regex(html, -1, "bdstoken=\"([^\"]+)\"", indies, &slist)) {
		return PCS_NOT_LOGIN;
	}
	if (pcs->bdstoken)
		pcs_free(pcs->bdstoken);
	pcs->bdstoken = pcs_utils_strdup(slist->string);
	pcs_slist_destroy(slist);
	if (strlen(pcs->bdstoken) > 0) {
		if (pcs->bduss)
			pcs_free(pcs->bduss);
		pcs->bduss = pcs_http_get_cookie(pcs->http, "BDUSS");
		if (!pcs_regex(html, -1, "FileUtils\\.sysUID=\"([^\"]+)\"", indies, &slist)) {
			return PCS_NOT_LOGIN;
		}
		if (pcs->sysUID)
			pcs_free(pcs->sysUID);
		pcs->sysUID = pcs_utils_strdup(slist->string);
		pcs_slist_destroy(slist);
		return PCS_LOGIN;
	}
	return PCS_NOT_LOGIN;
}

PCS_API PcsRes pcs_login(Pcs handle)
{
	struct pcs *pcs = (struct pcs *)handle;
	PcsRes res;
	char *p, *html, *url, *token, *code_string, captch[8], *post_data, *tt;
	cJSON *json, *root, *item;
	int error = -1, i;
	PcsSList *slist;
	int indies[2] = { 1, -1 };

	pcs_clear_error(handle);
	html = pcs_http_get(pcs->http, URL_HOME, PcsTrue);
	if (!html)
		return PCS_NETWORK_ERROR;
	html = pcs_http_get(pcs->http, URL_PASSPORT_API "login", PcsTrue);
	if (!html)
		return PCS_NETWORK_ERROR;
	url = pcs_utils_sprintf(URL_PASSPORT_API "getapi" "&tpl=ik" "&apiver=v3" "&class=login" "&tt=%d", 
		(int)time(0));
	html = pcs_http_get(pcs->http, url, PcsTrue); pcs_free(url);
	if (!html)
		return PCS_NETWORK_ERROR;
	json = cJSON_Parse(html);
	if (!json)
		return PCS_WRONG_RESPONSE;

	root = cJSON_GetObjectItem(json, "data");
	if (!root) {
		cJSON_Delete(json);
		return PCS_WRONG_RESPONSE;
	}

	item = cJSON_GetObjectItem(root, "token");
	if (!item) {
		cJSON_Delete(json);
		return PCS_WRONG_RESPONSE;
	}
	token = pcs_utils_strdup(item->valuestring);

	item = cJSON_GetObjectItem(root, "codeString");
	if (!item) {
		pcs_free(token);
		cJSON_Delete(json);
		return PCS_WRONG_RESPONSE;
	}
	code_string = pcs_utils_strdup(item->valuestring);

	cJSON_Delete(json);

	p = token;
	while (*p) {
		if (!((*p >= '0' && *p <= '9') || (*p >= 'a' && *p <= 'f') || (*p >= 'A' && *p <= 'F'))) {
			pcs_free(token);
			pcs_free(code_string);
			return PCS_WRONG_RESPONSE;
		}
		p++;
	}

	i = 0;
try_login:
	if (code_string[0]) {
		res = pcs_get_captcha(pcs, code_string, captch, sizeof(captch));
		if (res != PCS_OK) {
			pcs_free(token);
			pcs_free(code_string);
			return res;
		}
	}
	tt = pcs_utils_sprintf("%d", (int)time(0));
	post_data = pcs_http_build_post_data(pcs->http,
		"ppui_logintime", "6852",
		"charset", "UTF-8",
		"codestring", code_string,
		"token", token,
		"isPhone", "false",
		"index", "0",
		"u", "",
		"safeflg", "0",
		"staticpage", "http://www.baidu.com/cache/user/html/jump.html",
		"loginType", "1",
		"tpl", "mn",
		"callback", "parent.bdPass.api.login._postCallback",
		"username", pcs->username,
		"password", pcs->password,
		"verifycode", captch,
		"mem_pass", "on",
		"tt", tt,
		NULL);
	pcs_free(tt);
	if (!post_data) {
		pcs_free(token);
		pcs_free(code_string);
		return PCS_BUILD_POST_DATA;
	}
	html = pcs_http_post(pcs->http, URL_PASSPORT_API "login", post_data, PcsTrue);
	pcs_free(post_data);
	if (!html) {
		pcs_free(token);
		pcs_free(code_string);
		return PCS_NETWORK_ERROR;
	}

	if (!pcs_regex(html, strlen(html), "&error=(\\d+)", indies, &slist)) {
		pcs_free(token);
		pcs_free(code_string);
		return PCS_NETWORK_ERROR;
	}
	error = atoi(slist->string);
	pcs_slist_destroy(slist);

	if (error == 0) {
		if (pcs_islogin(pcs) == PCS_LOGIN) {
			pcs_free(token);
			pcs_free(code_string);
			return PCS_OK;
		}
		else {
			pcs_free(token);
			pcs_free(code_string);
			return PCS_FAIL;
		}
	}

	if (!pcs_regex(html, strlen(html), "&codestring=([a-zA-Z0-9_-]+)", indies, &slist)) {
		pcs_free(token);
		pcs_free(code_string);
		return PCS_FAIL;
	}
	if (code_string) pcs_free(code_string);
	code_string = pcs_utils_strdup(slist->string);
	pcs_slist_destroy(slist);
	if (i < 1 && code_string[0]) {
		i++;
		goto try_login;
	}
	pcs_free(token);
	pcs_free(code_string);

	return PCS_FAIL;
}

PCS_API PcsRes pcs_quota(Pcs handle, UInt64 *quota, UInt64 *used)
{
	struct pcs *pcs = (struct pcs *)handle;
	cJSON *json, *item;
	char *url, *html;
	int error;

	pcs_clear_error(handle);
	url = pcs_build_pan_api_url(handle, "quota", NULL);
	if (!url)
		return PCS_BUILD_URL;
	html = pcs_http_get(pcs->http, url, PcsTrue);
	pcs_free(url);
	if (!html)
		return PCS_NETWORK_ERROR;

	json = cJSON_Parse(html);
	if (!json)
		return PCS_WRONG_RESPONSE;

	item = cJSON_GetObjectItem(json, "errno");
	if (!item) {
		cJSON_Delete(json);
		return PCS_WRONG_RESPONSE;
	}
	error = item->valueint;
	if (error != 0) {
		cJSON_Delete(json);
		return PCS_FAIL;
	}

	item = cJSON_GetObjectItem(json, "total");
	if (!item) {
		cJSON_Delete(json);
		return PCS_WRONG_RESPONSE;
	}
	if (quota)
		*quota = (UInt64)item->valuedouble;

	item = cJSON_GetObjectItem(json, "used");
	if (!item) {
		cJSON_Delete(json);
		return PCS_WRONG_RESPONSE;
	}
	if (used)
		*used = (UInt64)item->valuedouble;

	cJSON_Delete(json);

	return PCS_OK;
}

PCS_API PcsRes pcs_mkdir(Pcs handle, const char *path)
{
	struct pcs *pcs = (struct pcs *)handle;
	int error;
	char *html, *postdata, *url;

	pcs_clear_error(handle);
	url = pcs_build_pan_api_url(handle, "create", "a", "commit", NULL);
	if (!url) {
		return PCS_BUILD_URL;
	}
	postdata = pcs_http_build_post_data(pcs->http, 
		"path", path,
		"isdir", "1",
		"size", "",
		"block_list", "[]",
		"method", "post",
		NULL);
	if (!postdata) {
		pcs_free(url);
		return PCS_BUILD_POST_DATA;
	}
	html = pcs_http_post(pcs->http, url, postdata, PcsTrue);
	pcs_free(url);
	pcs_free(postdata);
	if (!html) {
		return PCS_NETWORK_ERROR;
	}
	error = pcs_get_errno_from_api_res(handle, html);
	return (error ? PCS_FAIL : PCS_OK);
}

PCS_API PcsFileInfoList *pcs_list(Pcs handle, const char *dir, int pageindex, int pagesize, const char *order, PcsBool desc)
{
	struct pcs *pcs = (struct pcs *)handle;
	char *page, *pagenum, *tt;
	PcsFileInfoList *filist = NULL;
	pcs_clear_error(handle);
	page = pcs_utils_sprintf("%d", pageindex);
	pagenum = pcs_utils_sprintf("%d", pagesize);
	tt = pcs_utils_sprintf("%d", (int)time(0));
	filist = pcs_pan_api_1(handle, "list", 
		"_", tt,
		"dir", dir,
		"page", page,
		"num", pagenum,
		"order", order,
		desc ? "desc" : "", desc ? "1" : "0",
		NULL);
	pcs_free(page);
	pcs_free(pagenum);
	pcs_free(tt);
	return filist;
}

PCS_API PcsFileInfoList *pcs_search(Pcs handle, const char *dir, const char *key, PcsBool recursion)
{
	struct pcs *pcs = (struct pcs *)handle;
	PcsFileInfoList *filist = NULL;
	pcs_clear_error(handle);
	filist = pcs_pan_api_1(handle, "search", 
		"dir", dir,
		"key", key,
		recursion ? "recursion" : "", NULL,
		NULL);
	return filist;
}

PCS_API PcsFileInfo *pcs_meta(Pcs handle, const char *path)
{
	PcsFileInfoList *filist;
	PcsFileInfo *meta = NULL;
	char *dir, *key;
	PcsFileInfoListIterater iterater;
	
	pcs_clear_error(handle);
	dir = pcs_utils_basedir(path);
	key = pcs_utils_filename(path);
	filist = pcs_search(handle, dir, key, PcsFalse);
	pcs_free(dir);
	pcs_free(key);
	if (!filist)
		return NULL;
	pcs_filist_iterater_init(filist, &iterater);
	while(pcs_filist_iterater_next(&iterater)) {
		if (pcs_utils_strcmpi(path, iterater.current->path) == 0) {
			meta = iterater.current;
			break;
		}
	}
	if (meta) {
		meta = pcs_fileinfo_clone(meta);
		if (!meta) {
			pcs_set_errmsg(handle, PCS_ALLOC_MEMORY);
		}
	}
	else {
		pcs_set_errmsg(handle, PCS_NOT_EXIST);
	}
	pcs_filist_destroy(filist);
	return meta;
}

PCS_API PcsPanApiRes *pcs_delete(Pcs handle, PcsSList *slist)
{
	char *filelist;
	PcsPanApiRes *res;
	pcs_clear_error(handle);
	if (!slist) {
		pcs_set_errmsg(handle, PCS_WRONG_ARGS);
		return NULL;
	}
	filelist = pcs_build_filelist_1(handle, slist);
	if (!filelist) {
		return NULL;
	}
	res = pcs_pan_api_filemanager(handle, "delete", filelist);
	pcs_free(filelist);
	return res;
}

PCS_API PcsPanApiRes *pcs_rename(Pcs handle, PcsSList2 *slist)
{
	char *filelist;
	PcsPanApiRes *res;
	pcs_clear_error(handle);
	if (!slist) {
		pcs_set_errmsg(handle, PCS_WRONG_ARGS);
		return NULL;
	}
	filelist = pcs_build_filelist_2(handle, slist);
	if (!filelist) {
		return NULL;
	}
	res = pcs_pan_api_filemanager(handle, "rename", filelist);
	pcs_free(filelist);
	return res;
}

PCS_API PcsPanApiRes *pcs_move(Pcs handle, PcsSList2 *slist)
{
	char *filelist;
	PcsPanApiRes *res;
	pcs_clear_error(handle);
	if (!slist) {
		pcs_set_errmsg(handle, PCS_WRONG_ARGS);
		return NULL;
	}
	filelist = pcs_build_filelist_3(handle, slist);
	if (!filelist) {
		return NULL;
	}
	res = pcs_pan_api_filemanager(handle, "move", filelist);
	pcs_free(filelist);
	return res;
}

PCS_API PcsPanApiRes *pcs_copy(Pcs handle, PcsSList2 *slist)
{
	char *filelist;
	PcsPanApiRes *res;
	pcs_clear_error(handle);
	if (!slist) {
		pcs_set_errmsg(handle, PCS_WRONG_ARGS);
		return NULL;
	}
	filelist = pcs_build_filelist_3(handle, slist);
	if (!filelist) {
		return NULL;
	}
	res = pcs_pan_api_filemanager(handle, "copy", filelist);
	pcs_free(filelist);
	return res;
}

PCS_API PcsRes pcs_download(Pcs handle, const char *path)
{
	struct pcs *pcs = (struct pcs *)handle;
	char *url;

	pcs_clear_error(handle);
	url = pcs_http_build_url(pcs->http, URL_PCS_REST,
		"method", "download",
		"app_id", "250528",
		"path", path,
		NULL);
	if (!url) {
		return PCS_BUILD_URL;
	}
	if (pcs_http_get_download(pcs->http, url, PcsTrue))
		return PCS_OK;
	pcs_set_error(handle, pcs_http_strerror(pcs->http));
	return PCS_FAIL;
}

PCS_API const char *pcs_cat(Pcs handle, const char *path)
{
	struct pcs *pcs = (struct pcs *)handle;
	PcsRes res;
	char *url, *html;

	pcs_clear_error(handle);
	url = pcs_http_build_url(pcs->http, URL_PCS_REST,
		"method", "download",
		"app_id", "250528",
		"path", path,
		NULL);
	if (!url) {
		pcs_set_errmsg(handle, PCS_BUILD_URL);
		return NULL;
	}
	html = pcs_http_get(pcs->http, url, PcsTrue);
	pcs_free(url);
	if (!html) {
		pcs_set_errmsg(handle, PCS_NETWORK_ERROR);
		return NULL;
	}
	return html;
}

PCS_API PcsFileInfo *pcs_upload_buffer(Pcs handle, const char *path, PcsBool overwrite, 
									   const char *buffer, size_t buffer_size)
{
	struct pcs *pcs = (struct pcs *)handle;
	char *filename;
	PcsHttpForm form = NULL;
	PcsFileInfo *meta;

	pcs_clear_error(handle);
	filename = pcs_utils_filename(path);
	if (pcs_http_form_addbuffer(pcs->http, &form, "file", buffer, (long)buffer_size, filename) != PcsTrue) {
		pcs_set_errmsg(handle, PCS_BUILD_POST_DATA);
		pcs_free(filename);
		return NULL;
	}
	pcs_free(filename);
	meta = pcs_upload_form(handle, path, overwrite, form);
	pcs_http_form_destroy(pcs->http, form);
	return meta;
}

PCS_API PcsFileInfo *pcs_upload(Pcs handle, const char *path, PcsBool overwrite, 
									   const char *local_filename)
{
	struct pcs *pcs = (struct pcs *)handle;
	char *filename;
	PcsHttpForm form = NULL;
	PcsFileInfo *meta;

	pcs_clear_error(handle);
	filename = pcs_utils_filename(path);
	if (pcs_http_form_addfile(pcs->http, &form, "file", local_filename, filename) != PcsTrue) {
		pcs_set_errmsg(handle, PCS_BUILD_POST_DATA);
		pcs_free(filename);
		return NULL;
	}
	pcs_free(filename);
	meta = pcs_upload_form(handle, path, overwrite, form);
	pcs_http_form_destroy(pcs->http, form);
	return meta;
}
