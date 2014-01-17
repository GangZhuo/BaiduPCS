#ifndef _HASHTABLE_H_
#define _HASHTABLE_H_

#ifndef HASH_EXTEND_MULTIPLIER
#define HASH_EXTEND_MULTIPLIER ( 1.75F )
#endif

#ifdef __cplusplus
extern "C" {
#endif


typedef struct  hashtable_node hashtable_node;
struct  hashtable_node { 
	char			*key;
	unsigned int	hashA;
	unsigned int	hashB;
	void			*value;
	hashtable_node	*next;
};

typedef struct hashtable hashtable;
struct hashtable {
	int				capacity;
	int				real_capacity;
	int				count;
	hashtable_node	**table;
	void (*free_value)(void *);
	int				ignore_case;
};

typedef struct hashtable_iterater {
	hashtable		*ht;
	int				index;
	hashtable_node	*p;
} hashtable_iterater;

hashtable *hashtable_create(int capacity, int ignore_case, void (*free_value)(void *));
void hashtable_destroy(hashtable *ht);
int hashtable_expand(hashtable *ht, int capacity);
int hashtable_add(hashtable *ht, const char *key, void *value);
void *hashtable_get(hashtable *ht, const char *key);
int hashtable_clear(hashtable *ht);

hashtable_iterater *hashtable_iterater_create(hashtable *ht);
void hashtable_iterater_destroy(hashtable_iterater *iterater);
void hashtable_iterater_reset(hashtable_iterater *iterater);
int hashtable_iterater_next(hashtable_iterater *iterater);
void *hashtable_iterater_current(hashtable_iterater *iterater);

#ifdef __cplusplus
}
#endif

#endif
