#ifndef _PCS_DEFS_H
#define _PCS_DEFS_H

#include <curl/curl.h>

#if defined(_MSC_VER) || defined(__BORLANDC__)
typedef __int64 Int64;
typedef unsigned __int64 UInt64;
#define UINT64_CONST(n) n
#else
typedef long long int Int64;
typedef unsigned long long int UInt64;
#define UINT64_CONST(n) n ## ULL
#endif

#ifdef WIN32
#  ifdef __cplusplus
#    ifdef PCS_STATIC_LIB
#      define PCS_API extern "C" _declspec(dllexport)
#    else
#      define PCS_API extern "C"
#    endif
#    define PCS_EXTERN_C extern "C"
#    define PCS_EXTERN_C_BEGIN extern "C" {
#    define PCS_EXTERN_C_END }
#  else /* ifdef __cplusplus */
#    ifdef PCS_STATIC_LIB
#      define PCS_API _declspec(dllexport)
#    else
#      define PCS_API 
#    endif
#    define PCS_EXTERN_C
#    define PCS_EXTERN_C_BEGIN
#    define PCS_EXTERN_C_END
#    define inline __inline
#  endif /* ifdef __cplusplus */
#  define pcs_strcmpi  _strcmpi
#  define pcs_strncmpi _strnicmp
#  define pcs_snprintf _snprintf
#else /* ifdef WIN32 */
#  ifdef __cplusplus
#    ifdef PCS_STATIC_LIB
#      define PCS_API extern "C"
#    else
#      define PCS_API extern "C"
#    endif
#    define PCS_EXTERN_C extern "C"
#    define PCS_EXTERN_C_BEGIN extern "C" {
#    define PCS_EXTERN_C_END }
#  else /* ifdef __cplusplus */
#    ifdef PCS_STATIC_LIB
#      define PCS_API
#    else
#      define PCS_API
#    endif
#    define PCS_EXTERN_C
#    define PCS_EXTERN_C_BEGIN
#    define PCS_EXTERN_C_END
#  endif /* ifdef __cplusplus */
#  define pcs_strcmpi  strcmpi
#  define pcs_strncmpi strncmpi
#  define pcs_snprintf snprintf
#endif /* ifdef WIN32 */

#if defined(WIN32)
#	if !defined(__cplusplus)
#		define inline __inline
#	endif
#	define strcmpi _strcmpi
#	define strncmpi  _strnicmp
#	define snprintf _snprintf
#endif

#ifndef NULL
#  define NULL 0
#endif

#define ToString(x) #x  
#define Xstr(x) ToString(x)

typedef unsigned char PcsBool;
#    define PcsTrue  1
#    define PcsFalse 0


#endif
