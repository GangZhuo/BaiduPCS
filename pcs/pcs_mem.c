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

static struct pcs_mem *_mem = 0;
void (*_pcs_mem_printf)(const char *format, ...) = (void (*)(const char *format, ...))printf;

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

PCS_API void pcs_mem_free(void *ptr)
{
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
		_pcs_mem_printf("Memory leak on %p %s, %d\n", ent->ptr, ent->filename, ent->line);
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


