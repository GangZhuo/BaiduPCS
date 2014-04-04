#ifndef _PCS_HTTP_H
#define _PCS_HTTP_H

#include <stdarg.h>
#include <curl/curl.h>
#include "pcs_defs.h"

typedef void *PcsHttp;
typedef void *PcsHttpForm;

/*
 * 设定该回调后，Pcs每从网络获取到值，则调用该回调。例如下载时。
 *   ptr  从网络获取到的字节序
 *   size 字节序的大小，以字节为单位
 *   contentlength 本次请求，HTTP头中的Content-Length值
 *   userdata 使用PCS_OPTION_DOWNLOAD_WRITE_FUNCTION_DATA选项设定的值原样传入
 * 返回写入的字节数，如果返回值和传入的size不一样，将导致下载中断
*/
typedef size_t (*PcsHttpWriteFunction)(char *ptr, size_t size, size_t contentlength, void *userdata);

/*
 * 设定该回调后，Pcs每从网络获取到值，则调用该回调。
 * 和PcsHttpWriteFunction的区别是，该回调是在获取到全部内容后触发,
 * 而PcsHttpWriteFunction是每获取到一段字节序则触发。
 * 每个HTTP请求，PcsHttpResponseFunction只会触发一次，而PcsHttpWriteFunction可能触发多次
 *   ptr  从网络获取到的字节序
 *   size 字节序的大小，以字节为单位
 *   userdata 使用PCS_OPTION_HTTP_RESPONSE_FUNCTION_DATE选项设定的值原样传入
*/
typedef void (*PcsHttpResponseFunction)(unsigned char *ptr, size_t size, void *state);

/*
 * 设定该回调后，Pcs每上传或下载一段字节序到网络中时，则调用该回调。利用该回调可实现上传时的进度条
 * 注意：只有设定PCS_OPTION_PROGRESS的值为PcsTrue后才会启用进度条
 *   dltotal  从网络中需要下载多少字节
 *   dlnow    从网络中已经下载多少字节
 *   ultotal  需要上传多少字节
 *   ulnow    已经上传多少字节
 *   clientp 使用PCS_OPTION_PROGRESS_FUNCTION_DATE选项设定的值原样传入
 * 返回非零值，将导致中断上传或下载
*/
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

#endif
