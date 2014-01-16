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
	char		**block_list; /*  文件所有分片的md5数组json字符串, 只有 pcs_meta() 设置该值 */
	PcsBool		ifhassubdir; /* 是否含有子目录, 只有 pcs_meta() 设置该值  */
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
