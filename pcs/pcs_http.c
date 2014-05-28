#include <string.h>
#ifdef WIN32
# include <malloc.h>
#else
# include <alloca.h>
#endif

#include "pcs_mem.h"
#include "pcs_utils.h"
#include "pcs_http.h"

#define USAGE "Mozilla/5.0 (Windows NT 5.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/31.0.1650.57 Safari/537.36"

#define PCS_SKIP_SPACE(p) while((*p) && (*p == ' ' || *p == '\f' || *p == '\n' || *p == '\r' || *p == '\t' || *p == '\v')) p++

#define PCS_IS_TOKEN_CHAR(ch) (((ch) >= '0' && (ch) <= '9')\
								   || ((ch) >= 'a' && (ch) <= 'z')\
								   || ((ch) >= 'A' && (ch) <= 'Z')\
								   || (ch) == '_'\
								   || (ch) == '-')

#define PCS_HTTP_RES_TYPE_NORMAL		0
#define PCS_HTTP_RES_TYPE_VALIDATE_TEXT	2
#define PCS_HTTP_RES_TYPE_RAW			4
#define PCS_HTTP_RES_TYPE_DOWNLOAD		6

struct pcs_http {
	const char		*strerror;

	CURL			*curl;
	int				res_type;
	int				res_code;
	char			*res_header;
	int				res_header_size;
	int				res_content_length;
	char			*res_body;
	int				res_body_size;
	char			*res_encode;

	PcsHttpWriteFunction	write_func;
	void					*write_data;

	PcsHttpResponseFunction	response_func;
	void					*response_data;

	unsigned char			progress;
	PcsHttpProgressCallback progress_func;
	void					*progress_data;
};

struct http_post {
	struct curl_httppost *formpost;
    struct curl_httppost *lastptr;
};

static inline void pcs_http_reset_response(struct pcs_http *http)
{
	if (http->res_body)
		pcs_free(http->res_body);
	if (http->res_header)
		pcs_free(http->res_header);
	if (http->res_encode)
		pcs_free(http->res_encode);
	http->res_type = PCS_HTTP_RES_TYPE_NORMAL;
	http->res_code = 0;
	http->res_header = NULL;
	http->res_header_size = 0;
	http->res_body = NULL;
	http->res_body_size = 0;
	http->res_content_length = 0;
	http->res_encode = 0;
	http->strerror = NULL;
}

enum HttpMethod
{
	HTTP_METHOD_NONE = 0,
	HTTP_METHOD_GET,
	HTTP_METHOD_POST
};

static inline void pcs_http_prepare(struct pcs_http *http, enum HttpMethod method, const char *url, PcsBool follow_location,
							 PcsHttpWriteFunction write_func, void *state)
{
	pcs_http_reset_response(http);
	curl_easy_setopt(http->curl, CURLOPT_URL, url);
	switch(method)
	{
	case HTTP_METHOD_GET:
		curl_easy_setopt(http->curl, CURLOPT_HTTPGET, 1L);
		break;
	case HTTP_METHOD_POST:
		curl_easy_setopt(http->curl, CURLOPT_POST, 1L);
		break;
	}
	curl_easy_setopt(http->curl, CURLOPT_HEADER, 1L);
	curl_easy_setopt(http->curl, CURLOPT_WRITEFUNCTION, write_func);
	curl_easy_setopt(http->curl, CURLOPT_WRITEDATA, state);

	if (follow_location)
		curl_easy_setopt(http->curl, CURLOPT_FOLLOWLOCATION, 1L);
	else
		curl_easy_setopt(http->curl, CURLOPT_FOLLOWLOCATION, 0L);

	if (http->progress) {
		curl_easy_setopt(http->curl, CURLOPT_PROGRESSFUNCTION, http->progress_func);
		curl_easy_setopt(http->curl, CURLOPT_PROGRESSDATA, http->progress_data);
		curl_easy_setopt(http->curl, CURLOPT_NOPROGRESS, (long)0);
	}
	else {
		curl_easy_setopt(http->curl, CURLOPT_PROGRESSFUNCTION, NULL);
		curl_easy_setopt(http->curl, CURLOPT_PROGRESSDATA, NULL);
		curl_easy_setopt(http->curl, CURLOPT_NOPROGRESS, (long)1);
	}
}

static inline void skip_cookie_attr(char **p)
{
	while (**p != '\t' && **p != '\0')
		(*p)++; 
	if (**p != '\0') {
		(*p)++;
	}
}

static inline char *read_cookie_attr(char **p)
{
	char *start = *p,
		*res = 0;
	while (**p != '\t' && **p != '\0') 
		(*p)++;
	if (*p > start) {
		res = (char *)pcs_malloc(*p - start + 1);
		if (res) {
			memcpy(res, start, *p - start);
			res[*p - start] = '\0';
		}
		if (**p) (*p)++;
	}
	return res;
}

static inline PcsBool pcs_http_is_http_redirect_head(struct pcs_http *http, char *ptr, size_t size)
{
	if ((http->res_type % 2) == 1 && size >= 15 
		&& ptr[0] == 'H' && ptr[1] == 'T' && ptr[2] == 'T' && ptr[3] == 'P'
		&& ptr[4] == '/' && ptr[5] == '1' && ptr[6] == '.' && (ptr[7] == '0' || ptr[7] == '1')
		&& ptr[8] == ' ' && ptr[9] >= '0' && ptr[9] <= '9') {
		return PcsTrue;
	}
	return PcsFalse;
}

static inline PcsBool pcs_http_is_http_head(struct pcs_http *http, char *ptr, size_t size)
{
	return (http->res_type % 2) == 0 ? PcsTrue : PcsFalse;
}

static inline int pcs_http_get_content_length_from_header(const char *header, int size)
{
	int res = -1;
	char *val = NULL;
	const char *p = header, *key = "Content-Length",
		*end = NULL,
		*tmp;
	int i = 14;//strlen(key) = 14;
	while (*p) {
		if (*p == key[0] && pcs_utils_streq(p, key, i)) {
			tmp = p + i;
			PCS_SKIP_SPACE(tmp);
			if (*tmp != ':') continue; tmp++;
			PCS_SKIP_SPACE(tmp);
			end = tmp;
			while (*end && *end >= '0' && *end <= '9') end++;
			if (end > tmp) {
				val = (char *)pcs_malloc(end - tmp + 1);
				memcpy(val, tmp, end - tmp);
				val[end - tmp] = '\0';
				res = atoi(val);
				pcs_free(val);
				break;
			}
		}
		p++;
	}
	return res;
}

static inline char *pcs_http_get_charset_from_header(const char *header, int size)
{
	char *res = NULL;
	const char *p = header, *key = "charset",
		*end = NULL,
		*tmp;
	int i = 7;//strlen(key) = 7;
	while (*p) {
		if (*p == key[0] && pcs_utils_streq(p, key, i)) {
			tmp = p + i;
			if (*tmp != '=') continue; tmp++;
			end = tmp;
			while (*end && PCS_IS_TOKEN_CHAR(*end)) end++;
			if (end > tmp) {
				res = (char *)pcs_malloc(end - tmp + 1);
				memcpy(res, tmp, end - tmp);
				res[end - tmp] = '\0';
				break;
			}
		}
		p++;
	}
	return res;
}

//static inline char *pcs_http_get_charset_from_body(const char *body, int size)
//{
//	char *res = NULL;
//	const char *p = body, *key = "charset",
//		*end = NULL,
//		*tmp;
//	int i = 7;//strlen(key) = 7;
//	while (*p) {
//		if (*p == key[0] && pcs_utils_streq(p, key, i)) {
//			tmp = p + i;
//			PCS_SKIP_SPACE(tmp);
//			if (*tmp != '=') continue; tmp++;
//			PCS_SKIP_SPACE(tmp);
//			if (*tmp == '"') tmp++;
//			PCS_SKIP_SPACE(tmp);
//			end = tmp;
//			while (*end && PCS_IS_TOKEN_CHAR(*end)) end++;
//			if (end > tmp) {
//				res = (char *)pcs_malloc(end - tmp + 1);
//				memcpy(res, tmp, end - tmp);
//				res[end - tmp] = '\0';
//				break;
//			}
//		}
//		p++;
//	}
//
//	return res;
//}

static inline PcsBool pcs_http_parse_http_head(struct pcs_http *http, char **ptr, size_t *size, PcsBool try_get_encode)
{
	char *p, *cusor, *end;
	int cnt;
	p = *ptr;
	if (pcs_http_is_http_redirect_head(http, *ptr, *size)) {
		http->res_type--;
		if (http->res_header) {
			pcs_free(http->res_header);
			http->res_header = NULL;
			http->res_header_size = 0;
		}
	}
	if (pcs_http_is_http_head(http, *ptr, *size)) {
		p = (char *) pcs_malloc(http->res_header_size + *size + 1);
		if (!p)
			return PcsFalse;
		if (http->res_header) {
			memcpy(p, http->res_header, http->res_header_size);
			pcs_free(http->res_header);
			http->res_header = p;
			p = http->res_header + http->res_header_size;
			http->res_header_size += *size;
		} else {
			http->res_header = p;
			http->res_header_size = *size;
		}
		memcpy(p, *ptr, *size);
		p[*size] = '\0';

		//查找HTTP头结束标记
		cusor = p + *size;
		end = p - 2;
		if (end < http->res_header)
			end = http->res_header;
		cnt = 0;
		while (cusor > end) {
			if (*cusor == '\n') {
				cnt++;
				if (cnt == 2)
					break;
			}
			else if (*cusor != '\r') {
				cnt = 0;
			}
			cusor--;
		}
		if (cnt == 2) {
			cusor++;
			if (*cusor == '\r')
				cusor++;
			cusor++;
			*cusor = '\0';
			cnt = cusor - p;
			http->res_header_size -= *size - cnt;
			*ptr += cnt;
			*size -= cnt;

			http->res_type++;
			//从头中获取内容长度
			http->res_content_length = pcs_http_get_content_length_from_header(http->res_header, http->res_header_size);
			//从头中获取编码
			if (try_get_encode)
				http->res_encode = pcs_http_get_charset_from_header(http->res_header, http->res_header_size);
			else
				http->res_encode = NULL;
		} else {
			*ptr += *size;
			*size = 0;
		}
	}
	return PcsTrue;
}

static inline char *pcs_http_append_bytes(char *dest, int sz, char *src, int srcsz)
{
	char *p;

	p = (char *) pcs_malloc(sz + srcsz + 1);
	if (!p)
		return NULL;
	if (dest) {
		memcpy(p, dest, sz);
		pcs_free(dest);
	}
	memcpy(&p[sz], src, srcsz);
	p[sz + srcsz] = '\0';
	return p;
}

size_t pcs_http_write(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	struct pcs_http *http = (struct pcs_http *)userdata;
	size_t sz;
	char *p;

	if (size == 0 || nmemb == 0) {
		return 0;
	}
	sz = size * nmemb;
	if (!pcs_http_parse_http_head(http, &ptr, &sz, PcsTrue)) {
		http->strerror = "Cannot parse the http head. ";
		return 0;
	}
	if (sz > 0) {
		if (http->res_type == PCS_HTTP_RES_TYPE_NORMAL + 1) {
			http->res_body = pcs_http_append_bytes(http->res_body, http->res_body_size, ptr, sz);
			http->res_body_size += sz;
		}
		else if (http->res_type == PCS_HTTP_RES_TYPE_VALIDATE_TEXT + 1) {
			//验证内容正确性
			p = &ptr[sz - 1];
			while(p > ptr) {
				if (*p == 0) {
					http->strerror = "The response is not the validate text. ";
					return 0;
				}
				p--;
			}
			http->res_body = pcs_http_append_bytes(http->res_body, http->res_body_size, ptr, sz);
			http->res_body_size += sz;
		}
		else if (http->res_type == PCS_HTTP_RES_TYPE_RAW + 1) {
			http->res_body = pcs_http_append_bytes(http->res_body, http->res_body_size, ptr, sz);
			http->res_body_size += sz;
		}
		else if (http->res_type == PCS_HTTP_RES_TYPE_DOWNLOAD + 1) {
			if (!http->write_func) {
				http->strerror = "Have no write function. ";
				return 0;
			}
			return (*http->write_func)(ptr, sz, http->res_content_length, http->write_data);
		}
		else
			return 0;
	}

	return size * nmemb;
}

static inline char *pcs_http_perform(struct pcs_http *http)
{
	CURLcode res;
	long httpcode;

	res = curl_easy_perform(http->curl);
	curl_easy_getinfo(http->curl, CURLINFO_RESPONSE_CODE, &httpcode);
	http->res_code = httpcode;
	if(res != CURLE_OK) {
		if (!http->strerror) http->strerror = curl_easy_strerror(res);
		return NULL;
	}
	if (http->response_func)
		(*http->response_func)((unsigned char *)http->res_body, (size_t)http->res_body_size, http->response_data);
	return http->res_body;
}

PCS_API PcsHttp pcs_http_create(const char *cookie_file)
{
	struct pcs_http *http;

	http = (struct pcs_http *) pcs_malloc(sizeof(struct pcs_http));
	if (!http)
		return NULL;
	memset(http, 0, sizeof(struct pcs_http));
	http->curl = curl_easy_init();
	if (!http->curl) {
		pcs_free(http);
		return NULL;
	}
	curl_easy_setopt(http->curl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(http->curl, CURLOPT_SSL_VERIFYHOST, 0L);
	curl_easy_setopt(http->curl, CURLOPT_USERAGENT, USAGE);
	curl_easy_setopt(http->curl, CURLOPT_FOLLOWLOCATION, 1L);
	if (cookie_file) {
		curl_easy_setopt(http->curl, CURLOPT_COOKIEFILE, cookie_file);
		curl_easy_setopt(http->curl, CURLOPT_COOKIEJAR, cookie_file);
	}
	else {
		curl_easy_setopt(http->curl, CURLOPT_COOKIEFILE, "");
	}
	return http;
}

PCS_API void pcs_http_destroy(PcsHttp handle)
{
	struct pcs_http *http = (struct pcs_http *)handle;
	if (http->curl)
		curl_easy_cleanup(http->curl);
	if (http->res_header)
		pcs_free(http->res_header);
	if (http->res_body)
		pcs_free(http->res_body);
	if (http->res_encode)
		pcs_free(http->res_encode);
	pcs_free(http);
}

PCS_API const char *pcs_http_strerror(PcsHttp handle)
{
	struct pcs_http *http = (struct pcs_http *)handle;
	return http->strerror;
}

PCS_API int pcs_http_code(PcsHttp handle)
{
	struct pcs_http *http = (struct pcs_http *)handle;
	return http->res_code;
}

PCS_API void pcs_http_setopt(PcsHttp handle, PcsHttpOption opt, void *value)
{
	struct pcs_http *http = (struct pcs_http *)handle;
	switch (opt)
	{
	case PCS_HTTP_OPTION_HTTP_WRITE_FUNCTION:
		http->write_func = (PcsHttpWriteFunction)value;
		break;
	case PCS_HTTP_OPTION_HTTP_WRITE_FUNCTION_DATE:
		http->write_data = value;
		break;
	case PCS_HTTP_OPTION_HTTP_READ_FUNCTION:
		break;
	case PCS_HTTP_OPTION_HTTP_READ_FUNCTION_DATE:
		break;
	case PCS_HTTP_OPTION_HTTP_RESPONSE_FUNCTION:
		http->response_func = (PcsHttpResponseFunction)value;
		break;
	case PCS_HTTP_OPTION_HTTP_RESPONSE_FUNCTION_DATE:
		http->response_data = value;
		break;
	case PCS_HTTP_OPTION_PROGRESS_FUNCTION:
		http->progress_func = (PcsHttpProgressCallback)value;
		break;
	case PCS_HTTP_OPTION_PROGRESS_FUNCTION_DATE:
		http->progress_data = value;
		break;
	case PCS_HTTP_OPTION_PROGRESS:
		http->progress = (unsigned char)value;
		break;
	default:
		break;
	}
}

PCS_API void pcs_http_setopts(PcsHttp handle, ...)
{
	PcsHttpOption opt;
	void *val;
    va_list args;
    va_start(args, handle);
	while((opt = va_arg(args, PcsHttpOption)) != PCS_HTTP_OPTION_END) {
		val = va_arg(args, void *);
		pcs_http_setopt(handle, opt, val);
	}
    va_end(args);
}

PCS_API char *pcs_http_build_url_v(PcsHttp handle, const char *url, va_list args)
{
	struct pcs_http *http = (struct pcs_http *)handle;
	char *name, *val,
		*escapval, 
		*res = NULL, *p;
	int ressz, 
		ampersand = 0;

	ressz = strlen(url) + 1;
	res = (char *)pcs_malloc(ressz + 1);
	if (!res) return NULL;
	strcpy(res, url);

	p = res;
	while(*p) {
		if (*p == '?')
			break;
		p++;
	}
	if (*p) { // find the '?'
		ressz--;
		p++;
	}
	else { // not find the '?', so add '?' at end
		*p = '?';
		p++;
		*p = '\0';
	}
	if (*p) { // have char after '?'
		while(*p) p++; // find the end of url
		p--; // *p is the last char
		if (*p != '&') { // last char is not '&', so the next param need add '&' in the front
			ampersand = 1;
		}
	}

	while((name = va_arg(args, char *)) != NULL) {
		val = va_arg(args, char *);
		if (name[0] == '\0') continue;
		if (!val) {
			ressz += strlen(name) + 1;
			p = (char *)pcs_malloc(ressz + 1);
			if (!p) {
				pcs_free(res);
				return NULL;
			}
			strcpy(p, res);
			pcs_free(res);
			res = p;
			if (ampersand) {
				strcat(res, "&");
			}
			else {
				ampersand = 1;
				ressz--;
			}
			strcat(res, name);
			continue;
		}
		escapval = curl_easy_escape(http->curl, val, 0);

		ressz += strlen(name) + strlen(escapval) + 2;
		p = (char *)pcs_malloc(ressz + 1);
		if (!p) {
			pcs_free(res);
			return NULL;
		}
		strcpy(p, res);
		pcs_free(res);
		res = p;
		if (ampersand) {
			strcat(res, "&");
		}
		else {
			ampersand = 1;
			ressz--;
		}
		strcat(res, name);
		strcat(res, "=");
		strcat(res, escapval);

		curl_free((void *)escapval);
	}

	return res;
}

PCS_API char *pcs_http_build_url(PcsHttp handle, const char *url, ...)
{
	char *res;
	va_list args;
    va_start(args, url);
	res = pcs_http_build_url_v(handle, url, args);
    va_end(args);
	return res;
}

PCS_API char *pcs_http_build_post_data_v(PcsHttp handle, va_list args)
{
	struct pcs_http *http = (struct pcs_http *)handle;
	char *name, *val, 
		*escapval, *res = NULL, *p;
	int ressz;

	while((name = va_arg(args, char *)) != NULL) {
		val = va_arg(args, char *);
		if (name[0] == '\0') continue;
		escapval = curl_easy_escape(http->curl, val, 0);

		if (!res) {
			ressz = strlen(name) + strlen(escapval) + 1;
			res = (char *)pcs_malloc(ressz + 1);
			if (!res) {
				return NULL;
			}
			strcpy(res, name);
			strcat(res, "=");
			strcat(res, escapval);
		}
		else {
			ressz += strlen(name) + strlen(escapval) + 2;
			p = (char *)pcs_malloc(ressz + 1);
			if (!p) {
				pcs_free(res);
				return NULL;
			}
			strcpy(p, res);
			pcs_free(res);
			res = p;
			strcat(res, "&");
			strcat(res, name);
			strcat(res, "=");
			strcat(res, escapval);
		}
		curl_free((void *)escapval);
	}

	return res;
}

PCS_API char *pcs_http_build_post_data(PcsHttp handle, ...)
{
	char *res;
    va_list args;
    va_start(args, handle);
	res = pcs_http_build_post_data_v(handle, args);
    va_end(args);
	return res;
}

PCS_API char *pcs_http_get_cookie(PcsHttp handle, const char *cookie_name)
{
	struct pcs_http *http = (struct pcs_http *)handle;
	CURLcode res;
	struct curl_slist *cookies;
	struct curl_slist *nc;
	char *p;
	char *name = NULL,
		*value = NULL;

	res = curl_easy_getinfo(http->curl, CURLINFO_COOKIELIST, &cookies);
	if (res != CURLE_OK)
		return NULL;
	nc = cookies;
	while (nc) {
		p = nc->data;
		skip_cookie_attr(&p);
		skip_cookie_attr(&p);
		skip_cookie_attr(&p);
		skip_cookie_attr(&p);
		skip_cookie_attr(&p);
		name = read_cookie_attr(&p);
		value = read_cookie_attr(&p);

		if (pcs_utils_strcmpi(cookie_name, name) == 0) {
			pcs_free(name);
			break;
		}
		pcs_free(name);
		pcs_free(value);
		nc = nc->next;
	}
	curl_slist_free_all(cookies);
	return value;
}

PCS_API const char *pcs_http_get_response(PcsHttp handle)
{
	struct pcs_http *http = (struct pcs_http *)handle;
	return http->res_body;
}

PCS_API int pcs_http_get_response_size(PcsHttp handle)
{
	struct pcs_http *http = (struct pcs_http *)handle;
	return http->res_body_size;
}

PCS_API char *pcs_http_get(PcsHttp handle, const char *url, PcsBool follow_location)
{
	struct pcs_http *http = (struct pcs_http *)handle;
	pcs_http_prepare(http, HTTP_METHOD_GET, url, follow_location, &pcs_http_write, http);
	return pcs_http_perform(http);
}

PCS_API char *pcs_http_get_raw(PcsHttp handle, const char *url, PcsBool follow_location, size_t *sz)
{
	struct pcs_http *http = (struct pcs_http *)handle;
	char *data;
	pcs_http_prepare(http, HTTP_METHOD_GET, url, follow_location, &pcs_http_write, http);
	http->res_type = PCS_HTTP_RES_TYPE_RAW;
	data = pcs_http_perform(http);
	if (sz) *sz = http->res_body_size;
	return data;
}

PCS_API char *pcs_http_post(PcsHttp handle, const char *url, char *post_data, PcsBool follow_location)
{
	struct pcs_http *http = (struct pcs_http *)handle;
	pcs_http_prepare(http, HTTP_METHOD_POST, url, follow_location, &pcs_http_write, http);
	if (post_data)
		curl_easy_setopt(http->curl, CURLOPT_POSTFIELDS, post_data);  
	else
		curl_easy_setopt(http->curl, CURLOPT_POSTFIELDS, "");  
	return pcs_http_perform(http);
}

PCS_API PcsBool pcs_http_get_download(PcsHttp handle, const char *url, PcsBool follow_location)
{
	struct pcs_http *http = (struct pcs_http *)handle;
	pcs_http_prepare(http, HTTP_METHOD_GET, url, follow_location, &pcs_http_write, http);
	http->res_type = PCS_HTTP_RES_TYPE_DOWNLOAD;
	pcs_http_perform(http);
	return http->strerror == NULL ? PcsTrue : PcsFalse;
}

PCS_API PcsBool pcs_http_form_addfile(PcsHttp handle, PcsHttpForm *post, const char *param_name, 
									  const char *filename, const char *simulate_filename)
{
	struct pcs_http *http = (struct pcs_http *)handle;
	struct http_post *formpost = (struct http_post *)(*post);
	char *escape_param_name, *escape_simulate_filename;
	PcsBool res = PcsTrue;
	if (!formpost) {
		formpost = (struct http_post *)pcs_malloc(sizeof(struct http_post));
		if (!formpost)
			return PcsFalse;
		memset(formpost, 0, sizeof(struct http_post));
		(*post) = (PcsHttpForm)formpost;
	}
	escape_param_name = curl_easy_escape(http->curl, param_name, 0);//pcs_http_escape(handle, param_name);
	escape_simulate_filename = curl_easy_escape(http->curl, simulate_filename, 0);//pcs_http_escape(handle, simulate_filename);
	if (curl_formadd(&(formpost->formpost), &(formpost->lastptr), 
		CURLFORM_COPYNAME, escape_param_name,
		CURLFORM_FILE, filename,
		CURLFORM_FILENAME, escape_simulate_filename,
		CURLFORM_END)) {
		res = PcsFalse;
	}
	curl_free((void *)escape_param_name);//pcs_free(escape_param_name);
	curl_free((void *)escape_simulate_filename);//pcs_free(escape_simulate_filename);
	return res;
}

PCS_API PcsBool pcs_http_form_addbuffer(PcsHttp handle, PcsHttpForm *post, const char *param_name,
										const char *buffer, long buffer_size, const char *simulate_filename)
{
	struct pcs_http *http = (struct pcs_http *)handle;
	struct http_post *formpost = (struct http_post *)(*post);
	char *escape_param_name, *escape_simulate_filename;
	PcsBool res = PcsTrue;
	if (!formpost) {
		formpost = (struct http_post *)pcs_malloc(sizeof(struct http_post));
		if (!formpost)
			return PcsFalse;
		memset(formpost, 0, sizeof(struct http_post));
		(*post) = (PcsHttpForm)formpost;
	}
	escape_param_name = curl_easy_escape(http->curl, param_name, 0);//pcs_http_escape(handle, param_name);
	escape_simulate_filename = curl_easy_escape(http->curl, simulate_filename, 0);//pcs_http_escape(handle, simulate_filename);
	if (curl_formadd(&(formpost->formpost), &(formpost->lastptr), 
		CURLFORM_COPYNAME, escape_param_name,
		CURLFORM_BUFFER, escape_simulate_filename,
		CURLFORM_BUFFERPTR, buffer,
		CURLFORM_BUFFERLENGTH, buffer_size,
		CURLFORM_END))
		res = PcsFalse;
	curl_free((void *)escape_param_name);//pcs_free(escape_param_name);
	curl_free((void *)escape_simulate_filename);//pcs_free(escape_simulate_filename);
	return res;
}

PCS_API void pcs_http_form_destroy(PcsHttp handle, PcsHttpForm post)
{
	struct http_post *formpost = (struct http_post *)post;
	if (formpost) {
		curl_formfree(formpost->formpost);
		pcs_free(formpost);
	}
}

PCS_API char *pcs_post_httpform(PcsHttp handle, const char *url, PcsHttpForm data, PcsBool follow_location)
{
	struct pcs_http *http = (struct pcs_http *)handle;
	struct http_post *formpost = (struct http_post *)data;
	pcs_http_prepare(http, HTTP_METHOD_POST, url, follow_location, &pcs_http_write, http);
	if (data)
		curl_easy_setopt(http->curl, CURLOPT_HTTPPOST, formpost->formpost); 
	else
		curl_easy_setopt(http->curl, CURLOPT_POSTFIELDS, "");  
	return pcs_http_perform(http);
}

PCS_API char *pcs_http_cookie_data(PcsHttp handle)
{
	struct pcs_http *http = (struct pcs_http *)handle;
	CURLcode res;
	struct curl_slist *cookies;
	struct curl_slist *nc;
	char *data = NULL,
		*p;
	int sz = 0, i = 0;

	res = curl_easy_getinfo(http->curl, CURLINFO_COOKIELIST, &cookies);
	if (res != CURLE_OK) {
		return NULL;
	}
	nc = cookies;
	while (nc) {
		i = strlen(nc->data);
		if (!data) {
			data = (char *)pcs_malloc(i + 2);
			if (!data) {
				curl_slist_free_all(cookies);
				return NULL;
			}
			memcpy(data, nc->data, i);
			memcpy(data + i, "\n\0", 2);
			sz = i + 1;
		}
		else {
			p = (char *)pcs_malloc(sz + i + 2);
			if (!p) {
				pcs_free(data);
				curl_slist_free_all(cookies);
				return NULL;
			}
			memcpy(p, data, sz);
			memcpy(p + sz, nc->data, i);
			sz += i;
			memcpy(p + sz, "\n\0", 2);
			pcs_free(data);
			data = p;
			sz++;
		}
		nc = nc->next;
	}
	curl_slist_free_all(cookies);
	return data;
}

PCS_API const char *pcs_http_rawdata(PcsHttp handle, int *size, const char **encode)
{
	struct pcs_http *http = (struct pcs_http *)handle;
	*size = http->res_body_size;
	*encode = http->res_encode;
	return http->res_body;
}

