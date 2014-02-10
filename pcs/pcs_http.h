#ifndef _PCS_HTTP_H
#define _PCS_HTTP_H

#include <stdarg.h>
#include <curl/curl.h>
#include "pcs_defs.h"

typedef void *PcsHttp;
typedef void *PcsHttpForm;

typedef size_t (*PcsHttpWriteFunction)(char *ptr, size_t size, size_t contentlength, void *userdata);
typedef void (*PcsHttpResponseFunction)(unsigned char *ptr, size_t size, void *state);
typedef int (*PcsHttpProgressCallback)(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow);

typedef enum PcsHttpOption {
	PCS_HTTP_OPTION_END = 0,
	/* 值为PcsHttpWriteFunction类型的函数 */
	PCS_HTTP_OPTION_HTTP_WRITE_FUNCTION,
	/* PcsHttp本身不使用该值，仅原样传递到PcsHttpWriteFunction函数中 */
	PCS_HTTP_OPTION_HTTP_WRITE_FUNCTION_DATE,
	/* 值为PcsHttpReadFunction类型的函数 */
	PCS_HTTP_OPTION_HTTP_READ_FUNCTION,
	/* PcsHttp本身不使用该值，仅原样传递到PcsHttpReadFunction函数中 */
	PCS_HTTP_OPTION_HTTP_READ_FUNCTION_DATE,
	/* 值为PcsHttpResponseFunction类型的函数 */
	PCS_HTTP_OPTION_HTTP_RESPONSE_FUNCTION,
	/* PcsHttp本身不使用该值，仅原样传递到PcsHttpResponseFunction函数中 */
	PCS_HTTP_OPTION_HTTP_RESPONSE_FUNCTION_DATE,
	/* 值为PcsHttpProgressCallback类型的函数 */
	PCS_HTTP_OPTION_PROGRESS_FUNCTION,
	/* PcsHttp本身不使用该值，仅原样传递到PcsHttpProgressCallback函数中 */
	PCS_HTTP_OPTION_PROGRESS_FUNCTION_DATE,
	/* 设置是否启用下载或上传进度，值为unsigned char类型指针 */
	PCS_HTTP_OPTION_PROGRESS,


} PcsHttpOption;

PCS_API PcsHttp pcs_http_create(const char *cookie_file);
PCS_API void pcs_http_destroy(PcsHttp handle);
PCS_API const char *pcs_http_strerror(PcsHttp handle);
PCS_API int pcs_http_code(PcsHttp handle);
PCS_API void pcs_http_setopt(PcsHttp handle, PcsHttpOption opt, void *value);
PCS_API void pcs_http_setopts(PcsHttp handle, ...);
/*
 * Need call pcs_free(void *) to free the return value.
*/
PCS_API char *pcs_http_build_url_v(PcsHttp handle, const char *url, va_list args);
/*
 * Need call pcs_free(void *) to free the return value.
*/
PCS_API char *pcs_http_build_url(PcsHttp handle, const char *url, ...);
/*
 * Need call pcs_free(void *) to free the return value.
*/
PCS_API char *pcs_http_build_post_data_v(PcsHttp handle, va_list args);
/*
 * Need call pcs_free(void *) to free the return value.
*/
PCS_API char *pcs_http_build_post_data(PcsHttp handle, ...);
/*
 * Need call pcs_free(void *) to free the return value.
*/
PCS_API char *pcs_http_get_cookie(PcsHttp handle, const char *cookie_name);
/*
 * Not need call pcs_free(void *) to free the return value.
 * The memory will auto free when call pcs_http_destroy
*/
PCS_API char *pcs_http_get(PcsHttp handle, const char *url, PcsBool follow_location);
/*
 * Not need call pcs_free(void *) to free the return value.
 * The memory will auto free when call pcs_http_destroy
*/
PCS_API char *pcs_http_get_raw(PcsHttp handle, const char *url, PcsBool follow_location, size_t *sz);
/*
 * Not need call pcs_free(void *) to free the return value.
 * The memory will auto free when call pcs_http_destroy
*/
PCS_API char *pcs_http_post(PcsHttp handle, const char *url, char *post_data, PcsBool follow_location);

PCS_API PcsBool pcs_http_get_download(PcsHttp handle, const char *url, PcsBool follow_location);

PCS_API PcsBool pcs_http_form_addfile(PcsHttp handle, PcsHttpForm *post, const char *param_name, 
									  const char *filename, const char *simulate_filename);

PCS_API PcsBool pcs_http_form_addbuffer(PcsHttp handle, PcsHttpForm *post, const char *param_name,
										const char *buffer, long buffer_size, const char *simulate_filename);

PCS_API void pcs_http_form_destroy(PcsHttp handle, PcsHttpForm post);

PCS_API char *pcs_post_httpform(PcsHttp handle, const char *url, PcsHttpForm data, PcsBool follow_location);

PCS_API char *pcs_http_cookie_data(PcsHttp handle);

PCS_API const char *pcs_http_rawdata(PcsHttp handle, int *size, const char **encode);

#endif
