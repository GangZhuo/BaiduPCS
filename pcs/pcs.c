#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#ifdef WIN32
# include <malloc.h>
# include "openssl_md5.h"
#else
# include <alloca.h>
# include <openssl/md5.h>
#endif

#include "pcs_defs.h"
#include "pcs_mem.h"
#include "pcs_utils.h"
#include "pcs_http.h"
#include "cJSON.h"
#include "pcs.h"
#include "pcs_passport_dv.h"

#ifdef WIN32
#ifndef __MINGW32__
# define lseek _lseek
# define fileno _fileno
# define fseeko _fseeki64
# define ftello _ftelli64
#endif

# define snprintf _snprintf
# define vsnprintf _vsnprintf
#endif

#define PCS_MD5_SIZE	16 /*MD5的长度，固定为16。修改为其他值将导致校验错误*/

#define PCS_SKIP_SPACE(p) while((*p) && (*p == ' ' || *p == '\f' || *p == '\n' || *p == '\r' || *p == '\t' || *p == '\v')) p++

//#define PCS_IS_TOKEN_CHAR(ch) (((ch) >= '0' && (ch) <= '9')\
//								   || ((ch) >= 'a' && (ch) <= 'z')\
//								   || ((ch) >= 'A' && (ch) <= 'Z')\
//								   || (ch) == '_'\
//								   || (ch) == '-')

#define PCS_IS_TOKEN_CHAR(ch) ((ch) != '&' && (ch) != '?' && (ch) != '#' && (ch) > 32 && (ch) < 127)

#define URL_HOME			"http://www.baidu.com"
#define URL_DISK_HOME		"http://pan.baidu.com/disk/home"
#define URL_PASSPORT_API	"https://passport.baidu.com/v2/api/?"
#define URL_GET_PUBLIC_KEY	"https://passport.baidu.com/v2/getpublickey?"
#define URL_PASSPORT_LOGOUT	"https://passport.baidu.com/?logout&u=http://pan.baidu.com"
#define URL_CAPTCHA			"https://passport.baidu.com/cgi-bin/genimage?"
#define URL_PAN_API			"http://pan.baidu.com/api/"
#define URL_PCS_REST		"http://c.pcs.baidu.com/rest/2.0/pcs/file"
#define URL_AUTHWIDGET_VERIFY "http://passport.baidu.com/v2/sapi/authwidgetverify?" \
                            "authtoken=%s" \
							"&type=%s" \
							"&jsonp=1" \
							"&apiver=v3" \
							"&verifychannel=" \
							"&action=%s" \
                            "&vcode=%s" \
							"&questionAndAnswer=" \
							"&needsid=" \
							"&rsakey=" \
							"&countrycode=" \
                            "&subpro=netdisk_web" \
							"&callback=bd__cbs__dl4txf"

struct pcs {
	char		*username;
	char		*password;
	char		*bdstoken;
	char		*bduss;
	char		*sysUID;
	char		*errmsg;
	char		*public_key;
	char		*key;

	PcsGetCaptchaFunction		captcha_func;
	void						*captcha_data;

    PcsInputFunction        input_func;
    void                    *input_func_data;

	PcsHttp		http;

	PcsHttpWriteFunction	download_func;
	void					*download_data;

	char		*buffer;
	size_t		buffer_size;
};

#define PCS_MSG_BUFFER_SIZE		(1 * 1024)
#define PCS_BUFFER_SIZE			(16 * 1024)
#define PCS_ACTION_NONE			0
#define PCS_ACTION_DOWNLOAD		1
#define PCS_ACTION_UPLOAD		2

/* 在 err_msg.c 中实现 */
const char *get_login_errmsg(int error);
const char *get_errmsg_by_errno(int error);
const char *get_share_errmsg_by_errno(int error);
const char *get_download_errmsg_by_errno(int error);
const char *get_buy_errmsg_by_errno(int error);
const char *get_record_errmsg_by_errno(int error);

PCS_API void pcs_clear_errmsg(Pcs handle)
{
	struct pcs *pcs = (struct pcs *)handle;
	if (pcs->errmsg) {
		pcs_free(pcs->errmsg);
		pcs->errmsg = NULL;
	}
}

PCS_API void pcs_set_errmsg(Pcs handle, const char *fmt, ...)
{
	struct pcs *pcs = (struct pcs *)handle;
	va_list args;
	if (pcs->errmsg) {
		pcs_free(pcs->errmsg);
		pcs->errmsg = NULL;
	}
	va_start(args, fmt);
	pcs->errmsg = pcs_utils_vsprintf(fmt, args);
	va_end(args);
}

PCS_API void pcs_cat_errmsg(Pcs handle, const char *fmt, ...)
{
	struct pcs *pcs = (struct pcs *)handle;
	char *errmsg;
	size_t sz = 0;
	va_list args;
	va_start(args, fmt);
	errmsg = pcs_utils_vsprintf(fmt, args);
	va_end(args);
	pcs_cat_serrmsg(handle, errmsg);
	pcs_free(errmsg);
}

/* 设置错误消息 */
PCS_API void pcs_set_serrmsg(Pcs handle, const char *errmsg)
{
	struct pcs *pcs = (struct pcs *)handle;
	if (pcs->errmsg) {
		pcs_free(pcs->errmsg);
		pcs->errmsg = NULL;
	}
	pcs->errmsg = pcs_utils_strdup(errmsg);
}

/* 添加文本到错误消息的结尾 */
PCS_API void pcs_cat_serrmsg(Pcs handle, const char *errmsg)
{
	struct pcs *pcs = (struct pcs *)handle;
	char *p;
	size_t sz = 0;
	if (pcs->errmsg) {
		sz = strlen(pcs->errmsg) + strlen(errmsg);
		p = (char *)pcs_malloc(sz + 1);
		strcpy(p, pcs->errmsg);
		pcs_free(pcs->errmsg);
		pcs->errmsg = p;
		strcat(p, errmsg);
	}
	else {
		pcs->errmsg = pcs_utils_strdup(errmsg);
	}
}


//static inline void pcs_set_errmsg(Pcs handle, PcsRes error)
//{
//	if (error != PCS_NONE)
//		pcs_set_error(handle, pcs_strerror(handle, error));
//}

/*调用用户注册的验证码函数来让用户识别验证码图片，并让用户输入识别结果*/
PcsRes pcs_get_captcha(Pcs handle, const char *code_string, char *captcha, int captchaSize)
{
	struct pcs *pcs = (struct pcs *)handle;
	char *url, *img;
	size_t imgsz;

    memset(captcha, 0, captchaSize);
	if (!pcs->captcha_func) {
		pcs_set_errmsg(handle, "No captch function, please regist the function by call pcs_setopt(handle, PCS_OPTION_CAPTCHA_FUNCTION, pFun).");
		return PCS_NO_CAPTCHA_FUNC;
	}
	url = pcs_utils_sprintf(URL_CAPTCHA "%s", code_string);
	img = pcs_http_get_raw(pcs->http, url, PcsTrue, &imgsz); 
	if (!img) {
		pcs_set_errmsg(handle, "Can't get captch image from the server. url: %s. Error message: %s", url, pcs_http_strerror(pcs->http));
		pcs_free(url);
		return PCS_NETWORK_ERROR;
	}
	if (!(*pcs->captcha_func)((unsigned char *)img, imgsz, captcha, captchaSize, pcs->captcha_data)) {
		pcs_set_errmsg(handle, "Can't get captch from the captcha, which is regist by user.");
		pcs_free(url);
		return PCS_GET_CAPTCHA_FAIL;
	}
	pcs_free(url);
	return PCS_OK;
}

/* 调用用户注册的输入函数来让用户输入数据 */
PcsRes pcs_input(Pcs handle, const char *tips, char *value, size_t valueSize)
{
    struct pcs *pcs = (struct pcs *)handle;
    memset(value, 0, valueSize);
    if (!pcs->input_func) {
        pcs_set_errmsg(handle, "No input function, please regist the function by call pcs_setopt(handle, PCS_OPTION_INPUT_FUNCTION, pFun).");
        return PCS_NO_INPUT_FUNC;
    }
    if (!(*pcs->input_func)(tips, value, valueSize, pcs->input_func_data)) {
        pcs_set_errmsg(handle, "Can't get the user input");
        return PCS_GET_INPUT_FAIL;
    }
    return PCS_OK;
}

/*从html中解析出类似于 FileUtils.bdstoken="****" 的值。此例中，key传入"FileUtils.bdstoken"，将返回"****"值*/
static char *pcs_get_value_by_key(const char *html, const char *key)
{
	char *val = NULL;
	const char *p = html,
		*end = NULL,
		*tmp;
	int i = strlen(key);// \f\n\r\t\v
	while (*p) {
		if (*p == key[0] && pcs_utils_streq(p, key, i)) {
			tmp = p + i;
			PCS_SKIP_SPACE(tmp);
			if (*tmp != '=') continue; tmp++;
			PCS_SKIP_SPACE(tmp);
			if (*tmp != '"') continue; tmp++;
			end = tmp;
			while (*end && *end != '"') end++;
			if (*end == '"') {
				val = (char *)pcs_malloc(end - tmp + 1);
				memcpy(val, tmp, end - tmp);
				val[end - tmp] = '\0';
				return val;
			}
		}
		p++;
	}
	return val;
}

/*从html中解析出类似于 yunData.setData(****) 的参数。*/
static char *pcs_get_yunData(const char *html, const char *key)
{
#define ST_LEN 32
	char *val = NULL, st[ST_LEN];
	const char *p = html,
		*end = NULL,
		*tmp;
	int i = strlen(key),
		si = 0;

	st[0] = '\0';
#define st_push(c) st[++si] = (c)
#define st_pop() st[si--]
#define st_top() st[si]

	while (*p) {
		if (*p == key[0] && pcs_utils_streq(p, key, i)) {
			tmp = p + i;
			PCS_SKIP_SPACE(tmp);
			if (*tmp != '=') {
				p++; continue;
			}tmp++;
			PCS_SKIP_SPACE(tmp);
			if (*tmp != '{') {
				p++; continue;
			}
			end = tmp;

			while (*end) {
				if (st_top() == '\\') {
					st_pop();
				}
				else {
					switch (*end) {
					case '"':
						if (st_top() == '"') {
							st_pop();
						}
						else {
							st_push('"');
						}
						break;
					case '\\':
						st_push('\\');
						break;
					case '{':
					case '[':
					case '(':
						if (st_top() != '"')
							st_push(*end);
						break;
					case '}':
					case ']':
					case ')':
						if (st_top() != '"')
							st_pop();
						break;
					}
				}
				if (si == 0)
					break;
				end++;
			}
			if (*end == '}') {
				val = (char *)pcs_malloc(end - tmp + 2);
				memcpy(val, tmp, end - tmp + 1);
				val[end - tmp + 1] = '\0';
				return val;
			}
		}
		p++;
	}
	return val;
}

/*从URL地址的QueryString中解析出类似于 &error=123&a= 的值。此例中，key传入"&error"，将返回123*/
static char *pcs_get_embed_query_int_value_by_key(const char *html, const char *key)
{
	char *val = NULL;
	const char *p = html,
		*end = NULL,
		*tmp;
	int i = strlen(key);// \f\n\r\t\v
	while (*p) {
		if (*p == key[0] && pcs_utils_streq(p, key, i)) {
			tmp = p + i;
			if (*tmp != '=') continue; tmp++;
			end = tmp;
			while (*end && *end >= '0' && *end <= '9') end++;
			if (end > tmp) {
				val = (char *)pcs_malloc(end - tmp + 1);
				memcpy(val, tmp, end - tmp);
				val[end - tmp] = '\0';
				return val;
			}
		}
		p++;
	}
	return val;
}

/*从URL地址的QueryString中解析出类似于 &token=abc_ef-g&a= 的值。此例中，key传入"&token"，将返回abc_ef-g*/
static char *pcs_get_embed_query_token_by_key(const char *html, const char *key)
{
	char *val = NULL;
	const char *p = html,
		*end = NULL,
		*tmp;
	int i = strlen(key);// \f\n\r\t\v
	while (*p) {
		if (*p == key[0] && pcs_utils_streq(p, key, i)) {
			tmp = p + i;
			if (*tmp != '=') continue; tmp++;
			end = tmp;
			while (*end && PCS_IS_TOKEN_CHAR(*end)) end++;
			if (end > tmp) {
				val = (char *)pcs_malloc(end - tmp + 1);
				memcpy(val, tmp, end - tmp);
				val[end - tmp] = '\0';
				return val;
			}
		}
		p++;
	}
	return val;
}

/*pcs_build_pan_api_url()的矢量模式*/
static char *pcs_build_pan_api_url_v(Pcs handle, const char *action, va_list args)
{
	struct pcs *pcs = (struct pcs *)handle;
	char *baseurl, *url;
	baseurl = (char *)alloca(strlen(URL_PAN_API) + strlen(action) + 1);
	strcpy(baseurl, URL_PAN_API);
	strcat(baseurl, action);
	baseurl = pcs_http_build_url(pcs->http, baseurl,
		"channel", "chunlei",
		"clienttype", "0",
		"web","1", 
		"t", pcs_js_timestr(),
		"bdstoken", pcs->bdstoken,
		NULL);
	if (!baseurl)
		return NULL;
	url = pcs_http_build_url_v(pcs->http, baseurl, args);
	pcs_free(baseurl);
	return url;
}

/*传入action，即参数，拼接出最终字符串。参数以(key, value)对的形式传入，最后传入一个NULL表示终止
例：pcs_build_pan_api_url(handle, "list", k1, v1, k2, v2, NULL); 
//将返回：http://pan.baidu.com/api/list?channel=chunlei&clienttype=0&web=1&t=189343343&bdstoken=dfjewdfe&k1=v1&k2=v2
*/
static char *pcs_build_pan_api_url(Pcs handle, const char *action, ...)
{
	char *url;
	va_list args;
    va_start(args, action);
	url = pcs_build_pan_api_url_v(handle, action, args);
    va_end(args);
	return url;
}

/*转换JSON对象为PcsFileInfo对象。
JSON对象格式为：
{
	"fs_id": 123,
	"path": "/a/b",
	"server_filename": "b",
	"mtime": 1899383,
	"ctime": 1899383,
	"server_mtime": 1899383,
	"server_ctime": 1899383,
	"local_mtime": 1899383,
	"local_ctime": 1899383,
	"isdir": 1,
	"category": 1,
	"size": 0,
	"dir_empty": 0,
	"empty": 0,
	"ifhassubdir": 0,
	"md5": "",
	"dlink": "",
	"block_list": [ "/a/b/1", "a/b/2" ]
}
*/
static PcsFileInfo *pcs_parse_fileinfo(cJSON * item)
{
	cJSON *val, *list;
	PcsFileInfo *fi = pcs_fileinfo_create();
	val = cJSON_GetObjectItem(item, "fs_id");
	if ((val = cJSON_GetObjectItem(item, "fs_id")))
		fi->fs_id = (uint64_t)val->valuedouble;

	val = cJSON_GetObjectItem(item, "path");
	if (val)
		fi->path = pcs_utils_strdup(val->valuestring);

	val = cJSON_GetObjectItem(item, "server_filename");
	if (val)
		fi->server_filename = pcs_utils_strdup(val->valuestring);

	val = cJSON_GetObjectItem(item, "mtime");
	if (val)
		fi->server_mtime = (time_t)val->valuedouble;

	val = cJSON_GetObjectItem(item, "ctime");
	if (val)
		fi->server_ctime = (time_t)val->valuedouble;

	val = cJSON_GetObjectItem(item, "server_mtime");
	if (val)
		fi->server_mtime = (time_t)val->valuedouble;

	val = cJSON_GetObjectItem(item, "server_ctime");
	if (val)
		fi->server_ctime = (time_t)val->valuedouble;

	val = cJSON_GetObjectItem(item, "local_mtime");
	if (val)
		fi->local_mtime = (time_t)val->valuedouble;

	val = cJSON_GetObjectItem(item, "local_ctime");
	if (val)
		fi->local_ctime = (time_t)val->valuedouble;

	val = cJSON_GetObjectItem(item, "isdir");
	if (val)
		fi->isdir = val->valueint ? PcsTrue : PcsFalse;

	val = cJSON_GetObjectItem(item, "category");
	if (val)
		fi->category = val->valueint;

	val = cJSON_GetObjectItem(item, "size");
	if (val)
		fi->size = (int64_t)val->valuedouble;

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
		fi->md5 = pcs_utils_strdup(val->valuestring);

	val = cJSON_GetObjectItem(item, "dlink");
	if (val)
		fi->dlink = pcs_utils_strdup(val->valuestring);

	list = cJSON_GetObjectItem(item, "block_list");
	if (list) {
		int i, cnt = cJSON_GetArraySize(list);
		if (cnt > 0) {
			fi->block_list = (char **) pcs_malloc((cnt + 1) * sizeof(char *));
			if (!fi->block_list) return fi;
			memset(fi->block_list, 0, (cnt + 1) * sizeof(char *));
			for (i = 0; i < cnt; i++) {
				val = cJSON_GetArrayItem(list, i);
				fi->block_list[i] = pcs_utils_strdup(val->valuestring);
			}
		}
	}
    
    list = cJSON_GetObjectItem(item, "thumbs");
	if (list) {
        val = cJSON_GetObjectItem(list, "url3");
        if (val) pcs_fileinfo_add_thumb(fi, "url3", val->valuestring);

        val = cJSON_GetObjectItem(list, "url2");
        if (val) pcs_fileinfo_add_thumb(fi, "url2", val->valuestring);

        val = cJSON_GetObjectItem(list, "url1");
        if (val) pcs_fileinfo_add_thumb(fi, "url1", val->valuestring);

        val = cJSON_GetObjectItem(list, "icon");
        if (val) pcs_fileinfo_add_thumb(fi, "icon", val->valuestring);
    }
    return fi;
}

/*
根据传入参数，执行api函数。参数传入方法，参考pcs_build_pan_api_url()函数
action: list, search
*/
static PcsFileInfoList *pcs_pan_api_1(Pcs handle, const char *action, ...)
{
	struct pcs *pcs = (struct pcs *)handle;
	va_list args;
	cJSON *json, *item, *list;
	char *url, *html;
	const char *errmsg;
	int error, cnt, i;
	PcsFileInfoList *filist = NULL;
	PcsFileInfoListItem *filist_item;
	PcsFileInfo *fi;

	pcs_clear_errmsg(handle);
    va_start(args, action);
	url = pcs_build_pan_api_url_v(handle, action, args);
    va_end(args);
	if (!url) {
		pcs_set_errmsg(handle, "Can't build url.");
		return NULL;
	}
	html = pcs_http_get(pcs->http, url, PcsTrue);
	pcs_free(url);
	if (!html) {
		errmsg = pcs_http_strerror(pcs->http);
		if (errmsg)
			pcs_set_serrmsg(handle, errmsg);
		else
			pcs_set_errmsg(handle, "Can't get response from the remote server.");
		return NULL;
	}
	json = cJSON_Parse(html);
	if (!json){
		//printf("%s\n", html);
		pcs_set_errmsg(handle, "Can't parse the response as json: %s", html);
		return NULL;
	}
	item = cJSON_GetObjectItem(json, "errno");
	if (!item) {
		//printf("%s\n", html);
		pcs_set_errmsg(handle, "Can't read res.errno: %s", html);
		cJSON_Delete(json);
		return NULL;
	}
	error = item->valueint;
	if (error != 0) {
		//printf("%s\n", html);
		pcs_set_errmsg(handle, "%s, Error：%d", get_errmsg_by_errno(error), error);
		cJSON_Delete(json);
		return NULL;
	}
	list = cJSON_GetObjectItem(json, "list");
	if (!list) {
		pcs_set_errmsg(handle, "Can't read res.list: %s", html);
		cJSON_Delete(json);
		return NULL;
	}
	cnt = cJSON_GetArraySize(list);
	if (cnt <= 0) {
		cJSON_Delete(json);
		return NULL;
	}
	filist = pcs_filist_create();
	if (!filist) {
		pcs_set_errmsg(handle, "Can't create object: PcsFileInfoList");
		cJSON_Delete(json);
		return NULL;
	}
	for (i = 0; i < cnt; i++) {
		item = cJSON_GetArrayItem(list, i);
		filist_item = pcs_filistitem_create();
		if (!filist_item) {
			pcs_set_errmsg(handle, "Can't create object: PcsFileInfoListItem");
			cJSON_Delete(json);
			if (filist) pcs_filist_destroy(filist);
			return NULL;
		}
		fi = pcs_parse_fileinfo(item);
		if (!fi) {
			pcs_set_errmsg(handle, "Can't create object: PcsFileInfo");
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

/*根据slist字符串链表，构造成数组格式的json字符串，并把数组元素数量写入到count指定的内存中*/
static char *pcs_build_filelist_1(Pcs handle, PcsSList *slist, int *count)
{
	PcsSList *item;
	char *file_list_string;
	size_t sz;
	int cnt = 0;

	if (count) *count = cnt;
	if (!slist)
		return NULL;

	sz = 0;
	item = slist;
	while(item) {
		sz += strlen(item->string);
		sz += 3;
		item = item->next;
		cnt++;
	}
	sz += 2;
	if (count) *count = cnt;

	file_list_string = (char *) pcs_malloc(sz);
	if (!file_list_string) {
		pcs_set_errmsg(handle, "Can't alloc memory: size=0x%x", sz);
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

/*根据slist字符串链表，构造成数组格式的json字符串，并把数组元素数量写入到count指定的内存中*/
static char *pcs_build_filelist_2(Pcs handle, PcsSList2 *slist, int *count)
{
	PcsSList2 *item;
	char *file_list_string, *p;
	size_t sz;
	int cnt = 0;

	if (count) *count = cnt;
	if (!slist)
		return NULL;

	sz = 0;
	item = slist;
	while(item) {
		sz += strlen(item->string1);
		sz += strlen(item->string2);
		sz += 25;
		item = item->next;
		cnt++;
	}
	sz += 2;
	if (count) *count = cnt;

	file_list_string = (char *) pcs_malloc(sz);
	if (!file_list_string) {
		pcs_set_errmsg(handle, "Can't alloc memory: size=0x%x", sz);
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

/*根据slist字符串链表，构造成数组格式的json字符串，并把数组元素数量写入到count指定的内存中*/
static char *pcs_build_filelist_3(Pcs handle, PcsSList2 *slist, int *count)
{
	PcsSList2 *item;
	char *file_list_string, *p, *dir, *filename;
	size_t sz;
	int cnt = 0;

	if (count) *count = cnt;
	if (!slist)
		return NULL;

	sz = 0;
	item = slist;
	while(item) {
		sz += strlen(item->string1);
		sz += strlen(item->string2);
		sz += 35;
		item = item->next;
		cnt++;
	}
	sz += 2;
	if (count) *count = cnt;

	file_list_string = (char *) pcs_malloc(sz);
	if (!file_list_string) {
		pcs_set_errmsg(handle, "Can't alloc memory: size=0x%x", sz);
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
			pcs_set_errmsg(handle, "Can't alloc memory: size=0x%x", sz);
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

/*
根据传入参数，执行api函数。
opera: delete, rename, move, copy
*/
static PcsPanApiRes *pcs_pan_api_filemanager(Pcs handle, const char *opera, const char *filelist, int file_count)
{
	struct pcs *pcs = (struct pcs *)handle;
	cJSON *json, *item, *list, *val;
	char *url, *html, *postdata;
	const char *errmsg;
	int error, cnt, i;
	PcsPanApiRes *res = NULL;
	PcsPanApiResInfoList *tail, *ri;

	PcsBool err_no_space = PcsFalse, //errno = -10 剩余空间不足
		err_target_not_exist = PcsFalse, //errno = -8 文件已存在于目标文件夹中
		err_src_file_not_exist = PcsFalse, //errno = -9 文件不存在
		err_has_succ_items = PcsFalse; //是否存在处理成功的文件项

	url = pcs_build_pan_api_url(handle, "filemanager",
		"opera", opera,
		NULL);
	if (!url) {
		pcs_set_errmsg(handle, "Can't build the url.");
		return NULL;
	}

	postdata = pcs_http_build_post_data(pcs->http, "filelist", filelist, NULL);
	if (!postdata) {
		pcs_set_errmsg(handle, "Can't build the post data.");
		pcs_free(url);
		return NULL;
	}
	html = pcs_http_post(pcs->http, url, postdata, PcsTrue);
	pcs_free(url);
	pcs_free(postdata);
	if (!html) {
		errmsg = pcs_http_strerror(pcs->http);
		if (errmsg)
			pcs_set_serrmsg(handle, errmsg);
		else
			pcs_set_errmsg(handle, "Can't get response from the remote server.");
		return NULL;
	}
	json = cJSON_Parse(html);
	if (!json){
		pcs_set_errmsg(handle, "Can't parse the response as object. Response: %s", html);
		return NULL;
	}
	item = cJSON_GetObjectItem(json, "errno");
	if (!item) {
		pcs_set_errmsg(handle, "Can't read res.errno. Response: %s", html);
		cJSON_Delete(json);
		return NULL;
	}
	error = item->valueint;
	res = pcs_pan_api_res_create();
	if (!res) {
		pcs_set_errmsg(handle, "Can't create the object: PcsPanApiRes");
		cJSON_Delete(json);
		return NULL;
	}
	res->error = error;

	list = cJSON_GetObjectItem(json, "info");
	if (!list) {
		pcs_set_errmsg(handle, "Can't read res.info. Response: %s", html);
		cJSON_Delete(json);
		return NULL;
	}

	cnt = cJSON_GetArraySize(list);
	for (i = 0; i < cnt; i++) {
		item = cJSON_GetArrayItem(list, i);
		ri = pcs_pan_api_res_infolist_create();
		if (!ri) {
			pcs_set_errmsg(handle, "Can't create the object: PcsPanApiResInfoList");
			cJSON_Delete(json);
			if (res) pcs_pan_api_res_destroy(res);
			return NULL;
		}
		val = cJSON_GetObjectItem(item, "path");
		if (val)
			ri->info.path = pcs_utils_strdup(val->valuestring);
		val = cJSON_GetObjectItem(item, "errno");
		ri->info.error = val->valueint;
		if (!res->info_list) {
			res->info_list = tail = ri;
		}
		else {
			tail->next = ri;
			tail = ri;
		}
		switch (ri->info.error) {
		case 0: //I:处理成功
			err_has_succ_items = PcsTrue;
			break;
		case -8: //D:文件已存在于目标文件夹中
			err_target_not_exist = PcsTrue;
			break;
		case -9: //C:文件不存在
			err_src_file_not_exist = PcsTrue;
			break;
		case -10: //G:剩余空间不足
			err_no_space = PcsTrue;
			break;
		}
	}
	cJSON_Delete(json);
	if (res->error != 0) {
		int is_move = strcmp(opera, "move") == 0;
		if (is_move || strcmp(opera, "copy") == 0) {
			if (res->error == 12) {
				if (!err_no_space && !err_src_file_not_exist && !err_target_not_exist) {
					pcs_set_errmsg(handle, "%s%s", is_move ? "移动" : "复制", "失败，请稍候重试");
				}
				else {
					if (cnt == file_count && !err_has_succ_items) {
						if (err_no_space)
							pcs_set_errmsg(handle, "%s%s", is_move ? "移动" : "复制", "失败，剩余空间不足");
						else if (err_target_not_exist)
							pcs_set_errmsg(handle, "%s%s", is_move ? "移动" : "复制", "失败，文件已存在于目标文件夹中");
						else if (err_src_file_not_exist)
							pcs_set_errmsg(handle, "%s%s", is_move ? "移动" : "复制", "失败，待处理文件不存在");
					}
					else {
						if (err_no_space)
							pcs_set_errmsg(handle, "%s%s", is_move ? "移动" : "复制", "失败，剩余空间不足");
						else if (err_target_not_exist)
							pcs_set_errmsg(handle, "%s%s", is_move ? "移动" : "复制", "失败，部分文件已存在于目标文件夹中");
						else if (err_src_file_not_exist)
							pcs_set_errmsg(handle, "%s%s", is_move ? "移动" : "复制", "失败，部分待处理文件不存在");
					}
				}
			}
			else {
				pcs_set_errmsg(handle, "%s%s", is_move ? "移动" : "复制", "失败，未知错误");
			}
		}
		else if (strcmp(opera, "delete") == 0) {
			pcs_set_errmsg(handle, "删除文件失败，请稍候重试");
		}
		else if (strcmp(opera, "rename") == 0) {
			if (res->error == -8 || res->error == -9 || (cnt > 0 && (res->info_list->info.error == -8 || res->info_list->info.error == -9)))
				pcs_set_errmsg(handle, "名称已在使用，请使用其他名称");
			else if (res->error == 12 && cnt > 0 && res->info_list->info.error == -9)
				pcs_set_errmsg(handle, "重命名失败，文件已被删除或移走");
			else
				pcs_set_errmsg(handle, "文件(夹)重命名失败, 网络错误");
		}
	}
	return res;
}

static int pcs_get_errno_from_api_res(Pcs handle, const char *html)
{
	int res;
	cJSON *json, *item;
	
	json = cJSON_Parse(html);
	if (!json) {
		pcs_set_errmsg(handle, "Can't parse the response as json: %s", html);
		return 1;
	}

	item = cJSON_GetObjectItem(json, "errno");
	if (!item) {
		pcs_set_errmsg(handle, "Can't read res.errno: %s", html);
		cJSON_Delete(json);
		return 1;
	}
	res = item->valueint;
	cJSON_Delete(json);
	return res;
}

static PcsFileInfo *pcs_upload_form(Pcs handle, const char *path, PcsHttpForm form, curl_off_t max_speed, const char *ondup)
{
	struct pcs *pcs = (struct pcs *)handle;
	char *url, *html,
		*dir = pcs_utils_basedir(path),
		*filename = pcs_utils_filename(path);
	cJSON *json, *item;
	PcsFileInfo *meta;

	url = pcs_http_build_url(pcs->http, URL_PCS_REST,
		"method", "upload",
		"app_id", "250528",
		"ondup", ondup,
		"dir", dir,
		"filename", filename,
		"BDUSS", pcs->bduss,
		NULL);
	pcs_free(dir);
	pcs_free(filename);
	if (!url) {
		pcs_set_errmsg(handle, "Can't build the url.");
		return NULL;
	}
	html = pcs_post_httpform(pcs->http, url, form, max_speed, PcsTrue);
	pcs_free(url);
	if (!html) {
		const char *errmsg = pcs_http_strerror(pcs->http);
		if (errmsg)
			pcs_set_serrmsg(handle, errmsg);
		else
			pcs_set_errmsg(handle, "Can't get response from the remote server.");
		return NULL;
	}
	json = cJSON_Parse(html);
	if (!json) {
		pcs_set_errmsg(handle, "Can't parse the response as json: %s", html);
		return NULL;
	}
	//"{\"error_code\":31062,\"error_msg\":\"file name is invalid\",\"request_id\":3071564675}
	item = cJSON_GetObjectItem(json, "error_code");
	if (item) {
		int error_code = item->valueint;
		const char *error_msg = NULL;
		item = cJSON_GetObjectItem(json, "error_msg");
		if (item)
			error_msg = item->valuestring;
		pcs_set_errmsg(handle, "Can't upload file. error_code: %d. error_msg: %s. raw response: %s", error_code, error_msg, html);
		cJSON_Delete(json);
		return NULL;
	}

	meta = pcs_parse_fileinfo(json);
	cJSON_Delete(json);
	if (!meta) {
		pcs_set_errmsg(handle, "Can't parse the response as meta");
		return NULL;
	}
	return meta;
}

static PcsFileInfo *pcs_upload_slice_form(Pcs handle, PcsHttpForm form, curl_off_t max_speed)
{
	struct pcs *pcs = (struct pcs *)handle;
	char *url, *html;
	cJSON *json, *item;
	PcsFileInfo *meta;

	url = pcs_http_build_url(pcs->http, URL_PCS_REST,
		"method", "upload",
		"type", "tmpfile",
		"app_id", "250528",
		"BDUSS", pcs->bduss,
		NULL);
	if (!url) {
		pcs_set_errmsg(handle, "Can't build the url.");
		return NULL;
	}
	html = pcs_post_httpform(pcs->http, url, form, max_speed, PcsTrue);
	pcs_free(url);
	if (!html) {
		const char *errmsg = pcs_http_strerror(pcs->http);
		if (errmsg)
			pcs_set_serrmsg(handle, errmsg);
		else
			pcs_set_errmsg(handle, "Can't get response from the remote server.");
		return NULL;
	}
	json = cJSON_Parse(html);
	if (!json) {
		pcs_set_errmsg(handle, "Can't parse the response as json: %s", html);
		return NULL;
	}
	/*{
		"path": "\/temp\/putty-0.63-installer.exe",
		"size": 1869122,
		"ctime": 1420193926,
		"mtime": 1420193926,
		"md5": "18bd0948d254894441dd6f818d9b3811",
		"fs_id": 919365701714983,
		"isdir": 0,
		"request_id": 2401283243
	}*/
	item = cJSON_GetObjectItem(json, "error_code");
	if (item) {
		int error_code = item->valueint;
		const char *error_msg = NULL;
		item = cJSON_GetObjectItem(json, "error_msg");
		if (item)
			error_msg = item->valuestring;
		pcs_set_errmsg(handle, "Can't upload file. error_code: %d. error_msg: %s. raw response: %s", error_code, error_msg, html);
		cJSON_Delete(json);
		return NULL;
	}

	meta = pcs_parse_fileinfo(json);
	cJSON_Delete(json);
	if (!meta) {
		pcs_set_errmsg(handle, "Can't parse the response as meta");
		return NULL;
	}
	return meta;
}

/*移除字符串中的换行符*/
//static char * remove_enter(char *str)
//{
//	char *p1, *p2;
//	p1 = p2 = str;
//	for (p1 = p2 = str; *p1; p1++) {
//		if (*p1 != '\n') {
//			*p2++ = *p1;
//		}
//	}
//	*p2 = '\0';
//	return str;
//}

/**
* 登录时，转换字节串为其base64编码
* Use EVP to Base64 encode the input byte array to readable text
*/
//static char* base64(const char *inputBuffer, int inputLen)
//{
//	EVP_ENCODE_CTX  ctx;
//	int base64Len = (((inputLen + 2) / 3) * 4) + 1; // Base64 text length  
//	int pemLen = base64Len + base64Len / 64; // PEM adds a newline every 64 bytes  
//	char* base64 = (char *)pcs_malloc(pemLen + 1);
//	int result;
//	EVP_EncodeInit(&ctx);
//	EVP_EncodeUpdate(&ctx, (unsigned char *)base64, &result, (unsigned char *)inputBuffer, inputLen);
//	EVP_EncodeFinal(&ctx, (unsigned char *)&base64[result], &result);
//	return remove_enter(base64);
//}

/*登录时编码密码密文中的符号*/
//static char * escape_symbol(const char *buf)
//{
//	static char tb[] = "0123456789ABCDEF";
//	const char *p = buf;
//	char *tmp, *np;
//	int newLen = 0;
//#define IS_SYMBOL(p) (*p == '#' || *p == '%' || *p == '&' || *p == '+' || *p == '=' || *p == '/' || *p == '\\' || *p == ' ' || *p == '\f' || *p == '\r' || *p == '\n' || *p == '\t')
//	while (*p) {
//		if (IS_SYMBOL(p))
//			newLen += 3;
//		else
//			newLen++;
//		p++;
//	}
//	p = buf;
//	tmp = (char *)pcs_malloc(newLen + 1);
//	np = tmp;
//	while (*p) {
//		if (IS_SYMBOL(p)) {
//			np[0] = '%';
//			np[1] = tb[(((int)(*p)) >> 4) & 0xF];
//			np[2] = tb[(((int)(*p))) & 0xF];
//			np += 3;
//		}
//		else {
//			*np = *p;
//			np++;
//		}
//		p++;
//	}
//	*np = '\0';
//	return tmp;
//}

/*登录时加密用户密码*/
//static char *rsa_encrypt(const char *str, const char *pub_key){
//	char *p_en, *b64;
//	int rsa_len;
//	BIO *bufio;
//	RSA *rsa = NULL;
//
//	bufio = BIO_new_mem_buf((void *)(char *)pub_key, -1);
//	PEM_read_bio_RSA_PUBKEY(bufio, &rsa, NULL, NULL);
//	if (rsa == NULL){
//		ERR_print_errors_fp(stdout);
//		return NULL;
//	}
//	rsa_len = RSA_size(rsa);
//	p_en = (unsigned char *)pcs_malloc(rsa_len + 1);
//	memset(p_en, 0, rsa_len + 1);
//	if (RSA_public_encrypt(rsa_len, (unsigned char *)str, (unsigned char*)p_en, rsa, RSA_NO_PADDING)<0){
//		return NULL;
//	}
//	RSA_free(rsa);
//
//	b64 = base64(p_en, rsa_len);
//	pcs_free(p_en);
//	return b64;
//}

PCS_API const char *pcs_version()
{
#if defined(DEBUG) || defined(_DEBUG)
	return PCS_API_VERSION "-debug";
#else
	return PCS_API_VERSION;
#endif
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
	if (pcs->public_key)
		pcs_free(pcs->public_key);
	if (pcs->key)
		pcs_free(pcs->key);
	if (pcs->buffer)
		pcs_free(pcs->buffer);
	pcs_free(pcs);
}

PCS_API void pcs_clone_userinfo(Pcs dst, Pcs src)
{
	struct pcs *pcs_dst = (struct pcs *)dst;
	struct pcs *pcs_src = (struct pcs *)src;
	if (pcs_dst->username) {
		pcs_free(pcs_dst->username);
		pcs_dst->username = NULL;
	}
	if (pcs_src->username)
		pcs_dst->username = pcs_utils_strdup(pcs_src->username);

	if (pcs_dst->password) {
		pcs_free(pcs_dst->password);
		pcs_dst->password = NULL;
	}
	if (pcs_src->password)
		pcs_dst->password = pcs_utils_strdup(pcs_src->password);

	if (pcs_dst->bdstoken) {
		pcs_free(pcs_dst->bdstoken);
		pcs_dst->bdstoken = NULL;
	}
	if (pcs_src->bdstoken)
		pcs_dst->bdstoken = pcs_utils_strdup(pcs_src->bdstoken);

	if (pcs_dst->bduss) {
		pcs_free(pcs_dst->bduss);
		pcs_dst->bduss = NULL;
	}
	if (pcs_src->bduss)
		pcs_dst->bduss = pcs_utils_strdup(pcs_src->bduss);

	if (pcs_dst->sysUID) {
		pcs_free(pcs_dst->sysUID);
	}
	if (pcs_src->sysUID)
		pcs_dst->sysUID = pcs_utils_strdup(pcs_src->sysUID);

	//if (pcs_dst->errmsg) {
	//	pcs_free(pcs_dst->errmsg);
	//	pcs_dst->errmsg = NULL;
	//}
	//if (pcs_src->errmsg)
	//	pcs_dst->errmsg = pcs_utils_strdup(pcs_src->errmsg);

	if (pcs_dst->public_key) {
		pcs_free(pcs_dst->public_key);
		pcs_dst->public_key = NULL;
	}
	if (pcs_src->public_key)
		pcs_dst->public_key = pcs_utils_strdup(pcs_src->public_key);

	if (pcs_dst->key) {
		pcs_free(pcs_dst->key);
		pcs_dst->key = NULL;
	}
	if (pcs_src->key)
		pcs_dst->key = pcs_utils_strdup(pcs_src->key);
}

PCS_API const char *pcs_sysUID(Pcs handle)
{
	struct pcs *pcs = (struct pcs *)handle;
	return pcs->sysUID;
}

PCS_API const char *pcs_strerror(Pcs handle)
{
	struct pcs *pcs = (struct pcs *)handle;
	return pcs->errmsg;
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
    case PCS_OPTION_INPUT_FUNCTION:
        pcs->input_func = (PcsInputFunction)value;
        break;
    case PCS_OPTION_INPUT_FUNCTION_DATA:
        pcs->input_func_data = value;
        break;
	case PCS_OPTION_DOWNLOAD_WRITE_FUNCTION:
		//pcs_http_setopt(pcs->http, PCS_HTTP_OPTION_HTTP_WRITE_FUNCTION, value);
		pcs->download_func = (PcsHttpWriteFunction)value;
		break;
	case PCS_OPTION_DOWNLOAD_WRITE_FUNCTION_DATA:
		//pcs_http_setopt(pcs->http, PCS_HTTP_OPTION_HTTP_WRITE_FUNCTION_DATE, value);
		pcs->download_data = value;
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
	case PCS_OPTION_USAGE:
		pcs_http_setopt(pcs->http, PCS_HTTP_OPTION_USAGE, value);
		break;
	default:
		pcs_set_errmsg(handle, "Unknown option");
		res = PCS_UNKNOWN_OPT;
		break;
	case PCS_OPTION_TIMEOUT:
		pcs_http_setopt(pcs->http, PCS_HTTP_OPTION_TIMEOUT, value);
		break;
	case PCS_OPTION_CONNECTTIMEOUT:
		pcs_http_setopt(pcs->http, PCS_HTTP_OPTION_CONNECTTIMEOUT, value);
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
	const char *html, *errmsg;
	int http_code;

	pcs_clear_errmsg(handle);
	html = pcs_http_get(pcs->http, URL_DISK_HOME, PcsTrue);
	http_code = pcs_http_code(pcs->http);
	if (http_code != 200) {
		if (http_code != 302) {
			errmsg = pcs_http_strerror(pcs->http);
			if (errmsg)
				pcs_set_serrmsg(handle, errmsg);
			else
				pcs_set_errmsg(handle, "The server response wrong http code.");
			return PCS_NETWORK_ERROR;
		}
		return PCS_NOT_LOGIN;
	}
	if (!html) {
		errmsg = pcs_http_strerror(pcs->http);
		if (errmsg)
			pcs_set_serrmsg(handle, errmsg);
		else
			pcs_set_errmsg(handle, "Can't get response from the remote server.");
		return PCS_NETWORK_ERROR;
	}
	
	if (pcs->bdstoken) pcs_free(pcs->bdstoken);
	pcs->bdstoken = pcs_get_value_by_key(html, "yunData.MYBDSTOKEN");
	if (!pcs->bdstoken || strlen(pcs->bdstoken) == 0) {
		pcs->bdstoken = pcs_get_value_by_key(html, "FileUtils.bdstoken");
	}
	if (!pcs->bdstoken || strlen(pcs->bdstoken) == 0) {
		cJSON *json, *item;
		char *jsonData = pcs_get_yunData(html, "context");
		//char *jsonData = pcs_get_yunData(html, "yunData.setData");
		if (!jsonData) {
			return PCS_NOT_LOGIN;
		}
		json = cJSON_Parse(jsonData);
		if (!json) {
			pcs_free(jsonData);
			return PCS_NOT_LOGIN;
		}
		item = cJSON_GetObjectItem(json, "bdstoken");
		if (!item || !item->valuestring || strlen(item->valuestring) == 0) {
			pcs_free(jsonData);
			cJSON_Delete(json);
			return PCS_NOT_LOGIN;
		}
		pcs->bdstoken = pcs_utils_strdup(item->valuestring);
		if (pcs->bduss) pcs_free(pcs->bduss);
		pcs->bduss = pcs_http_get_cookie(pcs->http, "BDUSS");
		if (pcs->sysUID) pcs_free(pcs->sysUID);
		item = cJSON_GetObjectItem(json, "username");
		if (item && item->valuestring) {
			pcs->sysUID = pcs_utils_strdup(item->valuestring);
		}
		pcs_free(jsonData);
		cJSON_Delete(json);
		return PCS_LOGIN;
	}
	if (pcs->bdstoken != NULL && strlen(pcs->bdstoken) > 0) {
		//printf("bdstoken: %s\n", pcs->bdstoken);
		if (pcs->bduss) pcs_free(pcs->bduss);
		pcs->bduss = pcs_http_get_cookie(pcs->http, "BDUSS");
		if (pcs->sysUID) pcs_free(pcs->sysUID);
		pcs->sysUID = pcs_get_value_by_key(html, "yunData.MYNAME");
		if (!pcs->sysUID || strlen(pcs->sysUID) == 0) {
			pcs->sysUID = pcs_get_value_by_key(html, "FileUtils.sysUID");
		}
		return PCS_LOGIN;
	}
	return PCS_NOT_LOGIN;
}

static PcsRes pcs_prelogin(__in Pcs handle,
	__out char **token, __out char **code_string)
{
	struct pcs *pcs = (struct pcs *)handle;
	char *html, *url, *p;
	const char *errmsg;
	cJSON *json, *root, *item;

	pcs_clear_errmsg(handle);

	html = pcs_http_get(pcs->http, URL_HOME, PcsTrue);
	if (!html) {
		errmsg = pcs_http_strerror(pcs->http);
		if (errmsg)
			pcs_set_serrmsg(handle, errmsg);
		else
			pcs_set_errmsg(handle, "Can't get response from the remote server.");
		return PCS_NETWORK_ERROR;
	}

	url = pcs_utils_sprintf(URL_PASSPORT_API "getapi" 
		"&tpl=netdisk" "&apiver=v3" "&tt=%s"
		"&class=login" "&logintype=basicLogin"
		"&callback=bd__cbs__pwxtn7",
		pcs_js_timestr());
	html = pcs_http_get(pcs->http, url, PcsTrue);
	pcs_free(url);
	if (!html) {
		errmsg = pcs_http_strerror(pcs->http);
		if (errmsg)
			pcs_set_serrmsg(handle, errmsg);
		else
			pcs_set_errmsg(handle, "Can't get response from the remote server.");
		return PCS_NETWORK_ERROR;
	}

	json = cJSON_Parse(extract_json_from_callback(html));
	if (!json) {
		pcs_set_errmsg(handle, "Can't parse the response as object. Response: %s", html);
		return PCS_WRONG_RESPONSE;
	}

	root = cJSON_GetObjectItem(json, "data");
	if (!root) {
		pcs_set_errmsg(handle, "Can't read res.data. Response: %s", html);
		cJSON_Delete(json);
		return PCS_WRONG_RESPONSE;
	}

	item = cJSON_GetObjectItem(root, "token");
	if (!item) {
		pcs_set_errmsg(handle, "Can't read res.token. Response: %s", html);
		cJSON_Delete(json);
		return PCS_WRONG_RESPONSE;
	}
	*token = pcs_utils_strdup(item->valuestring);
	
	item = cJSON_GetObjectItem(root, "codeString");
	if (!item) {
		pcs_set_errmsg(handle, "Can't read res.codeString. Response: %s", html);
		cJSON_Delete(json);
		return PCS_WRONG_RESPONSE;
	}
	*code_string = pcs_utils_strdup(item->valuestring);

	cJSON_Delete(json);

	p = *token;
	while (*p) {
		if (!((*p >= '0' && *p <= '9') || (*p >= 'a' && *p <= 'f') || (*p >= 'A' && *p <= 'F'))) {
			pcs_set_errmsg(handle, "Invalid token. Token: %s", *token);
			return PCS_WRONG_RESPONSE;
		}
		p++;
	}

	url = pcs_utils_sprintf(URL_PASSPORT_API "logincheck" 
		"&token=%s" "&tpl=netdisk" 
		"&apiver=v3" "&tt=%s" 
		"&username=%s" "&isphone=false"
		"&callback=bd__cbs__q4ztud",
		*token, pcs_js_timestr(), pcs->username);
	html = pcs_http_get(pcs->http, url, PcsTrue);
	pcs_free(url);
	if (!html) {
		errmsg = pcs_http_strerror(pcs->http);
		if (errmsg)
			pcs_set_serrmsg(handle, errmsg);
		else
			pcs_set_errmsg(handle, "Can't get response from the remote server.");
		return PCS_NETWORK_ERROR;
	}
	json = cJSON_Parse(extract_json_from_callback(html));
	if (!json) {
		pcs_set_errmsg(handle, "Can't parse the response as object. Response: %s", html);
		return PCS_WRONG_RESPONSE;
	}
	root = cJSON_GetObjectItem(json, "data");
	if (!root) {
		pcs_set_errmsg(handle, "Can't read res.data. Response: %s", html);
		cJSON_Delete(json);
		return PCS_WRONG_RESPONSE;
	}
	item = cJSON_GetObjectItem(root, "codeString");
	if (!item) {
		pcs_set_errmsg(handle, "Can't read res.codeString. Response: %s", html);
		cJSON_Delete(json);
		return PCS_WRONG_RESPONSE;
	}
	pcs_free(*code_string);
	*code_string = pcs_utils_strdup(item->valuestring);

	cJSON_Delete(json);

	return PCS_OK;
}

static char *pcs_append_substr(char *dst, char **src,
	const char *begin, const char *end)
{
	char *first = strstr(*src, begin),
		*last;
	if (first) {
		first += strlen(begin);
		last = strstr(first, end);
		if (last) {
			dst = pcs_utils_strcat(dst, first, last - first, 1);
			*src = last + strlen(end);
		}
	}
	return dst;
}

static void escape_jump_url(char *url)
{
	char *p = url, *s = url;
	while (*p) {
		if (*p == '\\') {
			*(s++) = *(++p);
		}
		else {
			*(s++) = *p;
		}
		++p;
	}
	*s = '\0';
}

static char *pcs_get_pass_v3_jump_url(__in Pcs handle, char *html)
{
	struct pcs *pcs = (struct pcs *)handle;
	char *url = NULL, *accounts = NULL, *p = html;

	url = pcs_append_substr(url, &p, "decodeURIComponent(\"", "\")");
	escape_jump_url(url);

	url = pcs_utils_strcat(url, "?", 1, 1);

	accounts = pcs_append_substr(accounts, &p, "var accounts = '", "'");

	url = pcs_append_substr(url, &p, "href += \"", "\"");

	if (accounts) {
		url = pcs_utils_strcat(url, accounts, strlen(accounts), 1);
		pcs_free(accounts);
	}

	return url;
}

static PcsRes pcs_dologin(__in Pcs handle,
	__in int64_t starttime,
	__in const char *code_string,
	__in const char *token,
	__out int *errorno,
	__out char **out_html)
{
	struct pcs *pcs = (struct pcs *)handle;
	PcsRes res;
	char captch[32], *post_data, *html, *dv;
	const char *errmsg;

	memset(captch, 0, sizeof(captch));

	if (code_string && strlen(code_string)) {
		res = pcs_get_captcha(pcs, code_string, captch, sizeof(captch));
		if (res != PCS_OK) {
			pcs_set_errmsg(handle, "canceled");
			return res;
		}
	}

	if (passport_build_dv(&dv, starttime, pcs->username)) {
		pcs_set_errmsg(handle, "Can't build 'dv'.");
		return PCS_BUILD_POST_DATA;
	}

	post_data = pcs_http_build_post_data(pcs->http,
		"staticpage", "http://pan.baidu.com/res/static/thirdparty/pass_v3_jump.html",
		"charset", "utf-8",
		"token", token,
		"tpl", "netdisk",
		"subpro", "",
		"apiver", "v3",
		"tt", pcs_js_timestr(),
		"codestring", code_string,
		"safeflg", "0",
		"u", "http://pan.baidu.com/",
		"isPhone", "",
		"quick_user", "0",
		"logintype", "basicLogin",
		"logLoginType", "pc_loginBasic",
		"idc", "",
		"loginmerge", "true",
		"username", pcs->username,
		"password", pcs->password,
		"verifycode", captch,
		"mem_pass", "on",
		"rsakey", "",
		"crypttype", "",
		"ppui_logintime", "2602",
		"dv", dv,
		"callback", "parent.bd__pcbs__msdlhs",
		NULL);

	pcs_free(dv);

	if (!post_data) {
		pcs_set_errmsg(handle, "Can't build the post data.");
		return PCS_BUILD_POST_DATA;
	}
	html = pcs_http_post(pcs->http, URL_PASSPORT_API "login", post_data, PcsTrue);
	pcs_free(post_data);
	if (!html) {
		errmsg = pcs_http_strerror(pcs->http);
		if (errmsg)
			pcs_set_serrmsg(handle, errmsg);
		else
			pcs_set_errmsg(handle, "Can't get response from the remote server.");
		return PCS_NETWORK_ERROR;
	}
	else {
		char *jump_url,
			*errorStr = pcs_get_embed_query_int_value_by_key(html, "&error");
		if (!errorStr) errorStr = pcs_get_embed_query_int_value_by_key(html, "err_no");
		if (!errorStr) {
			pcs_set_errmsg(handle, "Can't read the error from the response. Response: %s", html);
			return PCS_NETWORK_ERROR;
		}
		*errorno = atoi(errorStr);
		pcs_free(errorStr);

		if (out_html != NULL)
			*out_html = pcs_utils_strdup(html);

		/* TODO: 应该不需要叫一次 jump_url 的，待验证？ */
		jump_url = pcs_get_pass_v3_jump_url(handle, html);
		html = pcs_http_get(pcs->http, jump_url, PcsTrue);
		pcs_free(jump_url);
		if (pcs_http_code(pcs->http) != 200) {
			pcs_set_errmsg(handle, "GET '%s' failed.", jump_url);
			return PCS_NETWORK_ERROR;
		}

		return PCS_OK;
	}
}

static PcsRes pcs_call_authwidgetverify(__in Pcs handle,
	__in const char *authtoken,
	__in const char *vtype,
	__in const char *action,
	__in const char *vcode,
	__out int *errorno,
	__out char **authsid,
	__out PcsSList2 **pdata,
	__out char **pmsg)
{
	struct pcs *pcs = (struct pcs *)handle;
	char *url, *html;
	const char *errmsg;
	cJSON *json, *root, *item;

	url = pcs_utils_sprintf(URL_AUTHWIDGET_VERIFY, authtoken, vtype, action, vcode);
	html = pcs_http_get(pcs->http, url, PcsTrue);
	pcs_free(url);
	if (!html) {
		errmsg = pcs_http_strerror(pcs->http);
		if (errmsg)
			pcs_set_serrmsg(handle, errmsg);
		else
			pcs_set_errmsg(handle, "Can't get response from the remote server.");
		return PCS_NETWORK_ERROR;
	}

	json = cJSON_Parse(extract_json_from_callback(html));
	if (!json) {
		pcs_set_errmsg(handle, "Can't parse the response as object. Response: %s", html);
		return PCS_WRONG_RESPONSE;
	}

	item = cJSON_GetObjectItem(json, "errno");
	if (!item || item->type != cJSON_String) {
		pcs_set_errmsg(handle, "Can't read res.errno. Response: %s", html);
		cJSON_Delete(json);
		return PCS_WRONG_RESPONSE;
	}
	*errorno = atoi(item->valuestring);

	if (pmsg) {
		item = cJSON_GetObjectItem(json, "msg");
		if (item)
			*pmsg = pcs_utils_strdup(item->valuestring);
		else
			*pmsg = NULL;
	}

	if (authsid) {
		item = cJSON_GetObjectItem(json, "authsid");
		if (item)
			*authsid = pcs_utils_strdup(item->valuestring);
		else
			*authsid = NULL;
	}

	if (pdata) {
		PcsSList2 *data = NULL;
		root = cJSON_GetObjectItem(json, "data");
		if (root && root->type == cJSON_Object) {
			static const char *verfify_items[] = { "softtoken", "mobile", "email", "password" };
			int i, len = sizeof(verfify_items) / sizeof(verfify_items[0]);
			for (i = 0; i < len; i++) {
				item = cJSON_GetObjectItem(root, verfify_items[i]);
				if (item && item->type == cJSON_String && strlen(item->valuestring) > 0) {
					if (data == NULL)
						data = pcs_slist2_create_ex(
							verfify_items[i], strlen(verfify_items[i]),
							item->valuestring, strlen(item->valuestring));
					else
						pcs_slist2_add_ex(data,
							verfify_items[i], strlen(verfify_items[i]),
							item->valuestring, strlen(item->valuestring));
				}
			}
		}
		*pdata = data;
	}
	
	cJSON_Delete(json);

	return PCS_OK;
}

static PcsRes pcs_call_auththaw(__in Pcs handle,
	__in const char *gotourl,
	__in const char *authsid)
{
	struct pcs *pcs = (struct pcs *)handle;
	char *url = NULL, *html;
	const char *errmsg;
	cJSON *json, *item;

	url = pcs_utils_strcat(url, gotourl, -1, 1);
	url = pcs_utils_strcat(url, "&authsid=", -1, 1);
	url = pcs_utils_strcat(url, authsid, -1, 1);
	url = pcs_utils_strcat(url, "&apiver=v3", -1, 1);
	url = pcs_utils_strcat(url, pcs_js_timestr(), -1, 1);
	url = pcs_utils_strcat(url, "&callback=bd__cbs__19usfv", -1, 1);

	html = pcs_http_get(pcs->http, url, PcsTrue);
	pcs_free(url);
	if (!html) {
		errmsg = pcs_http_strerror(pcs->http);
		if (errmsg)
			pcs_set_serrmsg(handle, errmsg);
		else
			pcs_set_errmsg(handle, "Can't get response from the remote server.");
		return PCS_NETWORK_ERROR;
	}

	json = cJSON_Parse(extract_json_from_callback(html));
	if (!json) {
		pcs_set_errmsg(handle, "Can't parse the response as object. Response: %s", html);
		return PCS_WRONG_RESPONSE;
	}

	item = cJSON_GetObjectItem(json, "errno");
	if (!item || item->type != cJSON_String) {
		pcs_set_errmsg(handle, "Can't read res.errno. Response: %s", html);
		cJSON_Delete(json);
		return PCS_WRONG_RESPONSE;
	}

	if (atoi(item->valuestring) != 0) {
		item = cJSON_GetObjectItem(json, "msg");
		if (item && item->type == cJSON_String && strlen(item->valuestring) > 0)
			pcs_set_serrmsg(handle, item->valuestring);
		else
			pcs_set_errmsg(handle, "Call auththaw api fail.");
		cJSON_Delete(json);
		return PCS_FAIL;
	}

	cJSON_Delete(json);

	return PCS_OK;
}

static char *build_verify_api_list_string(__in PcsSList2 *list,
	__out char *buf, __in int buf_len)
{
	int len, id;
	PcsSList2 *api;
	strncpy(buf, "Verify API:\n", buf_len);
	len = strlen(buf);

	api = list;
	id = 1;
	while (api) {
		snprintf(buf + len, buf_len - len,
			"  %d) %s \t%s\n",
			id, api->string1, api->string2);
		api = api->next;
		id++;
		len = strlen(buf);
	}
	strncat(buf, "Please input verify method number (default 1): ", buf_len);
	return buf;
}

static PcsSList2 *get_verify_api_by_index(__in PcsSList2 *list, __in int index)
{
	PcsSList2 * api = list;
	int i = 0;
	while (api) {
		if (i == index) {
			return api;
		}
		else {
			api = api->next;
			i++;
		}
	}
	return NULL;
}

static PcsRes pcs_do_auth_verify(__in Pcs handle,
	__in const char *authtoken,
	__out int *res_error,
	__out char **authsid)
{
	struct pcs *pcs = (struct pcs *)handle;
	PcsRes res;
	int error;
	char *msg, tips[PCS_MSG_BUFFER_SIZE], vm[128], captch[32];
	PcsSList2 *verify_api_list, *selected_item;

	res = pcs_call_authwidgetverify(handle, authtoken, "", "getapi", "",
		&error, NULL, &verify_api_list, &msg);
	*res_error = error;
	if (res != PCS_OK) {
		return res;
	}
	else if (error != 110000) {
		if (msg)
			pcs_set_serrmsg(handle, msg);
		else
			pcs_set_errmsg(handle, "Can't get verify api.");
		pcs_free(msg);
		return PCS_FAIL;
	}

	pcs_free(msg);

	if (verify_api_list == NULL) {
		pcs_set_errmsg(handle, "no verify api.");
		return PCS_FAIL;
	}

	memset(tips, 0, sizeof(tips));
	build_verify_api_list_string(verify_api_list, tips, sizeof(tips) - 1);

	memset(vm, 0, sizeof(vm));
	while (strlen(vm) == 0) {
		int tid;
		res = pcs_input(pcs, tips, vm, sizeof(vm));
		if (res != PCS_OK) {
			pcs_set_errmsg(handle, "canceled");
			pcs_slist2_destroy(verify_api_list);
			return res;
		}
		if (strlen(vm) == 0)
			tid = 1;
		else
			tid = atoi(vm);
		memset(vm, 0, sizeof(vm));
		selected_item = get_verify_api_by_index(verify_api_list, tid - 1);
		if (selected_item != NULL) {
			strncpy(vm, selected_item->string1, sizeof(vm) - 1);
			break;
		}
	}

	res = pcs_call_authwidgetverify(handle, authtoken, vm, "send", "",
		&error, NULL, NULL, &msg);
	*res_error = error;
	if (res != PCS_OK) {
		pcs_slist2_destroy(verify_api_list);
		return res;
	}
	else if (error != 110000) {
		if (msg)
			pcs_set_errmsg(handle, "Can't send verify code to \"%s\": %s", selected_item->string2, msg);
		else
			pcs_set_errmsg(handle, "Can't send verify code to \"%s\".", selected_item->string2);
		pcs_free(msg);
		pcs_slist2_destroy(verify_api_list);
		return PCS_FAIL;
	}

	pcs_free(msg);

	memset(tips, 0, sizeof(tips));
	if (strncmp(vm, "mobile", sizeof("mobile") - 1) == 0)
		snprintf(tips, sizeof(tips) - 1,
			"Please input %s password: ",
			"sms");
	else if (strncmp(vm, "email", sizeof("email") - 1) == 0)
		snprintf(tips, sizeof(tips) - 1,
			"Please input %s password: ",
			"email");

	memset(captch, 0, sizeof(captch));
	res = pcs_input(pcs, tips, captch, sizeof(captch));
	if (res != PCS_OK) {
		pcs_set_errmsg(handle, "canceled");
		pcs_slist2_destroy(verify_api_list);
		return res;
	}

	res = pcs_call_authwidgetverify(handle, authtoken, vm, "check", captch,
		&error, authsid, NULL, &msg);
	*res_error = error;
	if (res != PCS_OK) {
		pcs_slist2_destroy(verify_api_list);
		return res;
	}
	else if (error != 110000) {
		if (msg)
			pcs_set_errmsg(handle, "Can't verify: %s", msg);
		else
			pcs_set_serrmsg(handle, "Can't verify.");
		pcs_free(msg);
		pcs_slist2_destroy(verify_api_list);
		return PCS_FAIL;
	}
	pcs_free(msg);
	pcs_slist2_destroy(verify_api_list);

	return PCS_OK;
}

PCS_API PcsRes pcs_login(Pcs handle)
{
	struct pcs *pcs = (struct pcs *)handle;
	PcsRes res;
	char *html = NULL;
	char *token = NULL, *code_string = NULL;
	int error = -1, retry_times;
	int64_t starttime;

	res = pcs_prelogin(handle, &token, &code_string);
	if (res != PCS_OK) {
		pcs_free(token);
		pcs_free(code_string);
		return res;
	}

	starttime = pcs_jstime() - 20000; /* 设置为 20秒 前打开的页面 */

	retry_times = 0;

try_login:
	
	pcs_free(html);
	html = NULL;
	res = pcs_dologin(handle, starttime, code_string, token, &error, &html);
	if (res != PCS_OK) {
		pcs_free(token);
		pcs_free(code_string);
		pcs_free(html);
		return res;
	}

	/*  see error code map: https://github.com/GangZhuo/BaiduPCS/issues/29 */
	if (retry_times < 3 && (error == 3 || error == 6 || error == 257 || error == 200010)) {
		if (code_string) pcs_free(code_string);
		code_string = pcs_get_embed_query_token_by_key(html, "&codestring");
		if (!code_string)
			code_string = pcs_get_embed_query_token_by_key(html, "&codeString");
		if (!code_string) {
			pcs_set_errmsg(handle, "Can't read the codestring from the response. Response: %s", html);
			pcs_free(token);
			pcs_free(code_string);
			pcs_free(html);
			return PCS_FAIL;
		}
		pcs_set_errmsg(handle, "%s", get_login_errmsg(error));
		retry_times++;
		goto try_login;
	}
	else if (error == 0 || error == 18
		|| error == 400032 || error == 400034 || error == 400037 || error == 400401) {
		if (pcs_islogin(pcs) == PCS_LOGIN) {
			pcs_free(token);
			pcs_free(code_string);
			pcs_free(html);
			return PCS_OK;
		}
	}
    else if (error == 120019 || error == 120021) {
		char *authtoken = NULL;
		char *gotourl = NULL;
		char *authsid = NULL;

		authtoken = pcs_get_embed_query_token_by_key(html, "&authtoken");
		gotourl = pcs_get_embed_query_token_by_key(html, "&gotourl");

		res = pcs_do_auth_verify(handle, authtoken, &error, &authsid);
		if (res != PCS_OK) {
			pcs_free(authtoken);
			pcs_free(gotourl);
			pcs_free(authsid);
			pcs_free(token);
			pcs_free(code_string);
			pcs_free(html);
			return res;
		}

        pcs_free(authtoken);

		if (gotourl) {
			pcs_http_url_decode(pcs->http, gotourl);
			res = pcs_call_auththaw(handle, gotourl, authsid);
			pcs_free(gotourl);
			if (res != PCS_OK) {
				pcs_free(authsid);
				pcs_free(token);
				pcs_free(code_string);
				pcs_free(html);
				return res;
			}
		}

		pcs_free(authsid);

		retry_times = 0;

		pcs_free(code_string);
		code_string = NULL;

		goto try_login;
    }

	pcs_set_errmsg(handle, "error: %d %s", error, get_login_errmsg(error));
	pcs_free(token);
	pcs_free(code_string);
	pcs_free(html);
	return PCS_FAIL;
}

PCS_API PcsRes pcs_logout(Pcs handle)
{
	struct pcs *pcs = (struct pcs *)handle;
	int http_code;
	const char *errmsg;

	pcs_clear_errmsg(handle);
	pcs_http_get(pcs->http, URL_PASSPORT_LOGOUT, PcsFalse);
	http_code = pcs_http_code(pcs->http);
	if (http_code == 302) {
		if (pcs->username) {
			pcs_free(pcs->username);
			pcs->username = NULL;
		}
		if (pcs->password) {
			pcs_free(pcs->password);
			pcs->password = NULL;
		}
		if (pcs->bdstoken) {
			pcs_free(pcs->bdstoken);
			pcs->bdstoken = NULL;
		}
		if (pcs->bduss) {
			pcs_free(pcs->bduss);
			pcs->bduss = NULL;
		}
		if (pcs->sysUID) {
			pcs_free(pcs->sysUID);
			pcs->sysUID = NULL;
		}
		return PCS_OK;
	}
	errmsg = pcs_http_strerror(pcs->http);
	if (errmsg)
		pcs_set_serrmsg(handle, errmsg);
	else
		pcs_set_errmsg(handle, "Can't logout. Http Code: %d", http_code);
	return PCS_FAIL;
}

PCS_API PcsRes pcs_quota(Pcs handle, int64_t *quota, int64_t *used)
{
	struct pcs *pcs = (struct pcs *)handle;
	cJSON *json, *item;
	char *url, *html;
	int error;

	pcs_clear_errmsg(handle);
	url = pcs_build_pan_api_url(handle, "quota", NULL);
	if (!url) {
		pcs_set_errmsg(handle, "Can't build the url.");
		return PCS_BUILD_URL;
	}
	html = pcs_http_get(pcs->http, url, PcsTrue);
	pcs_free(url);
	if (!html) {
		const char *errmsg = pcs_http_strerror(pcs->http);
		if (errmsg)
			pcs_set_serrmsg(handle, errmsg);
		else
			pcs_set_errmsg(handle, "Can't get response from the remote server.");
		return PCS_NETWORK_ERROR;
	}

	json = cJSON_Parse(html);
	if (!json) {
		pcs_set_errmsg(handle, "Can't parse the response as json. Response: %s", html);
		return PCS_WRONG_RESPONSE;
	}

	item = cJSON_GetObjectItem(json, "errno");
	if (!item) {
		pcs_set_errmsg(handle, "Can't read res.errno. Response: %s", html);
		cJSON_Delete(json);
		return PCS_WRONG_RESPONSE;
	}
	error = item->valueint;
	if (error != 0) {
		pcs_set_errmsg(handle, "Unknown error. Response: %s", html);
		cJSON_Delete(json);
		return PCS_FAIL;
	}

	item = cJSON_GetObjectItem(json, "total");
	if (!item) {
		pcs_set_errmsg(handle, "Can't read res.total. Response: %s", html);
		cJSON_Delete(json);
		return PCS_WRONG_RESPONSE;
	}
	if (quota)
		*quota = (int64_t)item->valuedouble;

	item = cJSON_GetObjectItem(json, "used");
	if (!item) {
		pcs_set_errmsg(handle, "Can't read res.used. Response: %s", html);
		cJSON_Delete(json);
		return PCS_WRONG_RESPONSE;
	}
	if (used)
		*used = (int64_t)item->valuedouble;

	cJSON_Delete(json);

	return PCS_OK;
}

PCS_API PcsRes pcs_mkdir(Pcs handle, const char *path)
{
	struct pcs *pcs = (struct pcs *)handle;
	int error;
	char *html, *postdata, *url;

	pcs_clear_errmsg(handle);
	url = pcs_build_pan_api_url(handle, "create", "a", "commit", NULL);
	if (!url) {
		pcs_set_errmsg(handle, "Can't build the url.");
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
		pcs_set_errmsg(handle, "Can't build the post data.");
		pcs_free(url);
		return PCS_BUILD_POST_DATA;
	}
	html = pcs_http_post(pcs->http, url, postdata, PcsTrue);
	pcs_free(url);
	pcs_free(postdata);
	if (!html) {
		const char *errmsg = pcs_http_strerror(pcs->http);
		if (errmsg)
			pcs_set_serrmsg(handle, errmsg);
		else
			pcs_set_errmsg(handle, "Can't get response from the remote server.");
		return PCS_NETWORK_ERROR;
	}
	error = pcs_get_errno_from_api_res(handle, html);
	if (error) {
		return PCS_FAIL;
	}
	return PCS_OK;
}

PCS_API PcsFileInfoList *pcs_list(Pcs handle, const char *dir, int pageindex, int pagesize, const char *order, PcsBool desc)
{
	struct pcs *pcs = (struct pcs *)handle;
	char *page, *pagenum;
	PcsFileInfoList *filist = NULL;
	pcs_clear_errmsg(handle);
	page = pcs_utils_sprintf("%d", pageindex);
	pagenum = pcs_utils_sprintf("%d", pagesize);
	filist = pcs_pan_api_1(handle, "list", 
		"_", pcs_js_timestr(),
		"dir", dir,
		"page", page,
		"num", pagenum,
		"order", order,
		desc ? "desc" : "", desc ? "1" : "0",
		NULL);
	pcs_free(page);
	pcs_free(pagenum);
	return filist;
}

PCS_API PcsFileInfoList *pcs_search(Pcs handle, const char *dir, const char *key, PcsBool recursion)
{
	struct pcs *pcs = (struct pcs *)handle;
	PcsFileInfoList *filist = NULL;
	pcs_clear_errmsg(handle);
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
	
	pcs_clear_errmsg(handle);
	dir = pcs_utils_basedir(path);
	key = pcs_utils_filename(path);
	if (strlen(key) > 64) {
		unsigned char *p = (unsigned char *)key + 64;
		while ((*p) > 127 && (((*p) & 0xC0) != 0xC0) && p > (unsigned char *)key)
			p--;
		*p = '\0';
	}
	filist = pcs_search(handle, dir, key, PcsFalse);
	pcs_free(dir);
	pcs_free(key);
	if (!filist)
		return NULL;
	pcs_filist_iterater_init(filist, &iterater, PcsFalse);
	while(pcs_filist_iterater_next(&iterater)) {
		if (pcs_utils_strcmpi(path, iterater.current->path) == 0) {
			meta = iterater.current;
			break;
		}
	}
	if (meta) {
		meta = pcs_fileinfo_clone(meta);
		if (!meta) {
			pcs_set_errmsg(handle, "Can't clone the meta data.");
		}
	}
	else {
		pcs_set_errmsg(handle, "Can't find the file.");
	}
	pcs_filist_destroy(filist);
	return meta;
}

PCS_API PcsPanApiRes *pcs_delete(Pcs handle, PcsSList *slist)
{
	char *filelist;
	PcsPanApiRes *res;
	int file_count;
	pcs_clear_errmsg(handle);
	if (!slist) {
		pcs_set_errmsg(handle, "Wrong Arguments: No File");
		return NULL;
	}
	filelist = pcs_build_filelist_1(handle, slist, &file_count);
	if (!filelist) {
		return NULL;
	}
	res = pcs_pan_api_filemanager(handle, "delete", filelist, file_count);
	pcs_free(filelist);
	return res;
}

PCS_API PcsPanApiRes *pcs_rename(Pcs handle, PcsSList2 *slist)
{
	char *filelist;
	PcsPanApiRes *res;
	int file_count;
	pcs_clear_errmsg(handle);
	if (!slist) {
		pcs_set_errmsg(handle, "Wrong Arguments: No File");
		return NULL;
	}
	filelist = pcs_build_filelist_2(handle, slist, &file_count);
	if (!filelist) {
		return NULL;
	}
	res = pcs_pan_api_filemanager(handle, "rename", filelist, file_count);
	pcs_free(filelist);
	return res;
}

PCS_API PcsPanApiRes *pcs_move(Pcs handle, PcsSList2 *slist)
{
	char *filelist;
	PcsPanApiRes *res;
	int file_count;
	pcs_clear_errmsg(handle);
	if (!slist) {
		pcs_set_errmsg(handle, "Wrong Arguments: No File");
		return NULL;
	}
	filelist = pcs_build_filelist_3(handle, slist, &file_count);
	if (!filelist) {
		return NULL;
	}
	res = pcs_pan_api_filemanager(handle, "move", filelist, file_count);
	pcs_free(filelist);
	return res;
}

PCS_API PcsPanApiRes *pcs_copy(Pcs handle, PcsSList2 *slist)
{
	char *filelist;
	PcsPanApiRes *res;
	int file_count;
	pcs_clear_errmsg(handle);
	if (!slist) {
		pcs_set_errmsg(handle, "Wrong Arguments: No File");
		return NULL;
	}
	filelist = pcs_build_filelist_3(handle, slist, &file_count);
	if (!filelist) {
		return NULL;
	}
	res = pcs_pan_api_filemanager(handle, "copy", filelist, file_count);
	pcs_free(filelist);
	return res;
}

static PcsRes pcs_download_normal(Pcs handle, const char *path, PcsHttpWriteFunction write, void *write_state, curl_off_t max_speed, curl_off_t resume_from, curl_off_t max_length)
{
	struct pcs *pcs = (struct pcs *)handle;
	char *url;
	const char *errmsg;

	pcs_clear_errmsg(handle);
	if (!write) {
		pcs_set_errmsg(handle, "Please specify the write function.");
		return PCS_FAIL;
	}
	pcs_http_setopts(pcs->http,
		PCS_HTTP_OPTION_HTTP_WRITE_FUNCTION, write,
		PCS_HTTP_OPTION_HTTP_WRITE_FUNCTION_DATE, write_state,
		PCS_HTTP_OPTION_END);
	url = pcs_http_build_url(pcs->http, URL_PCS_REST,
		"method", "download",
		"app_id", "250528",
		"path", path,
		NULL);
	if (!url) {
		pcs_set_errmsg(handle, "Can't build the url.");
		return PCS_BUILD_URL;
	}
	if (pcs_http_get_download(pcs->http, url, PcsTrue, max_speed, resume_from, max_length)) {
		pcs_free(url);
		return PCS_OK;
	}
	pcs_free(url);
	errmsg = pcs_http_strerror(pcs->http);
	if (errmsg)
		pcs_set_serrmsg(handle, errmsg);
	else
		pcs_set_errmsg(handle, "Can't download the file: %s", pcs_http_strerror(pcs->http));
	return PCS_FAIL;
}

PCS_API PcsRes pcs_download(Pcs handle, const char *path, curl_off_t max_speed, curl_off_t resume_from, curl_off_t max_length)
{
	struct pcs *pcs = (struct pcs *)handle;
	return pcs_download_normal(handle, path, pcs->download_func, pcs->download_data, max_speed, resume_from, max_length);
}

PCS_API int64_t pcs_get_download_filesize(Pcs handle, const char *path)
{
    PcsFileInfo *meta;
    int64_t fs_sz = -1;
    meta = pcs_meta(handle, path);
	if (meta) {
	    if (!meta->isdir)
    		fs_sz = meta->size;
        pcs_fileinfo_destroy(meta);
	}
    return fs_sz;

	/*struct pcs *pcs = (struct pcs *)handle;
	char *url;
	const char *errmsg;
	int64_t size;

	pcs_clear_errmsg(handle);
	url = pcs_http_build_url(pcs->http, URL_PCS_REST,
		"method", "download",
		"app_id", "250528",
		"path", path,
		NULL);
	if (!url) {
		pcs_set_errmsg(handle, "Can't build the url.");
		return 0;
	}
	size = pcs_http_get_download_filesize(pcs->http, url, PcsTrue);
	pcs_free(url);
	errmsg = pcs_http_strerror(pcs->http);
	if (errmsg)
		pcs_set_serrmsg(handle, errmsg);
	return size;*/
}

static size_t pcs_cat_write_func(char *ptr, size_t size, size_t contentlength, void *userdata)
{
	struct pcs *pcs = (struct pcs *)userdata;
	size_t sz = pcs->buffer_size + size;
	char *buf = (char *)pcs_malloc(sz + 1);
	if (!buf) return 0;
	if (pcs->buffer) {
		memcpy(buf, pcs->buffer, pcs->buffer_size);
		memcpy(&buf[pcs->buffer_size], ptr, size);
		buf[sz] = '\0';
		pcs_free(pcs->buffer);
		pcs->buffer = buf;
		pcs->buffer_size = sz;
	}
	else {
		memcpy(buf, ptr, size);
		buf[sz] = '\0';
		pcs->buffer = buf;
		pcs->buffer_size = sz;
	}
	return size;
}

PCS_API const char *pcs_cat(Pcs handle, const char *path, size_t *dstsz)
{
	struct pcs *pcs = (struct pcs *)handle;
	PcsRes rc;

	if (pcs->buffer) pcs_free(pcs->buffer);
	pcs->buffer = NULL;
	pcs->buffer_size = 0;
	rc = pcs_download_normal(handle, path, &pcs_cat_write_func, pcs, 0, 0, 0);
	if (rc != PCS_OK) {
		return NULL;
	}
	if (dstsz) *dstsz = pcs->buffer_size;
	return pcs->buffer;
}

PCS_API PcsFileInfo *pcs_upload_buffer(Pcs handle, const char *path, PcsBool overwrite, const char *buffer, size_t buffer_size, curl_off_t max_speed)
{
	struct pcs *pcs = (struct pcs *)handle;
	char *filename;
	PcsHttpForm form = NULL;
	PcsFileInfo *meta;
	size_t sz = buffer_size;
	char *buf = (char *)buffer;

	pcs_clear_errmsg(handle);
	filename = pcs_utils_filename(path);
	if (pcs_http_form_addbuffer(pcs->http, &form, "file", buf, (long)sz, filename) != PcsTrue) {
		pcs_set_errmsg(handle, "Can't build the post data.");
		pcs_free(filename);
		if (buf != buffer) pcs_free(buf);
		return NULL;
	}
	pcs_free(filename);
	meta = pcs_upload_form(handle, path, form, max_speed, overwrite ? "overwrite" : "newcopy");
	pcs_http_form_destroy(pcs->http, form);
	if (buf != buffer) pcs_free(buf);
	return meta;
}

PCS_API PcsFileInfo *pcs_upload_slice(Pcs handle, const char *buffer, size_t buffer_size, curl_off_t max_speed)
{
	struct pcs *pcs = (struct pcs *)handle;
	PcsHttpForm form = NULL;
	PcsFileInfo *meta;
	size_t sz = buffer_size;
	char *buf = (char *)buffer;

	pcs_clear_errmsg(handle);
	if (pcs_http_form_addbuffer(pcs->http, &form, "file", buf, (long)sz, NULL) != PcsTrue) {
		pcs_set_errmsg(handle, "Can't build the post data.");
		if (buf != buffer) pcs_free(buf);
		return NULL;
	}
	meta = pcs_upload_slice_form(handle, form, max_speed);
	pcs_http_form_destroy(pcs->http, form);
	if (buf != buffer) pcs_free(buf);
	return meta;
}

PCS_API PcsFileInfo *pcs_upload_slicefile(Pcs handle, 
	size_t(*read_func)(void *ptr, size_t size, size_t nmemb, void *userdata),
	void *userdata,
	size_t content_size, curl_off_t max_speed)
{
	struct pcs *pcs = (struct pcs *)handle;
	PcsHttpForm form = NULL;
	PcsFileInfo *meta;

	pcs_clear_errmsg(handle);
	if (pcs_http_form_addbufferfile(pcs->http, &form, "file", NULL, read_func, userdata, content_size) != PcsTrue) {
		pcs_set_errmsg(handle, "Can't build the post data.");
		return NULL;
	}
	meta = pcs_upload_slice_form(handle, form, max_speed);
	pcs_http_form_destroy(pcs->http, form);
	return meta;
}

PCS_API PcsFileInfo *pcs_create_superfile(Pcs handle, const char *path, PcsBool overwrite, PcsSList *block_list)
{
	struct pcs *pcs = (struct pcs *)handle;
	char *url, *postdata, *block_list_data, *html;
	const char *errmsg;
	cJSON *json, *item;
	PcsFileInfo *meta;

	url = pcs_http_build_url(pcs->http, URL_PCS_REST,
		"method", "createsuperfile",
		"app_id", "250528",
		"path", path,
		"ondup", overwrite ? "overwrite" : "newcopy",
		"BDUSS", pcs->bduss,
		NULL);
	if (!url) {
		pcs_set_errmsg(handle, "Can't build the url.");
		return NULL;
	}
	block_list_data = pcs_build_filelist_1(handle, block_list, NULL);
	if (!block_list_data) {
		pcs_set_errmsg(handle, "Can't build the block list.");
		pcs_free(url);
		return NULL;
	}
	postdata = (char *)pcs_malloc(strlen(block_list_data) + 32);
	strcpy(postdata, "{\"block_list\":");
	strcat(postdata, block_list_data);
	strcat(postdata, "}");
	pcs_free(block_list_data);
	block_list_data = postdata;

	postdata = pcs_http_build_post_data(pcs->http, "param", block_list_data, NULL);
	pcs_free(block_list_data);
	if (!postdata) {
		pcs_set_errmsg(handle, "Can't build the post data.");
		pcs_free(url);
		return NULL;
	}
	html = pcs_http_post(pcs->http, url, postdata, PcsTrue);
	pcs_free(postdata);
	pcs_free(url);
	if (!html) {
		errmsg = pcs_http_strerror(pcs->http);
		if (errmsg)
			pcs_set_serrmsg(handle, errmsg);
		else
			pcs_set_errmsg(handle, "Can't get response from the remote server.");
		return NULL;
	}
	json = cJSON_Parse(html);
	if (!json) {
		pcs_set_errmsg(handle, "Can't parse the response as json: %s", html);
		return NULL;
	}
	/*{
		"path": "\/temp\/88.zip",
		"size": "1982432",
		"ctime": 1420209233,
		"mtime": 1420209233,
		"md5": "3058b1325c93ba9f24cb7a7ebbc03181",
		"fs_id": 214609865781387,
		"isdir": 0,
		"block_list": ["3058b1325c93ba9f24cb7a7ebbc03181"],
		"s3_handle": "3058b1325c93ba9f24cb7a7ebbc03181",
		"request_id": 1835246947
	}*/
	item = cJSON_GetObjectItem(json, "error_code");
	if (item) {
		int error_code = item->valueint;
		const char *error_msg = NULL;
		item = cJSON_GetObjectItem(json, "error_msg");
		if (item)
			error_msg = item->valuestring;
		pcs_set_errmsg(handle, "Can't upload file. error_code: %d. error_msg: %s. raw response: %s", error_code, error_msg, html);
		cJSON_Delete(json);
		return NULL;
	}

	meta = pcs_parse_fileinfo(json);
	cJSON_Delete(json);
	if (!meta) {
		pcs_set_errmsg(handle, "Can't parse the response as meta");
		return NULL;
	}
	return meta;
}

PCS_API PcsFileInfo *pcs_upload(Pcs handle, const char *path, PcsBool overwrite, 
									   const char *local_filename, curl_off_t max_speed)
{
	struct pcs *pcs = (struct pcs *)handle;
	char *filename;
	PcsHttpForm form = NULL;
	PcsFileInfo *meta;

	pcs_clear_errmsg(handle);
	filename = pcs_utils_filename(path);
	if (pcs_http_form_addfile(pcs->http, &form, "file", local_filename, filename) != PcsTrue) {
		pcs_set_errmsg(handle, "Can't build the post data.");
		pcs_free(filename);
		return NULL;
	}
	pcs_free(filename);
	meta = pcs_upload_form(handle, path, form, max_speed, overwrite ? "overwrite" : "newcopy");
	pcs_http_form_destroy(pcs->http, form);
	return meta;
}

PCS_API PcsFileInfo *pcs_upload_s(Pcs handle, const char *to_path, PcsBool overwrite,
	size_t(*read_func)(void *ptr, size_t size, size_t nmemb, void *userdata),
	void *userdata,
	size_t content_size, curl_off_t max_speed)
{
	struct pcs *pcs = (struct pcs *)handle;
	char *filename;
	PcsHttpForm form = NULL;
	PcsFileInfo *meta;

	pcs_clear_errmsg(handle);
	filename = pcs_utils_filename(to_path);
	if (pcs_http_form_addbufferfile(pcs->http, &form, "file", NULL, read_func, userdata, content_size) != PcsTrue) {
		pcs_set_errmsg(handle, "Can't build the post data.");
		return NULL;
	}
	meta = pcs_upload_form(handle, to_path, form, max_speed, overwrite ? "overwrite" : "newcopy");
	pcs_http_form_destroy(pcs->http, form);
	return meta;
}

PCS_API int64_t pcs_local_filesize(Pcs handle, const char *path)
{
	FILE *pf;
	int64_t sz = 0;
	pcs_clear_errmsg(handle);
	pf = fopen(path, "rb");
	if (!pf) {
		pcs_set_errmsg(handle, "Can't open the file. %s", path);
		return -1;
	}
	if (fseeko(pf, 0, SEEK_END)) {
		pcs_set_errmsg(handle, "fseeko() error. %s", path);
		fclose(pf);
		return -1;
	}
	sz = ftello(pf);
	fclose(pf);
	if (sz < 0) {
		pcs_set_errmsg(handle, "ftello() error. %s", path);
		return -1;
	}
	return sz;
}

PCS_API PcsBool pcs_md5_file(Pcs handle, const char *path, char *md5_buf)
{
	MD5_CTX md5;
	FILE *pf;
	size_t sz;
	unsigned char buf[PCS_BUFFER_SIZE];
	unsigned char md5_value[16];
	int i;

	pcs_clear_errmsg(handle);
	pf = fopen(path, "rb");
	if (!pf) {
		pcs_set_errmsg(handle, "Can't open the file. %s", path);
		return PcsFalse;
	}

	MD5_Init(&md5);
	while ((sz = fread(buf, 1, PCS_BUFFER_SIZE, pf)) > 0) {
		MD5_Update(&md5, buf, sz);
	}
	MD5_Final(md5_value, &md5);
	fclose(pf);
	for (i = 0; i < 16; i++) {
		sprintf(&md5_buf[i * 2], "%02x", md5_value[i]);
	}
	return PcsTrue;
}

PCS_API PcsBool pcs_md5_s(Pcs handle,
	size_t(*read_func)(void *ptr, size_t size, size_t nmemb, void *userdata),
	void *userdata,
	char *md5_buf)
{
	MD5_CTX md5;
	size_t sz;
	unsigned char buf[PCS_BUFFER_SIZE];
	unsigned char md5_value[16];
	int i;

	pcs_clear_errmsg(handle);

	MD5_Init(&md5);
	while (
		((sz = read_func(buf, 1, PCS_BUFFER_SIZE, userdata)) > 0)
		&& (sz != CURL_READFUNC_ABORT)
		&& (sz != CURL_READFUNC_PAUSE)) {
		MD5_Update(&md5, buf, sz);
	}
	if (sz >= 0 && sz != CURL_READFUNC_ABORT && sz != CURL_READFUNC_PAUSE) {
		MD5_Final(md5_value, &md5);
		for (i = 0; i < 16; i++) {
			sprintf(&md5_buf[i * 2], "%02x", md5_value[i]);
		}
		return PcsTrue;
	}
	return PcsFalse;
}

PCS_API PcsBool pcs_md5_file_slice(Pcs handle, const char *path, int64_t offset, int64_t length, char *md5_buf)
{
	MD5_CTX md5;
	FILE *pf;
	size_t sz;
	int64_t hashed_sz = 0;
	unsigned char buf[PCS_BUFFER_SIZE];
	unsigned char md5_value[16];
	int i;

	pcs_clear_errmsg(handle);
	pf = fopen(path, "rb");
	if (!pf) {
		pcs_set_errmsg(handle, "Can't open the file. %s", path);
		return PcsFalse;
	}

	if (fseeko(pf, offset, SEEK_SET)) {
		pcs_set_errmsg(handle, "fseeko() error. %s", path);
		fclose(pf);
		return PcsFalse;
	}

	MD5_Init(&md5);
	while ((hashed_sz < length) && ((sz = fread(buf, 1, PCS_BUFFER_SIZE, pf)) > 0)) {
		if (hashed_sz + sz > length) {
			MD5_Update(&md5, buf, (size_t)(length - hashed_sz));
			hashed_sz += (length - hashed_sz);
		}
		else {
			MD5_Update(&md5, buf, sz);
			hashed_sz += sz;
		}
	}
	MD5_Final(md5_value, &md5);
	fclose(pf);
	for (i = 0; i < 16; i++) {
		sprintf(&md5_buf[i * 2], "%02x", md5_value[i]);
	}
	return PcsTrue;
}

PCS_API PcsFileInfo *pcs_rapid_upload(Pcs handle, const char *path, PcsBool overwrite, const char *local_filename, char *out_content_md5, char *out_slice_md5)
{
	struct pcs *pcs = (struct pcs *)handle;
	int64_t content_length;
	char content_md5[33] = { 0 }, slice_md5[33] = { 0 };

	pcs_clear_errmsg(handle);
	content_length = pcs_local_filesize(handle, local_filename);
	if (content_length < 0) {
		return NULL;
	}
	if (content_length <= PCS_RAPIDUPLOAD_THRESHOLD) {
		pcs_set_errmsg(handle, "The file size is not satisfied, the file must be great than %d.", PCS_RAPIDUPLOAD_THRESHOLD);
		return NULL;
	}

	if (out_content_md5 != NULL && strnlen(out_content_md5, 33) == 32) {
		strcpy(content_md5, out_content_md5);
	}
	else {
		if (!pcs_md5_file(handle, local_filename, content_md5)) {
			return NULL;
		}
		if (out_content_md5) strcpy(out_content_md5, content_md5);
	}

	if (out_slice_md5 != NULL && strnlen(out_slice_md5, 33) == 32) {
		strcpy(slice_md5, out_slice_md5);
	}
	else {
		if (!pcs_md5_file_slice(handle, local_filename, 0, PCS_RAPIDUPLOAD_THRESHOLD, slice_md5)) {
			return NULL;
		}
		if (out_slice_md5) strcpy(out_slice_md5, slice_md5);
	}

	return pcs_rapid_upload_r(handle, path, overwrite, content_length, content_md5, slice_md5);
}

PCS_API PcsFileInfo *pcs_rapid_upload_r(Pcs handle, const char *path, PcsBool overwrite,
	int64_t content_length, const char *content_md5, const char *slice_md5)
{
	struct pcs *pcs = (struct pcs *)handle;
	char *content_length_str, *dir, *filename;
	char *url, *html;
	int http_code;
	const char *errmsg;
	cJSON *json, *item;
	PcsFileInfo *meta;

	pcs_clear_errmsg(handle);
	dir = pcs_utils_basedir(path);
	filename = pcs_utils_filename(path);
	content_length_str = pcs_utils_sprintf("%"PRId64, content_length);
	url = pcs_http_build_url(pcs->http, URL_PCS_REST,
		"method", "rapidupload",
		"app_id", "250528",
		"ondup", overwrite ? "overwrite" : "newcopy",
		"dir", dir,
		"filename", filename,
		"content-length", content_length_str,
		"content-md5", content_md5,
		"slice-md5", slice_md5,
		"path", path,
		"BDUSS", pcs->bduss,
		"bdstoken", pcs->bdstoken,
		NULL);
	if (!url) {
		pcs_set_errmsg(handle, "Can't build the url.");
		pcs_free(content_length_str);
		pcs_free(dir);
		pcs_free(filename);
		return NULL;
	}
	pcs_free(content_length_str);
	pcs_free(dir);
	pcs_free(filename);

	html = pcs_http_get(pcs->http, url, PcsTrue);
	pcs_free(url);
	http_code = pcs_http_code(pcs->http);
	if (http_code == 404)
		return NULL;
	if (!html) {
		errmsg = pcs_http_strerror(pcs->http);
		if (errmsg)
			pcs_set_serrmsg(handle, errmsg);
		else
			pcs_set_errmsg(handle, "Can't get response from the remote server.");
		return NULL;
	}
	json = cJSON_Parse(html);
	if (!json) {
		pcs_set_errmsg(handle, "Can't parse the response as json: %s", html);
		return NULL;
	}
	/*{
		"path": "\/temp\/putty-0.63-installer.exe",
		"size": 1869122,
		"ctime": 1420193926,
		"mtime": 1420193926,
		"md5": "18bd0948d254894441dd6f818d9b3811",
		"fs_id": 919365701714983,
		"isdir": 0,
		"request_id": 2401283243
	}*/
	item = cJSON_GetObjectItem(json, "error_code");
	if (item) {
		int error_code = item->valueint;
		const char *error_msg = NULL;
		item = cJSON_GetObjectItem(json, "error_msg");
		if (item)
			error_msg = item->valuestring;
		pcs_set_errmsg(handle, "Can't upload file. error_code: %d. error_msg: %s. raw response: %s", error_code, error_msg, html);
		cJSON_Delete(json);
		return NULL;
	}

	meta = pcs_parse_fileinfo(json);
	cJSON_Delete(json);
	if (!meta) {
		pcs_set_errmsg(handle, "Can't parse the response as meta");
		return NULL;
	}
	return meta;
}

PCS_API char *pcs_cookie_data(Pcs handle)
{
	struct pcs *pcs = (struct pcs *)handle;
	return pcs_http_cookie_data(pcs->http);
}

PCS_API const char *pcs_req_rawdata(Pcs handle, int *size, const char **encode)
{
	struct pcs *pcs = (struct pcs *)handle;
	return pcs_http_rawdata(pcs->http, size, encode);
}

PCS_API double pcs_speed_download(Pcs handle)
{
	struct pcs *pcs = (struct pcs *)handle;
	return pcs_http_speed_download(pcs->http);
}
