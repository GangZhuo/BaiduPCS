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


PCS_API PcsFileInfoListItem *pcs_filistitem_create()
{
	PcsFileInfoListItem *res = 0;
	res = (PcsFileInfoListItem *)pcs_malloc(sizeof(PcsFileInfoListItem));
	if (res)
		memset(res, 0, sizeof(PcsFileInfoListItem));
	return res;
}

PCS_API void pcs_filistitem_destroy(PcsFileInfoListItem *item)
{
	if (item->info) pcs_fileinfo_destroy(item->info);
	pcs_free(item);
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
	PcsFileInfoListItem *p = list->link, *p2;
	while(p) {
		p2 = p;
		p = p->next;
		pcs_filistitem_destroy(p2);
	}
	pcs_free(list);
}

PCS_API void pcs_filist_add(PcsFileInfoList *list, PcsFileInfoListItem *item)
{
	if (!list->link_tail) {
		list->link = list->link_tail = item;
		item->prev = item->next = 0;
	}
	else {
		item->prev = list->link_tail;
		item->next = 0;
		list->link_tail->next = item;
		list->link_tail = item;
	}
	list->count++;
}

PCS_API void pcs_filist_remove(PcsFileInfoList *list, PcsFileInfoListItem *item, PcsFileInfoListIterater *iterater)
{
	if (item->prev)
		item->prev->next = item->next;
	if (item->next)
		item->next->prev = item->prev;
	if (list->link == item)
		list->link = item->next;
	if (list->link_tail == item)
		list->link_tail = item->prev;
	if (iterater && iterater->cursor == item) {
		if (iterater->invert)
			iterater->cursor = item->next;
		else
			iterater->cursor = item->prev;
	}
	item->prev = item->next = 0;
	list->count--;
}

PCS_API void pcs_filist_combin(PcsFileInfoList *list, PcsFileInfoList *src)
{
	if (!src->link)
		return;
	if (!list->link_tail) {
		list->link = src->link;
		list->link_tail = src->link_tail;
		list->count = src->count;
		src->count = 0;
		src->link = src->link_tail = 0;
	}
	else {
		list->link_tail->next = src->link;
		src->link->prev = list->link_tail;
		list->link_tail = src->link_tail;
		list->count += src->count;
		src->count = 0;
		src->link = src->link_tail = 0;
	}
}


PCS_API void pcs_filist_iterater_init(PcsFileInfoList *list, PcsFileInfoListIterater *iterater, PcsBool invert)
{
	memset(iterater, 0, sizeof(PcsFileInfoListIterater));
	iterater->invert = invert;
	iterater->list = list;
}

PCS_API PcsBool pcs_filist_iterater_next(PcsFileInfoListIterater *iterater)
{
	if (!iterater->list->link)
		return PcsFalse;
	if (iterater->invert) {
		if (!iterater->cursor) {
			iterater->cursor = iterater->list->link_tail;
			iterater->current = iterater->cursor->info;
			return PcsTrue;
		}
		else if (iterater->cursor->prev) {
			iterater->cursor = iterater->cursor->prev;
			iterater->current = iterater->cursor->info;
			return PcsTrue;
		}
	}
	else {
		if (!iterater->cursor) {
			iterater->cursor = iterater->list->link;
			iterater->current = iterater->cursor->info;
			return PcsTrue;
		}
		else if (iterater->cursor->next) {
			iterater->cursor = iterater->cursor->next;
			iterater->current = iterater->cursor->info;
			return PcsTrue;
		}
	}
	return PcsFalse;
}

