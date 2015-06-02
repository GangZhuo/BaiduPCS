#ifndef _PCS_DEFS_H
#define _PCS_DEFS_H

#include <curl/curl.h>
#include <stdint.h>

#if defined(_MSC_VER) || defined(__BORLANDC__)
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
#  ifndef PRId64
#    define PRId64 "lld"
#  endif
#  ifndef PRIu64
#    define PRIu64 "llu"
#  endif
#else
#  ifndef PRId64
#    define PRId64 "lld"
#  endif
#  ifndef PRIu64
#    define PRIu64 "llu"
#  endif
#endif

#ifdef WIN32
#  ifdef __cplusplus
#    if defined(PCS_STATIC_LIB)
#      define PCS_API extern "C" _declspec(dllexport)
#    elif defined(PCS_IMPORT_LIB)
#      define PCS_API extern "C" __declspec(dllimport)
#    else
#      define PCS_API extern "C" 
#    endif
#    define PCS_EXTERN_C extern "C"
#    define PCS_EXTERN_C_BEGIN extern "C" {
#    define PCS_EXTERN_C_END }
#  else /* ifdef __cplusplus */
#    if defined(PCS_STATIC_LIB)
#      define PCS_API _declspec(dllexport)
#    elif defined(PCS_IMPORT_LIB)
#      define PCS_API __declspec(dllimport)
#    else
#      define PCS_API 
#    endif
#    define PCS_EXTERN_C
#    define PCS_EXTERN_C_BEGIN
#    define PCS_EXTERN_C_END
#    define inline __inline
#  endif /* ifdef __cplusplus */
#else /* ifdef WIN32 */
#  ifdef __cplusplus
#    if defined(PCS_STATIC_LIB)
#      define PCS_API extern "C"
#    elif defined(PCS_IMPORT_LIB)
#      define PCS_API extern "C"
#    else
#      define PCS_API extern "C"
#    endif
#    define PCS_EXTERN_C extern "C"
#    define PCS_EXTERN_C_BEGIN extern "C" {
#    define PCS_EXTERN_C_END }
#  else /* ifdef __cplusplus */
#    if defined(PCS_STATIC_LIB)
#      define PCS_API
#    elif defined(PCS_IMPORT_LIB)
#      define PCS_API
#    else
#      define PCS_API
#    endif
#    define PCS_EXTERN_C
#    define PCS_EXTERN_C_BEGIN
#    define PCS_EXTERN_C_END
#  endif /* ifdef __cplusplus */
#endif /* ifdef WIN32 */

#if defined(WIN32)
#	if !defined(__cplusplus)
#		define inline __inline
#	endif
#endif

#ifndef NULL
#  define NULL ((void *)0)
#endif

#define ToString(x) #x  
#define Xstr(x) ToString(x)

typedef unsigned char PcsBool;
#    define PcsTrue  1
#    define PcsFalse 0


#endif
