#ifndef _PCS_HTTP_H
#define _PCS_HTTP_H

#include <stdarg.h>
#include <curl/curl.h>
#include "pcs_defs.h"

typedef void *PcsHttp;
typedef void *PcsHttpForm;

/*
 * �趨�ûص���Pcsÿ�������ȡ��ֵ������øûص�����������ʱ��
 *   ptr  �������ȡ�����ֽ���
 *   size �ֽ���Ĵ�С�����ֽ�Ϊ��λ
 *   contentlength ��������HTTPͷ�е�Content-Lengthֵ
 *   userdata ʹ��PCS_OPTION_DOWNLOAD_WRITE_FUNCTION_DATAѡ���趨��ֵԭ������
 * ����д����ֽ������������ֵ�ʹ����size��һ���������������ж�
*/
typedef size_t (*PcsHttpWriteFunction)(char *ptr, size_t size, size_t contentlength, void *userdata);

/*
 * �趨�ûص���Pcsÿ�������ȡ��ֵ������øûص���
 * ��PcsHttpWriteFunction�������ǣ��ûص����ڻ�ȡ��ȫ�����ݺ󴥷�,
 * ��PcsHttpWriteFunction��ÿ��ȡ��һ���ֽ����򴥷���
 * ÿ��HTTP����PcsHttpResponseFunctionֻ�ᴥ��һ�Σ���PcsHttpWriteFunction���ܴ������
 *   ptr  �������ȡ�����ֽ���
 *   size �ֽ���Ĵ�С�����ֽ�Ϊ��λ
 *   userdata ʹ��PCS_OPTION_HTTP_RESPONSE_FUNCTION_DATEѡ���趨��ֵԭ������
*/
typedef void (*PcsHttpResponseFunction)(unsigned char *ptr, size_t size, void *state);

/*
 * �趨�ûص���Pcsÿ�ϴ�������һ���ֽ���������ʱ������øûص������øûص���ʵ���ϴ�ʱ�Ľ�����
 * ע�⣺ֻ���趨PCS_OPTION_PROGRESS��ֵΪPcsTrue��Ż����ý�����
 *   dltotal  ����������Ҫ���ض����ֽ�
 *   dlnow    ���������Ѿ����ض����ֽ�
 *   ultotal  ��Ҫ�ϴ������ֽ�
 *   ulnow    �Ѿ��ϴ������ֽ�
 *   clientp ʹ��PCS_OPTION_PROGRESS_FUNCTION_DATEѡ���趨��ֵԭ������
 * ���ط���ֵ���������ж��ϴ�������
*/
typedef int (*PcsHttpProgressCallback)(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow);

typedef enum PcsHttpOption {
	PCS_HTTP_OPTION_END = 0,
	/* ֵΪPcsHttpWriteFunction���͵ĺ�����������pcs_http_get_download����ʱ����ѡ���ĺ������ڴ�����������ص����ݡ� */
	PCS_HTTP_OPTION_HTTP_WRITE_FUNCTION,
	/* PcsHttp����ʹ�ø�ֵ����ԭ�����ݵ�PcsHttpWriteFunction�����С�
	������pcs_http_get_download����ʱ����ѡ���Ķ���ԭ�����ݵ�PcsHttpWriteFunctionָ���ĺ����С� */
	PCS_HTTP_OPTION_HTTP_WRITE_FUNCTION_DATE,
	/* ֵΪPcsHttpReadFunction���͵ĺ�����������δʹ�á� */
	PCS_HTTP_OPTION_HTTP_READ_FUNCTION,
	/* PcsHttp����ʹ�ø�ֵ����ԭ�����ݵ�PcsHttpReadFunction�����С�������δʹ�á� */
	PCS_HTTP_OPTION_HTTP_READ_FUNCTION_DATE,
	/* ֵΪPcsHttpResponseFunction���͵ĺ��� */
	PCS_HTTP_OPTION_HTTP_RESPONSE_FUNCTION,
	/* PcsHttp����ʹ�ø�ֵ����ԭ�����ݵ�PcsHttpResponseFunction������ */
	PCS_HTTP_OPTION_HTTP_RESPONSE_FUNCTION_DATE,
	/* ֵΪPcsHttpProgressCallback���͵ĺ��� */
	PCS_HTTP_OPTION_PROGRESS_FUNCTION,
	/* PcsHttp����ʹ�ø�ֵ����ԭ�����ݵ�PcsHttpProgressCallback������ */
	PCS_HTTP_OPTION_PROGRESS_FUNCTION_DATE,
	/* �����Ƿ��������ػ��ϴ����ȣ�ֵΪunsigned char����ָ�� */
	PCS_HTTP_OPTION_PROGRESS,


} PcsHttpOption;

/*
 * ����һ��PcsHttp����
 *   cookie_file   ָ������Cookie���ļ�������ļ������ڣ����Զ��������ļ���
 *                 ������һ������ʱ�������Ӹ��ļ��е�Cookie�������˳��󣬱������µ�Cookie�����ļ��С�
 * �ɹ��󣬷��ش����Ķ���ʧ���򷵻�NULL��ʹ����ɺ������pcs_http_destroy()���ͷ���Դ
 */
PCS_API PcsHttp pcs_http_create(const char *cookie_file);
/*
 * �ͷŵ�PcsHttp����
 */
PCS_API void pcs_http_destroy(PcsHttp handle);
/*
 * �������һ�η����Ĵ�������
 */
PCS_API const char *pcs_http_strerror(PcsHttp handle);
/*
 * �������һ������� HTTP״̬�롣״̬��ɲο�"http://zh.wikipedia.org/wiki/HTTP%E7%8A%B6%E6%80%81%E7%A0%81"
 */
PCS_API int pcs_http_code(PcsHttp handle);
/*
 * ����PcsHttp����ѡ�ÿ��ֻ������һ���� 
 */
PCS_API void pcs_http_setopt(PcsHttp handle, PcsHttpOption opt, void *value);

/*
 * һ���趨�������ѡ����һ�����ΪPCS_HTTP_OPTION_END��
 * ���� pcs_http_setopts(handle, PCS_HTTP_OPTION_HTTP_WRITE_FUNCTION, &cb_write, PCS_HTTP_OPTION_HTTP_READ_FUNCTION_DATE, state, PCS_HTTP_OPTION_END);
 */
PCS_API void pcs_http_setopts(PcsHttp handle, ...);
/*
 * pcs_http_build_url�Ĳ����б�ģʽ
 * Need call pcs_free(void *) to free the return value.
*/
PCS_API char *pcs_http_build_url_v(PcsHttp handle, const char *url, va_list args);
/*
 * ƴ��URL����α���ȫ��Ϊ Key-Value�ļ�ֵ�ԣ������ͱ���const char *���͡����һ�����ΪNULL��
 * ���ֵ�а������Ļ������ַ���������UTF-8�����롣
 *  url  - ����ַ
 *  ���� 
 *     pcs_http_build_url(pcs->http, "http://baidu.com", "key1", "value1", "key2", "value2", NULL); //���Ϊ��http://baidu.com?key1=value1&key2=value2
 *     pcs_http_build_url(pcs->http, "http://baidu.com?s=ab", "key1", "value1", "key2", "&ab", NULL); //���Ϊ��http://baidu.com??s=ab&key1=value1&key2=%26ab
 *     pcs_http_build_url(pcs->http, "http://baidu.com?s=ab&", "key1", "value1", "key2", "&ab", NULL); //���Ϊ��http://baidu.com??s=ab&key1=value1&key2=%26ab
 * �ɹ��󷵻�ƴ�Ӻ�ĵ�ַ��ʧ���򷵻�NULL
 * Need call pcs_free(void *) to free the return value.
*/
PCS_API char *pcs_http_build_url(PcsHttp handle, const char *url, ...);
/*
 * pcs_http_build_post_data�Ĳ����б�ģʽ
 * Need call pcs_free(void *) to free the return value.
*/
PCS_API char *pcs_http_build_post_data_v(PcsHttp handle, va_list args);
/*
 * ������pcs_http_build_url��ֻ�����ú���ƴ�ӵ�����Ҫ���͵���������ݡ�
 * ��α���ȫ��Ϊ Key-Value�ļ�ֵ�ԣ������ͱ���const char *���͡����һ�����ΪNULL��
 * ���ֵ�а������Ļ������ַ���������UTF-8�����롣
 *  url  - ����ַ
 *  ���� 
 *     pcs_http_build_post_data(pcs->http, "key1", "value1", "key2", "value2", NULL); //���Ϊ��key1=value1&key2=value2
 *     pcs_http_build_post_data(pcs->http, "key1", "value1", "key2", "&ab", NULL); //���Ϊ��key1=value1&key2=%26ab
 * �ɹ��󷵻�ƴ�Ӻ���ַ�����ʧ���򷵻�NULL
 * Need call pcs_free(void *) to free the return value.
*/
PCS_API char *pcs_http_build_post_data(PcsHttp handle, ...);
/*
 * �������ֻ�ȡCookieֵ��
 * �ɹ��󷵻�Cookieֵ��ʧ�ܻ򲻴����򷵻�NULL��
 * Need call pcs_free(void *) to free the return value.
*/
PCS_API char *pcs_http_get_cookie(PcsHttp handle, const char *cookie_name);

/*
 * ��ȡ���һ������ʱ�������ķ�������
 * �����δ�ӷ��������������򷵻�NULL������Ҫ����pcs_free�ͷ��ڴ档
 */
PCS_API const char *pcs_http_get_response(PcsHttp handle);
/*
 * ��ȡ���һ������ʱ�������������ݵ��ֽڳ���
 * �����δ�ӷ��������������򷵻�0��
 */
PCS_API int pcs_http_get_response_size(PcsHttp handle);
/*
 * �����������һ��GET����
 *   url             ��������ַ
 *   follow_location ���������������ת����һ��ҳ���ָ��ʱ���Ƿ��Զ���ת��ȥ�������ת�Ļ����򷵻���ת��ҳ�������
 * ���ط��������ص����ݡ������Զ�����Ϊ��ǰ����ϵͳʹ�õı��롣
 * ���磺��Windowsϵͳ�У����ϵͳ����ΪGB2312���򷵻������Զ�����ΪGB2312���룻��Linuxϵͳ�У�ϵͳ����ΪUTF-8���򷵻����ݵı�����ΪUTF-8
 * Not need call pcs_free(void *) to free the return value.
 * The memory will auto free when call pcs_http_destroy
*/
PCS_API char *pcs_http_get(PcsHttp handle, const char *url, PcsBool follow_location);
/*
 * �����������һ��GET����
 *   url             ��������ַ
 *   follow_location ���������������ת����һ��ҳ���ָ��ʱ���Ƿ��Զ���ת��ȥ�������ת�Ļ����򷵻���ת��ҳ�������
 * ���ط��������ص����ݡ����ݲ���ִ�н���������÷���һ�����ڻ�ȡͼƬ�����ڻ�ȡ�������ļ���ԭʼ���ݡ�
 * Not need call pcs_free(void *) to free the return value.
 * The memory will auto free when call pcs_http_destroy
*/
PCS_API char *pcs_http_get_raw(PcsHttp handle, const char *url, PcsBool follow_location, size_t *sz);
/*
 * �����������һ��POST����
 *   url             ��������ַ
 *   follow_location ���������������ת����һ��ҳ���ָ��ʱ���Ƿ��Զ���ת��ȥ�������ת�Ļ����򷵻���ת��ҳ�������
 * ���ط��������ص����ݡ������Զ�����Ϊ��ǰ����ϵͳʹ�õı��롣
 * ���磺��Windowsϵͳ�У����ϵͳ����ΪGB2312���򷵻������Զ�����ΪGB2312���룻��Linuxϵͳ�У�ϵͳ����ΪUTF-8���򷵻����ݵı�����ΪUTF-8
 * Not need call pcs_free(void *) to free the return value.
 * The memory will auto free when call pcs_http_destroy
*/
PCS_API char *pcs_http_post(PcsHttp handle, const char *url, char *post_data, PcsBool follow_location);

/*
 * �����������һ��GET���󡣵���ȡ���������������ݺ󣬵���PCS_HTTP_OPTION_HTTP_WRITE_FUNCTION����ĺ�����д�����ݡ�
 *   url             ��������ַ
 *   follow_location ���������������ת����һ��ҳ���ָ��ʱ���Ƿ��Զ���ת��ȥ�������ת�Ļ����򷵻���ת��ҳ�������
 * �����Ƿ����سɹ������ݲ���ִ�н���������÷���һ����������ͼƬ���ļ���
 * Not need call pcs_free(void *) to free the return value.
 * The memory will auto free when call pcs_http_destroy
*/
PCS_API PcsBool pcs_http_get_download(PcsHttp handle, const char *url, PcsBool follow_location);

/*
 * ��PcsHttpForm���������һ�������ļ���
 *   post        �ļ�����ӵ���PcsHttpForm�����С�
 *   param_name  ���͸��������Ĵ�����ļ����ݵĲ������֡�
 *   filename    ���ļ��ı����ļ�����PcsHttpForm���Ӹ�·����ȡ�ļ�����
 *   simulate_filename ���͵����������ļ���������ָ����ͬ��filename�����֣��������յ��ı����ļ�������simulate_filename������filename
 * ���� pcs_http_form_addfile(pcs->http, &form, "file", local_filename, "sample.dat"); //�������� "file"���ļ����ݴ洢��local_filenameָ�����ļ��У����������յ����ļ�������"sample.dat"
 * ��ӳɹ��󣬷���PcsTrue�����򷵻�PcsFalse��
*/
PCS_API PcsBool pcs_http_form_addfile(PcsHttp handle, PcsHttpForm *post, const char *param_name, 
									  const char *filename, const char *simulate_filename);
/* ͬpcs_http_form_addfile��ֻ�����Ǵ��ڴ��ж�ȡ�ļ����ݡ� */
PCS_API PcsBool pcs_http_form_addbuffer(PcsHttp handle, PcsHttpForm *post, const char *param_name,
										const char *buffer, long buffer_size, const char *simulate_filename);
/*�ͷŵ�PcsHttpForm��Դ*/
PCS_API void pcs_http_form_destroy(PcsHttp handle, PcsHttpForm post);

/*
 * �����������һ��POST���󡣸÷��������ϴ��ļ�����������
 *   url             ��������ַ
 *   data            ���͵������������ݡ�
 *   follow_location ���������������ת����һ��ҳ���ָ��ʱ���Ƿ��Զ���ת��ȥ�������ת�Ļ����򷵻���ת��ҳ�������
 * ���ط��������ص����ݡ������Զ�����Ϊ��ǰ����ϵͳʹ�õı��롣
 * ���磺��Windowsϵͳ�У����ϵͳ����ΪGB2312���򷵻������Զ�����ΪGB2312���룻��Linuxϵͳ�У�ϵͳ����ΪUTF-8���򷵻����ݵı�����ΪUTF-8
 * Not need call pcs_free(void *) to free the return value.
 * The memory will auto free when call pcs_http_destroy
*/
PCS_API char *pcs_post_httpform(PcsHttp handle, const char *url, PcsHttpForm data, PcsBool follow_location);

/*���ַ�����ʽ��������Cookie���ݡ�*/
PCS_API char *pcs_http_cookie_data(PcsHttp handle);

PCS_API const char *pcs_http_rawdata(PcsHttp handle, int *size, const char **encode);

#endif
