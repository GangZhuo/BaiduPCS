#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pcs/pcs_mem.h"
#include "hashtable.h"

static unsigned int calcHash1(const char *key, int key_size, int ignore_case)
{
	register unsigned int nr = 1, nr2 = 4, ch;
	int i;
	if (key_size == -1) key_size = strlen(key);
	for (i = 0; i < key_size; i++) {
		ch = (unsigned int)key[i];
		if (ch >= 'A' && ch <= 'Z' && ignore_case && (i == 0 || !(key[i - 1] & 0x80)))
			ch += 'a' - 'A';
		nr ^= (((nr & 63) + nr2) * ch) + (nr << 8);
		nr2 += 3;
	}
	return nr;
}

static unsigned int calcHash2(const char *key, int key_size, int ignore_case)
{
	unsigned int hash = 0, ch;
	int i;
	if (key_size == -1) key_size = strlen(key);
	for (i = 0; i < key_size; i++) {
		ch = (unsigned int)key[i];
		if (ch >= 'A' && ch <= 'Z' && ignore_case && (i == 0 || !(key[i - 1] & 0x80)))
			ch += 'a' - 'A';
		hash *= 16777619;
		hash ^= ch;
	}
	return (hash);
}

static unsigned int calcHash3(const char *key, int key_size, int ignore_case)
{
	unsigned int h, ch;
	int i;
	if (key_size == -1) key_size = strlen(key);
	for (h = 0, i = 0; i < key_size; i++) {
		ch = (unsigned int)key[i];
		if (ch >= 'A' && ch <= 'Z' && ignore_case && (i == 0 || !(key[i - 1] & 0x80)))
			ch += 'a' - 'A';
		h = 31 * h + (ch);
	}
	return h;
}

static HashtableNode *node_create(const char *key, int key_size, void *value, unsigned int hashA, unsigned int hashB)
{
	HashtableNode *node;
	node = (HashtableNode *) pcs_malloc(sizeof(HashtableNode));
	if (!node)
		return NULL;
	memset (node, 0, sizeof(HashtableNode));
	if (key_size == -1) key_size = strlen(key);
	node->key = (char *)pcs_malloc(key_size + 1);
	if (!node->key) {
		pcs_free(node);
		return NULL;
	}
	memcpy(node->key, key, key_size);
	node->key[key_size] = '\0';
	node->value = value;
	node->hashA = hashA;
	node->hashB = hashB;
	return node;
}

static void node_destroy(HashtableNode *node, void(*free_value)(void *))
{
	HashtableNode *tmp, *cusor;
	cusor = node;
	while(cusor) {
		tmp = cusor;
		cusor = cusor->next;
		if (tmp->key)
			pcs_free(tmp->key);
		if (free_value && tmp->value)
			(*free_value)(tmp->value);
		pcs_free(tmp);
	}
}

static int table_add_item(HashtableNode **table, int real_capacity, const char *key, int key_size, void *value, int ignore_case)
{
	//不同的字符串三次hash还会碰撞的几率无限接近于不可能
	unsigned int nHash, nHashA, nHashB;
	unsigned int pos;
	HashtableNode *last = NULL, *p;
	if (key_size == -1) key_size = strlen(key);
	nHash = calcHash1(key, key_size, ignore_case);
	nHashA = calcHash2(key, key_size, ignore_case);
	nHashB = calcHash3(key, key_size, ignore_case);
	pos = nHash % real_capacity;
	p = table[pos];
	if (!p) {
		table[pos] = node_create(key, key_size, value, nHashA, nHashB);
		if (!table[pos])
			return -1;
		return 0;
	}
	while (p) {
		if (p->hashA == nHashA && p->hashB == nHashB) {
			break;
		}
		last = p;
		p = p->next;
	}
	if (p) {
		return -1;
	}
	else {
		p = node_create(key, key_size, value, nHashA, nHashB);
		if (!p)
			return -1;
		last->next = p;
	}
	return 0;
}

static int table_set_item(HashtableNode **table, int real_capacity, const char *key, int key_size, void *value, int ignore_case, int *pHasOld, void **pOldVal)
{
	//不同的字符串三次hash还会碰撞的几率无限接近于不可能
	unsigned int nHash, nHashA, nHashB;
	unsigned int pos;
	HashtableNode *last = NULL, *p;
	if (key_size == -1) key_size = strlen(key);
	nHash = calcHash1(key, key_size, ignore_case);
	nHashA = calcHash2(key, key_size, ignore_case);
	nHashB = calcHash3(key, key_size, ignore_case);
	pos = nHash % real_capacity;
	p = table[pos];
	if (pHasOld) (*pHasOld) = 0;
	if (!p) {
		table[pos] = node_create(key, key_size, value, nHashA, nHashB);
		if (!table[pos]) return -1;
		return 0;
	}
	while (p) {
		if (p->hashA == nHashA && p->hashB == nHashB) {
			break;
		}
		last = p;
		p = p->next;
	}
	if (p) {
		if (pHasOld) (*pHasOld) = 1;
		if (pOldVal) (*pOldVal) = p->value;
		//p->key = key;
		p->value = value;
		return 0;
	}
	else {
		p = node_create(key, key_size, value, nHashA, nHashB);
		if (!p) return -1;
		last->next = p;
	}
	return 0;
}

static HashtableNode *table_get_item(HashtableNode **table, int real_capacity, const char *key, int key_size, int ignore_case)
{
	unsigned int nHash, nHashA, nHashB;
	unsigned int pos;
	HashtableNode *p;
	if (key_size == -1) key_size = strlen(key);
	nHash = calcHash1(key, key_size, ignore_case);
	pos = nHash % real_capacity;
	p = table[pos];
	if (!p) return NULL;
	nHashA = calcHash2(key, key_size, ignore_case);
	nHashB = calcHash3(key, key_size, ignore_case);
	while (p) {
		if (p->hashA == nHashA && p->hashB == nHashB) {
			break;
		}
		p = p->next;
	}
	return p;
}

static HashtableNode *table_remove_item(HashtableNode **table, int real_capacity, const char *key, int key_size, int ignore_case)
{
	unsigned int nHash, nHashA, nHashB;
	unsigned int pos;
	HashtableNode *p, *prev = NULL;
	if (key_size == -1) key_size = strlen(key);
	nHash = calcHash1(key, key_size, ignore_case);
	pos = nHash % real_capacity;
	p = table[pos];
	if (p) {
		nHashA = calcHash2(key, key_size, ignore_case);
		nHashB = calcHash3(key, key_size, ignore_case);
		while (p) {
			if (p->hashA == nHashA && p->hashB == nHashB) {
				break;
			}
			prev = p;
			p = p->next;
		}
		if (p) {
			if (prev) {
				prev->next = p->next;
			}
			else {
				table[pos] = p->next;
			}
			p->next = NULL;
		}
	}
	return p;
}

static void table_clear(HashtableNode **table, int real_capacity, void(*free_value)(void *))
{
	int i;
	HashtableNode *node;
	for(i = 0; i < real_capacity; i++) {
		node = table[i];
		node_destroy(node, free_value);
		table[i] = NULL;
	}
}

static void table_destroy(HashtableNode **table, int real_capacity, void(*free_value)(void *))
{
	table_clear(table, real_capacity, free_value);
	pcs_free(table);
}

Hashtable *ht_create(int capacity, int ignore_case, void (*free_value)(void *))
{
	Hashtable *ht;
	ht = (Hashtable *) pcs_malloc(sizeof(Hashtable));
	if (!ht)
		return NULL;
	memset (ht, 0, sizeof(Hashtable));
	if (capacity < 17) capacity = 17;
	ht->capacity = capacity;
	ht->real_capacity = (int)(ht->capacity * HASH_EXTEND_MULTIPLIER);
	ht->count = 0;
	ht->free_value = free_value;
	ht->table = (HashtableNode **)pcs_malloc(ht->real_capacity * sizeof(HashtableNode *));
	if (!ht->table) {
		pcs_free(ht);
		return NULL;
	}
	memset (ht->table, 0, ht->real_capacity * sizeof(HashtableNode *));
	ht->ignore_case = ignore_case;
	return ht;
}

void ht_destroy(Hashtable *ht)
{
	table_destroy(ht->table, ht->real_capacity, ht->free_value);
	pcs_free(ht);
}

int ht_expand(Hashtable *ht, int capacity)
{
	HashtableNode **table, *node;
	int real_capacity, i, cnt;
	real_capacity = (int)(capacity * HASH_EXTEND_MULTIPLIER);
	table = (HashtableNode **)pcs_malloc(real_capacity * sizeof(HashtableNode *));
	if (!table) {
		return -1;
	}
	memset (table, 0, real_capacity * sizeof(HashtableNode *));
	cnt = 0;
	for(i = 0; i < ht->real_capacity; i++) {
		node = ht->table[i];
		while(node) {
			if (table_add_item(table, real_capacity, node->key, -1, node->value, ht->ignore_case)) {
				table_destroy(table, real_capacity, NULL);
				return -1;
			}
			cnt++;
			node = node->next;
		}
	}
	table_destroy(ht->table, ht->real_capacity, NULL);
	ht->table = table;
	ht->capacity = capacity;
	ht->real_capacity = real_capacity;
	ht->count = cnt;
	return 0;
}

int ht_add(Hashtable *ht, const char *key, int key_size, void *value)
{
	if (ht->count >= ht->capacity) {
		if (ht_expand(ht, ht->count * 2))
			return -1;
	}
	if (table_add_item(ht->table, ht->real_capacity, key, key_size, value, ht->ignore_case))
		return -1;
	ht->count++;
	return 0;
}

int ht_set(Hashtable *ht, const char *key, int key_size, void *value, void **pOldVal)
{
	void *oldVal = NULL;
	int rc, pHasOld;
	rc = table_set_item(ht->table, ht->real_capacity, key, key_size, value, ht->ignore_case, &pHasOld, pOldVal ? pOldVal : &oldVal);
	if (oldVal && ht->free_value) (*ht->free_value)(oldVal); /*如果pOldVal传入NULL，且有旧值时，自动释放*/
	if (!pHasOld) ht->count++;
	return rc;
}

int ht_remove(Hashtable *ht, const char *key, int key_size, void **pVal)
{
	HashtableNode *p;
	p = table_remove_item(ht->table, ht->real_capacity, key, key_size, ht->ignore_case);
	if (p) {
		if (pVal) *pVal = p->value;
		node_destroy(p, pVal ? NULL : ht->free_value);
		ht->count--;
		return 0;
	}
	return -1;
}

void *ht_get(Hashtable *ht, const char *key, int key_size)
{
	HashtableNode *p;
	p = table_get_item(ht->table, ht->real_capacity, key, key_size, ht->ignore_case);
	if (!p)
		return NULL;
	return p->value;
}

HashtableNode *ht_get_node(Hashtable *ht, const char *key, int key_size)
{
	return table_get_item(ht->table, ht->real_capacity, key, key_size, ht->ignore_case);
}

int ht_has(Hashtable *ht, const char *key, int key_size)
{
	HashtableNode *p;
	p = table_get_item(ht->table, ht->real_capacity, key, key_size, ht->ignore_case);
	return (p ? 1 : 0);
}

int ht_clear(Hashtable *ht)
{
	table_clear(ht->table, ht->real_capacity, ht->free_value);
	ht->count = 0;
	return 0;
}

HashtableIterater *ht_it_create(Hashtable *ht)
{
	HashtableIterater *iterater;
	iterater = (HashtableIterater *) pcs_malloc(sizeof(HashtableIterater));
	if (!iterater)
		return NULL;
	memset(iterater, 0, sizeof(HashtableIterater));
	iterater->ht = ht;
	iterater->index = -1;
	return iterater;
}

void ht_it_destroy(HashtableIterater *iterater)
{
	pcs_free(iterater);
}

void ht_it_reset(HashtableIterater *iterater)
{
	iterater->index = -1;
}

int ht_it_next(HashtableIterater *iterater)
{
	if (iterater->ht->count == 0)
		return 0;
	if (iterater->p) {
		iterater->p = iterater->p->next;
		if (iterater->p)
			return 1;
	}
	if (iterater->index >= iterater->ht->real_capacity)
		return 0;
	iterater->index++;

	while(iterater->index < iterater->ht->real_capacity) {
		iterater->p = iterater->ht->table[iterater->index];
		if (iterater->p)
			break;
		iterater->index++;
	}
	if (iterater->p)
		return 1;
	return 0;
}

void *ht_it_current(HashtableIterater *iterater)
{
	if (iterater->p)
		return iterater->p->value;
	return NULL;
}

