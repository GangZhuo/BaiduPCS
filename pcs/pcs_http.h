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
	/* ֵΪPcsHttpWriteFunction���͵ĺ��� */
	PCS_HTTP_OPTION_HTTP_WRITE_FUNCTION,
	/* PcsHttp����ʹ�ø�ֵ����ԭ�����ݵ�PcsHttpWriteFunction������ */
	PCS_HTTP_OPTION_HTTP_WRITE_FUNCTION_DATE,
	/* ֵΪPcsHttpReadFunction���͵ĺ��� */
	PCS_HTTP_OPTION_HTTP_READ_FUNCTION,
	/* PcsHttp����ʹ�ø�ֵ����ԭ�����ݵ�PcsHttpReadFunction������ */
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
