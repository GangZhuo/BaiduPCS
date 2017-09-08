#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef WIN32
# include <malloc.h>
#else
# include <alloca.h>
#endif

#include "pcs_mem.h"
#include "pcs_utils.h"
#include "pcs_http.h"

#define USAGE "Mozilla/5.0 (Windows NT 6.3; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/35.0.1916.153 Safari/537.36"

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
	char			*strerror;
	char			*usage;

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

	int						timeout;
	int						connect_timeout;
};

struct http_post {
	struct curl_httppost *formpost;
    struct curl_httppost *lastptr;
	size_t(*read_func)(void *ptr, size_t size, size_t nmemb, void *userdata);
	void *read_func_data;
};

static inline void pcs_http_reset_response(struct pcs_http *http)
{
	if (http->res_body)
		pcs_free(http->res_body);
	if (http->res_header)
		pcs_free(http->res_header);
	if (http->res_encode)
		pcs_free(http->res_encode);
	if (http->strerror)
		pcs_free(http->strerror);
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

static inline void pcs_http_prepare(struct pcs_http *http, enum HttpMethod method,
	const char *url, PcsBool follow_location,
	PcsHttpWriteFunction write_func, void *state,
	curl_off_t max_recv_speed, curl_off_t max_send_speed,
	long min_recv_speed, long min_recv_speed_time,
	curl_off_t resume_from, curl_off_t max_length)
{
	pcs_http_reset_response(http);
	curl_easy_setopt(http->curl, CURLOPT_USERAGENT, http->usage ? http->usage : USAGE);
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
	curl_easy_setopt(http->curl, CURLOPT_CONNECTTIMEOUT, (long)http->connect_timeout);
	curl_easy_setopt(http->curl, CURLOPT_TIMEOUT, (long)http->timeout);
	curl_easy_setopt(http->curl, CURLOPT_NOSIGNAL, 1L);
	curl_easy_setopt(http->curl, CURLOPT_WRITEFUNCTION, write_func);
	curl_easy_setopt(http->curl, CURLOPT_WRITEDATA, state);

	if (min_recv_speed > 0 && min_recv_speed_time > 0) {
		curl_easy_setopt(http->curl, CURLOPT_LOW_SPEED_LIMIT, min_recv_speed);
		curl_easy_setopt(http->curl, CURLOPT_LOW_SPEED_TIME, min_recv_speed_time);
	}
	else {
		curl_easy_setopt(http->curl, CURLOPT_LOW_SPEED_LIMIT, 1024L);
		curl_easy_setopt(http->curl, CURLOPT_LOW_SPEED_TIME, 30L);
	}

	// 设置文件续传的位置给libcurl
	if (resume_from || max_length) {
		char *range = pcs_utils_sprintf("%" PRId64 "-%" PRId64, resume_from, resume_from + max_length - 1);
		curl_easy_setopt(http->curl, CURLOPT_RANGE, range);
		pcs_free(range);
	}
	else if (resume_from) {
		curl_easy_setopt(http->curl, CURLOPT_RANGE, NULL);
		curl_easy_setopt(http->curl, CURLOPT_RESUME_FROM_LARGE, resume_from);
	}
	else {
		curl_easy_setopt(http->curl, CURLOPT_RANGE, NULL);
		curl_easy_setopt(http->curl, CURLOPT_RESUME_FROM_LARGE, (curl_off_t)0);
	}

	curl_easy_setopt(http->curl, CURLOPT_MAX_RECV_SPEED_LARGE, max_recv_speed ? max_recv_speed : (curl_off_t)0);
	curl_easy_setopt(http->curl, CURLOPT_MAX_SEND_SPEED_LARGE, max_send_speed ? max_send_speed : (curl_off_t)0);

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
		if (http->strerror) pcs_free(http->strerror);
		http->strerror = pcs_utils_strdup("Cannot parse the http head. ");
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
					if (http->strerror) pcs_free(http->strerror);
					http->strerror = pcs_utils_strdup("The response is not the validate text. ");
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
				if (http->strerror) pcs_free(http->strerror);
				http->strerror = pcs_utils_strdup("Have no write function. ");
				return 0;
			}
			return (*http->write_func)(ptr, sz, http->res_content_length, http->write_data);
		}
		else
			return 0;
	}

	return size * nmemb;
}

static inline char *pcs_http_perform(struct pcs_http *http, const char *url)
{
	CURLcode res;
	long httpcode;

	res = curl_easy_perform(http->curl);
	curl_easy_getinfo(http->curl, CURLINFO_RESPONSE_CODE, &httpcode);
	http->res_code = httpcode;
	if(res != CURLE_OK) {
		if (!http->strerror) http->strerror = pcs_utils_sprintf("%s: %s", curl_easy_strerror(res), url);//pcs_utils_strdup(curl_easy_strerror(res));
		return NULL;
	}
	if (httpcode != 200 && httpcode != 206) { /*206 部分请求成功的返回码*/
		if (http->strerror) pcs_free(http->strerror);
		http->strerror = pcs_utils_sprintf("%d %s", httpcode, http->res_body);
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
	http->timeout = 0;
	http->connect_timeout = 10;
	http->curl = curl_easy_init();
	if (!http->curl) {
		pcs_free(http);
		return NULL;
	}
	curl_easy_setopt(http->curl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(http->curl, CURLOPT_SSL_VERIFYHOST, 0L);
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
	if (http->strerror)
		pcs_free(http->strerror);
	if (http->usage)
		pcs_free(http->usage);
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

PCS_API const char *pcs_http_redir_url(PcsHttp handle)
{
	struct pcs_http *http = (struct pcs_http *)handle;
	char *location;
	curl_easy_getinfo(http->curl, CURLINFO_REDIRECT_URL, &location);
	return location;
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
		http->progress = (unsigned char)((unsigned long)value);
		break;
	case PCS_HTTP_OPTION_USAGE:
		if (http->usage) pcs_free(http->usage);
		http->usage = value ? pcs_utils_strdup((char *)value) : NULL;
		break;
	case PCS_HTTP_OPTION_TIMEOUT:
		http->timeout = (int)((long)value);
		break;
	case PCS_HTTP_OPTION_CONNECTTIMEOUT:
		http->connect_timeout = (int)((long)value);
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

PCS_API char *pcs_http_url_decode(PcsHttp handle, char *s)
{
    struct pcs_http *http = (struct pcs_http *)handle;
    char *val;
    int outlen;
    val = curl_easy_unescape(http->curl, s, 0, &outlen);
    memcpy(s, val, outlen);
    s[outlen] = '\0';
    curl_free(val);
    return s;
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
		if (val == NULL) {
			if (!res) {
				ressz = strlen(name) + 1;
				res = (char *)pcs_malloc(ressz + 1);
				if (!res) {
					return NULL;
				}
				strcpy(res, name);
				strcat(res, "=");
			}
			else {
				ressz += strlen(name) + 2;
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
			}
		}
		else {
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
	pcs_http_prepare(http, HTTP_METHOD_GET, url, follow_location, &pcs_http_write, http, 0, 0, 0, 0, 0, 0);
	return pcs_http_perform(http, url);
}

PCS_API char *pcs_http_get_raw(PcsHttp handle, const char *url, PcsBool follow_location, size_t *sz)
{
	struct pcs_http *http = (struct pcs_http *)handle;
	char *data;
	pcs_http_prepare(http, HTTP_METHOD_GET, url, follow_location, &pcs_http_write, http, 0, 0, 0, 0, 0, 0);
	http->res_type = PCS_HTTP_RES_TYPE_RAW;
	data = pcs_http_perform(http, url);
	if (sz) *sz = http->res_body_size;
	return data;
}

PCS_API char *pcs_http_post(PcsHttp handle, const char *url, char *post_data, PcsBool follow_location)
{
	struct pcs_http *http = (struct pcs_http *)handle;
	pcs_http_prepare(http, HTTP_METHOD_POST, url, follow_location, &pcs_http_write, http, 0, 0, 0, 0, 0, 0);
	if (post_data)
		curl_easy_setopt(http->curl, CURLOPT_POSTFIELDS, post_data);  
	else
		curl_easy_setopt(http->curl, CURLOPT_POSTFIELDS, "");  
	return pcs_http_perform(http, url);
}

PCS_API PcsBool pcs_http_get_download(PcsHttp handle, const char *url, PcsBool follow_location, curl_off_t max_speed, curl_off_t resume_from, curl_off_t max_length)
{
	struct pcs_http *http = (struct pcs_http *)handle;
	pcs_http_prepare(http, HTTP_METHOD_GET, url, follow_location, &pcs_http_write, http,
		max_speed, 0,
		5 * 1024L, 10L, /* 10秒内的平均下载速度低于 5KB/s 的话，则取消下载 */
		resume_from, max_length);
	http->res_type = PCS_HTTP_RES_TYPE_DOWNLOAD;
	pcs_http_perform(http, url);
	return http->strerror == NULL ? PcsTrue : PcsFalse;
}

size_t pcs_http_null_write(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	return size * nmemb;
}

PCS_API int64_t pcs_http_get_download_filesize(PcsHttp handle, const char *url, PcsBool follow_location)
{
	CURLcode res;
	double downloadFileLenth = 0;
	struct pcs_http *http = (struct pcs_http *)handle;
	pcs_http_prepare(http, HTTP_METHOD_GET, url, follow_location, pcs_http_null_write, NULL, 0, 0, 0, 0, 0, 0);
	curl_easy_setopt(http->curl, CURLOPT_NOBODY, 1L);   //不需要body
	res = curl_easy_perform(http->curl);
	if (res == CURLE_OK) {
		curl_easy_getinfo(http->curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &downloadFileLenth);
	}
	else {
		if (!http->strerror) http->strerror = pcs_utils_strdup(curl_easy_strerror(res));
		downloadFileLenth = 0;
	}
	return (int64_t)downloadFileLenth;
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
	escape_param_name = curl_easy_escape(http->curl, param_name, 0);
	escape_simulate_filename = simulate_filename ? curl_easy_escape(http->curl, simulate_filename, 0) : NULL;
	if (curl_formadd(&(formpost->formpost), &(formpost->lastptr),
		CURLFORM_COPYNAME, escape_param_name,
		CURLFORM_FILE, filename,
		CURLFORM_FILENAME, escape_simulate_filename ? escape_simulate_filename : " ",
		CURLFORM_END)) {
		res = PcsFalse;
	}
	curl_free((void *)escape_param_name);
	if (escape_simulate_filename) curl_free((void *)escape_simulate_filename);
	return res;
}

PCS_API PcsBool pcs_http_form_addbufferfile(PcsHttp handle, PcsHttpForm *post, const char *param_name, const char *simulate_filename,
	size_t(*read_func)(void *ptr, size_t size, size_t nmemb, void *userdata), void *userdata, size_t content_size)
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
	escape_param_name = curl_easy_escape(http->curl, param_name, 0);
	escape_simulate_filename = simulate_filename ? curl_easy_escape(http->curl, simulate_filename, 0) : NULL;
	if (curl_formadd(&(formpost->formpost), &(formpost->lastptr),
		CURLFORM_COPYNAME, escape_param_name,
		CURLFORM_STREAM, userdata,
		CURLFORM_CONTENTSLENGTH, (long) content_size,
		CURLFORM_FILENAME, escape_simulate_filename ? escape_simulate_filename : "part",
		CURLFORM_END)) {
		res = PcsFalse;
	}
	curl_free((void *)escape_param_name);
	if (escape_simulate_filename) curl_free((void *)escape_simulate_filename);
	formpost->read_func = read_func;
	formpost->read_func_data = userdata;
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
	escape_param_name = curl_easy_escape(http->curl, param_name, 0);
	escape_simulate_filename = simulate_filename ? curl_easy_escape(http->curl, simulate_filename, 0) : NULL;
	if (curl_formadd(&(formpost->formpost), &(formpost->lastptr), 
		CURLFORM_COPYNAME, escape_param_name,
		CURLFORM_BUFFER, escape_simulate_filename ? escape_simulate_filename : "part",
		CURLFORM_BUFFERPTR, buffer,
		CURLFORM_BUFFERLENGTH, buffer_size,
		CURLFORM_END))
		res = PcsFalse;
	curl_free((void *)escape_param_name);
	if (escape_simulate_filename) curl_free((void *)escape_simulate_filename);
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

PCS_API char *pcs_post_httpform(PcsHttp handle, const char *url, PcsHttpForm data, curl_off_t max_speed, PcsBool follow_location)
{
	struct pcs_http *http = (struct pcs_http *)handle;
	struct http_post *formpost = (struct http_post *)data;
	char *rc;
	pcs_http_prepare(http, HTTP_METHOD_POST, url, follow_location, &pcs_http_write, http, 0, max_speed, 0, 0, 0, 0);
	if (data){
		curl_easy_setopt(http->curl, CURLOPT_HTTPPOST, formpost->formpost); 
		if (formpost->read_func) {
			curl_easy_setopt(http->curl, CURLOPT_READFUNCTION, formpost->read_func);
			curl_easy_setopt(http->curl, CURLOPT_READDATA, formpost->read_func_data);
		}
	}
	else
		curl_easy_setopt(http->curl, CURLOPT_POSTFIELDS, "");  
	rc = pcs_http_perform(http, url);
	if (data && formpost->read_func) {
		curl_easy_setopt(http->curl, CURLOPT_READFUNCTION, NULL);
		curl_easy_setopt(http->curl, CURLOPT_READDATA, NULL);
	}
	return rc;
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

PCS_API double pcs_http_speed_download(PcsHttp handle)
{
	double downloadSpeed;// 记录下载速度  
	struct pcs_http *http = (struct pcs_http *)handle;
	CURLcode re = curl_easy_getinfo(http->curl, CURLINFO_SPEED_DOWNLOAD, &downloadSpeed);  // 获取下载速度
	if (re == CURLE_OK)
		return downloadSpeed;
	else
		return 0.0;
}
