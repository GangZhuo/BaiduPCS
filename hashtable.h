#ifndef _PCS_SHELL_HASHTABLE_H_
#define _PCS_SHELL_HASHTABLE_H_

#ifndef HASH_EXTEND_MULTIPLIER
#define HASH_EXTEND_MULTIPLIER ( 1.75F )
#endif

#ifdef __cplusplus
extern "C" {
#endif


typedef struct  HashtableNode HashtableNode;
struct  HashtableNode { 
	char			*key;
	unsigned int	hashA;
	unsigned int	hashB;
	void			*value;
	HashtableNode	*next;
};

typedef struct Hashtable Hashtable;
struct Hashtable {
	int				capacity;
	int				real_capacity;
	int				count;
	HashtableNode	**table;
	void (*free_value)(void *);
	int				ignore_case;
};

typedef struct HashtableIterater {
	Hashtable		*ht;
	int				index;
	HashtableNode	*p;
} HashtableIterater;

/*
 * 创建一个哈希表
 * capacity 的最大容量，即内部节点表格的元素数量
 * ignore_case  - Key是否忽略大小写。0-表示不忽略；非0值表示忽略大小写
 * free_value   - 一个函数，清空哈希表时，将使用每一个节点的value值来调用该函数
*/
Hashtable *ht_create(int capacity, int ignore_case, void (*free_value)(void *));
/*释放掉哈希表*/
void ht_destroy(Hashtable *ht);
/*扩容哈希表。成功返回0，失败返回非0值。*/
int ht_expand(Hashtable *ht, int capacity);
/*
添加一项到哈希表中。
key_size 传入-1时，自动使用strlen()计算
如果失败或已经存在，则返回非0值，否则返回0
*/
int ht_add(Hashtable *ht, const char *key, int key_size, void *value);
/*设置一个项的值，
 * 如果key已经存在，则修改其值为value，并设定(*pOldVal)为旧值，
 * 如果key不存在，则添加一个新项，不设置(*pOldVal)的值。
 * 如果不需要接收旧值，设置 pOldVal 的值为 NULL。
 * 如果失败，则返回非0值，否则返回0
*/
int ht_set(Hashtable *ht, const char *key, int key_size, void *value, void **pOldVal);
/*
 * 从哈希表中移除一项。如果失败或不存在，则返回非0值，否则返回0。
 * 如果 pVal 值不为NULL，函数执行成功时，将把被移除项的value值写入pVal指定的地址中。
 * 如果不需要接收被移除项的值，设置 pVal 的值为 NULL。
*/
int ht_remove(Hashtable *ht, const char *key, int key_size, void **pVal);
/*返回哈希表中是否存在该项。不存在返回0，存在返回1。*/
int ht_has(Hashtable *ht, const char *key, int key_size);
/*返回哈希表中key指定项的值。如果不存在则返回NULL，存在则返回实际值*/
void *ht_get(Hashtable *ht, const char *key, int key_size);
/*返回哈希表中key指定的节点项。如果不存在则返回NULL，存在则返回该节点*/
HashtableNode *ht_get_node(Hashtable *ht, const char *key, int key_size);
/*清空哈希表。注意函数不会自动释放每项的值，清空前需迭代每一项来释放其值。*/
int ht_clear(Hashtable *ht);

/*创建一个哈希表的迭代器*/
HashtableIterater *ht_it_create(Hashtable *ht);
/*释放迭代器*/
void ht_it_destroy(HashtableIterater *iterater);
/*重置迭代器*/
void ht_it_reset(HashtableIterater *iterater);
/*迭代器移动到下一条。移动成功则返回1，否则返回0*/
int ht_it_next(HashtableIterater *iterater);
/*返回迭代器当前项的值*/
void *ht_it_current(HashtableIterater *iterater);

#ifdef __cplusplus
}
#endif

#endif
