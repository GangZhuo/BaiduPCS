#include "pcs_defs.h"
#include "pcs_mem.h"
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

/* 用于测试 */
typedef struct type_size_t
{
	int sz_int;
	int sz_long;
	int sz_long_long;
	int sz_int64_t;
	int sz_size_t;
	int sz_curl_off_t;
	int sz_time_t;
	int sz_pcs_bool_t;

} type_size_t;

/* 用于测试 */
/* For C# invoke */
PCS_API type_size_t* type_size()
{
	static type_size_t ts = {
		.sz_int = sizeof(int),
		.sz_long = sizeof(long),
		.sz_long_long = sizeof(long long),
		.sz_int64_t = sizeof(int64_t),
		.sz_size_t = sizeof(size_t),
		.sz_curl_off_t = sizeof(curl_off_t),
		.sz_time_t = sizeof(time_t),
		.sz_pcs_bool_t = sizeof(PcsBool),
	};
	return &ts;
}

/* For C# invoke */
PCS_API int str_len(const char *s)
{
	return strlen(s);
}

/* For C# invoke */
PCS_API void *pcs_malloc_4_net(size_t sz)
{
	return pcs_malloc(sz);
}

/* For C# invoke */
PCS_API void pcs_free_4_net(void *ptr)
{
	pcs_free(ptr);
}

