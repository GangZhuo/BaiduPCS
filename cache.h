#ifndef _PCS_SHELL_CAHCE_H
#define _PCS_SHELL_CAHCE_H

#include "pcs/pcs.h"

#ifndef MAX_CACHE_SIZE
#define MAX_CACHE_SIZE	(1 * 1024) /* 1MiB */
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct block {
	curl_off_t	start;
	size_t		size;
	char		*data;
	struct block *prev;
	struct block *next;
} block_t;

typedef struct cache {
	block_t	blocks;
	FILE	*fp;
	size_t	total_size;
} cathe_t;

int cache_init(cathe_t *cache);

void cache_uninit(cathe_t *cache);

int cache_add(cathe_t *cache, curl_off_t start, char *data, size_t size);

int cache_flush(cathe_t *cache);

int cache_reset(cathe_t *cache);

#ifdef __cplusplus
}
#endif

#endif
