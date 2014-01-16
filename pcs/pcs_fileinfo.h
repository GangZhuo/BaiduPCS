#ifndef _PCS_FILEINFO_H
#define _PCS_FILEINFO_H

#include "pcs_defs.h"

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
} PcsFileInfo;

typedef struct PcsFileInfoList {
	PcsFileInfo				info;
	struct PcsFileInfoList	*next;
} PcsFileInfoList;


PCS_API PcsFileInfo *pcs_fileinfo_create();
PCS_API void pcs_fileinfo_destroy(PcsFileInfo *fi);
PCS_API PcsFileInfo *pcs_fileinfo_clone(PcsFileInfo *fi);

PCS_API PcsFileInfoList *pcs_filist_create();
PCS_API void pcs_filist_destroy(PcsFileInfoList *list);


#endif
