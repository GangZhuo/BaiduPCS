#ifndef _PCS_PAN_API_RESINFO_H
#define _PCS_PAN_API_RESINFO_H

/*网盘API返回数据格式*/
typedef struct PcsPanApiResInfo {
	char	*path;
	int		error;
} PcsPanApiResInfo;

typedef struct PcsPanApiResInfoList {
	PcsPanApiResInfo			info;
	struct PcsPanApiResInfoList	*next;
} PcsPanApiResInfoList;

typedef struct PcsPanApiRes {
	int						error;
	PcsPanApiResInfoList	*info_list;
} PcsPanApiRes;

PCS_API const char *pcs_pan_api_res_info_errmsg(int error);

PCS_API PcsPanApiRes *pcs_pan_api_res_create();
PCS_API void pcs_pan_api_res_destroy(PcsPanApiRes *res);

PCS_API PcsPanApiResInfoList *pcs_pan_api_res_infolist_create();
PCS_API void pcs_pan_api_res_infolist_destroy(PcsPanApiResInfoList *infolist);

#endif
