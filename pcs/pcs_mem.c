#include <stdio.h>
#include <stdlib.h>

#include "pcs_defs.h"

struct pcs_mem {
	struct pcs_mem	*prev;
	struct pcs_mem	*next;

	void				*ptr;
	const char			*filename;
	int					line;

};

typedef void(*pcs_mem_leak_print_fun)(void *ptr, const char *filename, int line);

static void cb_leak_printf(void *ptr, const char *filename, int line);

static struct pcs_mem *_mem = 0;
static pcs_mem_leak_print_fun _print = cb_leak_printf;

static void cb_leak_printf(void *ptr, const char *filename, int line)
{
	printf("Memory leak on %p %s, %d\n", ptr, filename, line);
}

static inline void append_mem_ent(struct pcs_mem *mem)
{
	if (!_mem) {
		_mem = mem;
		mem->next = mem;
		mem->prev = mem;
	}
	else {
		mem->next = _mem;
		mem->prev = _mem->prev;
		_mem->prev->next = mem;
		_mem->prev = mem;
	}
}

static inline void remove_mem_ent(struct pcs_mem *mem)
{
	if (mem->next == mem) {
		_mem = 0;
		return;
	}
	else {
		if (mem == _mem) _mem = _mem->next;
		mem->prev->next = mem->next;
		mem->next->prev = mem->prev;
	}
}

PCS_API void *pcs_mem_malloc(size_t size, const char *filename, int line)
{
	struct pcs_mem *ent;
	ent = (struct pcs_mem *) malloc(sizeof(struct pcs_mem) + size);
	ent->ptr = (void *)(((char *)ent) + sizeof(struct pcs_mem));
	ent->filename = filename;
	ent->line = line;
	append_mem_ent(ent);
	return ent->ptr;
}

PCS_API void *pcs_mem_malloc_arg1(size_t sz)
{
	return pcs_mem_malloc(sz, "unknow", 0);
}

PCS_API void pcs_mem_free(void *ptr)
{
    if (!ptr) return;
	struct pcs_mem *ent;
	ent = (struct pcs_mem *)(((char *)ptr) - sizeof(struct pcs_mem));
	remove_mem_ent(ent);
	free(ent);
}

PCS_API void pcs_mem_print_leak()
{
	struct pcs_mem *ent, *p;
	if (!_mem) return;
	ent = _mem;
	do {
		_print(ent->ptr, ent->filename, ent->line);
		ent = ent->next;
	} while(ent != _mem);
	ent = _mem;
	do {
		p = ent;
		ent = ent->next;
		free(p);
	} while(ent != _mem);
	_mem = 0;
}

PCS_API void *pcs_mem_malloc_raw(size_t size)
{
	return malloc(size);
}

PCS_API void pcs_mem_free_raw(void *ptr)
{
	free(ptr);
}

PCS_API void pcs_mem_set_print_func(pcs_mem_leak_print_fun print)
{
	_print = print;
}

