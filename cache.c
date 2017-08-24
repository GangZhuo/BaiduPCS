#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <inttypes.h>
#include <string.h>
#include <assert.h>
#ifdef WIN32
# include <WinSock2.h>
# include <Windows.h>
#else
# include <unistd.h>
#endif

#include "cache.h"
#include "pcs/pcs_mem.h"

#ifdef WIN32
#ifndef __MINGW32__
# define lseek _lseek
# define fileno _fileno
# define fseeko _fseeki64
# define ftello _ftelli64
#endif
#endif

int cache_init(cathe_t *cache)
{
	memset(cache, 0, sizeof(cathe_t));
	cache->blocks.prev = &cache->blocks;
	cache->blocks.next = &cache->blocks;
	return 0;
}

void cache_uninit(cathe_t *cache)
{
	if (cache) {
		block_t *block = cache->blocks.next, *tmp;
		while (block != &cache->blocks) {
			tmp = block;
			block = block->next;
			pcs_free(tmp->data);
			pcs_free(tmp);
		}
	}
}

int cache_reset(cathe_t *cache)
{
	cache_uninit(cache);
	cache->blocks.prev = &cache->blocks;
	cache->blocks.next = &cache->blocks;
	cache->total_size = 0;
	return 0;
}

block_t *cache_newblock(curl_off_t start, char *data, size_t size)
{
	block_t *block;
	block = pcs_malloc(sizeof(block_t));
	if (!block)
		return NULL;
	memset(block, 0, sizeof(block_t));
	block->start = start;
	block->size = size;
	block->data = pcs_malloc(size);
	if (!block->data) {
		free(block);
		return NULL;
	}
	memcpy(block->data, data, size);
	return block;
}

int cache_addblock(cathe_t *cache, block_t *block)
{
	block_t *pos = cache->blocks.next;
	cache->total_size += block->size;
	/* find position */
	while (pos != &cache->blocks) {
		if (pos->start > block->start)
			break;
		pos = pos->next;
	}
	block->next = pos;
	block->prev = pos->prev;
	pos->prev->next = block;
	pos->prev = block;
	return 0;
}

int cache_add(cathe_t *cache, curl_off_t start, char *data, size_t size)
{
	block_t *block, *pos = cache->blocks.next;
	block = cache_newblock(start, data, size);
	if (!block)
		return -1;
	return cache_addblock(cache, block);
}

int cache_flush(cathe_t *cache)
{
	block_t *block = cache->blocks.next;
	int r;
	while (block != &cache->blocks) {
		r = fseeko((cache->fp), block->start, SEEK_SET);
		if (r)
			return -1;
		r = fwrite(block->data, 1, block->size, cache->fp);
		if (r != block->size)
			return -1;
		block = block->next;
	}
	return 0;
}

