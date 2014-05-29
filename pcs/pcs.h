#ifndef _PCS_H
#define _PCS_H

#include "pcs_defs.h"
#include "pcs_mem.h"
#include "pcs_fileinfo.h"
#include "pcs_pan_api_resinfo.h"
#include "pcs_http.h"
#include "pcs_slist.h"
#include "pcs_utils.h"

#define PCS_API_VERSION "v1.0.3"

typedef enum PcsOption {
	PCS_OPTION_END = 0,
	/* 值为以0结尾的C格式字符串 */
	PCS_OPTION_USERNAME,
	/* 值为以0结尾的C格式字符串 */
	PCS_OPTION_PASSWORD,
	/* 值为PcsGetCaptcha类型的函数 */
	PCS_OPTION_CAPTCHA_FUNCTION,
	/* Pcs本身不使用该值，仅原样传递到PcsGetCaptcha函数中 */
	PCS_OPTION_CAPTCHA_FUNCTION_DATA,
	/* 值为PcsHttpWriteFunction类型的函数 */
	PCS_OPTION_DOWNLOAD_WRITE_FUNCTION,
	/* Pcs本身不使用该值，仅原样传递到PcsHttpWriteFunction函数中 */
	PCS_OPTION_DOWNLOAD_WRITE_FUNCTION_DATA,
	/* 值为PcsHttpResponseFunction类型的函数 */
	PCS_OPTION_HTTP_RESPONSE_FUNCTION,
	/* Pcs本身不使用该值，仅原样传递到PcsHttpResponseFunction函数中 */
	PCS_OPTION_HTTP_RESPONSE_FUNCTION_DATE,
	/* 值为PcsHttpProgressCallback类型的函数 */
	PCS_OPTION_PROGRESS_FUNCTION,
	/* Pcs本身不使用该值，仅原样传递到PcsHttpProgressCallback函数中 */
	PCS_OPTION_PROGRESS_FUNCTION_DATE,
	/* 设置是否启用下载或上传进度，值为PcsBool类型 */
	PCS_OPTION_PROGRESS,


} PcsOption;

typedef struct PcsInfo {
	enum PcsOption	option;
	void			*value;
} PcsInfo;

typedef enum PcsRes {
	PCS_NONE = -1,
	PCS_OK = 0,
	PCS_FAIL,
	PCS_LOGIN,
	PCS_NOT_LOGIN,
	PCS_UNKNOWN_OPT,
	PCS_NO_BDSTOKEN,
	PCS_NETWORK_ERROR,
	PCS_WRONG_RESPONSE,
	PCS_NO_CAPTCHA_FUNC,
	PCS_GET_CAPTCHA_FAIL,
	PCS_ALLOC_MEMORY,
	PCS_BUILD_POST_DATA,
	PCS_BUILD_URL,
	PCS_NO_LIST,
	PCS_CREATE_OBJ,
	PCS_NOT_EXIST,
	PCS_CLONE_OBJ,
	PCS_WRONG_ARGS,



} PcsRes;

/*
 * 登录时，当需要输入验证码时，回调该函数用于获取验证码
 *   ptr  验证码图片的字节序
 *   size 验证码图片字节序的大小，以字节为单位
 *   captcha 用于接收验证码字符
 *   captchaSize captcha的最大长度
 *   state 使用PCS_OPTION_CAPTCHA_FUNCTION_DATA选项设定的值原样传入
 * 返回是否成功，返回PcsFalse，将导致登录中断
*/
typedef PcsBool (*PcsGetCaptchaFunction)(unsigned char *ptr, size_t size, char *captcha, size_t captchaSize, void *state);

typedef void *Pcs;

/*输出PCS API的版本号*/
PCS_API const char *pcs_version();

/*
 * 创建Pcs。
 * 成功后返回该Pcs的handle，否则返回NULL。注意此函数无法使用pcs_strerror()获取错误编号
*/
PCS_API Pcs pcs_create(const char *cookie_file);

/*
 * 释放Pcs对象
*/
PCS_API void pcs_destroy(Pcs handle);

/*
 * 如果已经登录，
 * 则返回用户的UID，否则返回NULL
*/
PCS_API const char *pcs_sysUID(Pcs handle);

/*
 * 获取错误消息。
 * 如果程序中存在错误，则返回错误描述，否则返回NULL
 * 例：
 *    PcsRes res;
 *    const char *err;
 *    res = pcs_login(handle);
 *    if (res != PCS_LOGIN) {
 *        err = pcs_strerror(handle);
 *        printf("Error: %s\n", err);
 *    }
 *
 * 例2：
 *    PcsRes res;
 *    const char *err;
 *    UInt64 quota, used;
 *    res = pcs_quota(handle, &quota, &used);
 *    if (res != PCS_OK) {
 *        err = pcs_strerror(handle);
 *        printf("Error: %s\n", err);
 *    }
 *
 * 例3：
 *    PcsFileInfoList list;
 *    const char *err;
 *    list = pcs_list(handle, "/", 1, 100, "name", PcsFalse);
 *    if (!list) {
 *        err = pcs_strerror(handle);
 *        if (err)
 *            printf("Error: %s\n", err);
 *        else
 *            printf("Empty Directory\n");
 *    }
 *    else {
 *        Print the list ...
 *    }
 * 
*/
PCS_API const char *pcs_strerror(Pcs handle);

/*
 * 设定单个选项，
 * 成功后返回PCS_OK，失败则返回错误编号
*/
PCS_API PcsRes pcs_setopt(Pcs handle, PcsOption opt, void *value);

/*
 * 一次设定多个选项，最后一项必须为PCS_OPTION_END。
 * 例： pcs_setopts(handle, PCS_OPTION_USERNAME, "username", PCS_OPTION_PASSWORD, "password", PCS_OPTION_END);
 *      pcs_setopts(handle, PCS_OPTION_CAPTCHA_FUNCTION, &get_captcha, PCS_OPTION_CAPTCHA_FUNCTION_DATA, state, PCS_OPTION_END);
 * 成功后返回PCS_OK，失败则返回错误编号
 */
PCS_API PcsRes pcs_setopts(Pcs handle, ...);

/*
 * 返回是否已经登录，
 * 已经登录则返回PCS_LOGIN，否则返回PCS_NOT_LOGIN
*/
PCS_API PcsRes pcs_islogin(Pcs handle);

/*
 * 登录百度帐号，
 * 成功后返回PCS_LOGIN，失败则返回错误编号
*/
PCS_API PcsRes pcs_login(Pcs handle);

/*
 * 获取网盘配额
 *   quota 用于接收总大小
 *   used  用于接收已使用值
 * 成功后返回PCS_OK，失败则返回错误编号
*/
PCS_API PcsRes pcs_quota(Pcs handle, UInt64 *quota, UInt64 *used);

/*
 * 创建一个目录
 * path  待创建的目录，地址需写全，如/temp
 * 成功后返回PCS_OK，失败则返回错误编号
*/
PCS_API PcsRes pcs_mkdir(Pcs handle, const char *path);

/*
 * 获取文件列表
 *   dir		待获取的目录，地址需写全，如/temp
 *   pageindex	页索引，从1开始
 *   pagesize	页大小
 *   order		排序字段，可选值：name|time|size
 *   desc		倒序传PcsTrue，正序传PcsFalse
 * 成功后，返回PcsFileInfoList类型实例，该类型为文件的双向链表，
 * 使用完成后需调用 pcs_filist_destroy() 方法释放。
 * 失败或目录为空则返回 NULL。返回NULL时，可根据pcs_strerror()的返回值来判断是否出错，如果未出错则错误消息为NULL
*/
PCS_API PcsFileInfoList *pcs_list(Pcs handle, const char *dir, int pageindex, int pagesize, const char *order, PcsBool desc);

/*
 * 在dir指定的文件夹中搜索关键字key
 *   dir		待搜索的目录，地址需写全，如/temp
 *   key	    关键词
 *   recursion	是否递归搜索其子目录
 * 成功后，返回PcsFileInfoList类型实例，该类型为文件的双向链表，
 * 使用完成后需调用 pcs_filist_destroy() 方法释放。
 * 失败或未找到则返回 NULL。返回NULL时，可根据pcs_strerror()的返回值来判断是否出错，如果未出错则错误消息为NULL
*/
PCS_API PcsFileInfoList *pcs_search(Pcs handle, const char *dir, const char *key, PcsBool recursion);

/*
 * 获取文件或目录的元信息，该方法通过pcs_search实现。
 *   path		待获取的文件或目录，地址需写全，如/temp, /temp/file.txt
 * 成功后，返回PcsFileInfo类型实例
 * 使用完成后需调用 pcs_fileinfo_destroy() 方法释放。
 * 失败或未找到则返回 NULL。返回NULL时，可根据pcs_strerror()的返回值来判断是否出错，如果未出错则错误消息为NULL
*/
PCS_API PcsFileInfo *pcs_meta(Pcs handle, const char *path);

/*
 * 删除多个文件
 *   slist  字符串的单项链表，一次可传入多个文件
 * 成功后返回PcsPanApiRes类型实例，可循环获取每个文件或目录的删除情况，成功则PcsPanApiResInfo->error为0，否则为错误编码
 * 使用完后需调用pcs_pan_api_res_destroy释放
 * 失败则返回 NULL。
 */
PCS_API PcsPanApiRes *pcs_delete(Pcs handle, PcsSList *slist);

/*
 * 重命名多个文件
 *   slist  字符串的单项链表，一次可传入多个文件，
 *          每项中的string1为待重命名的文件，路径需写全（如：/temp/file.txt）
 *          string2为新的名字，仅需文件名，例如newfile.txt，写全路径则重命名失败。
 *          如需要移动请用pcs_move()方法
 * 成功后返回PcsPanApiRes类型实例，可循环获取每个文件或目录的删除情况，成功则PcsPanApiResInfo->error为0，否则为错误编码
 * 使用完后需调用pcs_pan_api_res_destroy释放
 * 失败则返回 NULL。
 */
PCS_API PcsPanApiRes *pcs_rename(Pcs handle, PcsSList2 *slist);

/*
 * 移动多个文件
 *   slist  字符串的单项链表，一次可传入多个文件，
 *          每项中的string1为待移动的文件，路径需写全（如：/temp/file.txt）
 *          string2为新的名字，路径需写全，例如/temp/subfolder/newfile.txt。
 * 成功后返回PcsPanApiRes类型实例，可循环获取每个文件或目录的删除情况，成功则PcsPanApiResInfo->error为0，否则为错误编码
 * 使用完后需调用pcs_pan_api_res_destroy释放
 * 失败则返回 NULL。
 */
PCS_API PcsPanApiRes *pcs_move(Pcs handle, PcsSList2 *slist);

/*
 * 复制多个文件
 *   slist  字符串的单项链表，一次可传入多个文件，
 *          每项中的string1为待复制的文件，路径需写全（如：/temp/file.txt）
 *          string2为新的名字，路径需写全，例如/temp/subfolder/newfile.txt。
 * 成功后返回PcsPanApiRes类型实例，可循环获取每个文件或目录的删除情况，成功则PcsPanApiResInfo->error为0，否则为错误编码
 * 使用完后需调用pcs_pan_api_res_destroy释放
 * 失败则返回 NULL。
 */
PCS_API PcsPanApiRes *pcs_copy(Pcs handle, PcsSList2 *slist);

/*
 * 不下载文件，直接获取文本文件的内容
 *   path		待获取的文件或目录，地址需写全，如/temp, /temp/file.txt
 *   dstsz      获取到的内容的字节长度
 * 成功后返回文本内容
 * 使用完后无需调用pcs_free()释放
 * 失败或无内容则返回 NULL。
 */
PCS_API const char *pcs_cat(Pcs handle, const char *path, size_t *dstsz);

/*
 * 下载文件
 *   path   待下载的文件，地址需写全，如/temp/file.txt
 * 必须指定写入下载内容的函数，可通过PCS_OPTION_DOWNLOAD_WRITE_FUNCTION选项来指定
 * 成功后返回PCS_OK，失败则返回错误编号
 */
PCS_API PcsRes pcs_download(Pcs handle, const char *path);

/*
 * 把内存中的字节序上传到网盘
 *   path		目标文件，地址需写全，如/temp/file.txt
 *   overwrite  指定是否覆盖原文件，传入PcsTrue则覆盖，传入PcsFalse，则会使用当前日期重命名。
 *              例，如果文件file.txt以存在，则上传后新的文件自动变更为file20140117.txt
 *   buffer     待上传的字节序
 *   buffer_size 字节序的字节大小
 * 成功后，返回PcsFileInfo类型实例，该实例包含网盘中新文件的路径等信息
 * 使用完成后需调用 pcs_fileinfo_destroy() 方法释放。
 * 失败则返回 NULL。
 */
PCS_API PcsFileInfo *pcs_upload_buffer(Pcs handle, const char *path, PcsBool overwrite, 
									   const char *buffer, size_t buffer_size);

/*
 * 上传文件到网盘
 *   path		目标文件，地址需写全，如/temp/file.txt
 *   overwrite  指定是否覆盖原文件，传入PcsTrue则覆盖，传入PcsFalse，则会使用当前日期重命名。
 *              例，如果文件file.txt以存在，则上传后新的文件自动变更为file20140117.txt
 *   local_filename 待上传的本地文件
 * 通过PCS_OPTION_PROGRESS_FUNCTION选项设定进度条回调用，使用PCS_OPTION_PROGRESS启用进度条回调，可简单实现上传进度
 * 成功后，返回PcsFileInfo类型实例，该实例包含网盘中新文件的路径等信息
 * 使用完成后需调用 pcs_fileinfo_destroy() 方法释放。
 * 失败则返回 NULL。
 */
PCS_API PcsFileInfo *pcs_upload(Pcs handle, const char *path, PcsBool overwrite, 
									   const char *local_filename);
/*
 * 获取Cookie 数据。
 * 成功则返回Cookie数据，失败或没有返回NULL
 * 使用完后需要使用pcs_free释放
*/
PCS_API char *pcs_cookie_data(Pcs handle);

/*
* 获取最后一次请求的原始数据。
* @size 用于接收原始数据的长度
* @encode 用于接收原始数据的编码
* 返回原始数据的指针。
*/
PCS_API const char *pcs_req_rawdata(Pcs handle, int *size, const char **encode);

#endif
