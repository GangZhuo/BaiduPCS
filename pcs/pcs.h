#ifndef _PCS_H
#define _PCS_H

#include "pcs_defs.h"
#include "pcs_fileinfo.h"
#include "pcs_pan_api_resinfo.h"

typedef enum PcsOption {
	PCS_OPTION_END = 0,
	/* ֵΪ��0��β��C��ʽ�ַ��� */
	PCS_OPTION_USERNAME,
	/* ֵΪ��0��β��C��ʽ�ַ��� */
	PCS_OPTION_PASSWORD,
	/* ֵΪPcsGetCaptcha���͵ĺ��� */
	PCS_OPTION_CAPTCHA_FUNCTION,
	/* Pcs����ʹ�ø�ֵ����ԭ�����ݵ�PcsGetCaptcha������ */
	PCS_OPTION_CAPTCHA_FUNCTION_DATA,
	/* ֵΪPcsHttpWriteFunction���͵ĺ��� */
	PCS_OPTION_DOWNLOAD_WRITE_FUNCTION,
	/* Pcs����ʹ�ø�ֵ����ԭ�����ݵ�PcsHttpWriteFunction������ */
	PCS_OPTION_DOWNLOAD_WRITE_FUNCTION_DATA,
	/* ֵΪPcsHttpResponseFunction���͵ĺ��� */
	PCS_OPTION_HTTP_RESPONSE_FUNCTION,
	/* Pcs����ʹ�ø�ֵ����ԭ�����ݵ�PcsHttpResponseFunction������ */
	PCS_OPTION_HTTP_RESPONSE_FUNCTION_DATE,
	/* ֵΪPcsHttpProgressCallback���͵ĺ��� */
	PCS_OPTION_PROGRESS_FUNCTION,
	/* Pcs����ʹ�ø�ֵ����ԭ�����ݵ�PcsHttpProgressCallback������ */
	PCS_OPTION_PROGRESS_FUNCTION_DATE,
	/* �����Ƿ��������ػ��ϴ����ȣ�ֵΪunsigned char����ָ�� */
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

typedef PcsBool (*PcsGetCaptchaFunction)(unsigned char *ptr, size_t size, char *captcha, size_t captchaSize, void *state);
typedef size_t (*PcsHttpWriteFunction)(char *ptr, size_t size, size_t contentlength, void *userdata);
typedef void (*PcsHttpResponseFunction)(unsigned char *ptr, size_t size, void *state);
typedef int (*PcsHttpProgressCallback)(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow);

typedef void *Pcs;

PCS_API Pcs pcs_create(const char *cookie_file);

PCS_API void pcs_destroy(Pcs handle);

PCS_API const char *pcs_sysUID(Pcs handle);

PCS_API const char *pcs_strerror(Pcs handle, PcsRes error);

PCS_API PcsRes pcs_setopt(Pcs handle, PcsOption opt, void *value);

PCS_API PcsRes pcs_setopts(Pcs handle, ...);

PCS_API PcsRes pcs_islogin(Pcs handle);

PCS_API PcsRes pcs_login(Pcs handle);

PCS_API PcsRes pcs_filemanager(Pcs handle);

PCS_API PcsRes pcs_quota(Pcs handle, UInt64 *quota, UInt64 *used);

PCS_API PcsRes pcs_mkdir(Pcs handle, const char *path);

/*
 * ��ȡ�ļ��б�
 *   dir ����ȡ��Ŀ¼
 *   pageindex ҳ����
 *   pagesize  ҳ��С
 *   order	 �����ֶ�
 *   desc		 ����1������0
 * �ɹ��󣬷���pcs_file_link_t ����ʵ����������Ϊ�ļ���˫������
 * ʹ����ɺ������ pcs_file_link_destroy() �����ͷš�
 * ʧ�ܣ��򷵻� NULL 
*/
PCS_API PcsFileInfoList *pcs_list(Pcs handle, const char *dir, int pageindex, int pagesize, const char *order, PcsBool desc);

PCS_API PcsFileInfoList *pcs_search(Pcs handle, const char *dir, const char *key, PcsBool recursion);

PCS_API PcsFileInfo *pcs_meta(Pcs handle, const char *path);

/* ɾ������ļ� */
PCS_API PcsPanApiRes *pcs_delete(Pcs handle, PcsSList *slist);

PCS_API PcsPanApiRes *pcs_rename(Pcs handle, PcsSList2 *slist);

PCS_API PcsPanApiRes *pcs_move(Pcs handle, PcsSList2 *slist);

PCS_API PcsPanApiRes *pcs_copy(Pcs handle, PcsSList2 *slist);

PCS_API const char *pcs_cat(Pcs handle, const char *path);

PCS_API PcsRes pcs_download(Pcs handle, const char *path);

PCS_API PcsFileInfo *pcs_upload_buffer(Pcs handle, const char *path, PcsBool overwrite, 
									   const char *buffer, size_t buffer_size);

PCS_API PcsFileInfo *pcs_upload(Pcs handle, const char *path, PcsBool overwrite, 
									   const char *local_filename);


#endif
