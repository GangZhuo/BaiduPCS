#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pcs_mem.h"
#include "hashtable.h"

unsigned int calcHash1(const char *key, int ignore_case)
{
	register unsigned int nr = 1, nr2 = 4, ch;
	const char *p = key;
	while (*p) {
		ch = (unsigned int)*p++;
		if (ch >= 'A' && ch <= 'Z' && ignore_case && (p == key || !(p[-1] & 0x80)))
			ch += 'a' - 'A';
		nr ^= (((nr & 63) + nr2) * ch) + (nr << 8);
		nr2 += 3;
	}
	return nr;
}

unsigned int calcHash2(const char *key)
{
	unsigned int hash = 0;
	while (*key) {
		hash *= 16777619;
		hash ^= (unsigned int) *key++;
	}
	return (hash);
}

unsigned int calcHash3(const char *key)
{
	register unsigned int h;
	register const char *p;

	for(h = 0, p = key; *p ; p++)
		h = 31 * h + ((unsigned int)*p);
	return h;
}

hashtable_node *node_create(const char *key, void *value, unsigned int hashA, unsigned int hashB)
{
	hashtable_node *node;
	node = (hashtable_node *) pcs_malloc(sizeof(hashtable_node));
	if (!node)
		return NULL;
	memset (node, 0, sizeof(hashtable_node));
	node->key = (char *) pcs_malloc(strlen(key) + 1);
	if (!node->key) {
		pcs_free(node);
		return NULL;
	}
	strcpy(node->key, key);
	node->value = value;
	node->hashA = hashA;
	node->hashB = hashB;
	return node;
}

void node_destroy(hashtable_node *node, void (*free_value)(void *))
{
	hashtable_node *tmp, *cusor;
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

int table_add_item(hashtable_node **table, int real_capacity, const char *key, void *value, int ignore_case)
{
	//不同的字符串三次hash还会碰撞的几率无限接近于不可能
	unsigned int nHash = calcHash1(key, ignore_case),
		nHashA = calcHash2(key),
		nHashB = calcHash3(key);
	unsigned int pos = nHash % real_capacity;
	hashtable_node *last = NULL, 
		*p = table[pos];
	if (!p) {
		table[pos] = node_create(key, value, nHashA, nHashB);
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
		p = node_create(key, value, nHashA, nHashB);
		if (!p)
			return -1;
		last->next = p;
	}
	return 0;
}

hashtable_node *table_get_item(hashtable_node **table, int real_capacity, const char *key, int ignore_case)
{
	unsigned int nHash = calcHash1(key, ignore_case),
		nHashA, nHashB;
	unsigned int pos = nHash % real_capacity;
	hashtable_node *p = table[pos];
	if (!p)
		return NULL;
	nHashA = calcHash2(key);
	nHashB = calcHash3(key);
	while (p) {
		if (p->hashA == nHashA && p->hashB == nHashB) {
			break;
		}
		p = p->next;
	}
	return p;
}

void table_clear(hashtable_node **table, int real_capacity, void (*free_value)(void *))
{
	int i;
	hashtable_node *node;
	for(i = 0; i < real_capacity; i++) {
		node = table[i];
		node_destroy(node, free_value);
		table[i] = NULL;
	}
}

void table_destroy(hashtable_node **table, int real_capacity, void (*free_value)(void *))
{
	table_clear(table, real_capacity, free_value);
	pcs_free(table);
}

hashtable *hashtable_create(int capacity, int ignore_case, void (*free_value)(void *))
{
	hashtable *ht;
	ht = (hashtable *) pcs_malloc(sizeof(hashtable));
	if (!ht)
		return NULL;
	memset (ht, 0, sizeof(hashtable));
	if (capacity < 17) capacity = 17;
	ht->capacity = capacity;
	ht->real_capacity = (int)(ht->capacity * HASH_EXTEND_MULTIPLIER);
	ht->count = 0;
	ht->free_value = free_value;
	ht->table = (hashtable_node **)pcs_malloc(ht->real_capacity * sizeof(hashtable_node *));
	if (!ht->table) {
		pcs_free(ht);
		return NULL;
	}
	memset (ht->table, 0, ht->real_capacity * sizeof(hashtable_node *));
	ht->ignore_case = ignore_case;
	return ht;
}

void hashtable_destroy(hashtable *ht)
{
	table_destroy(ht->table, ht->real_capacity, ht->free_value);
	pcs_free(ht);
}

int hashtable_expand(hashtable *ht, int capacity)
{
	hashtable_node **table, *node;
	int real_capacity, i, cnt;
	real_capacity = (int)(capacity * HASH_EXTEND_MULTIPLIER);
	table = (hashtable_node **)pcs_malloc(real_capacity * sizeof(hashtable_node *));
	if (!table) {
		return -1;
	}
	memset (table, 0, real_capacity * sizeof(hashtable_node *));
	cnt = 0;
	for(i = 0; i < ht->real_capacity; i++) {
		node = ht->table[i];
		while(node) {
			if (table_add_item(table, real_capacity, node->key, node->value, ht->ignore_case)) {
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

int hashtable_add(hashtable *ht, const char *key, void *value)
{
	if (ht->count >= ht->capacity) {
		if (hashtable_expand(ht, ht->count * 2))
			return -1;
	}
	if (table_add_item(ht->table, ht->real_capacity, key, value, ht->ignore_case))
		return -1;
	ht->count++;
	return 0;
}

void *hashtable_get(hashtable *ht, const char *key)
{
	hashtable_node *p = table_get_item(ht->table, ht->real_capacity, key, ht->ignore_case);
	if (!p)
		return NULL;
	return p->value;
}

int hashtable_clear(hashtable *ht)
{
	table_clear(ht->table, ht->real_capacity, ht->free_value);
	ht->count = 0;
}

hashtable_iterater *hashtable_iterater_create(hashtable *ht)
{
	hashtable_iterater *iterater;
	iterater = (hashtable_iterater *) pcs_malloc(sizeof(hashtable_iterater));
	if (!iterater)
		return NULL;
	memset(iterater, 0, sizeof(hashtable_iterater));
	iterater->ht = ht;
	iterater->index = -1;
	return iterater;
}

void hashtable_iterater_destroy(hashtable_iterater *iterater)
{
	pcs_free(iterater);
}

void hashtable_iterater_reset(hashtable_iterater *iterater)
{
	iterater->index = -1;
}

int hashtable_iterater_next(hashtable_iterater *iterater)
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

void *hashtable_iterater_current(hashtable_iterater *iterater)
{
	if (iterater->p)
		return iterater->p->value;
	return NULL;
}

