#include <string.h>
#ifdef WIN32
# include <malloc.h>
#else
# include <alloca.h>
#endif

#include "pcs_mem.h"
#include "pcs_slist.h"

PCS_API PcsSList *pcs_slist_create()
{
	PcsSList *res = 0;
	res = (PcsSList *)pcs_malloc(sizeof(PcsSList));
	if (res)
		memset(res, 0, sizeof(PcsSList));
	return res;
}

PCS_API PcsSList *pcs_slist_create_ex(const char *src, int len)
{
	PcsSList *res = pcs_slist_create();
	if (res) {
		if (len == -1) len = strlen(src);
		res->string = (char *)pcs_malloc(len + 1);
		if (res->string) {
			memcpy(res->string, src, len);
			res->string[len] = '\0';
		}
		else {
			pcs_free(res);
			res = 0;
		}
	}
	return res;
}

PCS_API void pcs_slist_destroy(PcsSList *slist)
{
	PcsSList *p = slist, *p2;
	while(p) {
		p2 = p;
		p = p->next;
		if (p2->string)
			pcs_free(p2->string);
		pcs_free(p2);
	}
}

PCS_API PcsSList *pcs_slist_add_ex(PcsSList *slist, const char *src, int len)
{
	PcsSList *p = pcs_slist_create_ex(src, len);
	if (p) {
		while(slist->next) {
			slist = slist->next;
		}
		slist->next = p;
		p->next = 0;
	}
	return p;
}

PCS_API PcsSList2 *pcs_slist2_create()
{
	PcsSList2 *res = 0;
	res = (PcsSList2 *)pcs_malloc(sizeof(PcsSList2));
	if (res)
		memset(res, 0, sizeof(PcsSList2));
	return res;
}

PCS_API PcsSList2 *pcs_slist2_create_ex(const char *src1, int len1, const char *src2, int len2)
{
	PcsSList2 *res = pcs_slist2_create();
	if (res) {
		res->string1 = (char *)pcs_malloc(len1 + 1);
		if (res->string1) {
			memcpy(res->string1, src1, len1);
			res->string1[len1] = '\0';
		}
		else {
			pcs_free(res);
			res = 0;
			return res;
		}
		res->string2 = (char *)pcs_malloc(len2 + 1);
		if (res->string2) {
			memcpy(res->string2, src2, len2);
			res->string2[len2] = '\0';
		}
		else {
			pcs_free(res->string1);
			pcs_free(res);
			res = 0;
			return res;
		}
	}
	return res;
}

PCS_API void pcs_slist2_destroy(PcsSList2 *slist)
{
	PcsSList2 *p = slist, *p2;
	while(p) {
		p2 = p;
		p = p->next;
		if (p2->string1)
			pcs_free(p2->string1);
		if (p2->string2)
			pcs_free(p2->string2);
		pcs_free(p2);
	}
}

PCS_API PcsSList2 *pcs_slist2_add_ex(PcsSList2 *slist, const char *src1, int len1, const char *src2, int len2)
{
	PcsSList2 *p = pcs_slist2_create_ex(src1, len1, src2, len2);
	if (p) {
		while(slist->next) {
			slist = slist->next;
		}
		slist->next = p;
		p->next = 0;
	}
	return p;
}

