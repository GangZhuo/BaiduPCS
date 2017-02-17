#ifndef _PCS_HTTP_H
#define _PCS_HTTP_H

#include <stdarg.h>
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
	/* 值为PcsHttpWriteFunction类型的函数。当调用pcs_http_get_download方法时，此选项传入的函数用于处理服务器返回的数据。 */
	PCS_HTTP_OPTION_HTTP_WRITE_FUNCTION,
	/* PcsHttp本身不使用该值，仅原样传递到PcsHttpWriteFunction函数中。
	当调用pcs_http_get_download方法时，此选项传入的对象原样传递到PcsHttpWriteFunction指定的函数中。 */
	PCS_HTTP_OPTION_HTTP_WRITE_FUNCTION_DATE,
	/* 值为PcsHttpReadFunction类型的函数。保留，未使用。 */
	PCS_HTTP_OPTION_HTTP_READ_FUNCTION,
	/* PcsHttp本身不使用该值，仅原样传递到PcsHttpReadFunction函数中。保留，未使用。 */
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
	/* 设置USAGE，值为char类型指针 */
	PCS_HTTP_OPTION_USAGE,
	/*设置整个请求的超时时间，值为long类型*/
	PCS_HTTP_OPTION_TIMEOUT,
	/*设置连接前的等待时间，值为long类型*/
	PCS_HTTP_OPTION_CONNECTTIMEOUT,


} PcsHttpOption;

/*
 * 创建一个PcsHttp对象
 *   cookie_file   指定保存Cookie的文件，如果文件不存在，将自动创建该文件。
 *                 当发送一个请求时，将附加该文件中的Cookie。程序退出后，保存最新的Cookie到该文件中。
 * 成功后，返回创建的对象，失败则返回NULL。使用完成后需调用pcs_http_destroy()来释放资源
 */
PCS_API PcsHttp pcs_http_create(const char *cookie_file);
/*
 * 释放掉PcsHttp对象
 */
PCS_API void pcs_http_destroy(PcsHttp handle);
/*
 * 返回最后一次发生的错误描述
 */
PCS_API const char *pcs_http_strerror(PcsHttp handle);
/*
 * 返回最后一次请求的 HTTP状态码。状态码可参考"http://zh.wikipedia.org/wiki/HTTP%E7%8A%B6%E6%80%81%E7%A0%81"
 */
PCS_API int pcs_http_code(PcsHttp handle);
/*
 * 设置PcsHttp配置选项，每次只能设置一个。 
 */
PCS_API void pcs_http_setopt(PcsHttp handle, PcsHttpOption opt, void *value);

/*
 * 一次设定多个配置选项，最后一项必须为PCS_HTTP_OPTION_END。
 * 例： pcs_http_setopts(handle, PCS_HTTP_OPTION_HTTP_WRITE_FUNCTION, &cb_write, PCS_HTTP_OPTION_HTTP_READ_FUNCTION_DATE, state, PCS_HTTP_OPTION_END);
 */
PCS_API void pcs_http_setopts(PcsHttp handle, ...);
/*
 * pcs_http_build_url的参数列表模式
 * Need call pcs_free(void *) to free the return value.
*/
PCS_API char *pcs_http_build_url_v(PcsHttp handle, const char *url, va_list args);
/*
 * 拼接URL。多参必须全部为 Key-Value的键值对，且类型必须const char *类型。最后一项必须为NULL。
 * 如果值中包含中文或特殊字符，将采用UTF-8来编码。
 *  url  - 基地址
 *  例： 
 *     pcs_http_build_url(pcs->http, "http://baidu.com", "key1", "value1", "key2", "value2", NULL); //结果为：http://baidu.com?key1=value1&key2=value2
 *     pcs_http_build_url(pcs->http, "http://baidu.com?s=ab", "key1", "value1", "key2", "&ab", NULL); //结果为：http://baidu.com??s=ab&key1=value1&key2=%26ab
 *     pcs_http_build_url(pcs->http, "http://baidu.com?s=ab&", "key1", "value1", "key2", "&ab", NULL); //结果为：http://baidu.com??s=ab&key1=value1&key2=%26ab
 * 成功后返回拼接后的地址，失败则返回NULL
 * Need call pcs_free(void *) to free the return value.
*/
PCS_API char *pcs_http_build_url(PcsHttp handle, const char *url, ...);
/*
 * pcs_http_build_post_data的参数列表模式
 * Need call pcs_free(void *) to free the return value.
*/
PCS_API char *pcs_http_build_post_data_v(PcsHttp handle, va_list args);
/*
 * 类似于pcs_http_build_url，只不过该函数拼接的是需要发送到服务的数据。
 * 多参必须全部为 Key-Value的键值对，且类型必须const char *类型。最后一项必须为NULL。
 * 如果值中包含中文或特殊字符，将采用UTF-8来编码。
 *  url  - 基地址
 *  例： 
 *     pcs_http_build_post_data(pcs->http, "key1", "value1", "key2", "value2", NULL); //结果为：key1=value1&key2=value2
 *     pcs_http_build_post_data(pcs->http, "key1", "value1", "key2", "&ab", NULL); //结果为：key1=value1&key2=%26ab
 * 成功后返回拼接后的字符串，失败则返回NULL
 * Need call pcs_free(void *) to free the return value.
*/
PCS_API char *pcs_http_build_post_data(PcsHttp handle, ...);
/*
 * 根据名字获取Cookie值。
 * 成功后返回Cookie值，失败或不存在则返回NULL。
 * Need call pcs_free(void *) to free the return value.
*/
PCS_API char *pcs_http_get_cookie(PcsHttp handle, const char *cookie_name);

/*
 * 获取最后一次请求时服务器的返回内容
 * 如果从未从服务器请求数据则返回NULL。不需要调用pcs_free释放内存。
 */
PCS_API const char *pcs_http_get_response(PcsHttp handle);
/*
 * 获取最后一次请求时服务器返回内容的字节长度
 * 如果从未从服务器请求数据则返回0。
 */
PCS_API int pcs_http_get_response_size(PcsHttp handle);
/*
 * 向服务器发送一个GET请求。
 *   url             服务器地址
 *   follow_location 假如服务器返回跳转到另一个页面的指令时，是否自动跳转过去，如果跳转的话，则返回跳转后页面的内容
 * 返回服务器返回的内容。内容自动解码为当前操作系统使用的编码。
 * 例如：在Windows系统中，如果系统编码为GB2312，则返回内容自动解码为GB2312编码；在Linux系统中，系统编码为UTF-8，则返回内容的编码则为UTF-8
 * Not need call pcs_free(void *) to free the return value.
 * The memory will auto free when call pcs_http_destroy
*/
PCS_API char *pcs_http_get(PcsHttp handle, const char *url, PcsBool follow_location);
/*
 * 向服务器发送一个GET请求。
 *   url             服务器地址
 *   follow_location 假如服务器返回跳转到另一个页面的指令时，是否自动跳转过去，如果跳转的话，则返回跳转后页面的内容
 * 返回服务器返回的内容。内容不会执行解码操作。该方法一般用于获取图片或用于获取服务中文件的原始内容。
 * Not need call pcs_free(void *) to free the return value.
 * The memory will auto free when call pcs_http_destroy
*/
PCS_API char *pcs_http_get_raw(PcsHttp handle, const char *url, PcsBool follow_location, size_t *sz);
/*
 * 向服务器发送一个POST请求。
 *   url             服务器地址
 *   follow_location 假如服务器返回跳转到另一个页面的指令时，是否自动跳转过去，如果跳转的话，则返回跳转后页面的内容
 * 返回服务器返回的内容。内容自动解码为当前操作系统使用的编码。
 * 例如：在Windows系统中，如果系统编码为GB2312，则返回内容自动解码为GB2312编码；在Linux系统中，系统编码为UTF-8，则返回内容的编码则为UTF-8
 * Not need call pcs_free(void *) to free the return value.
 * The memory will auto free when call pcs_http_destroy
*/
PCS_API char *pcs_http_post(PcsHttp handle, const char *url, char *post_data, PcsBool follow_location);

/*
 * 向服务器发送一个GET请求。当获取到服务器返回数据后，调用PCS_HTTP_OPTION_HTTP_WRITE_FUNCTION传入的函数来写入内容。
 *   url             服务器地址
 *   follow_location 假如服务器返回跳转到另一个页面的指令时，是否自动跳转过去，如果跳转的话，则返回跳转后页面的内容
 * 返回是否下载成功。内容不会执行解码操作。该方法一般用于下载图片或文件。
 * Not need call pcs_free(void *) to free the return value.
 * The memory will auto free when call pcs_http_destroy
*/
PCS_API PcsBool pcs_http_get_download(PcsHttp handle, const char *url, PcsBool follow_location, curl_off_t max_speed, curl_off_t resume_from, curl_off_t max_length);

/*获取待下载文件的大小*/
PCS_API int64_t pcs_http_get_download_filesize(PcsHttp handle, const char *url, PcsBool follow_location);

/*
 * 向PcsHttpForm对象中添加一个本地文件。
 *   post        文件将添加到该PcsHttpForm对象中。
 *   param_name  发送给服务器的代表该文件内容的参数名字。
 *   filename    该文件的本地文件名，PcsHttpForm将从该路径读取文件内容
 *   simulate_filename 发送到服务器的文件名。可以指定不同于filename的名字，服务器收到的本地文件名将是simulate_filename而不是filename
 * 例： pcs_http_form_addfile(pcs->http, &form, "file", local_filename, "sample.dat"); //参数名是 "file"，文件内容存储在local_filename指定的文件中，服务器接收到的文件名字是"sample.dat"
 * 添加成功后，返回PcsTrue，否则返回PcsFalse。
*/
PCS_API PcsBool pcs_http_form_addfile(PcsHttp handle, PcsHttpForm *post, const char *param_name, 
									  const char *filename, const char *simulate_filename);
/*
* 向PcsHttpForm对象中添加一个内存文件。程序将调用
*   post        文件将添加到该PcsHttpForm对象中。
*   param_name  发送给服务器的代表该文件内容的参数名字。
*   simulate_filename 发送到服务器的文件名。可以指定不同于filename的名字，服务器收到的本地文件名将是simulate_filename而不是filename
*   read_func  用于读取内存文件的函数。
*               The data area pointed at by the pointer ptr may be filled with at most size multiplied with nmemb number of bytes. 
*               Your function must return the actual number of bytes that you stored in that memory area. Returning 0 will signal 
*               end-of-file to the library and cause it to stop the current transfer.
*   userdata    传递到read_func函数第4个参数的值
*   content_size 期望传递到服务器的内容长度。将会添加到HTTP头中。
* 添加成功后，返回PcsTrue，否则返回PcsFalse。
*/
PCS_API PcsBool pcs_http_form_addbufferfile(PcsHttp handle, PcsHttpForm *post, const char *param_name, const char *simulate_filename,
	size_t(*read_func)(void *ptr, size_t size, size_t nmemb, void *userdata), void *userdata, size_t content_size);

/* 同pcs_http_form_addfile，只不过是从内存中读取文件内容。 */
PCS_API PcsBool pcs_http_form_addbuffer(PcsHttp handle, PcsHttpForm *post, const char *param_name,
										const char *buffer, long buffer_size, const char *simulate_filename);
/*释放掉PcsHttpForm资源*/
PCS_API void pcs_http_form_destroy(PcsHttp handle, PcsHttpForm post);

/*
 * 向服务器发送一个POST请求。该方法可以上传文件到服务器。
 *   url             服务器地址
 *   data            发送到服务器的数据。
 *   max_speed       最大上传速度。
 *   follow_location 假如服务器返回跳转到另一个页面的指令时，是否自动跳转过去，如果跳转的话，则返回跳转后页面的内容
 * 返回服务器返回的内容。内容自动解码为当前操作系统使用的编码。
 * 例如：在Windows系统中，如果系统编码为GB2312，则返回内容自动解码为GB2312编码；在Linux系统中，系统编码为UTF-8，则返回内容的编码则为UTF-8
 * Not need call pcs_free(void *) to free the return value.
 * The memory will auto free when call pcs_http_destroy
*/
PCS_API char *pcs_post_httpform(PcsHttp handle, const char *url, PcsHttpForm data, curl_off_t max_speed, PcsBool follow_location);

/*以字符串形式返回所有Cookie数据。*/
PCS_API char *pcs_http_cookie_data(PcsHttp handle);

/*返回最后一次请求的服务器回应内容。
 *  size   - 用于接收服务器返回值的字节大小
 *  encode - 用于接收服务器返回值的编码
 *返回缓存中的服务器返回内容。
 */
PCS_API const char *pcs_http_rawdata(PcsHttp handle, int *size, const char **encode);

/*返回下载速度。通过curl_easy_getinfo(http->curl, CURLINFO_SPEED_DOWNLOAD, &downloadSpeed)来获取。*/
PCS_API double pcs_http_speed_download(PcsHttp handle);

/*
 * 解码s字符串，结果保存在s字符串中。
 * 返回 s 字符串指针。
 */
PCS_API char *pcs_http_url_decode(PcsHttp handle, char *s);

/* 当 pcs_http_get() 设置为不跟随跳转时，可通过此方法获取目标地址。
 * 返回 30x 的目标地址。
 */
PCS_API const char *pcs_http_redir_url(PcsHttp handle);

#endif
