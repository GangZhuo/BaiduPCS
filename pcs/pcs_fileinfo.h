#ifndef _PCS_FILEINFO_H
#define _PCS_FILEINFO_H

#include "pcs_defs.h"

/* ���ڴ洢�������ļ���Ԫ���� */
typedef struct PcsFileInfo {
	UInt64		fs_id;
	char		*path;
	char		*server_filename;
	UInt64		server_ctime;
	UInt64		server_mtime;
	UInt64		local_ctime;
	UInt64		local_mtime;
	UInt64		size;
	int			category;
	PcsBool		isdir;
	PcsBool		dir_empty;
	PcsBool		empty;
	char		*md5;
	char		*dlink;
	char		**block_list; /*  �ļ����з�Ƭ��md5����json�ַ���, ֻ�� pcs_meta() ���ø�ֵ */
	PcsBool		ifhassubdir; /* �Ƿ�����Ŀ¼, ֻ�� pcs_meta() ���ø�ֵ  */

	int			user_flag;
} PcsFileInfo;

/*�������ļ�Ԫ��������ĵ����ڵ�*/
typedef struct PcsFileInfoListItem {
	PcsFileInfo					*info;
	struct PcsFileInfoListItem	*prev;
	struct PcsFileInfoListItem	*next;
} PcsFileInfoListItem;

/*��������ʽ�洢�������ļ�Ԫ�����б�*/
typedef struct PcsFileInfoList {
	int						count;
	PcsFileInfoListItem		*link;
	PcsFileInfoListItem		*link_tail;
} PcsFileInfoList;

/*�����ļ�Ԫ�����б�ĵ�����*/
typedef struct PcsFileInfoListIterater {
	PcsFileInfoList		*list;
	PcsFileInfoListItem	*cursor;
	PcsFileInfo			*current;
} PcsFileInfoListIterater;

PCS_API PcsFileInfo *pcs_fileinfo_create();
PCS_API void pcs_fileinfo_destroy(PcsFileInfo *fi);
PCS_API PcsFileInfo *pcs_fileinfo_clone(PcsFileInfo *fi);

PCS_API PcsFileInfoListItem *pcs_filistitem_create();
PCS_API void pcs_filistitem_destroy(PcsFileInfoListItem *item);

PCS_API PcsFileInfoList *pcs_filist_create();
PCS_API void pcs_filist_destroy(PcsFileInfoList *list);
PCS_API void pcs_filist_add(PcsFileInfoList *list, PcsFileInfoListItem *item);
PCS_API void pcs_filist_remove(PcsFileInfoList *list, PcsFileInfoListItem *item, PcsFileInfoListIterater *iterater);
PCS_API void pcs_filist_combin(PcsFileInfoList *list, PcsFileInfoList *src);

PCS_API void pcs_filist_iterater_init(PcsFileInfoList *list, PcsFileInfoListIterater *iterater);
PCS_API PcsBool pcs_filist_iterater_next(PcsFileInfoListIterater *iterater);


#endif
