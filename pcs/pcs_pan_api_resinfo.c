#include <string.h>
#ifdef WIN32
# include <malloc.h>
#else
# include <alloca.h>
#endif

#include "pcs_mem.h"
#include "pcs_pan_api_resinfo.h"

PCS_API const char *pcs_pan_api_res_info_errmsg(int error)
{
	const char *errmsg = NULL;
	switch (error) {
	case 0: //处理成功
		errmsg = "成功";
		break;
	case -8: //文件已存在于目标文件夹中
		errmsg = "文件已存在于目标文件夹中";
		break;
	case -9: //文件不存在
		errmsg = "文件不存在";
		break;
	case -10: //剩余空间不足
		errmsg = "剩余空间不足";
		break;
	default:
		errmsg = "未知错误";
	}
	return errmsg;
}

PCS_API PcsPanApiRes *pcs_pan_api_res_create()
{
	PcsPanApiRes *res = (PcsPanApiRes *)pcs_malloc(sizeof(PcsPanApiRes));
	if (res)
		memset(res, 0, sizeof(PcsPanApiRes));
	return res;
}

PCS_API void pcs_pan_api_res_destroy(PcsPanApiRes *res)
{
	if (res->info_list)
		pcs_pan_api_res_infolist_destroy(res->info_list);
	pcs_free(res);
}

PCS_API PcsPanApiResInfoList *pcs_pan_api_res_infolist_create()
{
	PcsPanApiResInfoList *res = (PcsPanApiResInfoList *)pcs_malloc(sizeof(PcsPanApiResInfoList));
	if (res)
		memset(res, 0, sizeof(PcsPanApiResInfoList));
	return res;
}

PCS_API void pcs_pan_api_res_infolist_destroy(PcsPanApiResInfoList *infolist)
{
	PcsPanApiResInfoList *p, *p2;
	p = infolist;
	while(p) {
		p2 = p;
		p = p->next;
		if (p2->info.path)
			pcs_free(p2->info.path);
		pcs_free(p2);
	}
}

