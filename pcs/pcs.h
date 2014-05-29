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
	/* �����Ƿ��������ػ��ϴ����ȣ�ֵΪPcsBool���� */
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
 * ��¼ʱ������Ҫ������֤��ʱ���ص��ú������ڻ�ȡ��֤��
 *   ptr  ��֤��ͼƬ���ֽ���
 *   size ��֤��ͼƬ�ֽ���Ĵ�С�����ֽ�Ϊ��λ
 *   captcha ���ڽ�����֤���ַ�
 *   captchaSize captcha����󳤶�
 *   state ʹ��PCS_OPTION_CAPTCHA_FUNCTION_DATAѡ���趨��ֵԭ������
 * �����Ƿ�ɹ�������PcsFalse�������µ�¼�ж�
*/
typedef PcsBool (*PcsGetCaptchaFunction)(unsigned char *ptr, size_t size, char *captcha, size_t captchaSize, void *state);

typedef void *Pcs;

/*���PCS API�İ汾��*/
PCS_API const char *pcs_version();

/*
 * ����Pcs��
 * �ɹ��󷵻ظ�Pcs��handle�����򷵻�NULL��ע��˺����޷�ʹ��pcs_strerror()��ȡ������
*/
PCS_API Pcs pcs_create(const char *cookie_file);

/*
 * �ͷ�Pcs����
*/
PCS_API void pcs_destroy(Pcs handle);

/*
 * ����Ѿ���¼��
 * �򷵻��û���UID�����򷵻�NULL
*/
PCS_API const char *pcs_sysUID(Pcs handle);

/*
 * ��ȡ������Ϣ��
 * ��������д��ڴ����򷵻ش������������򷵻�NULL
 * ����
 *    PcsRes res;
 *    const char *err;
 *    res = pcs_login(handle);
 *    if (res != PCS_LOGIN) {
 *        err = pcs_strerror(handle);
 *        printf("Error: %s\n", err);
 *    }
 *
 * ��2��
 *    PcsRes res;
 *    const char *err;
 *    UInt64 quota, used;
 *    res = pcs_quota(handle, &quota, &used);
 *    if (res != PCS_OK) {
 *        err = pcs_strerror(handle);
 *        printf("Error: %s\n", err);
 *    }
 *
 * ��3��
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
 * �趨����ѡ�
 * �ɹ��󷵻�PCS_OK��ʧ���򷵻ش�����
*/
PCS_API PcsRes pcs_setopt(Pcs handle, PcsOption opt, void *value);

/*
 * һ���趨���ѡ����һ�����ΪPCS_OPTION_END��
 * ���� pcs_setopts(handle, PCS_OPTION_USERNAME, "username", PCS_OPTION_PASSWORD, "password", PCS_OPTION_END);
 *      pcs_setopts(handle, PCS_OPTION_CAPTCHA_FUNCTION, &get_captcha, PCS_OPTION_CAPTCHA_FUNCTION_DATA, state, PCS_OPTION_END);
 * �ɹ��󷵻�PCS_OK��ʧ���򷵻ش�����
 */
PCS_API PcsRes pcs_setopts(Pcs handle, ...);

/*
 * �����Ƿ��Ѿ���¼��
 * �Ѿ���¼�򷵻�PCS_LOGIN�����򷵻�PCS_NOT_LOGIN
*/
PCS_API PcsRes pcs_islogin(Pcs handle);

/*
 * ��¼�ٶ��ʺţ�
 * �ɹ��󷵻�PCS_LOGIN��ʧ���򷵻ش�����
*/
PCS_API PcsRes pcs_login(Pcs handle);

/*
 * ��ȡ�������
 *   quota ���ڽ����ܴ�С
 *   used  ���ڽ�����ʹ��ֵ
 * �ɹ��󷵻�PCS_OK��ʧ���򷵻ش�����
*/
PCS_API PcsRes pcs_quota(Pcs handle, UInt64 *quota, UInt64 *used);

/*
 * ����һ��Ŀ¼
 * path  ��������Ŀ¼����ַ��дȫ����/temp
 * �ɹ��󷵻�PCS_OK��ʧ���򷵻ش�����
*/
PCS_API PcsRes pcs_mkdir(Pcs handle, const char *path);

/*
 * ��ȡ�ļ��б�
 *   dir		����ȡ��Ŀ¼����ַ��дȫ����/temp
 *   pageindex	ҳ��������1��ʼ
 *   pagesize	ҳ��С
 *   order		�����ֶΣ���ѡֵ��name|time|size
 *   desc		����PcsTrue������PcsFalse
 * �ɹ��󣬷���PcsFileInfoList����ʵ����������Ϊ�ļ���˫������
 * ʹ����ɺ������ pcs_filist_destroy() �����ͷš�
 * ʧ�ܻ�Ŀ¼Ϊ���򷵻� NULL������NULLʱ���ɸ���pcs_strerror()�ķ���ֵ���ж��Ƿ�������δ�����������ϢΪNULL
*/
PCS_API PcsFileInfoList *pcs_list(Pcs handle, const char *dir, int pageindex, int pagesize, const char *order, PcsBool desc);

/*
 * ��dirָ�����ļ����������ؼ���key
 *   dir		��������Ŀ¼����ַ��дȫ����/temp
 *   key	    �ؼ���
 *   recursion	�Ƿ�ݹ���������Ŀ¼
 * �ɹ��󣬷���PcsFileInfoList����ʵ����������Ϊ�ļ���˫������
 * ʹ����ɺ������ pcs_filist_destroy() �����ͷš�
 * ʧ�ܻ�δ�ҵ��򷵻� NULL������NULLʱ���ɸ���pcs_strerror()�ķ���ֵ���ж��Ƿ�������δ�����������ϢΪNULL
*/
PCS_API PcsFileInfoList *pcs_search(Pcs handle, const char *dir, const char *key, PcsBool recursion);

/*
 * ��ȡ�ļ���Ŀ¼��Ԫ��Ϣ���÷���ͨ��pcs_searchʵ�֡�
 *   path		����ȡ���ļ���Ŀ¼����ַ��дȫ����/temp, /temp/file.txt
 * �ɹ��󣬷���PcsFileInfo����ʵ��
 * ʹ����ɺ������ pcs_fileinfo_destroy() �����ͷš�
 * ʧ�ܻ�δ�ҵ��򷵻� NULL������NULLʱ���ɸ���pcs_strerror()�ķ���ֵ���ж��Ƿ�������δ�����������ϢΪNULL
*/
PCS_API PcsFileInfo *pcs_meta(Pcs handle, const char *path);

/*
 * ɾ������ļ�
 *   slist  �ַ����ĵ�������һ�οɴ������ļ�
 * �ɹ��󷵻�PcsPanApiRes����ʵ������ѭ����ȡÿ���ļ���Ŀ¼��ɾ��������ɹ���PcsPanApiResInfo->errorΪ0������Ϊ�������
 * ʹ����������pcs_pan_api_res_destroy�ͷ�
 * ʧ���򷵻� NULL��
 */
PCS_API PcsPanApiRes *pcs_delete(Pcs handle, PcsSList *slist);

/*
 * ����������ļ�
 *   slist  �ַ����ĵ�������һ�οɴ������ļ���
 *          ÿ���е�string1Ϊ�����������ļ���·����дȫ���磺/temp/file.txt��
 *          string2Ϊ�µ����֣������ļ���������newfile.txt��дȫ·����������ʧ�ܡ�
 *          ����Ҫ�ƶ�����pcs_move()����
 * �ɹ��󷵻�PcsPanApiRes����ʵ������ѭ����ȡÿ���ļ���Ŀ¼��ɾ��������ɹ���PcsPanApiResInfo->errorΪ0������Ϊ�������
 * ʹ����������pcs_pan_api_res_destroy�ͷ�
 * ʧ���򷵻� NULL��
 */
PCS_API PcsPanApiRes *pcs_rename(Pcs handle, PcsSList2 *slist);

/*
 * �ƶ�����ļ�
 *   slist  �ַ����ĵ�������һ�οɴ������ļ���
 *          ÿ���е�string1Ϊ���ƶ����ļ���·����дȫ���磺/temp/file.txt��
 *          string2Ϊ�µ����֣�·����дȫ������/temp/subfolder/newfile.txt��
 * �ɹ��󷵻�PcsPanApiRes����ʵ������ѭ����ȡÿ���ļ���Ŀ¼��ɾ��������ɹ���PcsPanApiResInfo->errorΪ0������Ϊ�������
 * ʹ����������pcs_pan_api_res_destroy�ͷ�
 * ʧ���򷵻� NULL��
 */
PCS_API PcsPanApiRes *pcs_move(Pcs handle, PcsSList2 *slist);

/*
 * ���ƶ���ļ�
 *   slist  �ַ����ĵ�������һ�οɴ������ļ���
 *          ÿ���е�string1Ϊ�����Ƶ��ļ���·����дȫ���磺/temp/file.txt��
 *          string2Ϊ�µ����֣�·����дȫ������/temp/subfolder/newfile.txt��
 * �ɹ��󷵻�PcsPanApiRes����ʵ������ѭ����ȡÿ���ļ���Ŀ¼��ɾ��������ɹ���PcsPanApiResInfo->errorΪ0������Ϊ�������
 * ʹ����������pcs_pan_api_res_destroy�ͷ�
 * ʧ���򷵻� NULL��
 */
PCS_API PcsPanApiRes *pcs_copy(Pcs handle, PcsSList2 *slist);

/*
 * �������ļ���ֱ�ӻ�ȡ�ı��ļ�������
 *   path		����ȡ���ļ���Ŀ¼����ַ��дȫ����/temp, /temp/file.txt
 *   dstsz      ��ȡ�������ݵ��ֽڳ���
 * �ɹ��󷵻��ı�����
 * ʹ������������pcs_free()�ͷ�
 * ʧ�ܻ��������򷵻� NULL��
 */
PCS_API const char *pcs_cat(Pcs handle, const char *path, size_t *dstsz);

/*
 * �����ļ�
 *   path   �����ص��ļ�����ַ��дȫ����/temp/file.txt
 * ����ָ��д���������ݵĺ�������ͨ��PCS_OPTION_DOWNLOAD_WRITE_FUNCTIONѡ����ָ��
 * �ɹ��󷵻�PCS_OK��ʧ���򷵻ش�����
 */
PCS_API PcsRes pcs_download(Pcs handle, const char *path);

/*
 * ���ڴ��е��ֽ����ϴ�������
 *   path		Ŀ���ļ�����ַ��дȫ����/temp/file.txt
 *   overwrite  ָ���Ƿ񸲸�ԭ�ļ�������PcsTrue�򸲸ǣ�����PcsFalse�����ʹ�õ�ǰ������������
 *              ��������ļ�file.txt�Դ��ڣ����ϴ����µ��ļ��Զ����Ϊfile20140117.txt
 *   buffer     ���ϴ����ֽ���
 *   buffer_size �ֽ�����ֽڴ�С
 * �ɹ��󣬷���PcsFileInfo����ʵ������ʵ���������������ļ���·������Ϣ
 * ʹ����ɺ������ pcs_fileinfo_destroy() �����ͷš�
 * ʧ���򷵻� NULL��
 */
PCS_API PcsFileInfo *pcs_upload_buffer(Pcs handle, const char *path, PcsBool overwrite, 
									   const char *buffer, size_t buffer_size);

/*
 * �ϴ��ļ�������
 *   path		Ŀ���ļ�����ַ��дȫ����/temp/file.txt
 *   overwrite  ָ���Ƿ񸲸�ԭ�ļ�������PcsTrue�򸲸ǣ�����PcsFalse�����ʹ�õ�ǰ������������
 *              ��������ļ�file.txt�Դ��ڣ����ϴ����µ��ļ��Զ����Ϊfile20140117.txt
 *   local_filename ���ϴ��ı����ļ�
 * ͨ��PCS_OPTION_PROGRESS_FUNCTIONѡ���趨�������ص��ã�ʹ��PCS_OPTION_PROGRESS���ý������ص����ɼ�ʵ���ϴ�����
 * �ɹ��󣬷���PcsFileInfo����ʵ������ʵ���������������ļ���·������Ϣ
 * ʹ����ɺ������ pcs_fileinfo_destroy() �����ͷš�
 * ʧ���򷵻� NULL��
 */
PCS_API PcsFileInfo *pcs_upload(Pcs handle, const char *path, PcsBool overwrite, 
									   const char *local_filename);
/*
 * ��ȡCookie ���ݡ�
 * �ɹ��򷵻�Cookie���ݣ�ʧ�ܻ�û�з���NULL
 * ʹ�������Ҫʹ��pcs_free�ͷ�
*/
PCS_API char *pcs_cookie_data(Pcs handle);

/*
* ��ȡ���һ�������ԭʼ���ݡ�
* @size ���ڽ���ԭʼ���ݵĳ���
* @encode ���ڽ���ԭʼ���ݵı���
* ����ԭʼ���ݵ�ָ�롣
*/
PCS_API const char *pcs_req_rawdata(Pcs handle, int *size, const char **encode);

#endif
