#ifndef _PCS_MEM_H
#define _PCS_MEM_H

/* 自定义malloc,free方法。用于检查内存泄漏。 */
#ifdef WIN32
# include <malloc.h>
#else
# include <alloca.h>
#endif
#include "pcs_defs.h"

typedef void(*pcs_mem_leak_print_fun)(void *ptr, const char *filename, int line);

/* 原始的 malloc() 函数 */
PCS_API void *pcs_mem_malloc_raw(size_t size);
/* 原始的 free() 函数 */
PCS_API void pcs_mem_free_raw(void *ptr);

/* 带有泄漏检测的 malloc() 函数 */
PCS_API void *pcs_mem_malloc(size_t size, const char *filename, int line);
/* 带有泄漏检测的 malloc() 函数 */
PCS_API void *pcs_mem_malloc_arg1(size_t sz);
/* 带有泄漏检测的 free() 函数 */
PCS_API void pcs_mem_free(void *p);
/* 打印泄漏的内存 */
PCS_API void pcs_mem_print_leak();

PCS_API void pcs_mem_set_print_func(pcs_mem_leak_print_fun print);


#if defined(DEBUG) || defined(_DEBUG)

#  define pcs_print_leak()			pcs_mem_print_leak()
#  define pcs_malloc(size)			pcs_mem_malloc(size, __FILE__, __LINE__)
#  define pcs_free(ptr)				pcs_mem_free(ptr)

#else
#  include <stdlib.h>
#  define pcs_print_leak()			while(0)
#  define pcs_malloc(size)			malloc(size)
#  define pcs_free(ptr)				free(ptr)
#endif

#endif
