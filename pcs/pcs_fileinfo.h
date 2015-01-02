#ifndef _PCS_FILEINFO_H
#define _PCS_FILEINFO_H

#include "pcs_defs.h"

/* 用于存储网盘中文件的元数据 */
typedef struct PcsFileInfo {
	uint64_t		fs_id;
	char		*path;
	char		*server_filename;
	time_t		server_ctime;
	time_t		server_mtime;
	time_t		local_ctime;
	time_t		local_mtime;
	int64_t		size;
	int			category;
	PcsBool		isdir;
	PcsBool		dir_empty;
	PcsBool		empty;
	char		*md5;
	char		*dlink;
	char		**block_list; /*  文件所有分片的md5数组json字符串, 只有 pcs_meta() 设置该值 */
	PcsBool		ifhassubdir; /* 是否含有子目录, 只有 pcs_meta() 设置该值  */

	int			user_flag;
} PcsFileInfo;

/*网盘中文件元数据链表的单个节点*/
typedef struct PcsFileInfoListItem {
	PcsFileInfo					*info;
	struct PcsFileInfoListItem	*prev;
	struct PcsFileInfoListItem	*next;
} PcsFileInfoListItem;

/*以链表形式存储的网盘文件元数据列表*/
typedef struct PcsFileInfoList {
	int						count;
	PcsFileInfoListItem		*link;
	PcsFileInfoListItem		*link_tail;
} PcsFileInfoList;

/*网盘文件元数据列表的迭代器*/
typedef struct PcsFileInfoListIterater {
	PcsBool				invert;
	PcsFileInfoList		*list;
	PcsFileInfoListItem	*cursor;
	PcsFileInfo			*current;
} PcsFileInfoListIterater;

PCS_API PcsFileInfo *pcs_fileinfo_create();
PCS_API void pcs_fileinfo_destroy(PcsFileInfo *fi);
/*复制一份PcsFileInfo。注意是深克隆。*/
PCS_API PcsFileInfo *pcs_fileinfo_clone(PcsFileInfo *fi);

PCS_API PcsFileInfoListItem *pcs_filistitem_create();
PCS_API void pcs_filistitem_destroy(PcsFileInfoListItem *item);

PCS_API PcsFileInfoList *pcs_filist_create();
PCS_API void pcs_filist_destroy(PcsFileInfoList *list);
PCS_API void pcs_filist_add(PcsFileInfoList *list, PcsFileInfoListItem *item);
PCS_API void pcs_filist_remove(PcsFileInfoList *list, PcsFileInfoListItem *item, PcsFileInfoListIterater *iterater);
PCS_API void pcs_filist_combin(PcsFileInfoList *list, PcsFileInfoList *src);

/*
 * invert - 是否从后向前迭代
 */
PCS_API void pcs_filist_iterater_init(PcsFileInfoList *list, PcsFileInfoListIterater *iterater, PcsBool invert);
PCS_API PcsBool pcs_filist_iterater_next(PcsFileInfoListIterater *iterater);


#endif
