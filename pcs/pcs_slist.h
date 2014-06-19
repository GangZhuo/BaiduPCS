#ifndef _PCS_SLIST_H
#define _PCS_SLIST_H

/* 定义了字符串列表。列表以链表形式存储。 */
typedef struct PcsSList {
	char			*string;
	struct PcsSList	*next;
} PcsSList;

typedef struct PcsSList2 {
	char				*string1;
	char				*string2;
	struct PcsSList2	*next;
} PcsSList2;

PCS_API PcsSList *pcs_slist_create();
PCS_API PcsSList *pcs_slist_create_ex(const char *src, int len);
PCS_API void pcs_slist_destroy(PcsSList *slist);
PCS_API PcsSList *pcs_slist_add_ex(PcsSList *slist, const char *src, int len);

PCS_API PcsSList2 *pcs_slist2_create();
PCS_API PcsSList2 *pcs_slist2_create_ex(const char *src1, int len1, const char *src2, int len2);
PCS_API void pcs_slist2_destroy(PcsSList2 *slist);
PCS_API PcsSList2 *pcs_slist2_add_ex(PcsSList2 *slist, const char *src, int len, const char *src2, int len2);


#endif
