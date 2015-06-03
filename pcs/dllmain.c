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

/*时间转换*/
PCS_API const char *time_str(time_t time)
{
	struct tm *tm = NULL;
	time_t t = time;
	static char tmp[64];

	if (time)
		tm = localtime(&t);

	if (tm) {
		sprintf(tmp, "%d-%02d-%02d %02d:%02d:%02d",
			1900 + tm->tm_year,
			tm->tm_mon + 1,
			tm->tm_mday,
			tm->tm_hour,
			tm->tm_min,
			tm->tm_sec);
		return tmp;
	}
	else {
		return "";
	}
}

