#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pcs_mem.h"
#include "pcs_utils.h"
#include "pcs_fileinfo.h"

PCS_API PcsFileInfo *pcs_fileinfo_create()
{
	PcsFileInfo *res = 0;
	res = (PcsFileInfo *)pcs_malloc(sizeof(PcsFileInfo));
	if (res)
		memset(res, 0, sizeof(PcsFileInfo));
	return res;
}

PCS_API void pcs_fileinfo_destroy(PcsFileInfo *fi)
{
	if (fi->path) pcs_free(fi->path);
	if (fi->server_filename) pcs_free(fi->server_filename);
	if (fi->md5) pcs_free(fi->md5);
	if (fi->dlink) pcs_free(fi->dlink);
	if (fi->block_list) {
		char **p = fi->block_list;
		while(*p) {
			pcs_free(*p);
			p++;
		}
		pcs_free(fi->block_list);
	}
	pcs_free(fi);
}


PCS_API PcsFileInfo *pcs_fileinfo_clone(PcsFileInfo *fi)
{
	PcsFileInfo *res = pcs_fileinfo_create();
	if (!res) return 0;
	res->fs_id = fi->fs_id;
	res->path = pcs_utils_strdup(fi->path);
	res->server_filename = pcs_utils_strdup(fi->server_filename);
	res->server_ctime = fi->server_ctime;
	res->server_mtime = fi->server_mtime;
	res->local_ctime = fi->local_ctime;
	res->local_mtime = fi->local_mtime;
	res->size = fi->size;
	res->category = fi->category;
	res->isdir = fi->isdir;
	res->dir_empty = fi->dir_empty;
	res->empty = fi->empty;
	res->md5 = pcs_utils_strdup(fi->md5);
	res->dlink = pcs_utils_strdup(fi->dlink);
	if (fi->block_list) {
		char **p = fi->block_list;
		int i = 0;
		while(*p) {
			i++;
			p++;
		}
		res->block_list = (char **)pcs_malloc((i + 1) * sizeof(char *));
		if (res->block_list) {
			i = 0;
			p = fi->block_list;
			while(*p) {
				res->block_list[i++] = pcs_utils_strdup(*p);
				p++;
			}
		}
	}
	res->ifhassubdir = fi->ifhassubdir;
	return res;
}


PCS_API PcsFileInfoList *pcs_filist_create()
{
	PcsFileInfoList *res = 0;
	res = (PcsFileInfoList *)pcs_malloc(sizeof(PcsFileInfoList));
	if (res)
		memset(res, 0, sizeof(PcsFileInfoList));
	return res;
}

PCS_API void pcs_filist_destroy(PcsFileInfoList *list)
{
	PcsFileInfoList *p = list, *p2;
	while(p) {
		p2 = p;
		p = p->next;
		if (p2->info.path) pcs_free(p2->info.path);
		if (p2->info.server_filename) pcs_free(p2->info.server_filename);
		if (p2->info.md5) pcs_free(p2->info.md5);
		if (p2->info.dlink) pcs_free(p2->info.dlink);
		if (p2->info.block_list) {
			char **p = p2->info.block_list;
			while(*p) {
				pcs_free(*p);
				p++;
			}
			pcs_free(p2->info.block_list);
		}
		pcs_free(p2);
	}
}
