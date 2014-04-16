#ifndef _PCS_MEM_H
#define _PCS_MEM_H

/* �Զ���malloc,free���������ڼ���ڴ�й©�� */
#ifdef WIN32
# include <malloc.h>
#else
# include <alloca.h>
#endif
#include "pcs_defs.h"

#if defined(DEBUG) || defined(_DEBUG)

   PCS_API void *pcs_mem_malloc(size_t size, const char *filename, int line);
   PCS_API void pcs_mem_free(void *p);
   /*��ӡй©���ڴ�*/
   PCS_API void pcs_mem_print_leak();

#  define pcs_malloc(size)			pcs_mem_malloc(size, __FILE__, __LINE__)
#  define pcs_free(ptr)				pcs_mem_free(ptr)
#else
#  include <stdlib.h>
#  define pcs_mem_print_leak()		while(0)
#  define pcs_malloc(size)			malloc(size)
#  define pcs_free(ptr)				free(ptr)
#endif

#endif
