#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <locale.h>
#ifdef WIN32
# include <conio.h>
# include <direct.h>
# include <WinSock2.h>
# include <Windows.h>
# include "pcs/openssl_aes.h"
# include "pcs/openssl_md5.h"
#else
# include <inttypes.h>
# include <termios.h>
# include <openssl/aes.h>
# include <openssl/md5.h>
#endif

#include "pcs/pcs_mem.h"
#include "pcs/cJSON.h"
#include "pcs/pcs_utils.h"
#include "pcs/pcs.h"
#include "rb_tree/red_black_tree.h"
#include "version.h"
#include "dir.h"
#include "utils.h"
#include "arg.h"
#ifdef WIN32
# include "utf8.h"
#endif
#include "shell.h"

#define USAGE "Mozilla/5.0 (Windows NT 6.3; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/35.0.1916.153 Safari/537.36"
#define TIMEOUT						60
#define CONNECTTIMEOUT				10

#define PCS_CONTEXT_ENV				"PCS_CONTEXT"
#define PCS_COOKIE_ENV				"PCS_COOKIE"
#define PCS_CAPTCHA_ENV				"PCS_CAPTCHA"
#define TEMP_FILE_SUFFIX			".pcs_temp"
//#define PCS_DEFAULT_CONTEXT_FILE	"/tmp/pcs_context.json"


#ifndef PCS_BUFFER_SIZE
#  define PCS_BUFFER_SIZE			(AES_BLOCK_SIZE * 1024)
#endif

#define NONE         "\033[m"
#define RED          "\033[0;32;31m"
#define LIGHT_RED    "\033[1;31m"
#define GREEN        "\033[0;32;32m"
#define LIGHT_GREEN  "\033[1;32m"
#define BLUE         "\033[0;32;34m"
#define LIGHT_BLUE   "\033[1;34m"
#define DARY_GRAY    "\033[1;30m"
#define CYAN         "\033[0;36m"
#define LIGHT_CYAN   "\033[1;36m"
#define PURPLE       "\033[0;35m"
#define LIGHT_PURPLE "\033[1;35m"
#define BROWN        "\033[0;33m"
#define YELLOW       "\033[1;33m"
#define LIGHT_GRAY   "\033[0;37m"
#define WHITE        "\033[1;37m"

#define PRINT_PAGE_SIZE			20		/*列出目录或列出比较结果时，分页大小*/

#define OP_NONE					0
#define OP_EQ					1		/*文件相同*/
#define OP_LEFT					2		/*文件应更新到左边*/
#define OP_RIGHT				4		/*文件应更新到右边*/
#define OP_CONFUSE				8		/*困惑，不知道如何更新*/

#define OP_ST_NONE				0
#define OP_ST_SUCC				1		/*操作成功*/
#define OP_ST_FAIL				2		/*操作失败*/
#define OP_ST_SKIP				4		/*跳过本操作*/
#define OP_ST_CONFUSE			8		/*困惑操作*/
#define OP_ST_PROCESSING		16		/*正在执行操作*/

#define FLAG_NONE				0
#define FLAG_ON_LOCAL			1
#define FLAG_ON_REMOTE			2
#define FLAG_PARENT_NOT_ON_REMOTE 4

/* 文件元数据*/
typedef struct MyMeta MyMeta;
struct MyMeta
{
	char		*path;			/*文件路径*/

	time_t		local_mtime;	/*本地文件的修改时间*/
	int			local_isdir;	/*本地文件是否是目录。0表示不是目录；非0值表示本地文件是目录*/
	int			local_filecount; /*当本地是目录时，存储目录下文件的数目。递归统计。*/

	char		*remote_path;	/*文件路径*/
	time_t		remote_mtime;	/*文件在网盘中的最后修改时间*/
	int			remote_isdir;	/*文件在网盘中是以文件存在还是以目录存在。0表示以文件存在；非0值表示以目录存在*/

	int			flag;

	int			op;				/*需要执行的操作*/
	int			op_st;			/*操作的结果*/
	char		*msg;			/*操作失败时的错误消息*/

	MyMeta		*parent;		/*父目录的元数据*/
	void		*userdata;		/*用户数据*/
};

struct DownloadState
{
	FILE *pf;
	char *msg;
	size_t size;
};

struct RBEnumerateState
{
	int		first, second, other;
	int		print_op;	/* 允许打印的 OP 标记。
						 * 例如："OP_EQ | OP_LEFT | OP_RIGHT" 
						 * 将只打印 op 为 OP_EQ, OP_LEFT 和 OP_RIGHT 三项的节点
						 */
	int		print_flag;
	int		print_fail;

	int		no_print_op;
	int		no_print_flag;

	rb_red_blk_tree *tree;
	ShellContext   *context;

	int		page_size;

	int		cnt_total;
	int		cnt_valid_total;
	int		cnt_left;
	int		cnt_right;
	int		cnt_eq;
	int		cnt_confuse;
	int		cnt_none;

	int		cnt_fail;

	int		printed_count;
	int		page_index;
	int		page_enable;

	const char	*local_basedir;
	const char	*remote_basedir;

	/*在打印meta后调用一次*/
	int (*process)(MyMeta *meta, struct RBEnumerateState *s, void *state);
	void *processState;

	int dry_run;

	const char *prefixion;
};

struct PcsAesHead
{
	int					magic; /*Should be AES*/
	int					bits;
	int					polish;
	int					reserve;
};

struct ScanLocalFileState
{
	rb_red_blk_tree *rb;
	int				total;
};

static char *app_name = NULL;

/*
* 检查是否登录
*   msg   - 检测到未登录时的打印消息。传入NULL的话，则使用默认消息。
*/
static PcsBool is_login(ShellContext *context, const char *msg);

#ifdef WIN32

/*判断当前操作系统编码是否是UTF-8编码*/
int u8_is_utf8_sys()
{
	UINT codepage = GetConsoleCP();
	return codepage == 65001;
}

/*多字节字符转换成UTF-8编码*/
int u8_mbs_toutf8(char *dest, int sz, const char *src, int srcsz)
{
	wchar_t *unicode;
	int wchars, err;

	wchars = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, src, srcsz, NULL, 0);
	if (wchars == 0) {
		fprintf(stderr, "Unicode translation error %d\n", GetLastError());
		return -1;
	}

	unicode = (wchar_t *)alloca((wchars + 1) * sizeof(unsigned short));
	err = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, src, srcsz, unicode, wchars);
	if (err != wchars) {
		fprintf(stderr, "Unicode translation error %d\n", GetLastError());
		return -1;
	}
	unicode[wchars] = L'\0';
	return u8_toutf8(dest, sz, unicode, wchars);
}

int u8_mbs_toutf8_size(const char *src, int srcsz)
{
	wchar_t *unicode;
	int wchars, err;

	wchars = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, src, srcsz, NULL, 0);
	if (wchars == 0) {
		fprintf(stderr, "Unicode translation error %d\n", GetLastError());
		return -1;
	}

	unicode = (wchar_t *)alloca((wchars + 1) * sizeof(unsigned short));
	err = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, src, srcsz, unicode, wchars);
	if (err != wchars) {
		fprintf(stderr, "Unicode translation error %d\n", GetLastError());
		return -1;
	}
	unicode[wchars] = L'\0';
	//wprintf(L"UNICODE: %ls\n", unicode);
	return u8_size(unicode, wchars);
}

int u8_tombs(char *dest, int sz, const char *src, int srcsz)
{
	int unicode_size;
	wchar_t *unicode;
	//int unicode_len;
	int /*chars,*/ err;

	unicode_size = u8_wc_size(src, srcsz);
	unicode = (wchar_t *)alloca((unicode_size + 1) * sizeof(unsigned short));
	unicode[unicode_size] = L'\0';
	u8_toucs(unicode, unicode_size, src, srcsz);

	err = WideCharToMultiByte(GetConsoleCP(), WC_COMPOSITECHECK, unicode, unicode_size, dest, sz, NULL, NULL);
	if (err < 1)
	{
		fprintf(stderr, "Unicode translation error %d\n", GetLastError());
		return -1;
	}

	return err;
}

int u8_tombs_size(const char *src, int srcsz)
{
	int unicode_size;
	wchar_t *unicode;
	//int unicode_len;
	int /*chars,*/ err;

	unicode_size = u8_wc_size(src, srcsz);
	unicode = (wchar_t *)alloca((unicode_size + 1) * sizeof(unsigned short));
	unicode[unicode_size] = L'\0';
	u8_toucs(unicode, unicode_size, src, srcsz);

	err = WideCharToMultiByte(GetConsoleCP(), WC_COMPOSITECHECK, unicode, unicode_size, NULL, 0, NULL, NULL);
	return err;
}
# define printf u8_printf

# define sleep(s) Sleep((s) * 1000)

#endif

#pragma region 获取默认路径

/*获取上下文存储文件路径*/
static const char *contextfile()
{
	static char filename[1024] = { 0 };
	char *env_value = getenv(PCS_CONTEXT_ENV);
	if (env_value) return env_value;
	if (!filename[0]) {
#ifdef WIN32
		strcpy(filename, getenv("UserProfile"));
		strcat(filename, "\\.pcs");
		CreateDirectoryRecursive(filename);
		strcat(filename, "\\pcs.context");
#else
		strcpy(filename, getenv("HOME"));
		strcat(filename, "/.pcs");
		CreateDirectoryRecursive(filename);
		strcat(filename, "/pcs.context");
#endif
	}
	return filename;
}

/*返回COOKIE文件路径*/
static const char *cookiefile()
{
	static char filename[1024] = { 0 };
	char *env_value = getenv(PCS_COOKIE_ENV);
	if (env_value) return env_value;
	if (!filename[0]){ /*如果已经处理过，则直接返回*/
#ifdef WIN32
		strcpy(filename, getenv("UserProfile"));
		strcat(filename, "\\.pcs");
		CreateDirectoryRecursive(filename);
		strcat(filename, "\\");
		strcat(filename, "default.cookie");
#else
		strcpy(filename, getenv("HOME"));
		strcat(filename, "/.pcs");
		CreateDirectoryRecursive(filename);
		strcat(filename, "/");
		strcat(filename, "default.cookie");
#endif
	}
	return filename;
}

/*返回验证码图片文件路径*/
static const char *captchafile()
{
	static char filename[1024] = { 0 };
	char *env_value = getenv(PCS_CAPTCHA_ENV);
	if (env_value) return env_value;
	if (!filename[0]){ /*如果已经处理过，则直接返回*/
#ifdef WIN32
		strcpy(filename, getenv("UserProfile"));
		strcat(filename, "\\.pcs");
		CreateDirectoryRecursive(filename);
		strcat(filename, "\\");
		strcat(filename, "captcha.gif");
#else
		strcpy(filename, getenv("HOME"));
		strcat(filename, "/.pcs");
		CreateDirectoryRecursive(filename);
		strcat(filename, "/");
		strcat(filename, "captcha.gif");
#endif
	}
	return filename;
}

#pragma endregion

#pragma region 三个回调： 输入验证码、显示上传进度、写下载文件

/*输出验证码图片，并等待用户输入识别结果*/
static PcsBool verifycode(unsigned char *ptr, size_t size, char *captcha, size_t captchaSize, void *state)
{
	static char filename[1024] = { 0 };
	FILE *pf;

	if (!filename[0]) {
#ifdef WIN32
		strcpy(filename, getenv("UserProfile"));
		strcat(filename, "\\.pcs");
		CreateDirectoryRecursive(filename);
		strcat(filename, "\\vc.gif");
#else
		strcpy(filename, getenv("HOME"));
		strcat(filename, "/.pcs");
		CreateDirectoryRecursive(filename);
		strcat(filename, "/vc.gif");
#endif
	}

	pf = fopen(filename, "wb");
	if (!pf) return PcsFalse;
	fwrite(ptr, 1, size, pf);
	fclose(pf);

	printf("The captcha image at %s.\nPlease input the captcha code: ", filename);
	std_string(captcha, captchaSize);
	return PcsTrue;
}

/*显示上传进度*/
static int upload_progress(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow)
{
	char *path = (char *)clientp;
	static char tmp[64];

	tmp[63] = '\0';
	if (path)
		printf("Upload %s ", path);
	printf("%s", pcs_utils_readable_size(ulnow, tmp, 63, NULL));
	printf("/%s      \r", pcs_utils_readable_size(ultotal, tmp, 63, NULL));
	fflush(stdout);

	return 0;
}

/*下载文件时，写入文件并显示进度*/
static int download_write(char *ptr, size_t size, size_t contentlength, void *userdata)
{
	struct DownloadState *ds = (struct DownloadState *)userdata;
	FILE *pf = ds->pf;
	size_t i;
	char tmp[64];
	tmp[63] = '\0';
	i = fwrite(ptr, 1, size, pf);
	ds->size += i;
	if (ds->msg)
		printf("%s", ds->msg);
	printf("%s", pcs_utils_readable_size(ds->size, tmp, 63, NULL));
	printf("/%s      \r", pcs_utils_readable_size(contentlength, tmp, 63, NULL));
	fflush(stdout);
	return i;
}

#pragma endregion

#pragma region 公用函数

/*找到str第一次出现ch的位置*/
static inline char *findchar(char *str, int ch)
{
	char *p = str;
	while (*p && ((int)((unsigned int)(*p))) != ch) p++;
	return p;
}

/*回到上一行*/
static inline void clear_current_print_line()
{
	//printf("\033[1A"); //先回到上一行
	printf("\033[K");  //清除该行
}

/*回到上一行*/
static inline void back_prev_print_line()
{
	printf("\033[1A"); //先回到上一行
	printf("\033[K");  //清除该行
}

/*把文件大小转换成字符串*/
static const char *size_tostr(size_t size, int *fix_width, char ch)
{
	static char str[128], *p;
	int i;
	int j, cn, mod;
	size_t sz;

	if (size == 0) {
		i = 0;
		if (*fix_width > 0) {
			for (; i < *fix_width - 1; i++) {
				str[i] = ch;
			}
		}
		str[i] = '0';
		str[i + 1] = '\0';
		if (*fix_width < 0)
			*fix_width = 1;
		return str;
	}

	sz = size;
	j = 127;
	str[j] = '\0';
	cn = 0;
	while (sz != 0) {
		mod = sz % 10;
		sz = sz / 10;
		str[--j] = (char)('0' + mod);
		cn++;
	}

	i = 0;
	if (*fix_width > 0) {
		for (; i < *fix_width - cn; i++) {
			str[i] = ch;
		}
	}
	p = &str[j];
	while (*p){
		str[i++] = *p++;
	}
	str[i] = '\0';
	if (*fix_width < 0)
		*fix_width = (int)i;
	return str;
}

/*打印文件时间*/
static void print_time(const char *format, time_t time)
{
	struct tm *tm = NULL;
	time_t t = time;
	char tmp[64];

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
		printf(format, tmp);
	}
	else {
		printf(format, "0000-00-00 00:00:00");
	}
}

/*打印可读的文件大小*/
static void print_size(const char *format, size_t size)
{
	char tmp[64];
	tmp[63] = '\0';
	pcs_utils_readable_size((double)size, tmp, 63, NULL);
	printf(format, tmp);
}

/*打印文件列表的头*/
static void print_filelist_head(int size_width)
{
	int i;
	putchar('D');
	putchar(' ');
	for (i = 0; i < size_width - 4; i++)
		putchar(' ');
	printf("Size");
	putchar(' ');
	printf("Modify Date Time");
	putchar(' ');
	putchar(' ');
	putchar(' ');
	putchar(' ');
	printf("File Name\n");
}

/*打印文件列表的数据行*/
static void print_filelist_row(PcsFileInfo *f, int size_width)
{
	const char *p;

	if (f->isdir)
		putchar('d');
	else
		putchar('-');
	putchar(' ');

	p = size_tostr(f->size, &size_width, ' ');
	while (*p) {
		putchar(*p++);
	}
	putchar(' ');
	print_time("%s", f->server_mtime);
	putchar(' ');
	printf("%s\n", f->path);
}

/*打印文件列表*/
static void print_filelist(PcsFileInfoList *list, int *pFileCount, int *pDirCount, size_t *pTotalSize)
{
	char tmp[64] = { 0 };
	int cnt_file = 0,
		cnt_dir = 0,
		size_width = 1,
		w;
	PcsFileInfo *file = NULL;
	size_t total = 0;
	PcsFileInfoListIterater iterater;

	pcs_filist_iterater_init(list, &iterater, PcsFalse);
	while (pcs_filist_iterater_next(&iterater)) {
		file = iterater.current;
		w = -1;
		size_tostr(file->size, &w, ' ');
		if (size_width < w)
			size_width = w;
		total += (size_t)file->size;
		if (file->isdir)
			cnt_dir++;
		else
			cnt_file++;
	}

	if (size_width < 4)
		size_width = 4;
	print_filelist_head(size_width);
	puts("------------------------------------------------------------------------------");
	pcs_filist_iterater_init(list, &iterater, PcsFalse);
	while (pcs_filist_iterater_next(&iterater)) {
		file = iterater.current;
		print_filelist_row(file, size_width);
	}
	puts("------------------------------------------------------------------------------");
	pcs_utils_readable_size(total, tmp, 63, NULL);
	tmp[63] = '\0';
	printf("Total: %s, File Count: %d, Directory Count: %d\n", tmp, cnt_file, cnt_dir);
	putchar('\n');
	if (pFileCount) *pFileCount += cnt_file;
	if (pDirCount) *pDirCount += cnt_dir;
	if (pTotalSize) *pTotalSize += total;
}

/*打印文件或目录的元数据*/
static void print_fileinfo(PcsFileInfo *f, const char *prex)
{
	if (!prex) prex = "";
	printf("%sfs_id:\t%llu\n", prex, f->fs_id);
	printf("%sCategory:\t%d\n", prex, f->category);
	printf("%sPath:\t\t%s\n", prex, f->path);
	printf("%sFilename:\t%s\n", prex, f->server_filename);
	printf("%s", prex);
	print_time("Create time:\t%s\n", f->server_ctime);
	printf("%s", prex);
	print_time("Modify time:\t%s\n", f->server_mtime);
	printf("%sIs Dir:\t%s\n", prex, f->isdir ? "Yes" : "No");
	if (!f->isdir) {
		printf("%s", prex);
		print_size("Size:\t\t%s\n", f->size);
		printf("%smd5:\t\t%s\n", prex, f->md5);
		printf("%sdlink:\t\t%s\n", prex, f->dlink);
	}
}

#pragma endregion

#pragma region 初始化相关函数

/*hood cJSON 库中分配内存的方法，用于检查内存泄漏*/
static void hook_cjson()
{
	cJSON_Hooks hooks = { 0 };
#if defined(DEBUG) || defined(_DEBUG)
	hooks.malloc_fn = &pcs_mem_malloc_arg1;
	hooks.free_fn = &pcs_mem_free;
#else

#endif
	cJSON_InitHooks(&hooks);
}

/*把上下文转换为字符串*/
static char *context2str(ShellContext *context)
{
	char *json;
	cJSON *root, *item;

	root = cJSON_CreateObject();
	assert(root);

	item = cJSON_CreateString(context->cookiefile);
	assert(item);
	cJSON_AddItemToObject(root, "cookiefile", item);

	item = cJSON_CreateString(context->captchafile);
	assert(item);
	cJSON_AddItemToObject(root, "captchafile", item);

	item = cJSON_CreateString(context->workdir);
	assert(item);
	cJSON_AddItemToObject(root, "workdir", item);

	item = cJSON_CreateNumber((double)context->list_page_size);
	assert(item);
	cJSON_AddItemToObject(root, "list_page_size", item);

	item = cJSON_CreateString(context->list_sort_name);
	assert(item);
	cJSON_AddItemToObject(root, "list_sort_name", item);

	item = cJSON_CreateString(context->list_sort_direction);
	assert(item);
	cJSON_AddItemToObject(root, "list_sort_direction", item);

	item = cJSON_CreateString(context->secure_method);
	assert(item);
	cJSON_AddItemToObject(root, "secure_method", item);

	item = cJSON_CreateString(context->secure_key);
	assert(item);
	cJSON_AddItemToObject(root, "secure_key", item);

	item = cJSON_CreateBool(context->secure_enable);
	assert(item);
	cJSON_AddItemToObject(root, "secure_enable", item);

	item = cJSON_CreateBool(context->timeout_retry);
	assert(item);
	cJSON_AddItemToObject(root, "timeout_retry", item);

	json = cJSON_Print(root);
	assert(json);

	cJSON_Delete(root);
	return json;
}

/*保存上下文*/
static void save_context(ShellContext *context)
{
	const char *filename;
	char *json;
	FILE *pf;

	json = context2str(context);
	assert(json);

	filename = contextfile();
	pf = fopen(filename, "wb");
	if (!pf) {
		fprintf(stderr, "Error: Can't open the file: %s\n", filename);
		pcs_free(json);
		return;
	}
	fwrite(json, 1, strlen(json), pf);
	fclose(pf);
	pcs_free(json);
}

/*还原保存的上下文。
成功返回0，失败返回非0值。*/
static int restore_context(ShellContext *context, const char *filename)
{
	char *filecontent = NULL;
	int filesize = 0;
	cJSON *root, *item;

	if (!filename) {
		filename = context->contextfile;
	}
	else {
		if (context->contextfile) pcs_free(context->contextfile);
		context->contextfile = pcs_utils_strdup(filename);
	}
	filesize = read_file(filename, &filecontent);
	if (filesize <= 0) {
		fprintf(stderr, "Error: Can't read the context file (%s).\n", filename, app_name);
		if (filecontent) pcs_free(filecontent);
		return -1;
	}
	root = cJSON_Parse(filecontent);
	if (!root) {
		fprintf(stderr, "Error: Broken context file (%s).\n", filename, app_name);
		pcs_free(filecontent);
		return -1;
	}

	item = cJSON_GetObjectItem(root, "cookiefile");
	if (item && item->valuestring && item->valuestring[0]) {
		if (!is_absolute_path(item->valuestring)) {
			printf("warning: Invalid context.cookiefile, the value should be absolute path, use default value: %s.\n", context->cookiefile);
		}
		else {
			if (context->cookiefile) pcs_free(context->cookiefile);
			context->cookiefile = pcs_utils_strdup(item->valuestring);
		}
	}

	item = cJSON_GetObjectItem(root, "captchafile");
	if (item && item->valuestring && item->valuestring[0]) {
		if (!is_absolute_path(item->valuestring)) {
			printf("warning: Invalid context.captchafile, the value should be absolute path, use default value: %s.\n", context->captchafile);
		}
		else {
			if (context->captchafile) pcs_free(context->captchafile);
			context->captchafile = pcs_utils_strdup(item->valuestring);
		}
	}

	item = cJSON_GetObjectItem(root, "workdir");
	if (item && item->valuestring && item->valuestring[0]) {
		if (item->valuestring[0] != '/') {
			printf("warning: Invalid context.workdir, the value should be absolute path, use default value: %s.\n", context->workdir);
		}
		else {
			if (context->workdir) pcs_free(context->workdir);
			context->workdir = pcs_utils_strdup(item->valuestring);
		}
	}

	item = cJSON_GetObjectItem(root, "list_page_size");
	if (item) {
		if (((int)item->valueint) < 1) {
			printf("warning: Invalid context.list_page_size, the value should be great than 0, use default value: %d.\n", context->list_page_size);
		}
		else {
			context->list_page_size = (int)item->valueint;
		}
	}

	item = cJSON_GetObjectItem(root, "list_sort_name");
	if (item && item->valuestring && item->valuestring[0]) {
		if (strcmp(item->valuestring, "name") && strcmp(item->valuestring, "time") && strcmp(item->valuestring, "size")) {
			printf("warning: Invalid context.list_sort_name, the value should be one of [name|time|size], use default value: %s.\n", context->list_sort_name);
		}
		else {
			if (context->list_sort_name) pcs_free(context->list_sort_name);
			context->list_sort_name = pcs_utils_strdup(item->valuestring);
		}
	}

	item = cJSON_GetObjectItem(root, "list_sort_direction");
	if (item && item->valuestring && item->valuestring[0]) {
		if (strcmp(item->valuestring, "asc") && strcmp(item->valuestring, "desc")) {
			printf("warning: Invalid context.list_sort_direction, the value should be one of [asc|desc], use default value: %s.\n", context->list_sort_direction);
		}
		else {
			if (context->list_sort_direction) pcs_free(context->list_sort_direction);
			context->list_sort_direction = pcs_utils_strdup(item->valuestring);
		}
	}

	item = cJSON_GetObjectItem(root, "secure_method");
	if (item && item->valuestring && item->valuestring[0]) {
		if (strcmp(item->valuestring, "plaintext") && strcmp(item->valuestring, "aes-cbc-128") && strcmp(item->valuestring, "aes-cbc-192") && strcmp(item->valuestring, "aes-cbc-256")) {
			printf("warning: Invalid context.secure_method, the value should be one of [plaintext|aes-cbc-128|aes-cbc-192|aes-cbc-256], use default value: %s.\n", context->secure_method);
		}
		else {
			if (context->secure_method) pcs_free(context->secure_method);
			context->secure_method = pcs_utils_strdup(item->valuestring);
		}
	}

	item = cJSON_GetObjectItem(root, "secure_key");
	if (item && item->valuestring && item->valuestring[0]) {
		if (context->secure_key) pcs_free(context->secure_key);
		context->secure_key = pcs_utils_strdup(item->valuestring);
	}

	item = cJSON_GetObjectItem(root, "secure_enable");
	if (item) {
		context->secure_enable = item->valueint ? 1 : 0;
	}

	item = cJSON_GetObjectItem(root, "timeout_retry");
	if (item) {
		context->timeout_retry = item->valueint ? 1 : 0;
	}

	cJSON_Delete(root);
	pcs_free(filecontent);
	if (context->contextfile) pcs_free(context->contextfile);
	context->contextfile = pcs_utils_strdup(filename);
	return 0;
}

/*初始化上下文*/
static void init_context(ShellContext *context, struct args *arg)
{
	memset(context, 0, sizeof(ShellContext));
	context->contextfile = pcs_utils_strdup(contextfile());
	context->cookiefile = pcs_utils_strdup(cookiefile());
	context->captchafile = pcs_utils_strdup(captchafile());
	context->workdir = pcs_utils_strdup("/");
	context->list_page_size = PRINT_PAGE_SIZE;
	context->list_sort_name = pcs_utils_strdup("name");
	context->list_sort_direction = pcs_utils_strdup("asc");
	
	context->secure_method = pcs_utils_strdup("plaintext");
	context->secure_key = pcs_utils_strdup("");
	context->secure_enable = 0;

	context->timeout_retry = 1;
}

/*释放上下文*/
static void free_context(ShellContext *context)
{
	if (context->cookiefile) pcs_free(context->cookiefile);
	if (context->captchafile) pcs_free(context->captchafile);
	if (context->workdir) pcs_free(context->workdir);
	if (context->list_sort_name) pcs_free(context->list_sort_name);
	if (context->list_sort_direction) pcs_free(context->list_sort_direction);
	if (context->pcs) pcs_destroy(context->pcs);
	if (context->secure_method) pcs_free(context->secure_method);
	if (context->secure_key) pcs_free(context->secure_key);
	if (context->contextfile) pcs_free(context->contextfile);
	memset(context, 0, sizeof(ShellContext));
}

/*初始化PCS的安全选项*/
static void init_pcs_secure(ShellContext *context)
{
	int method = 0;
	if (context->secure_method && context->secure_method[0]) {
		if (!strcmp(context->secure_method, "aes-cbc-128")) {
			method = PCS_SECURE_AES_CBC_128;
		}
		else if (!strcmp(context->secure_method, "aes-cbc-192")) {
			method = PCS_SECURE_AES_CBC_192;
		}
		else if (!strcmp(context->secure_method, "aes-cbc-256")) {
			method = PCS_SECURE_AES_CBC_256;
		}
		else if (!strcmp(context->secure_method, "plaintext")) {
			method = PCS_SECURE_PLAINTEXT;
		}
	}
	if (method){
		pcs_setopts(context->pcs,
			PCS_OPTION_SECURE_METHOD, (void *)((long)method),
			PCS_OPTION_SECURE_KEY, context->secure_key,
			PCS_OPTION_SECURE_ENABLE, context->secure_enable ? ((void *)((long)PcsTrue)) : ((void *)((long)PcsFalse)),
			PCS_OPTION_END);
	}
	else {
		pcs_setopts(context->pcs,
			PCS_OPTION_SECURE_METHOD, NULL,
			PCS_OPTION_SECURE_KEY, NULL,
			PCS_OPTION_SECURE_ENABLE, ((void *)((long)PcsFalse)),
			PCS_OPTION_END);
	}
}

/*初始化PCS*/
static void init_pcs(ShellContext *context)
{
	context->pcs = pcs_create(context->cookiefile);
	pcs_setopt(context->pcs, PCS_OPTION_CAPTCHA_FUNCTION, (void *)&verifycode);
	pcs_setopts(context->pcs,
		PCS_OPTION_PROGRESS_FUNCTION, (void *)&upload_progress,
		PCS_OPTION_PROGRESS, (void *)((long)PcsFalse),
		PCS_OPTION_USAGE, (void *)USAGE,
		//PCS_OPTION_TIMEOUT, (void *)((long)TIMEOUT),
		PCS_OPTION_CONNECTTIMEOUT, (void *)((long)CONNECTTIMEOUT),
		PCS_OPTION_END);
	init_pcs_secure(context);
}

#pragma endregion

#pragma region 打印用法相关函数

/*打印版本*/
static void version()
{
	printf(program_full_name "\n", app_name);
}

/*打印cat命令用法*/
static void usage_cat()
{
	version();
	printf("\nUsage: %s cat [-h] <path>\n", app_name);
	printf("\nDescription:\n");
	printf("  Print the file content\n");
	printf("\nOptions:\n");
	printf("  -h    Print the usage.\n");
	printf("\nSamples:\n");
	printf("  %s cat -h\n", app_name);
	printf("  %s cat /music/list.txt\n", app_name);
	printf("  %s cat list.txt\n", app_name);
	printf("  %s cat \"/music/Europe and America/list.txt\"\n", app_name);
}

/*打印cd命令用法*/
static void usage_cd()
{
	version();
	printf("\nUsage: %s cd [-h] <path>\n", app_name);
	printf("\nDescription:\n");
	printf("  Change the work directory\n");
	printf("\nOptions:\n");
	printf("  -h    Print the usage.\n");
	printf("\nSamples:\n");
	printf("  %s cd -h\n", app_name);
	printf("  %s cd /music\n", app_name);
	printf("  %s cd subdir\n", app_name);
	printf("  %s cd \"/music/Europe and America\"\n", app_name);
}

/*打印copy命令用法*/
static void usage_copy()
{
	version();
	printf("\nUsage: %s copy [-h] <src> <dst>\n", app_name);
	printf("\nDescription:\n");
	printf("  Copy the file|directory\n");
	printf("\nOptions:\n");
	printf("  -h    Print the usage.\n");
	printf("\nSamples:\n");
	printf("  %s copy -h\n", app_name);
	printf("  %s copy src.txt dst.txt\n", app_name);
	printf("  %s copy /music/src.mp3 /music/mp3/dst.mp3\n", app_name);
	printf("  %s copy /music/src.mp3 \"/music/Europe and America/dst.mp3\"\n", app_name);
}

/*打印compare命令用法*/
static void usage_compare()
{
	version();
	printf("\nUsage: %s compare [-cdehru] <local path> <net disk path>\n", app_name);
	printf("\nDescription:\n");
	printf("  Print the differents between local and net disk. \n"
		   "  Default options is '-cdu'. \n");
	printf("\nOptions:\n");
	printf("  -c    Print the files that confuse, which is don't know how to do.\n");
	printf("  -d    Print the files that is old than the net disk.\n");
	printf("  -e    Print the files that is same between local and net disk.\n");
	printf("  -h    Print the usage.\n");
	printf("  -r    Recursive compare the sub directories.\n");
	printf("  -u    Print the files that is newer than the net disk.\n");
	printf("\nSamples:\n");
	printf("  %s compare -h\n", app_name);
	printf("  %s compare ~/music /music\n", app_name);
	printf("  %s compare music /music\n", app_name);
	printf("  %s compare -r music /music\n", app_name);
}

/*打印context命令用法*/
static void usage_context()
{
	version();
	printf("\nUsage: %s context [-h]\n", app_name);
	printf("\nDescription:\n");
	printf("  Print the context\n");
	printf("\nOptions:\n");
	printf("  -h    Print the usage.\n");
	printf("\nSamples:\n");
	printf("  %s context -h\n", app_name);
	printf("  %s context\n", app_name);
}

/*打印download命令用法*/
static void usage_download()
{
	version();
	printf("\nUsage: %s download [-fh] <file> <local file>\n", app_name);
	printf("\nDescription:\n");
	printf("  Download the file\n");
	printf("\nOptions:\n");
	printf("  -f    Force override the local file when the file exists on local file system.\n");
	printf("  -h    Print the usage.\n");
	printf("\nSamples:\n");
	printf("  %s download -h\n", app_name);
	printf("  %s download dst.txt ~/dst.txt\n", app_name);
	printf("  %s download dst.txt dst.txt\n", app_name);
	printf("  %s download -f dst.txt ~/dst.txt\n", app_name);
	printf("  %s download \"/music/dst.mp3\" \"/home/pcs/music/dst.mp3\"\n", app_name);
}

/*打印echo命令用法*/
static void usage_echo()
{
	version();
	printf("\nUsage: %s echo [-ah] <path> <text>\n", app_name);
	printf("\nDescription:\n");
	printf("  Write the text into net disk file\n");
	printf("\nOptions:\n");
	printf("  -a    Append the text.\n");
	printf("  -h    Print the usage.\n");
	printf("\nSamples:\n");
	printf("  %s echo -h\n", app_name);
	printf("  %s echo src.txt \"This is from 'echo' command.\" \n", app_name);
	printf("  %s echo /docs/src.txt \"This is from 'echo' command.\"\n", app_name);
	printf("  %s echo -a \"for test/src.txt\" \"This is from 'echo' command.\"\n", app_name);
}

/*打印encode命令用法*/
static void usage_encode()
{
	version();
	printf("\nUsage: %s encode [-deh] <src> <dst>\n", app_name);
	printf("\nDescription:\n");
	printf("  Encrypt/decrypt the file. Default option is '-d'\n");
	printf("\nOptions:\n");
	printf("  -d    Decrypt the file.\n");
	printf("  -e    Encrypt the file.\n");
	printf("  -f    Force override the dest file, if it's exist.\n");
	printf("  -h    Print the usage.\n");
	printf("\nSamples:\n");
	printf("  %s encode -h\n", app_name);
	printf("  %s encode plain.txt cipher.txt \n", app_name);
	printf("  %s encode -d cipher.txt plain.txt\n", app_name);
}

/*打印help命令用法*/
static void usage_help()
{
	version();
	printf("\nUsage: %s help [-h] [command]\n", app_name);
	printf("\nDescription:\n");
	printf("  Print the usage\n");
	printf("\nOptions:\n");
	printf("  -h    Print the usage.\n");
	printf("\nSamples:\n");
	printf("  %s help -h\n", app_name);
	printf("  %s help\n", app_name);
	printf("  %s help synch\n", app_name);
}

/*打印list命令用法*/
static void usage_list()
{
	version();
	printf("\nUsage: %s list [-h] [path]\n", app_name);
	printf("\nDescription:\n");
	printf("  List the directory\n");
	printf("\nOptions:\n");
	printf("  -h    Print the usage.\n");
	printf("\nSamples:\n");
	printf("  %s list -h\n", app_name);
	printf("  %s list\n", app_name);
	printf("  %s list /music\n", app_name);
	printf("  %s list \"/music/Europe and America\"\n", app_name);
}

/*打印login命令用法*/
static void usage_login()
{
	version();
	printf("\nUsage: %s login [-h] [--username=<user name>] [--password=<password>]\n", app_name);
	printf("\nDescription:\n");
	printf("  The command maybe save captcha image into your local file system, \n"
		   "  then you can see the captcha. You can use '%s set --captcha_file=<path>' \n"
		   "  to change the save path, and use '%s context' command to view the path.\n"
		   "  The cookie can reusable, so you can login in your pc, and then upload the \n"
		   "  cookie file to your vps.\n"
		   "  After login, you can use '%s who' to see the user id, \n"
		   "  and use '%s pwd' to see your work directory.\n",
		   app_name, app_name, app_name, app_name);
	printf("\nOptions:\n");
	printf("  -h            Print the usage.\n");
	printf("  --username    Specify user name.\n");
	printf("  --password    Specify password.\n");
	printf("\nSamples:\n");
	printf("  %s login -h\n", app_name);
	printf("  %s login\n", app_name);
	printf("  %s login --username=gang\n", app_name);
	printf("  %s login --username=gang --password=\"password\"\n", app_name);
}

/*打印logout命令用法*/
static void usage_logout()
{
	version();
	printf("\nUsage: %s logout [-h]\n", app_name);
	printf("\nDescription:\n");
	printf("  \n");
	printf("\nOptions:\n");
	printf("  -h    Print the usage.\n");
	printf("\nSamples:\n");
	printf("  %s logout -h\n", app_name);
	printf("  %s logout\n", app_name);
}

/*打印meta命令用法*/
static void usage_meta()
{
	version();
	printf("\nUsage: %s meta [-h] [path]\n", app_name);
	printf("\nDescription:\n");
	printf("  Print the file|directory meta information\n");
	printf("\nOptions:\n");
	printf("  -h    Print the usage.\n");
	printf("\nSamples:\n");
	printf("  %s meta -h\n", app_name);
	printf("  %s meta\n", app_name);
	printf("  %s meta /music\n", app_name);
	printf("  %s meta \"/music/Europe and America\"\n", app_name);
}

/*打印mkdir命令用法*/
static void usage_mkdir()
{
	version();
	printf("\nUsage: %s mkdir [-h] <path>\n", app_name);
	printf("\nDescription:\n");
	printf("  Make a new directory\n");
	printf("\nOptions:\n");
	printf("  -h    Print the usage.\n");
	printf("\nSamples:\n");
	printf("  %s mkdir -h\n", app_name);
	printf("  %s mkdir subdir\n", app_name);
	printf("  %s mkdir /music\n", app_name);
	printf("  %s mkdir \"/music/Europe and America\"\n", app_name);
}

/*打印move命令用法*/
static void usage_move()
{
	version();
	printf("\nUsage: %s move [-h] <src> <dst>\n", app_name);
	printf("\nDescription:\n");
	printf("  Move the file|directory into other file|directory\n");
	printf("\nOptions:\n");
	printf("  -h    Print the usage.\n");
	printf("\nSamples:\n");
	printf("  %s move -h\n", app_name);
	printf("  %s move src.txt dst.txt\n", app_name);
	printf("  %s move /music/src.mp3 /music/mp3/dst.mp3\n", app_name);
	printf("  %s move /music/src.mp3 \"/music/Europe and America/dst.mp3\"\n", app_name);
}

/*打印pwd命令用法*/
static void usage_pwd()
{
	version();
	printf("\nUsage: %s pwd [-h]\n", app_name);
	printf("\nDescription:\n");
	printf("  Print the current work directory\n");
	printf("\nOptions:\n");
	printf("  -h    Print the usage.\n");
	printf("\nSamples:\n");
	printf("  %s pwd -h\n", app_name);
	printf("  %s pwd\n", app_name);
}

/*打印quota命令用法*/
static void usage_quota()
{
	version();
	printf("\nUsage: %s quota [-eh]\n", app_name);
	printf("\nDescription:\n");
	printf("  Print the quota\n");
	printf("\nOptions:\n");
	printf("  -e    Print the exact size.\n");
	printf("  -h    Print the usage.\n");
	printf("\nSamples:\n");
	printf("  %s quota -h\n", app_name);
	printf("  %s quota\n", app_name);
	printf("  %s quota -e\n", app_name);
}

/*打印remove命令用法*/
static void usage_remove()
{
	version();
	printf("\nUsage: %s remove [-h] <path>\n", app_name);
	printf("\nDescription:\n");
	printf("  Remove the file|directory\n");
	printf("\nOptions:\n");
	printf("  -h    Print the usage.\n");
	printf("\nSamples:\n");
	printf("  %s remove -h\n", app_name);
	printf("  %s remove src.txt\n", app_name);
	printf("  %s remove /music/src.mp3\n", app_name);
	printf("  %s remove \"/music/Europe and America/dst.mp3\"\n", app_name);
}

/*打印rename命令用法*/
static void usage_rename()
{
	version();
	printf("\nUsage: %s rename [-h] <src> <new name>\n", app_name);
	printf("\nDescription:\n");
	printf("  Rename the file|directory\n");
	printf("\nOptions:\n");
	printf("  -h    Print the usage.\n");
	printf("\nSamples:\n");
	printf("  %s rename -h\n", app_name);
	printf("  %s rename src.txt dst.txt\n", app_name);
	printf("  %s rename /music/src.mp3 dst.mp3\n", app_name);
	printf("  %s rename \"/music/Europe and America/src.mp3\" \"dst 2.mp3\"\n", app_name);
}

/*打印list命令用法*/
static void usage_set()
{
	version();
	printf("\nUsage: %s set [-h] [--captcha_file=<path>] [--cookie_file=<path>] ...\n", app_name);
	printf("\nDescription:\n");
	printf("  Change the context, you can see the context by '%s context' command.\n", app_name);
	printf("\nOptions:\n");
	printf("  -h    Print the usage.\n");
	printf("\nOptions:\n");
	printf("  Option Name          Type       Possible Values \n");
	printf("  -----------------------------------------------\n");
	printf("  captcha_file         String     not null\n");
	printf("  cookie_file          String     not null\n");
	printf("  list_page_size       UInt       >0\n");
	printf("  list_sort_direction  Enum       asc|desc\n");
	printf("  list_sort_name       Enum       name|time|size\n");
	printf("  secure_enable        Boolean    true|false\n");
	printf("  secure_key           String     not null when 'secure_method' is not 'plaintext'\n");
	printf("  secure_method        Enum       plaintext|aes-cbc-128|aes-cbc-192|aes-cbc-256\n");
	printf("\nSamples:\n");
	printf("  %s set -h\n", app_name);
	printf("  %s set --cookie_file=\"/tmp/pcs.cookie\"\n", app_name);
	printf("  %s set --cookie_file=\"/tmp/pcs.cookie\" --captcha_file=\"/tmp/vc.git\"\n", app_name);
	printf("  %s set --list_page_size=20 --list_sort_name=name --list_sort_direction=desc\n", app_name);
}

/*打印search命令用法*/
static void usage_search()
{
	version();
	printf("\nUsage: %s search [-hr] [target dir] <key word>\n", app_name);
	printf("\nDescription:\n");
	printf("  Search the files in the specify directory\n");
	printf("\nOptions:\n");
	printf("  -h    Print the usage.\n");
	printf("  -r    Recursive search the sub directories.\n");
	printf("\nSamples:\n");
	printf("  %s search -h\n", app_name);
	printf("  %s search dst.txt\n", app_name);
	printf("  %s search -r dst.txt\n", app_name);
	printf("  %s search /music dst.mp3\n", app_name);
	printf("  %s search -r /music dst.mp3\n", app_name);
	printf("  %s search \"/music/Europe and America\" \"dst 2.mp3\"\n", app_name);
}

/*打印synch命令用法*/
static void usage_synch()
{
	version();
	printf("\nUsage: %s synch [-cdehnru] <local path> <net disk path>\n", app_name);
	printf("\nDescription:\n");
	printf("  Synch between local and net disk. \n"
		   "  Default options is '-cdu', means download newer files, upload newer files \n"
		   "  and print confuse files. You can use '-u' to upload newer files only, use \n"
		   "  '-d' to download newer files only, and use '-c' to view confuse files.\n"
		   "  Notes:\n"
		   "    The confuse items will do nothing, \n"
		   "    e.g. A side of the target is file and another is directory.\n",
		   app_name);
	printf("\nOptions:\n");
	printf("  -c    Print the files that confuse, which is don't know how to do.\n");
	printf("  -d    Synch the new files from the net disk. \n"
		   "        This option will download the new files from the net disk.\n"
		   "        You can use 'compare -dr <local dir> <disk dir>' to view \n"
		   "        how many and which files will download.\n");
	printf("  -e    Print the files that is same between local and net disk.\n");
	printf("  -h    Print the usage.\n");
	printf("  -n    Dry run.\n");
	printf("  -r    Recursive synch the sub directories.\n");
	printf("  -u    Synch the new files to the net disk.\n \n"
		   "        This option will upload new files from the net disk.\n"
		   "        You can use 'compare -ur <local dir> <disk dir>' to view \n"
		   "        how many and which files will upload.\n");
	printf("\nSamples:\n");
	printf("  %s synch -h\n", app_name);
	printf("  %s synch ~/music /music  \n", app_name);
	printf("  %s synch -d ~/music /music  \n", app_name);
	printf("  %s synch -u music /music\n", app_name);
	printf("  %s synch -c music /music\n", app_name);
	printf("  %s synch -cdu music /music\n", app_name);
	printf("  %s synch -r music /music\n", app_name);
}

/*打印upload命令用法*/
static void usage_upload()
{
	version();
	printf("\nUsage: %s upload [-fh] <local file> <file>\n", app_name);
	printf("\nDescription:\n");
	printf("  Upload the file\n");
	printf("\nOptions:\n");
	printf("  -f    Force override the local file when the file exists on local file system.\n");
	printf("  -h    Print the usage.\n");
	printf("\nSamples:\n");
	printf("  %s upload -h\n", app_name);
	printf("  %s upload ~/dst.txt dst.txt\n", app_name);
	printf("  %s upload dst.txt dst.txt\n", app_name);
	printf("  %s upload -f ~/dst.txt dst.txt\n", app_name);
	printf("  %s upload \"/home/pcs/music/dst.mp3\" \"/music/dst.mp3\"\n", app_name);
}

/*打印version命令用法*/
static void usage_version()
{
	version();
	printf("\nUsage: %s version [-h]\n", app_name);
	printf("\nDescription:\n");
	printf("  Print the version\n");
	printf("\nOptions:\n");
	printf("  -h    Print the usage.\n");
	printf("\nSamples:\n");
	printf("  %s version -h\n", app_name);
	printf("  %s version\n", app_name);
}

/*打印who命令用法*/
static void usage_who()
{
	version();
	printf("\nUsage: %s who [-h]\n", app_name);
	printf("\nDescription:\n");
	printf("  Print the current user\n");
	printf("\nOptions:\n");
	printf("  -h    Print the usage.\n");
	printf("\nSamples:\n");
	printf("  %s who -h\n", app_name);
	printf("  %s who\n", app_name);
}

/*打印用法*/
static void usage()
{
	version();
	printf("\nUsage: %s command [options] [arg1|arg2...]\n", app_name);
	printf("\nDescription:\n");
	printf("  The %s is client of baidu net disk. It supplied many functions, \n"
		"  which can manage baidu net disk on terminal, such as ls, cp, rm, \n"
		"  mv, rename, download, upload, search and so on. \n"
		"  The %s provided AES encryption, which can protected your data.\n", 
		app_name, app_name, app_name);
	printf("\nOptions:\n");
	printf("  --context=<file path>  Specify context.\n");
	printf("\nCommands:\n"
		"  cat      Print the file content\n"
		"  cd       Change the work directory\n"
		"  copy     Copy the file|directory\n"
		"  compare  Print the differents between local and net disk\n"
		"  context  Print the context\n"
		"  download Download the file\n"
		"  echo     Write the text into net disk file\n"
		"  encode   Encrypt/decrypt the file\n"
		"  help     Print the usage\n"
		"  list     List the directory\n"
		"  login    Login\n"
		"  logout   Logout\n"
		"  meta     Print the file|directory meta information\n"
		"  mkdir    Make a new directory\n"
		"  move     Move the file|directory into other file|directory\n"
		"  pwd      Print the current work directory\n"
		"  quota    Print the quota\n"
		"  remove   Remove the file|directory\n"
		"  rename   Rename the file|directory\n"
		"  set      Change the context, you can print the context by 'context' command\n"
		"  search   Search the files in the specify directory\n"
		"  synch    Synch between local and net disk. You can 'compare' first.\n"
		"  upload   Upload the file\n"
		"  version  Print the version\n"
		"  who      Print the current user\n"
		);
	printf("Use '%s <command> -h' to print the details of the command. \n", app_name);
	printf("Sample: \n");
	printf("  %s help\n", app_name);
	printf("  %s cat /note.txt\n", app_name);
	printf("  %s cd /temp\n", app_name);
	printf("  %s cat /note.txt --context=/home/gang/.pcs_context\n", app_name);
}

#pragma endregion

#pragma region 'set'命令的分支函数

/*设置上下文中的cookiefile值*/
static int set_cookiefile(ShellContext *context, const char *val)
{
	if (!val || !val[0]) return -1;
	if (streq(context->cookiefile, val, -1)) return 0;
	if (context->cookiefile) pcs_free(context->cookiefile);
	context->cookiefile = pcs_utils_strdup(val);
	if (context->pcs) {
		pcs_destroy(context->pcs);
		context->pcs = NULL;
		init_pcs(context);
	}
	return 0;
}

/*设置上下文中的captchafile值*/
static int set_captchafile(ShellContext *context, const char *val)
{
	if (!val || !val[0]) return -1;
	if (streq(context->captchafile, val, -1)) return 0;
	if (context->captchafile) pcs_free(context->captchafile);
	context->captchafile = pcs_utils_strdup(val);
	return 0;
}

/*设置上下文中的list_page_size值*/
static int set_list_page_size(ShellContext *context, const char *val)
{
	const char *p = val;
	int v;
	if (!val || !val[0]) return -1;
	while (*p) {
		if (*p < '0' || *p > '9')
			return -1;
		p++;
	}
	v = atoi(val);
	if (v < 1) return -1;
	context->list_page_size = v;
	return 0;
}

/*设置上下文中的list_sort_name值*/
static int set_list_sort_name(ShellContext *context, const char *val)
{
	if (!val || !val[0]) return -1;
	if (strcmp(val, "name") && strcmp(val, "time") && strcmp(val, "size")) {
		return -1;
	}
	if (streq(context->list_sort_name, val, -1)) return 0;
	if (context->list_sort_name) pcs_free(context->list_sort_name);
	context->list_sort_name = pcs_utils_strdup(val);
	return 0;
}

/*设置上下文中的list_sort_direction值*/
static int set_list_sort_direction(ShellContext *context, const char *val)
{
	if (!val || !val[0]) return -1;
	if (strcmp(val, "asc") && strcmp(val, "desc")) {
		return -1;
	}
	if (streq(context->list_sort_direction, val, -1)) return 0;
	if (context->list_sort_direction) pcs_free(context->list_sort_direction);
	context->list_sort_direction = pcs_utils_strdup(val);
	return 0;
}

/*设置上下文中的secure_method值*/
static int set_secure_method(ShellContext *context, const char *val)
{
	if (!val || !val[0]) return -1;
	if (strcmp(val, "plaintext") && strcmp(val, "aes-cbc-128") && strcmp(val, "aes-cbc-192") && strcmp(val, "aes-cbc-256")) {
		return -1;
	}
	if (streq(context->secure_method, val, -1)) return 0;
	if (context->secure_method) pcs_free(context->secure_method);
	context->secure_method = pcs_utils_strdup(val);

	if (context->pcs) {
		init_pcs_secure(context);
	}
	return 0;
}

/*设置上下文中的secure_key值*/
static int set_secure_key(ShellContext *context, const char *val)
{
	if (!val || !val[0]) return -1;
	if (streq(context->secure_key, val, -1)) return 0;
	if (context->secure_key) pcs_free(context->secure_key);
	context->secure_key = pcs_utils_strdup(val);

	if (context->pcs) {
		init_pcs_secure(context);
	}
	return 0;
}

/*设置上下文中的secure_enable值*/
static int set_secure_enable(ShellContext *context, const char *val)
{
	if (!val || !val[0]) return -1;
	if (strcmp(val, "true") == 0 || strcmp(val, "1") == 0) {
		context->secure_enable = 1;
	}
	else if (strcmp(val, "false") == 0 || strcmp(val, "0") == 0) {
		context->secure_enable = 0;
	}
	else {
		return -1;
	}
	if (context->pcs) {
		init_pcs_secure(context);
	}
	return 0;
}

#pragma endregion

#pragma region Meta 相关方法

static void meta_destroy(MyMeta *meta);

#pragma region 红黑树的几个回调函数

/*红黑树中用于释放 key */
static void rb_destory_key(void *a, void *state)
{

}

/*红黑树中用于释放 info */
static void rb_destory_info(void *a, void *state)
{
	if (a) meta_destroy((MyMeta *)a);
}

/*红黑树中用于比较 key 值。当 *a > *b 时，返回 1; 当 *a < *b 时，返回 -1; 当相等时，返回 0。 */
static int rb_compare(const void *a, const void *b, void *state)
{
	int rc;
	if (!a && !b) return 0;
	if (!a) return -1;
	if (!b) return 1;
	rc = pcs_utils_strcmpi(a, b);
	return (rc < 0 ? -1 : (rc > 0 ? 1 : 0));
}

/*红黑树中用于打印 key */
static void rb_print_key(const void *a, void *state)
{
	printf("%s", a);
}

/*红黑树中用于打印 info */
static void rb_print_info(void *a, void *state)
{
	MyMeta *meta = (MyMeta *)a;
	if (!meta) {
		printf("null\n");
		return;
	}
	printf("{\n");

	printf("  path: \"%s\",\n", meta->path);

	printf("  local_exist: %s,\n", (meta->flag & FLAG_ON_LOCAL) ? "true" : "false");
	print_time("  local_mtime: \"%s\",\n", meta->local_mtime);
	printf("  local_isdir: %s,\n", meta->local_isdir ? "true" : "false");
	
	printf("  remote_path: \"%s\",\n", meta->remote_path);
	printf("  remote_exist: %s,\n", (meta->flag & FLAG_ON_REMOTE) ? "true" : "false");
	print_time("  remote_mtime: \"%s\",\n", meta->remote_mtime);
	printf("  remote_isdir: %s\n", meta->remote_isdir ? "true" : "false");

	printf("}\n");
}

#pragma endregion

/*创建一个MyMeta*/
static MyMeta *meta_create(const char *path)
{
	MyMeta *meta;
	meta = (MyMeta *)pcs_malloc(sizeof(MyMeta));
	memset(meta, 0, sizeof(MyMeta));
	if (path) {
		meta->path = pcs_utils_strdup(path);
	}
	return meta;
}

/*释放掉一个MyMeta*/
static void meta_destroy(MyMeta *meta)
{
	if (!meta) return;
	if (meta->path) pcs_free(meta->path);
	if (meta->remote_path) pcs_free(meta->remote_path);
	if (meta->msg) pcs_free(meta->msg);
	pcs_free(meta);
}

/*meta_load()函数中当获取到一个文件后的回调函数*/
static void onGotLocalFile(LocalFileInfo *info, LocalFileInfo *parent, void *state)
{
	struct ScanLocalFileState *st = (struct ScanLocalFileState *)state;
	rb_red_blk_tree *rb = st->rb;
	MyMeta *meta;
	if (!info->isdir && endsWith(info->path, TEMP_FILE_SUFFIX)) {
		return;
	}
	fix_unix_path(info->path);
	meta = meta_create(info->path);
	meta->flag |= FLAG_ON_LOCAL;
	meta->local_mtime = info->mtime;
	meta->local_isdir = info->isdir;
	meta->local_filecount = info->filecount;
	meta->parent = (parent ? (MyMeta *)parent->userdata : NULL);
	info->userdata = meta;
	RBTreeInsert(rb, (void *)meta->path, (void *)meta);
	st->total++;
	printf("Scanned %d                     \r", st->total);
	fflush(stdout);
}

/*
 * 从本地文件系统的目录树中创建<MyMeta>对象，并存入红黑树中
 * 返回红黑树对象
*/
static rb_red_blk_tree *meta_load(const char *dir, int recursive)
{
	rb_red_blk_tree *rb = NULL;
	LocalFileInfo *link = NULL;
	int cnt = 0;
	struct ScanLocalFileState state = { 0 };

	rb = RBTreeCreate(&rb_compare, &rb_destory_key, &rb_destory_info, &rb_print_key, &rb_print_info);

	state.rb = rb;
	state.total = 0;
	cnt = GetDirectoryFiles(&link, dir, recursive, &onGotLocalFile, &state);
	if (cnt < 0) {
		RBTreeDestroy(rb);
		return NULL;
	}
	if (cnt > 0) putchar('\n');
	if (link)
		DestroyLocalFileInfoLink(link);
	return rb;
}

/*
* 决定文件执行何种操作。
*   1) 当文件在本地和网盘中都存在时，比较本地和网盘的元数据。
*         a) 如果本地文件修改时间小于网盘文件创建时间，则需要下载
*         b) 如果本地文件修改时间大于网盘文件创建时间，则需要上传
*         c) 如果本地文件修改时间等于网盘文件创建时间，则不需做任何操作
*  2) 当文件在本地存在，但是在网盘中不存在时，则需要上传
*  3) 当文件在本地不存在，但是在网盘中存在时，则需要下载
*  4) 其他情况，不需做任何操作。
*/
static inline void decide_op(MyMeta *meta)
{
	if ((meta->flag & FLAG_ON_LOCAL) && (meta->flag & FLAG_ON_REMOTE)) { /*文件在本地和网盘中都存在*/
		if (meta->local_isdir && meta->remote_isdir) {
			meta->op = OP_EQ;
		}
		else if (!meta->local_isdir && !meta->remote_isdir) {
			if (meta->local_mtime < meta->remote_mtime)
				meta->op = OP_LEFT;
			else if (meta->local_mtime > meta->remote_mtime)
				meta->op = OP_RIGHT;
			else
				meta->op = OP_EQ;
		}
		else {
			meta->op = OP_CONFUSE;
		}
	}
	else if (meta->flag & FLAG_ON_LOCAL) {
		meta->op = OP_RIGHT;
	}
	else if (meta->flag & FLAG_ON_REMOTE) {
		meta->op = OP_LEFT;
	}
	else {
		meta->op = OP_NONE;
	}
}

/*
 * 打印显示meta时的列表头
 *   first  - 第一列宽度，第一列为操作成功还是失败的标记列。不存在时，传入0
 *   second - 第二列宽度，第二列为本地文件的路径
 *   other  - 剩下列的总宽度，不包括第三列。第三列宽度为固定值2
 *   page_index - 分页编号，不打印分页的话，传入0
*/
static void print_meta_list_head(int first, int second, int other)
{
	int i, total = 0;
	for (i = 0; i < first; i++) putchar(' ');
	if (first > 0) putchar(' ');
	printf("Local File");
	for (i = 10; i < second; i++) putchar(' ');
	printf(" OP Net Disk File\n");
	if (first > 0) total += first + 1;
	total += second + 1;
	total += 2 + 1; /*2为列头"OP"的长度，1为分隔空白*/
	total += other;
	for (i = 0; i < total; i++) putchar('-');
	putchar('\n');
}


/*
* 打印显示meta时的列表头
*   first  - 第一列宽度，第一列为操作成功还是失败的标记列。不存在时，传入0
*   second - 第二列宽度，第二列为本地文件的路径
*   other  - 剩下列的总宽度，不包括第三列。第三列宽度为固定值2
*   meta   - 待打印的 meta 
*/
static void print_meta_list_row(int first, int second, int other, MyMeta *meta)
{
	int i;
	if (first > 0) {
		switch (meta->op_st) {
		case OP_ST_SUCC:
			printf("[" GREEN " ok " NONE "] ");
			break;
		case OP_ST_FAIL:
			printf("[" RED "fail" NONE "] ");
			break;
		case OP_ST_SKIP:
			printf("[" WHITE "skip" NONE "] ");
			break;
		case OP_ST_CONFUSE:
			printf("[" YELLOW "confus" NONE "] ");
			break;
		case OP_ST_PROCESSING:
			printf("[ing...] ");
			break;
		default:
			printf("[    ] ");
			break;
		}
	}
	if (meta->flag & FLAG_ON_LOCAL) {
		printf("%s", meta->path);
		i = strlen(meta->path);
		if (meta->local_isdir && i > 0 && meta->path[i - 1] != '/' && meta->path[i - 1] != '\\') {
			putchar('/');
			i++;
		}
	}
	else {
		i = 0;
	}
	for (; i < second; i++) putchar(' ');
	putchar(' ');
	switch (meta->op) {
	case OP_LEFT:
		printf(BLUE"<-"NONE);
		break;
	case OP_RIGHT:
		printf(LIGHT_BLUE"->"NONE);
		break;
	case OP_EQ:
		printf(GREEN"=="NONE);
		break;
	case OP_CONFUSE:
		printf(RED"><"NONE);
		break;
	default:
		printf("  ");
		break;
	}
	putchar(' ');
	if (meta->flag & FLAG_ON_REMOTE) {
		printf("%s", meta->remote_path);
		i = strlen(meta->remote_path);
		if (meta->remote_isdir && i > 0 && meta->remote_path[i - 1] != '/' && meta->remote_path[i - 1] != '\\') {
			putchar('/');
		}
	}
	putchar('\n');
	//if (meta->msg) {
	//	fprintf(stderr, RED "Error: %s" NONE "\n", meta->msg);
	//}
}

static void print_meta_list_row_err(int first, int second, int other, MyMeta *meta)
{
	int i;
	if (first > 0) {
		switch (meta->op_st) {
		case OP_ST_SUCC:
			fprintf(stderr, "[" GREEN " ok " NONE "] ");
			break;
		case OP_ST_FAIL:
			fprintf(stderr, "[" RED "fail" NONE "] ");
			break;
		case OP_ST_SKIP:
			fprintf(stderr, "[" WHITE "skip" NONE "] ");
			break;
		case OP_ST_CONFUSE:
			fprintf(stderr, "[" YELLOW "confus" NONE "] ");
			break;
		case OP_ST_PROCESSING:
			fprintf(stderr, "[ing...] ");
			break;
		default:
			fprintf(stderr, "[    ] ");
			break;
		}
	}
	if (meta->flag & FLAG_ON_LOCAL) {
		fprintf(stderr, "%s", meta->path);
		i = strlen(meta->path);
		if (meta->local_isdir && i > 0 && meta->path[i - 1] != '/' && meta->path[i - 1] != '\\') {
			fprintf(stderr, "/");
			i++;
		}
	}
	else {
		i = 0;
	}
	for (; i < second; i++) fprintf(stderr, " ");
	fprintf(stderr, " ");
	switch (meta->op) {
	case OP_LEFT:
		fprintf(stderr, BLUE"<-"NONE);
		break;
	case OP_RIGHT:
		fprintf(stderr, LIGHT_BLUE"->"NONE);
		break;
	case OP_EQ:
		fprintf(stderr, GREEN"=="NONE);
		break;
	case OP_CONFUSE:
		fprintf(stderr, RED"><"NONE);
		break;
	default:
		fprintf(stderr, "  ");
		break;
	}
	fprintf(stderr, " ");
	if (meta->flag & FLAG_ON_REMOTE) {
		fprintf(stderr, "%s", meta->remote_path);
		i = strlen(meta->remote_path);
		if (meta->remote_isdir && i > 0 && meta->remote_path[i - 1] != '/' && meta->remote_path[i - 1] != '\\') {
			fprintf(stderr, "/");
		}
	}
	if (meta->msg) {
		fprintf(stderr, "\n");
		fprintf(stderr, RED "      %s" NONE, meta->msg);
	}
	fprintf(stderr, "\n");
}

/*
* 打印统计
*/
static void print_meta_list_statistic(struct RBEnumerateState *s, int print_fail)
{
	int i, total = 0;

	printf("\nStatistic:\n");
	if (s->first > 0) total += s->first + 1;
	total += s->second + 1;
	total += 2 + 1; /*2为列头"OP"的长度，1为分隔空白*/
	total += s->other;
	for (i = 0; i < total; i++) putchar('-');
	putchar('\n');
	printf("Need Download: %d, Need Upload: %d\n"
		   "Confuse: %d, Equal: %d, Other: %d\n"
		   "Total: %d",
		s->cnt_left, s->cnt_right, s->cnt_confuse, s->cnt_eq, s->cnt_none, s->cnt_total);
	if (print_fail) {
		printf(", Fail: %d\n", s->cnt_fail);
	}
	else {
		putchar('\n');
	}
}

/*
* 打印显示meta时的列表头
*   first  - 第一列宽度，第一列为操作成功还是失败的标记列。不存在时，传入0
*   second - 第二列宽度，第二列为本地文件的路径
*   other  - 剩下列的总宽度，不包括第三列。第三列宽度为固定值2
*/
static void print_meta_list_notes(int first, int second, int other)
{
	printf("\nNotes:\n"
		"  <- means the right file will download into \n"
		"     the local file system. \n");
	printf("  -> means the left file will upload into the disk. \n");
	printf("  == means left file same as right file. \n");
	printf("  >< means confuse, don't known how to. \n");
}

/*判断是否允许打印。返回0表示不允许，返回非0值表示允许*/
static inline int rb_print_enabled(MyMeta *meta, struct RBEnumerateState *s)
{
	if (!(meta->op & s->print_op) || !((meta->op) & (~(s->no_print_op))))
		return 0;
	if (!(meta->flag & s->print_flag) || !((meta->flag) & (~(s->no_print_flag))))
		return 0;
	return 1;
}

/* 枚举时用于打印每一项 */
static int rb_print_meta(void *a, void *state)
{
	MyMeta *meta = (MyMeta *)a;
	struct RBEnumerateState *s = (struct RBEnumerateState *)state;
	char tmp[8];

	if (!meta || !s || !rb_print_enabled(meta, s)) return 0;

	if (s->printed_count == 0) {
		if (s->page_enable && s->page_size > 0)
			printf("\n%sPAGE #%d\n", s->prefixion ? s->prefixion : "", s->page_index);
		else
			putchar('\n');
		print_meta_list_head(s->first, s->second, s->other);
	}
	else if (s->page_enable && s->page_size > 0 && (s->printed_count % s->page_size) == 0) {
		s->page_index++;
		printf("\nPrint next page #%d?\n"
			   "  yes - Continue print\n"
			   "  no  - Abort print\n"
			   "  YES - Continue, but print all left items\n"
			   "Press <enter> to continue or input your choice: ", s->page_index);
		std_string(tmp, 8);
		if (strlen(tmp) != 0 && pcs_utils_strcmpi(tmp, "y") && pcs_utils_strcmpi(tmp, "yes")) {
			return -1;
		}
		if (strlen(tmp) != 0 && (strcmp(tmp, "Y") == 0 || strcmp(tmp, "YES") == 0)) {
			s->page_enable = 0;
		}
		printf("\n%sPAGE #%d\n", s->prefixion ? s->prefixion : "", s->page_index);
		print_meta_list_head(s->first, s->second, s->other);
	}

	if (s->process)
		meta->op_st = OP_ST_PROCESSING;

	print_meta_list_row(s->first, s->second, s->other, meta);
	
	if (s->process) {
		int rc = (*s->process)(meta, s, s->processState);
		clear_current_print_line();
		back_prev_print_line();
		if (meta->op_st == OP_ST_PROCESSING) meta->op_st = OP_ST_FAIL;
		if (meta->op_st == OP_ST_FAIL) {
			fprintf(stderr, "\033[K");  //清除该行
			fprintf(stderr, "\033[1A"); //先回到上一行
			fprintf(stderr, "\033[K");  //清除该行
			print_meta_list_row_err(s->first, s->second, s->other, meta);
		}
		else {
			print_meta_list_row(s->first, s->second, s->other, meta);
		}
		if (rc) return rc;
	}

	if (meta->op_st == OP_ST_FAIL)
		s->cnt_fail++;

	s->printed_count++;

	return 0;
}

/* 枚举决定红黑树中项的op值，同时也决定flag中的FLAG_PARENT_NOT_ON_REMOTE标记，同时统计一些值 */
static int rb_decide_op(void *a, void *state)
{
	MyMeta *meta = (MyMeta *)a;
	struct RBEnumerateState *s = (struct RBEnumerateState *)state;
	int len;

	if (!meta || !s) return 0;
	
	decide_op(meta);
	meta->op_st = OP_ST_NONE;

	if (meta->parent) {
		if (!(meta->parent->flag & FLAG_ON_REMOTE))
			meta->flag |= FLAG_PARENT_NOT_ON_REMOTE;
	}

	switch (meta->op) {
	case OP_EQ:
		s->cnt_eq++;
		break;
	case OP_LEFT:
		s->cnt_left++;
		break;
	case OP_RIGHT:
		s->cnt_right++;
		break;
	case OP_CONFUSE:
		s->cnt_confuse++;
		break;
	default:
		s->cnt_none++;
		break;
	}
	s->cnt_total++;

	if (!rb_print_enabled(meta, s)) return 0;

	if ((meta->flag & FLAG_ON_LOCAL)) {
		len = strlen(meta->path);
		if (meta->local_isdir) len++;
		if (s->second < len) s->second = len;
	}
	s->cnt_valid_total++;
	printf("Compared %d                     \r", s->cnt_total);
	fflush(stdout);
	return 0;
}

#pragma endregion

#pragma region 各命令函数体

/*
 * 检查是否登录
 *   msg   - 检测到未登录时的打印消息。传入NULL的话，则使用默认消息。
*/
static PcsBool is_login(ShellContext *context, const char *msg)
{
	PcsRes pcsres;
	time_t now;
	time(&now);
	pcsres = pcs_islogin(context->pcs);
	if (pcsres == PCS_LOGIN)
		return PcsTrue;
	if (msg) {
		if (msg[0])
			printf("%s\n", msg);
	}
	else {
		printf("You are not logon or your session is time out. You can login by 'login' command.\n");
	}
	return PcsFalse;
}

/*
 * 执行下载操作
 *   context        - 上下文
 *   local_file     - 文件本地存储路径
 *   remote_file    - 远端文件
 *   remote_mtime   - 远端文件的最后修改时间
 *   local_basedir  - 本地存储路径是基于哪一个目录的
 *   remote_basedir - 远端文件是基于哪一个目录的
 *   pErrMsg        - 如果下载失败时，用于接收失败消息，如果无需失败消息，则传入NULL
 *                    使用完后需调用pcs_free()
 *   op_st          - 用于接收操作状态的。即 OP_ST_FAIL， OP_ST_SUCC， OP_ST_SKIP
 * 成功后返回0，失败后返回非0值
 */
static inline int do_download(ShellContext *context, 
	const char *local_file, const char *remote_file, time_t remote_mtime,
	const char *local_basedir, const char *remote_basedir,
	char **pErrMsg, int *op_st)
{
	PcsRes res;
	struct DownloadState ds = { 0 };
	char *local_path, *remote_path, *dir,
		*tmp_local_path;

	local_path = combin_path(local_basedir, -1, local_file);

	/*创建目录*/
	dir = base_dir(local_path, -1);
	if (dir) {
		if (CreateDirectoryRecursive(dir) != MKDIR_OK) {
			if (pErrMsg) {
				if (*pErrMsg) pcs_free(*pErrMsg);
				(*pErrMsg) = pcs_utils_sprintf("Error: Can't create the directory: %s", dir);
			}
			if (op_st) (*op_st) = OP_ST_FAIL;
			pcs_free(dir);
			pcs_free(local_path);
			return -1;
		}
		pcs_free(dir);
	}

	tmp_local_path = (char *)pcs_malloc(strlen(local_path) + strlen(TEMP_FILE_SUFFIX) + 1);
	strcpy(tmp_local_path, local_path);
	strcat(tmp_local_path, TEMP_FILE_SUFFIX);

	/*打开文件*/
	ds.pf = fopen(tmp_local_path, "wb");
	if (!ds.pf) {
		if (pErrMsg) {
			if (*pErrMsg) pcs_free(*pErrMsg);
			(*pErrMsg) = pcs_utils_sprintf("Error: Can't open or create the temp file: %s\n", tmp_local_path);
		}
		if (op_st) (*op_st) = OP_ST_FAIL;
		DeleteFileRecursive(tmp_local_path);
		pcs_free(tmp_local_path);
		pcs_free(local_path);
		return -1;
	}

	dir = combin_net_disk_path(context->workdir, remote_basedir);
	if (!dir) {
		if (pErrMsg) {
			if (*pErrMsg) pcs_free(*pErrMsg);
			(*pErrMsg) = pcs_utils_sprintf("Error: Can't combin remote path\n");
		}
		if (op_st) (*op_st) = OP_ST_FAIL;
		DeleteFileRecursive(tmp_local_path);
		pcs_free(tmp_local_path);
		pcs_free(local_path);
		return -1;
	}
	remote_path = combin_net_disk_path(dir, remote_file);
	pcs_free(dir);
	if (!dir) {
		if (pErrMsg) {
			if (*pErrMsg) pcs_free(*pErrMsg);
			(*pErrMsg) = pcs_utils_sprintf("Error: Can't combin remote path\n");
		}
		if (op_st) (*op_st) = OP_ST_FAIL;
		DeleteFileRecursive(tmp_local_path);
		pcs_free(tmp_local_path);
		pcs_free(local_path);
		return -1;
	}

	/*启动下载*/
	pcs_setopts(context->pcs,
		PCS_OPTION_DOWNLOAD_WRITE_FUNCTION, &download_write,
		PCS_OPTION_DOWNLOAD_WRITE_FUNCTION_DATA, &ds,
		//PCS_OPTION_TIMEOUT, (void *)((long)(60 * 60)),
		PCS_OPTION_END);
	res = pcs_download(context->pcs, remote_path);
	fclose(ds.pf);
	//pcs_setopts(context->pcs,
	//	PCS_OPTION_TIMEOUT, (void *)((long)TIMEOUT),
	//	PCS_OPTION_END);
	if (res != PCS_OK) {
		if (pErrMsg) {
			if (*pErrMsg) pcs_free(*pErrMsg);
			(*pErrMsg) = pcs_utils_sprintf("Error: %s. local_path=%s, remote_path=%s\n", 
				pcs_strerror(context->pcs), tmp_local_path, remote_path);
		}
		if (op_st) (*op_st) = OP_ST_FAIL;
		DeleteFileRecursive(tmp_local_path);
		pcs_free(tmp_local_path);
		pcs_free(local_path);
		pcs_free(remote_path);
		return -1;
	}

	DeleteFileRecursive(local_path);
	if (rename(tmp_local_path, local_path)) {
		if (pErrMsg) {
			if (*pErrMsg) pcs_free(*pErrMsg);
			(*pErrMsg) = pcs_utils_sprintf("Error: The file have been download at %s, but can't rename to %s.\n"
				" You should be rename manual.", tmp_local_path, local_path);
		}
		if (op_st) (*op_st) = OP_ST_FAIL;
		//DeleteFileRecursive(tmp_local_path);
		pcs_free(tmp_local_path);
		pcs_free(local_path);
		pcs_free(remote_path);
		return -1;
	}

	/*设置文件最后修改时间为网盘文件最后修改时间*/
	SetFileLastModifyTime(local_path, remote_mtime);
	if (op_st) (*op_st) = OP_ST_SUCC;
	pcs_free(tmp_local_path);
	pcs_free(local_path);
	pcs_free(remote_path);
	return 0;
}

static inline int do_upload(ShellContext *context, 
	const char *local_file, const char *remote_file, PcsBool is_force,
	const char *local_basedir, const char *remote_basedir,
	char **pErrMsg, int *op_st)
{
	PcsFileInfo *res = NULL;
	char *local_path, *remote_path, *dir;

	local_path = combin_path(local_basedir, -1, local_file);
	dir = combin_net_disk_path(context->workdir, remote_basedir);
	remote_path = combin_net_disk_path(dir, remote_file);
	pcs_free(dir);
	pcs_setopts(context->pcs,
		PCS_OPTION_PROGRESS_FUNCTION, &upload_progress,
		PCS_OPTION_PROGRESS_FUNCTION_DATE, NULL,
		PCS_OPTION_PROGRESS, (void *)((long)PcsTrue),
		//PCS_OPTION_TIMEOUT, (void *)0L,
		PCS_OPTION_END);
	res = pcs_upload(context->pcs, remote_path, is_force, local_path);
	//pcs_setopts(context->pcs,
	//	PCS_OPTION_TIMEOUT, (void *)((long)TIMEOUT),
	//	PCS_OPTION_END);
	if (!res || !res->path || !res->path[0]) {
		if (pErrMsg) {
			if (*pErrMsg) pcs_free(*pErrMsg);
			(*pErrMsg) = pcs_utils_sprintf("Error: %s. local_path=%s, remote_path=%s\n", 
				pcs_strerror(context->pcs), local_path, remote_path);
		}
		if (op_st) (*op_st) = OP_ST_FAIL;
		if (res) pcs_fileinfo_destroy(res);
		pcs_free(local_path);
		pcs_free(remote_path);
		return -1;
	}
	/*当文件名以.(点号)开头的话，则网盘会自动去除第一个点。以下if语句的目的就是把网盘文件重命名为以点号开头。*/
	if (res) {
		char *diskName = pcs_utils_filename(res->path),
			*orgName = pcs_utils_filename(remote_file);
		if (diskName && orgName && orgName[0] == '.' && diskName[0] != '.') {
			PcsPanApiRes *res2;
			PcsSList2 sl = {
				res->path,
				orgName,
				NULL
			};
			PcsSList sl2 = {
				res->path, NULL
			};
			pcs_free(orgName);
			orgName = (char *)malloc(strlen(diskName) + 2);
			orgName[0] = '.';
			strcpy(&orgName[1], diskName);
			while (1) {
				sl.string2 = orgName;
				//printf("\nrename %s -> %s \n", sl.string1, sl.string2);
				res2 = pcs_rename(context->pcs, &sl);
				//printf("\nrename %s -> %s %d \n", sl.string1, sl.string2, res2->error);
				if (res2 && res2->error == 0) {
					pcs_pan_api_res_destroy(res2);
					res2 = NULL;
					break;
				}
				else {
					if (res2) { pcs_pan_api_res_destroy(res2); res2 = NULL; }
					if (is_force) {
						sl2.string = remote_path;
						//printf("\ndelete %s \n", sl2.string);
						res2 = pcs_delete(context->pcs, &sl2);
						//printf("\ndelete %s %d \n", sl2.string, res2->error);
						if (!res2 || res2->error != 0) {
							if (pErrMsg) {
								if (*pErrMsg) pcs_free(*pErrMsg);
								(*pErrMsg) = pcs_utils_sprintf("Error: Can't delete the %s, so can't rename %s to %s. You can rename manually.\n",
									remote_path, sl.string1, sl.string2);
							}
							if (res2) { pcs_pan_api_res_destroy(res2); res2 = NULL; }
							break;
						}
						if (res2) { pcs_pan_api_res_destroy(res2); res2 = NULL; }
					}
					else {
						if (pErrMsg) {
							if (*pErrMsg) pcs_free(*pErrMsg);
							(*pErrMsg) = pcs_utils_sprintf("Error: Can't rename %s to %s. You can rename manually.\n",
								sl.string1, sl.string2);
						}
						break;
					}
				}
			}
		}
		if (diskName) pcs_free(diskName);
		if (orgName) pcs_free(orgName);
		//print_fileinfo(res, " ");
	}
	if (op_st) (*op_st) = OP_ST_SUCC;
	if (res) pcs_fileinfo_destroy(res);
	pcs_free(local_path);
	pcs_free(remote_path);
	return 0;
}

/*打印网盘文件内容*/
static int cmd_cat(ShellContext *context, struct args *arg)
{
	char *path;
	const char *res;
	size_t sz;
	if (test_arg(arg, 1, 1, "h", "help", NULL)) {
		usage_cat();
		return -1;
	}
	if (has_opts(arg, "h","help", NULL)) {
		usage_cat();
		return 0;
	}
	if (!is_login(context, NULL)) return -1;
	path = combin_net_disk_path(context->workdir, arg->argv[0]);
	assert(path);
	if (strcmp(path, "/") == 0) {
		fprintf(stderr, "Error: Can't cat root directory.");
		pcs_free(path);
		return -1;
	}
	res = pcs_cat(context->pcs, path, &sz);
	if (res == NULL) {
		fprintf(stderr, "Error: %s path=%s.\n", pcs_strerror(context->pcs), path);
		pcs_free(path);
		return -1;
	}
	pcs_free(path);
	printf("%s\n\n", res);
	return 0;
}

/*更改网盘当前工作目录*/
static int cmd_cd(ShellContext *context, struct args *arg)
{
	char *p;
	PcsFileInfo *meta;
	if (test_arg(arg, 1, 1, "h", "help", NULL)) {
		usage_cd();
		return -1;
	}
	if (has_opts(arg, "h", "help", NULL)) {
		usage_cd();
		return 0;
	}
	if (!is_login(context, NULL)) return -1;
	p = combin_net_disk_path(context->workdir, arg->argv[0]);
	if (strcmp(p, "/")) {
		meta = pcs_meta(context->pcs, p);
		if (!meta) {
			fprintf(stderr, "Error: The target directory not exist, or have error: %s\n", pcs_strerror(context->pcs));
			pcs_free(p);
			return -1;
		}
		if (!meta->isdir) {
			fprintf(stderr, "Error: The target is not directory\n");
			pcs_free(p);
			pcs_fileinfo_destroy(meta);
			return -1;
		}
		pcs_fileinfo_destroy(meta);
	}
	if (context->workdir) pcs_free(context->workdir);
	context->workdir = pcs_utils_strdup(p);
	pcs_free(p);
	printf("Work directory changed to %s\n", context->workdir);
	return 0;
}

/*复制网盘文件*/
static int cmd_copy(ShellContext *context, struct args *arg)
{
	PcsPanApiRes *res;
	PcsPanApiResInfoList *info;
	PcsSList2 slist;
	if (test_arg(arg, 2, 2, "h", "help", NULL)) {
		usage_copy();
		return -1;
	}
	if (has_opts(arg, "h", "help", NULL)) {
		usage_copy();
		return 0;
	}
	if (!is_login(context, NULL)) return -1;

	slist.string1 = combin_net_disk_path(context->workdir, arg->argv[0]); /* path */
	slist.string2 = combin_net_disk_path(context->workdir, arg->argv[1]); /* new_name */
	slist.next = NULL;
	if (strcmp(slist.string1, "/") == 0) {
		fprintf(stderr, "Error: Can't copy root directory.");
		pcs_free(slist.string1);
		pcs_free(slist.string2);
		return -1;
	}
	if (strcmp(slist.string2, "/") == 0) {
		fprintf(stderr, "Error: The new name can't be root directory.");
		pcs_free(slist.string1);
		pcs_free(slist.string2);
		return -1;
	}

	res = pcs_copy(context->pcs, &slist);
	if (!res) {
		fprintf(stderr, "Error: %s src=%s, dst=%s.\n", pcs_strerror(context->pcs), slist.string1, slist.string2);
		pcs_free(slist.string1);
		pcs_free(slist.string2);
		return -1;
	}
	info = res->info_list;
	if (info->info.error) {
		fprintf(stderr, "Error: unknow. src=%s, dst=%s. \n", slist.string1, slist.string2);
		pcs_pan_api_res_destroy(res);
		pcs_free(slist.string1);
		pcs_free(slist.string2);
		return -1;
	}
	printf("Copy %s to %s Success.\n", slist.string1, slist.string2);
	pcs_pan_api_res_destroy(res);
	pcs_free(slist.string1);
	pcs_free(slist.string2);
	return 0;
}

#pragma region cmd_compare

typedef struct compare_arg compare_arg;
struct compare_arg
{
	int			recursive;		/*是否递归*/
	int			print_eq;		/*是否打印未改动的文件*/
	int			print_left;		/*是否打印需下载的文件*/
	int			print_right;	/*是否打印需上传的文件*/
	int			print_confuse;	/*是否打印无法确定是下载还是上传的文件*/
	int			dry_run;		/*用于演示，不执行任何上传和下载操作*/

	const char	*local_file;	/*本地路径*/
	const char	*remote_file;	/*远端路径*/

	int			check_local_dir_exist;

	/*当State准备好后调用一次本方法*/
	void (*onRBEnumerateStatePrepared)(ShellContext *context, compare_arg *arg, rb_red_blk_tree *rb, struct RBEnumerateState *state, void *st);
};

/*
 * 解析compare的参数。
 * 成功返回0；失败返回非0值
*/
static int parse_compare_args(struct args *g, compare_arg *cmpArg)
{
	cmpArg->print_confuse = has_opt(g, "c");
	cmpArg->print_left = has_opt(g, "d");
	cmpArg->print_eq = has_opt(g, "e");
	cmpArg->dry_run = has_opt(g, "n");
	cmpArg->recursive = has_opt(g, "r");
	cmpArg->print_right = has_opt(g, "u");

	cmpArg->local_file = g->argv[0];
	cmpArg->remote_file = g->argv[1];
	
	return 0;
}

/*
* 比较两个文件的异同。
*   local       - 本地文件对象
*   remote		- 网盘文件对象
*   skip        - 目录跳过的字节数。
*/
static MyMeta *compare_file(const LocalFileInfo *local, const PcsFileInfo *remote)
{
	MyMeta *meta = NULL;

	meta = meta_create(local->path);
	meta->flag |= FLAG_ON_LOCAL;
	meta->local_mtime = local->mtime;
	meta->local_isdir = local->isdir;

	if (remote) {
		meta->flag |= FLAG_ON_REMOTE;
		meta->remote_path = pcs_utils_strdup(remote->path);
		meta->remote_mtime = remote->server_mtime;
		meta->remote_isdir = remote->isdir;
	}

	decide_op(meta);

	return meta;
}

/*
* 列出网盘目录文件，并把结果合并到代表本地文件元数据的红黑树中。
*   context     - 上下文
*   rb          - 自己维护的一个文件元数据
*   remote_dir  - 网盘文件对象
*   recursive   - 表示是否递归
*   skip        - 从什么位置开始截取路径，截取后的路径作为红黑树中项的Key
*   total_cnt   - 用于统计
*   check_local_dir_exist - 如果传入非0值的话，
*                     将判断网盘目录在本地是否存在，只有存在时，才会继续加载其下文件和目录
* 成功则返回0；否则返回非0值
*/
static int combin_with_remote_dir_files(ShellContext *context, rb_red_blk_tree *rb,
	const char *remote_dir, int recursive, int skip, int *total_cnt, int check_local_dir_exist)
{
	PcsFileInfoList *list = NULL;
	PcsFileInfoListIterater iterater;
	PcsFileInfo *info = NULL;
	int page_index = 1,
		page_size = 1000;
	int cnt = 0, second;
	rb_red_blk_node *rbn;
	MyMeta *meta;

	while (1) {
		list = pcs_list(context->pcs, remote_dir,
			page_index, page_size,
			"name", PcsFalse);
		if (!list) {
			if (pcs_strerror(context->pcs)) {
				fprintf(stderr, "Error: %s \n", pcs_strerror(context->pcs));
				if (context->timeout_retry && strstr(pcs_strerror(context->pcs), "Can't get response from the remote server") >= 0) {
					second = 10;
					while (second > 0) {
						printf("Retry after %d second...\n", second);
						sleep(1);
						second--;
					}
					//printf("Retrying...\n");
					continue;
				}
				return -1;
			}
			return 0;
		}

		cnt = list->count;
		if (total_cnt) (*total_cnt) += cnt;
		if (total_cnt && cnt > 0) {
			printf("Fetch %d                     \r", *total_cnt);
			fflush(stdout);
		}

		pcs_filist_iterater_init(list, &iterater, PcsFalse);
		while (pcs_filist_iterater_next(&iterater)) {
			info = iterater.current;
			rbn = RBExactQuery(rb, (void *)(info->path + skip));
			if (rbn) {
				meta = (MyMeta *)rbn->info;
				meta->flag |= FLAG_ON_REMOTE;
				if (meta->remote_path) pcs_free(meta->remote_path);
				meta->remote_path = pcs_utils_strdup(info->path + skip);
				meta->remote_mtime = info->server_mtime;
				meta->remote_isdir = info->isdir;
			}
			else {
				meta = meta_create(info->path + skip);
				meta->flag |= FLAG_ON_REMOTE;
				meta->remote_path = pcs_utils_strdup(info->path + skip);
				meta->remote_mtime = info->server_mtime;
				meta->remote_isdir = info->isdir;
				RBTreeInsert(rb, (void *)meta->path, (void *)meta);
			}
		}

		if (recursive) {
			pcs_filist_iterater_init(list, &iterater, PcsFalse);
			while (pcs_filist_iterater_next(&iterater)) {
				info = iterater.current;
				if (info->isdir) {
					if (check_local_dir_exist) {
						rbn = RBExactQuery(rb, (void *)(info->path + skip));
						meta = (MyMeta *)rbn->info;
						if (meta->flag & FLAG_ON_LOCAL) {
							if (combin_with_remote_dir_files(context, rb, info->path, recursive, skip, total_cnt, check_local_dir_exist)) {
								pcs_filist_destroy(list); 
								return -1;
							}
						}
					}
					else if (combin_with_remote_dir_files(context, rb, info->path, recursive, skip, total_cnt, check_local_dir_exist)) {
						pcs_filist_destroy(list); 
						return -1;
					}
				}
			}
		}
		pcs_filist_destroy(list);
		if (cnt < page_size) {
			break;
		}
		page_index++;
	}
	return 0;
}

static int on_compared_file(ShellContext *context, compare_arg *arg, MyMeta *mm, void *state)
{
	int first, second, other;
	first = 0; second = strlen(mm->path); other = 13;
	if (second < 10) second = 10;
	print_meta_list_head(first, second, other);
	print_meta_list_row(first, second, other, mm);
	print_meta_list_notes(first, second, other);
	return 0;
}

static int on_compared_dir(ShellContext *context, compare_arg *arg, rb_red_blk_tree *rb, void *st)
{
	struct RBEnumerateState state = { 0 };
	int printed_count = 0;
	if (!arg->print_eq && !arg->print_left && !arg->print_right && !arg->print_confuse) {
		arg->print_left = arg->print_right = arg->print_confuse = 1;
	}
	if (arg->print_eq) state.print_op |= OP_EQ;
	if (arg->print_left) state.print_op |= OP_LEFT;
	if (arg->print_right) state.print_op |= OP_RIGHT;
	//if (arg->print_confuse) state.print_op |= OP_CONFUSE;
	state.print_flag = FLAG_ON_LOCAL | FLAG_ON_REMOTE;
	state.no_print_flag = FLAG_PARENT_NOT_ON_REMOTE;
	state.tree = rb;
	state.context = context;
	state.page_size = context->list_page_size;
	state.page_index = 1;
	state.page_enable = 1;
	state.dry_run = arg->dry_run;
	state.local_basedir = arg->local_file;
	state.remote_basedir = arg->remote_file;
	rb->enumerateInfoState = &state;
	rb->EnumerateInfo = &rb_decide_op;
	printf("Comparing...\n");
	RBTreeEnumerateInfo(rb);
	if (state.cnt_total > 0) putchar('\n');
	printf("Completed\n");
	state.first = 0;
	if (state.second < 10) state.second = 10;
	state.other = 13;
	if (arg->onRBEnumerateStatePrepared) {
		(*arg->onRBEnumerateStatePrepared)(context, arg, rb, &state, st);
	}
	else {
		printf("\nPrint Download: %s, Print Upload: %s, Print Confuse: %s, Print Equal: %s\n",
			arg->print_left ? "on" : "off",
			arg->print_right ? "on" : "off",
			arg->print_confuse ? "on" : "off",
			arg->print_eq ? "on" : "off");
	}
	rb->EnumerateInfo = &rb_print_meta;
	if (state.print_op && state.print_flag) {
		printf("Printing|Synching...\n");
		RBTreeEnumerateInfo(rb);
		printed_count += state.printed_count;
		printf("Completed\n");
		/*if (state.printed_count == 0)
			print_meta_list_head(state.first, state.second, state.other);
		print_notes = 1;
		print_head = 0;*/
	}
	if (state.cnt_confuse > 0 && arg->print_confuse && !(state.print_op & OP_CONFUSE)) {
		//printf("\nwarning: There are number of confuse items. The confuse means that don't know how to process.\n");
		state.print_op = OP_CONFUSE;
		state.page_index = 1;
		state.printed_count = 0;
		state.prefixion = "[Confuse] ";
		putchar('\n');
		printf("Printing Confuse...\n");
		RBTreeEnumerateInfo(rb);
		printf("Completed\n");
		printed_count += state.printed_count;
	}
	if (printed_count == 0) {
		print_meta_list_head(state.first, state.second, state.other);
	}
	putchar('\n');
	print_meta_list_statistic(&state, state.print_fail);
	putchar('\n');
	print_meta_list_notes(state.first, state.second, state.other);
	return 0;
}

static int compare(ShellContext *context, compare_arg *arg, 
	int (*onComparedFile)(ShellContext *context, compare_arg *arg, MyMeta *mm, void *state),
	void *comparedFileState,
	int(*onComparedDir)(ShellContext *context, compare_arg *arg, rb_red_blk_tree *rb, void *state),
	void *comparedDirState)
{
	char *path = NULL;

	LocalFileInfo *local = NULL;
	PcsFileInfo *remote = NULL;

	/*检查本地文件 - 开始*/
	local = GetLocalFileInfo(arg->local_file);
	if (!local) {
		fprintf(stderr, "Error: The local file not exist.\n");
		return -1;
	}
	/*检查本地文件 - 结束*/

	//检查是否已经登录
	if (!is_login(context, NULL)) {
		DestroyLocalFileInfo(local);
		return -1;
	}

	//拼接出网盘文件的完整路径
	path = combin_net_disk_path(context->workdir, arg->remote_file);
	if (!path) {
		DestroyLocalFileInfo(local);
		return -1;
	}
	if (strcmp(path, "/") == 0) {
		remote = pcs_fileinfo_create();
		remote->path = pcs_utils_strdup("/");
		remote->isdir = PcsTrue;
	}
	else {
		/*获取网盘文件元数据*/
		remote = pcs_meta(context->pcs, path);
		if (!remote) {
			fprintf(stderr, "Error: The remote file not exist, or have error: %s\n", pcs_strerror(context->pcs));
			DestroyLocalFileInfo(local);
			pcs_free(path);
			return -1;
		}
	}

	/*本地是文件，远端是目录*/
	if (!local->isdir && remote->isdir) {
		fprintf(stderr, "Error: Can't compare the file with directory.\n");
		DestroyLocalFileInfo(local);
		pcs_fileinfo_destroy(remote);
		pcs_free(path);
		return -1;
	}

	/*本地是目录，远端是文件*/
	if (local->isdir && !remote->isdir) {
		fprintf(stderr, "Error: Can't compare the directory with file.\n");
		DestroyLocalFileInfo(local);
		pcs_fileinfo_destroy(remote);
		pcs_free(path);
		return -1;
	}

	/*本地和远端都是文件*/
	if (!local->isdir && !remote->isdir) {
		MyMeta *mm = NULL;
		int rc;
		mm = compare_file(local, remote);
		if (onComparedFile)
			rc = (*onComparedFile)(context, arg, mm, comparedFileState);
		meta_destroy(mm);
		DestroyLocalFileInfo(local);
		pcs_fileinfo_destroy(remote);
		pcs_free(path);
		return rc;
	}

	/*本地和远端都是目录*/
	if (local->isdir && remote->isdir) {
		rb_red_blk_tree *rb = NULL;
		int skip = 0, total_cnt = 0;
		int rc;
		printf("Scanning local file system...\n");
		rb = meta_load(arg->local_file, arg->recursive);
		if (!rb) {
			fprintf(stderr, "Error: Can't list the local directory.\n");
			DestroyLocalFileInfo(local);
			pcs_fileinfo_destroy(remote);
			pcs_free(path);
			return -1;
		}
		printf("Completed\n");
		skip = strlen(remote->path);
		if (remote->path[skip - 1] != '/' && remote->path[skip - 1] != '\\') skip++;
		printf("Fetching net disk file list...\n");
		if (combin_with_remote_dir_files(context, rb, remote->path, arg->recursive, skip, &total_cnt, arg->check_local_dir_exist)) {
			fprintf(stderr, "Error: Can't list the remote directory.\n");
			RBTreeDestroy(rb);
			DestroyLocalFileInfo(local);
			pcs_fileinfo_destroy(remote);
			pcs_free(path);
			return -1;
		}
		if (total_cnt > 0) putchar('\n');
		printf("Completed\n");
		if (onComparedDir)
			rc = (*onComparedDir)(context, arg, rb, comparedDirState);
		RBTreeDestroy(rb);
		DestroyLocalFileInfo(local);
		pcs_fileinfo_destroy(remote);
		pcs_free(path);
		return rc;
	}
	DestroyLocalFileInfo(local);
	pcs_fileinfo_destroy(remote);
	pcs_free(path);
	return -1;
}

/*比较本地和网盘目录的异同*/
static int cmd_compare(ShellContext *context, struct args *arg)
{
	compare_arg cmpArg = { 0 };

	if (test_arg(arg, 2, 2, "c", "d", "e", "r", "u", "h", "help", NULL)) {
		usage_compare();
		return -1;
	}
	if (has_opts(arg, "h", "help", NULL)) {
		usage_compare();
		return 0;
	}
	if (parse_compare_args(arg, &cmpArg)) {
		usage_compare();
		return -1;
	}
	cmpArg.check_local_dir_exist = 1;

	return compare(context, &cmpArg, &on_compared_file, NULL, &on_compared_dir, NULL);
}

#pragma endregion

/*打印出上下文*/
static int cmd_context(ShellContext *context, struct args *arg)
{
	char *json;
	if (test_arg(arg, 0, 0, "h", "help", NULL)) {
		usage_context();
		return -1;
	}
	if (has_opts(arg, "h", "help", NULL)) {
		usage_context();
		return 0;
	}
	json = context2str(context);
	assert(json);
	printf("%s\n", json);
	pcs_free(json);
	return 0;
}

/*下载*/
static int cmd_download(ShellContext *context, struct args *arg)
{
	int is_force = 0;
	char *path = NULL, *errmsg = NULL;
	const char *relPath = NULL, *locPath = NULL;

	LocalFileInfo *local;
	PcsFileInfo *meta;

	if (test_arg(arg, 2, 2, "f", "h", "help", NULL)) {
		usage_download();
		return -1;
	}
	if (has_opts(arg, "h", "help", NULL)) {
		usage_download();
		return 0;
	}

	is_force = has_opt(arg, "f");
	relPath = arg->argv[0];
	locPath = arg->argv[1];

	/*检查本地文件 - 开始*/
	local = GetLocalFileInfo(locPath);
	if (local && local->isdir) {
		fprintf(stderr, "Error: The local file exist, but it's a directory.\n");
		DestroyLocalFileInfo(local);
		return -1;
	}
	else if (local && !local->isdir) {
		if (!is_force) {
			fprintf(stderr, "Error: The local file exist. You can specify '-f' to force override.\n", locPath);
			DestroyLocalFileInfo(local);
			return -1;
		}
	}
	if (local) DestroyLocalFileInfo(local);
	/*检查本地文件 - 结束*/

	//检查是否已经登录
	if (!is_login(context, NULL)) {
		return -1;
	}

	path = combin_net_disk_path(context->workdir, relPath);
	if (strcmp(path, "/") == 0) {
		fprintf(stderr, "Error: Can't download root directory. \nYou can use 'synch -cd' command to download the directory.\n");
		pcs_free(path);
		return -1;
	}

	/*检查网盘文件 - 开始*/
	meta = pcs_meta(context->pcs, path);
	if (!meta) {
		fprintf(stderr, "Error: The remote file not exist, or have error: %s\n", pcs_strerror(context->pcs));
		pcs_free(path);
		return -1;
	}
	if (meta->isdir) {
		fprintf(stderr, "Error: The remote file is directory. \nYou can use 'synch -cd' command to download the directory.\n");
		pcs_fileinfo_destroy(meta);
		pcs_free(path);
		return -1;
	}
	/*检查网盘文件 - 结束*/

	/*开始下载*/

	if (do_download(context,
		locPath, meta->path, meta->server_mtime,
		"", context->workdir,
		&errmsg, NULL)) {
		fprintf(stderr, "Error: %s\n", errmsg);
		pcs_fileinfo_destroy(meta);
		if (errmsg) pcs_free(errmsg);
		pcs_free(path);
		return -1;
	}
	printf("Download %s to %s Success.\n", meta->path, locPath);
	pcs_fileinfo_destroy(meta);
	if (errmsg) pcs_free(errmsg);
	pcs_free(path);
	return 0;
}

/*输出文本到网盘某一个文件*/
static int cmd_echo(ShellContext *context, struct args *arg)
{
	int is_append = 0;
	char *path = NULL, *t = NULL;
	const char *relPath = NULL, *text = NULL;
	size_t sz;
	PcsFileInfo *f;
	PcsFileInfo *meta;
	if (test_arg(arg, 2, 2, "a", "h", "help", NULL)) {
		usage_echo();
		return -1;
	}
	if (has_opts(arg, "h", "help", NULL)) {
		usage_echo();
		return 0;
	}
	is_append = has_opt(arg, "a");
	relPath = arg->argv[0];
	text = arg->argv[1];

	if (!is_login(context, NULL)) return -1;
	path = combin_net_disk_path(context->workdir, relPath);
	assert(path);
	sz = strlen(text);

	if (strcmp(path, "/") == 0) {
		fprintf(stderr, "Error: Can't specify root directory, you should specify the file name.\n");
		pcs_free(path);
		return -1;
	}

	/*检查网盘文件 - 开始*/
	meta = pcs_meta(context->pcs, path);
	if (meta && meta->isdir) {
		fprintf(stderr, "Error: The remote file is directory. \n");
		pcs_fileinfo_destroy(meta);
		pcs_free(path);
		return -1;
	}
	if (meta) {
		pcs_fileinfo_destroy(meta);
		meta = NULL;
	}
	/*检查网盘文件 - 结束*/

	if (is_append) {
		const char *org;
		size_t len;
		//获取文件的内容
		org = pcs_cat(context->pcs, path, &len);
		if (org == NULL) {
			fprintf(stderr, "Error: %s path=%s.\n", pcs_strerror(context->pcs), path);
			pcs_free(path);
			return -1;
		}
		//拼接两个字符串
		t = (char *)pcs_malloc(sz + len + 1);
		assert(t);
		memcpy(t, org, len);
		memcpy(t + len, text, sz + 1);
		sz += len;
	}
	else {
		t = (char *)text;
	}
	f = pcs_upload_buffer(context->pcs, path, PcsTrue, t, sz);
	if (!f) {
		fprintf(stderr, "Error: %s. \n", pcs_strerror(context->pcs), path);
		pcs_free(path);
		if (t != text) pcs_free(t);
		return -1;
	}
	printf("Success. You can use '%s cat %s' to print the file content.\n", app_name, path);
	pcs_fileinfo_destroy(f);
	pcs_free(path);
	if (t != text) pcs_free(t);
	return 0;
}

static int encrypt_file(const char *src, const char *dst, int secure_method, const char *secure_key)
{
	MD5_CTX md5;
	unsigned char md5_value[16], buf[PCS_BUFFER_SIZE], out_buf[PCS_BUFFER_SIZE];
	FILE *srcFile, *dstFile;
	char *tmp_local_path;
	long file_sz, sz, buf_sz;
	AES_KEY				aes = { 0 };
	unsigned char		key[AES_BLOCK_SIZE] = { 0 };
	unsigned char		iv[AES_BLOCK_SIZE] = { 0 };
	int rc;
	rc = AES_set_encrypt_key(md5_string_raw(secure_key), secure_method, &aes);
	if (rc < 0) {
		fprintf(stderr, "Error: Can't set encrypt key.\n");
		return -1;
	}
	MD5_Init(&md5);
	srcFile = fopen(src, "rb");
	if (!srcFile) {
		fprintf(stderr, "Error: Can't open the source file: %s\n", src);
		return -1;
	}
	tmp_local_path = (char *)pcs_malloc(strlen(dst) + strlen(TEMP_FILE_SUFFIX) + 1);
	strcpy(tmp_local_path, dst);
	strcat(tmp_local_path, TEMP_FILE_SUFFIX);
	dstFile = fopen(tmp_local_path, "wb");
	if (!dstFile) {
		fprintf(stderr, "Error: Can't create the temp file: %s\n", tmp_local_path);
		fclose(srcFile);
		pcs_free(tmp_local_path);
		return -1;
	}

	fseek(srcFile, 0L, SEEK_END);
	file_sz = ftell(srcFile);
	fseek(srcFile, 0L, SEEK_SET);

	sz = 0;
	if (file_sz % AES_BLOCK_SIZE == 0) {
		sz = file_sz;
	}
	else {
		sz = (file_sz / AES_BLOCK_SIZE + 1) * AES_BLOCK_SIZE;
	}
	int2Buffer(PCS_AES_MAGIC, buf);
	int2Buffer(secure_method, &buf[4]);
	int2Buffer((int)(sz - file_sz), &buf[8]);
	int2Buffer(0, &buf[12]);
	sz = fwrite(buf, 1, 16, dstFile);
	if (sz != 16) {
		fprintf(stderr, "Error: Write data to %s error. \n", dst);
		fclose(srcFile);
		fclose(dstFile);
		DeleteFileRecursive(tmp_local_path);
		pcs_free(tmp_local_path);
		return -1;
	}

	while ((sz = fread(buf, 1, PCS_BUFFER_SIZE, srcFile)) > 0) {
		MD5_Update(&md5, buf, sz);
		if ((sz % AES_BLOCK_SIZE) != 0) {
			buf_sz = (sz / AES_BLOCK_SIZE + 1) * AES_BLOCK_SIZE;
			memset(&buf[sz], 0, buf_sz - sz);
		}
		else {
			buf_sz = sz;
		}
		// encrypt (iv will change)
		AES_cbc_encrypt(buf, out_buf, buf_sz, &aes, iv, AES_ENCRYPT);
		sz = fwrite(out_buf, 1, buf_sz, dstFile);
		if (sz != buf_sz) {
			fprintf(stderr, "Error: Write data to %s error. \n", dst);
			fclose(srcFile);
			fclose(dstFile);
			DeleteFileRecursive(tmp_local_path);
			pcs_free(tmp_local_path);
			return -1;
		}
	}
	MD5_Final(md5_value, &md5);
	sz = fwrite(md5_value, 1, 16, dstFile);
	if (sz != 16) {
		fprintf(stderr, "Error: Write data to %s error. \n", dst);
		fclose(srcFile);
		fclose(dstFile);
		DeleteFileRecursive(tmp_local_path);
		pcs_free(tmp_local_path);
		return -1;
	}
	fclose(srcFile);
	fclose(dstFile);
	DeleteFileRecursive(dst);
	if (rename(tmp_local_path, dst)) {
		fprintf(stderr, "Error: The file have been encrypted at %s, but can't rename to %s.\n You should be rename manual.\n", tmp_local_path, dst);
		DeleteFileRecursive(tmp_local_path);
		pcs_free(tmp_local_path);
		return -1;
	}
	pcs_free(tmp_local_path);
	printf("Success\n");
	return 0;
}

static inline void readToAesHead(char *buf, struct PcsAesHead *head)
{
	head->magic = readInt(buf);
	head->bits = readInt(&buf[4]);
	head->polish = readInt(&buf[8]);
	head->reserve = readInt(&buf[12]);
}

int decrypt_file(ShellContext *context, const char *src, const char *dst, const char *secure_key)
{
	struct PcsAesHead head = { 0 };
	MD5_CTX md5;
	unsigned char md5_value[16], buf[PCS_BUFFER_SIZE], out_buf[PCS_BUFFER_SIZE];
	char *tmp_local_path;
	FILE *srcFile, *dstFile;
	long file_sz, sz, buf_sz;
	AES_KEY				aes = { 0 };
	unsigned char		key[AES_BLOCK_SIZE] = { 0 };
	unsigned char		iv[AES_BLOCK_SIZE] = { 0 };
	int rc, i;
	MD5_Init(&md5);
	srcFile = fopen(src, "rb");
	if (!srcFile) {
		fprintf(stderr, "Error: Can't open the source file: %s\n", src);
		return -1;
	}
	tmp_local_path = (char *)pcs_malloc(strlen(dst) + strlen(TEMP_FILE_SUFFIX) + 1);
	strcpy(tmp_local_path, dst);
	strcat(tmp_local_path, TEMP_FILE_SUFFIX);
	dstFile = fopen(tmp_local_path, "wb");
	if (!dstFile) {
		fprintf(stderr, "Error: Can't create the temp file: %s\n", tmp_local_path);
		fclose(srcFile);
		pcs_free(tmp_local_path);
		return -1;
	}

	fseek(srcFile, 0L, SEEK_END);
	file_sz = ftell(srcFile);
	fseek(srcFile, 0L, SEEK_SET);

	if (file_sz < 32 || ((file_sz - 32) % AES_BLOCK_SIZE) != 0) {
		fprintf(stderr, "Error: The file is not a encrypt file or is broken.\n");
		fclose(srcFile);
		fclose(dstFile);
		DeleteFileRecursive(tmp_local_path);
		pcs_free(tmp_local_path);
		return -1;
	}

	sz = fread(buf, 1, 16, srcFile);
	if (sz != 16) {
		fprintf(stderr, "Error: Can't read the source file: %s\n", src);
		fclose(srcFile);
		fclose(dstFile);
		DeleteFileRecursive(tmp_local_path);
		pcs_free(tmp_local_path);
		return -1;
	}

	readToAesHead(buf, &head);
	if (head.magic != PCS_AES_MAGIC || (head.bits != 128 && head.bits != 192 && head.bits != 256)) {
		fprintf(stderr, "Error: The file is not a encrypt file or is broken.\n");
		fclose(srcFile);
		fclose(dstFile);
		DeleteFileRecursive(tmp_local_path);
		pcs_free(tmp_local_path);
		return -1;
	}

	rc = AES_set_decrypt_key(md5_string_raw(secure_key), head.bits, &aes);
	if (rc < 0) {
		fprintf(stderr, "Error: Can't set decrypt key.\n");
		fclose(srcFile);
		fclose(dstFile);
		DeleteFileRecursive(tmp_local_path);
		pcs_free(tmp_local_path);
		return -1;
	}
	file_sz -= 32;
	if (file_sz < PCS_BUFFER_SIZE) {
		buf_sz = file_sz;
	}
	else {
		buf_sz = PCS_BUFFER_SIZE;
	}
	while ((sz = fread(buf, 1, buf_sz, srcFile)) > 0) {
		if (sz != buf_sz) {
			fprintf(stderr, "Error: Can't read the source file: %s\n", src);
			fclose(srcFile);
			fclose(dstFile);
			DeleteFileRecursive(tmp_local_path);
			pcs_free(tmp_local_path);
			return -1;
		}
		AES_cbc_encrypt(buf, out_buf, buf_sz, &aes, iv, AES_DECRYPT);
		if (file_sz == buf_sz) {
			buf_sz -= head.polish;
			file_sz = buf_sz;
		}
		MD5_Update(&md5, out_buf, buf_sz);
		sz = fwrite(out_buf, 1, buf_sz, dstFile);
		if (sz != buf_sz) {
			fprintf(stderr, "Error: Write data to %s error. \n", dst);
			fclose(srcFile);
			fclose(dstFile);
			DeleteFileRecursive(tmp_local_path);
			pcs_free(tmp_local_path);
			return -1;
		}
		file_sz -= buf_sz;
		if (file_sz == 0) break;
		if (file_sz < PCS_BUFFER_SIZE) {
			buf_sz = file_sz;
		}
		else {
			buf_sz = PCS_BUFFER_SIZE;
		}
	}
	MD5_Final(md5_value, &md5);
	sz = fread(buf, 1, 16, srcFile);
	if (sz != 16) {
		fprintf(stderr, "Error: Can't read the source file: %s\n", src);
		fclose(srcFile);
		fclose(dstFile);
		DeleteFileRecursive(tmp_local_path);
		pcs_free(tmp_local_path);
		return -1;
	}
	for (i = 0; i < 16; i++) {
		if (md5_value[i] != buf[i]) {
			fprintf(stderr, "Error: Wrong key or broken file\n");
			fclose(srcFile);
			fclose(dstFile);
			DeleteFileRecursive(tmp_local_path);
			pcs_free(tmp_local_path);
			return -1;
		}
	}
	fclose(srcFile);
	fclose(dstFile);
	DeleteFileRecursive(dst);
	if (rename(tmp_local_path, dst)) {
		fprintf(stderr, "Error: The file have been decrypted at %s, but can't rename to %s.\n You should be rename manual.\n", tmp_local_path, dst);
		//DeleteFileRecursive(tmp_local_path);
		pcs_free(tmp_local_path);
		return -1;
	}
	pcs_free(tmp_local_path);
	printf("Success\n");
	return 0;
}

static inline int get_secure_method(ShellContext *context)
{
	if (!context->secure_method) return -1;
	if (strcmp(context->secure_method, "aes-cbc-128")) return PCS_SECURE_AES_CBC_128;
	if (strcmp(context->secure_method, "aes-cbc-192")) return PCS_SECURE_AES_CBC_192;
	if (strcmp(context->secure_method, "aes-cbc-256")) return PCS_SECURE_AES_CBC_256;
	return -1;
}

static int cmd_encode(ShellContext *context, struct args *arg)
{
	int encrypt = 0, decrypt = 0, force = 0, secure_method;
	LocalFileInfo *dstInfo;
	if (test_arg(arg, 2, 2, "d", "e", "f", "h", "help", NULL)) {
		usage_encode();
		return -1;
	}
	if (has_opts(arg, "h", "help", NULL)) {
		usage_encode();
		return 0;
	}

	if (has_opt(arg, "d"))
		decrypt = 1;
	if (has_opt(arg, "e"))
		encrypt = 1;
	if (has_opt(arg, "f"))
		force = 1;

	if (encrypt && decrypt) {
		usage_encode();
		return -1;
	}

	if (!encrypt && !decrypt)
		decrypt = 1;

	dstInfo = GetLocalFileInfo(arg->argv[1]);
	if (dstInfo) {
		if (dstInfo->isdir) {
			fprintf(stderr, "Error: The dest is directory. %s", arg->argv[1]);
			DestroyLocalFileInfo(dstInfo);
			return -1;
		}
		else if (!force) {
			fprintf(stderr, "Error: The dest is exist. You can use '-f' to force override. %s", arg->argv[1]);
			DestroyLocalFileInfo(dstInfo);
			return -1;
		}
		DestroyLocalFileInfo(dstInfo);
	}

	if (encrypt) {
		secure_method = get_secure_method(context);
		if (secure_method != PCS_SECURE_AES_CBC_128 && secure_method != PCS_SECURE_AES_CBC_192 && secure_method != PCS_SECURE_AES_CBC_256) {
			fprintf(stderr, "Error: You have not set the encrypt method, the method should be one of [aes-cbc-128|aes-cbc-192|aes-cbc-256]. You can set it by '%s set'.\n", app_name);
			return -1;
		}
		if (!context->secure_key || strlen(context->secure_key) == 0) {
			fprintf(stderr, "Error: You have not set the encrypt key. You can set it by '%s set'.", app_name);
			return -1;
		}
		return encrypt_file(arg->argv[0], arg->argv[1], secure_method, context->secure_key);
	}
	else if (decrypt) {
		if (!context->secure_key || strlen(context->secure_key) == 0) {
			fprintf(stderr, "Error: You have not set the encrypt key. You can set it by '%s set'.", app_name);
			return -1;
		}
		return decrypt_file(context, arg->argv[0], arg->argv[1], context->secure_key);
	}
	else {
		usage_encode();
		return -1;
	}

	return 0;
}

/*打印帮助信息*/
static int cmd_help(ShellContext *context, struct args *arg)
{
	int rc;
	char *cmd;
	if (test_arg(arg, 0, 1, "h", "help", NULL)) {
		usage_help();
		return -1;
	}
	if (has_opts(arg, "h", "help", NULL)) {
		usage_help();
		return 0;
	}
	if (arg->argc == 0) {
		usage();
		return 0;
	}
	cmd = arg->argv[0];
	if (strcmp(cmd, "cat") == 0) {
		usage_cat();
		rc = 0;
	}
	else if (strcmp(cmd, "cd") == 0) {
		usage_cd();
		rc = 0;
	}
	else if (strcmp(cmd, "copy") == 0
		|| strcmp(cmd, "cp") == 0) {
		usage_copy();
		rc = 0;
	}
	else if (strcmp(cmd, "compare") == 0
		|| strcmp(cmd, "c") == 0) {
		usage_compare();
		rc = 0;
	}
	else if (strcmp(cmd, "context") == 0
		|| strcmp(cmd, "config") == 0) {
		usage_context();
		rc = 0;
	}
	else if (strcmp(cmd, "download") == 0
		|| strcmp(cmd, "d") == 0) {
		usage_download();
		rc = 0;
	}
	else if (strcmp(cmd, "echo") == 0) {
		usage_echo();
		rc = 0;
	}
	else if (strcmp(cmd, "encode") == 0) {
		usage_encode();
		rc = 0;
	}
	else if (strcmp(cmd, "list") == 0
		|| strcmp(cmd, "ls") == 0) {
		usage_list();
		rc = 0;
	}
	else if (strcmp(cmd, "login") == 0) {
		usage_login();
		rc = 0;
	}
	else if (strcmp(cmd, "logout") == 0) {
		usage_logout();
		rc = 0;
	}
	else if (strcmp(cmd, "meta") == 0) {
		usage_meta();
		rc = 0;
	}
	else if (strcmp(cmd, "mkdir") == 0) {
		usage_mkdir();
		rc = 0;
	}
	else if (strcmp(cmd, "move") == 0
		|| strcmp(cmd, "mv") == 0) {
		usage_move();
		rc = 0;
	}
	else if (strcmp(cmd, "pwd") == 0) {
		usage_pwd();
		rc = 0;
	}
	else if (strcmp(cmd, "quota") == 0) {
		usage_quota();
		rc = 0;
	}
	else if (strcmp(cmd, "remove") == 0
		|| strcmp(cmd, "rm") == 0) {
		usage_remove();
		rc = 0;
	}
	else if (strcmp(cmd, "rename") == 0
		|| strcmp(cmd, "ren") == 0) {
		usage_rename();
		rc = 0;
	}
	else if (strcmp(cmd, "set") == 0) {
		usage_set();
		rc = 0;
	}
	else if (strcmp(cmd, "search") == 0) {
		usage_search();
		rc = 0;
	}
	else if (strcmp(cmd, "synch") == 0
		|| strcmp(cmd, "s") == 0) {
		usage_synch();
		rc = 0;
	}
	else if (strcmp(cmd, "upload") == 0
		|| strcmp(cmd, "u") == 0) {
		usage_upload();
		rc = 0;
	}
	else if (strcmp(cmd, "version") == 0) {
		usage_version();
		rc = 0;
	}
	else if (strcmp(cmd, "who") == 0) {
		usage_who();
		rc = 0;
	}
	else if (strcmp(cmd, "help") == 0
		|| strcmp(cmd, "?") == 0) {
		usage_help();
		rc = 0;
	}
	else {
		usage();
		rc = -1;
	}
	return rc;
}

/*列出目录*/
static int cmd_list(ShellContext *context, struct args *arg)
{
	char *path;
	PcsFileInfoList *list = NULL;
	int page_index = 1;
	char tmp[64] = { 0 };
	int fileCount = 0, dirCount = 0;
	size_t totalSize = 0;

	if (test_arg(arg, 0, 1, "h", "help", NULL)) {
		usage_list();
		return -1;
	}
	if (has_opts(arg, "h", "help", NULL)) {
		usage_list();
		return 0;
	}

	if (!is_login(context, NULL)) return -1;

	if (arg->argc == 0) {
		path = pcs_utils_strdup(context->workdir);
	}
	else if (arg->argc == 1) {
		path = combin_net_disk_path(context->workdir, arg->argv[0]);
	}
	else {
		usage_list();
		return -1;
	}

	while (1) {
		list = pcs_list(context->pcs, path,
			page_index, context->list_page_size,
			context->list_sort_name,
			streq(context->list_sort_direction, "desc", -1) ? PcsTrue: PcsFalse);
		if (!list) {
			if (pcs_strerror(context->pcs)) {
				fprintf(stderr, "Error: %s\n", pcs_strerror(context->pcs));
				pcs_free(path);
				return -1;
			}
			break;
		}
		printf("PAGE#%d\n", page_index);
		print_filelist(list, &fileCount, &dirCount, &totalSize);
		if (list->count < context->list_page_size) {
			pcs_filist_destroy(list);
			break;
		}
		pcs_filist_destroy(list);
		printf("Print next page#%d [Y|N]? ", page_index + 1);
		std_string(tmp, 10);
		//printf("[%s] %d\n", tmp, strlen(tmp));
		if (strlen(tmp) != 0 && pcs_utils_strcmpi(tmp, "y") && pcs_utils_strcmpi(tmp, "yes")) {
			break;
		}
		page_index++;
	}
	if (page_index > 1) {
		puts("\n------------------------------------------------------------------------------");
		pcs_utils_readable_size(totalSize, tmp, 63, NULL);
		tmp[63] = '\0';
		printf("Total Page: %d, Total Size: %s, File Count: %d, Directory Count: %d\n", page_index, tmp, fileCount, dirCount);
	}
	putchar('\n');
	pcs_free(path);
	return 0;
}

/*登录*/
static int cmd_login(ShellContext *context, struct args *arg)
{
	PcsRes pcsres;
	char str[50], *p = NULL;
	if (test_arg(arg, 0, 0, "username", "password", "h", "help", NULL)) {
		usage_login();
		return -1;
	}
	if (has_opts(arg, "h", "help", NULL)) {
		usage_login();
		return 0;
	}
	if (is_login(context, "")) {
		printf("You have been logon, you can use 'who' command to print the UID. \n"
			"You can logout by 'logout' command and then relogin.\n");
		return -1;
	}
	if (has_optEx(arg, "username", &p) && p && strlen(p) > 0) {
		pcs_setopt(context->pcs, PCS_OPTION_USERNAME, p);
	}
	else {
		printf("User Name: ");
		std_string(str, 50);
		pcs_setopt(context->pcs, PCS_OPTION_USERNAME, str);
	}

	if (has_optEx(arg, "password", &p) && p && strlen(p) > 0) {
		pcs_setopt(context->pcs, PCS_OPTION_PASSWORD, p);
	}
	else {
		printf("Password: ");
		std_password(str, 50);
		pcs_setopt(context->pcs, PCS_OPTION_PASSWORD, str);
	}
	pcsres = pcs_login(context->pcs);
	if (pcsres != PCS_OK) {
		printf("Login Failed: %s\n", pcs_strerror(context->pcs));
		return -1;
	}
	printf("Login Success. UID: %s\n", pcs_sysUID(context->pcs));
	return 0;
}

/*注销*/
static int cmd_logout(ShellContext *context, struct args *arg)
{
	PcsRes pcsres;
	if (test_arg(arg, 0, 0, "h", "help", NULL)) {
		usage_logout();
		return -1;
	}
	if (has_opts(arg, "h", "help", NULL)) {
		usage_logout();
		return 0;
	}
	if (!is_login(context, "")) {
		fprintf(stderr, "You are not login, you can use 'login' command to login.\n");
		return -1;
	}
	pcsres = pcs_logout(context->pcs);
	if (pcsres != PCS_OK) {
		printf("Logout Fail: %s\n", pcs_strerror(context->pcs));
		return -1;
	}
	printf("Logout Success.\n");
	return 0;
}

/*打印文件或目录的元数据*/
static int cmd_meta(ShellContext *context, struct args *arg)
{
	char *path;
	PcsFileInfo *fi;
	if (test_arg(arg, 0, 1, "h", "help", NULL)) {
		usage_meta();
		return -1;
	}
	if (has_opts(arg, "h", "help", NULL)) {
		usage_meta();
		return 0;
	}
	if (!is_login(context, NULL)) return -1;
	if (arg->argc == 1)
		path = combin_net_disk_path(context->workdir, arg->argv[0]);
	else
		path = context->workdir;
	assert(path);
	if (strcmp(path, "/") == 0) {
		fprintf(stderr, "Error: Can't get meta for root directory.\n");
		if (path != context->workdir) pcs_free(path);
		return -1;
	}
	fi = pcs_meta(context->pcs, path);
	if (!fi) {
		fprintf(stderr, "Error: The target not exist, or have error: %s\n", pcs_strerror(context->pcs));
		if (path != context->workdir) pcs_free(path);
		return -1;
	}
	print_fileinfo(fi, " ");
	pcs_fileinfo_destroy(fi);
	if (path != context->workdir) pcs_free(path);
	return 0;
}

/*创建目录*/
static int cmd_mkdir(ShellContext *context, struct args *arg)
{
	PcsRes res;
	char *path;
	if (test_arg(arg, 1, 1, "h", "help", NULL)) {
		usage_mkdir();
		return -1;
	}
	if (has_opts(arg, "h", "help", NULL)) {
		usage_mkdir();
		return 0;
	}
	if (!is_login(context, NULL)) return -1;
	path = combin_net_disk_path(context->workdir, arg->argv[0]);
	assert(path);
	if (strcmp(path, "/") == 0) {
		fprintf(stderr, "Error: The root directory have been exist.\n");
		pcs_free(path);
		return -1;
	}
	res = pcs_mkdir(context->pcs, path);
	if (res != PCS_OK) {
		fprintf(stderr, "Error: %s\n", pcs_strerror(context->pcs));
		pcs_free(path);
		return -1;
	}
	printf("Success.\n");
	pcs_free(path);
	return 0;
}

/*移动文件*/
static int cmd_move(ShellContext *context, struct args *arg)
{
	PcsPanApiRes *res;
	PcsSList2 slist;
	PcsPanApiResInfoList *info;
	if (test_arg(arg, 2, 2, "h", "help", NULL)) {
		usage_move();
		return -1;
	}
	if (has_opts(arg, "h", "help", NULL)) {
		usage_move();
		return 0;
	}
	if (!is_login(context, NULL)) return -1;
	slist.string1 = combin_net_disk_path(context->workdir, arg->argv[0]); /* path */
	slist.string2 = combin_net_disk_path(context->workdir, arg->argv[1]); /* new_name */
	slist.next = NULL;
	if (strcmp(slist.string1, "/") == 0) {
		fprintf(stderr, "Error: Can't move root directory.\n");
		pcs_free(slist.string1);
		pcs_free(slist.string2);
		return -1;
	}
	if (strcmp(slist.string2, "/") == 0) {
		fprintf(stderr, "Error: Can't move to root directory.\n");
		pcs_free(slist.string1);
		pcs_free(slist.string2);
		return -1;
	}

	res = pcs_move(context->pcs, &slist);
	if (!res) {
		fprintf(stderr, "Error: %s src=%s, dst=%s.\n", pcs_strerror(context->pcs), slist.string1, slist.string2);
		pcs_free(slist.string1);
		pcs_free(slist.string2);
		return -1;
	}
	info = res->info_list;
	if (info->info.error) {
		fprintf(stderr, "Error: unknow src=%s, dst=%s. \n", slist.string1, slist.string2);
		pcs_pan_api_res_destroy(res);
		pcs_free(slist.string1);
		pcs_free(slist.string2);
		return -1;
	}
	printf("Move %s to %s Success.\n", slist.string1, slist.string2);
	pcs_pan_api_res_destroy(res);
	pcs_free(slist.string1);
	pcs_free(slist.string2);
	return 0;
}

/*打印当前工作目录*/
static int cmd_pwd(ShellContext *context, struct args *arg)
{
	if (test_arg(arg, 0, 0, "h", "help", NULL)) {
		usage_pwd();
		return -1;
	}
	if (has_opts(arg, "h", "help", NULL)) {
		usage_pwd();
		return 0;
	}
	printf("%s\n", context->workdir);
	return 0;
}

/*打印配额*/
static int cmd_quota(ShellContext *context, struct args *arg)
{
	const char *opts[] = { "e", NULL };
	PcsRes pcsres;
	size_t quota, used;
	int is_exact = 0;
	char str[32] = { 0 };
	if (test_arg(arg, 0, 0, "e", "h", "help", NULL)) {
		usage_quota();
		return -1;
	}
	if (has_opts(arg, "h", "help", NULL)) {
		usage_quota();
		return 0;
	}
	is_exact = has_opt(arg, "e");

	if (!is_login(context, NULL)) return -1;
	pcsres = pcs_quota(context->pcs, &quota, &used);
	if (pcsres != PCS_OK) {
		fprintf(stderr, "Error: %s\n", pcs_strerror(context->pcs));
		return -1;
	}
	if (is_exact) {
		printf("%llu Bytes", used);
		putchar('/');
		printf("%llu Bytes  ", quota);
	}
	else {
		pcs_utils_readable_size((double)used, str, 30, NULL);
		printf("%s", str);
		putchar('/');
		pcs_utils_readable_size((double)quota, str, 30, NULL);
		printf("%s", str);
	}
	printf("\n");
	return 0;
}

/*删除文件*/
static int cmd_remove(ShellContext *context, struct args *arg)
{
	PcsPanApiRes *res;
	PcsSList slist;
	PcsPanApiResInfoList *info;
	if (test_arg(arg, 1, 1, "h", "help", NULL)) {
		usage_remove();
		return -1;
	}
	if (has_opts(arg, "h", "help", NULL)) {
		usage_remove();
		return 0;
	}
	if (!is_login(context, NULL)) return -1;
	slist.string = combin_net_disk_path(context->workdir, arg->argv[0]); /* path */
	slist.next = NULL;
	if (strcmp(slist.string, "/") == 0) {
		fprintf(stderr, "Error: Can't remove root directory.\n");
		pcs_free(slist.string);
		return -1;
	}

	res = pcs_delete(context->pcs, &slist);
	if (!res) {
		fprintf(stderr, "Error: %s path=%s.\n", pcs_strerror(context->pcs), slist.string);
		pcs_free(slist.string);
		return -1;
	}
	info = res->info_list;
	if (info->info.error) {
		fprintf(stderr, "Error: unknow path=%s. \n", slist.string);
		pcs_pan_api_res_destroy(res);
		pcs_free(slist.string);
		return -1;
	}
	printf("Remove %s Success.\n", slist.string);
	pcs_pan_api_res_destroy(res);
	pcs_free(slist.string);
	return 0;
}

/*重命名文件*/
static int cmd_rename(ShellContext *context, struct args *arg)
{
	PcsPanApiRes *res;
	PcsSList2 slist;
	PcsPanApiResInfoList *info;
	char *p;
	if (test_arg(arg, 2, 2, "h", "help", NULL)) {
		usage_rename();
		return -1;
	}
	if (has_opts(arg, "h", "help", NULL)) {
		usage_rename();
		return 0;
	}
	p = arg->argv[1];
	while (*p) {
		if (*p == '/' || *p == '\\' || *p == ':' || *p == '*' || *p == '?' || *p == '"' || *p == '<' || *p == '>' || *p == '|') {
			fprintf(stderr, "Error: The file name can't include \"/\\:*?\"<>|\"\n");
			return -1;
		}
		p++;
	}
	if (!is_login(context, NULL)) return -1;
	slist.string1 = combin_net_disk_path(context->workdir, arg->argv[0]); /* path */
	slist.string2 = arg->argv[1]; /* new_name */
	slist.next = NULL;
	if (strcmp(slist.string1, "/") == 0) {
		fprintf(stderr, "Error: Can't rename root directory.\n");
		pcs_free(slist.string1);
		return -1;
	}
	if (strcmp(slist.string2, "/") == 0) {
		fprintf(stderr, "Error: Can't rename to root directory.\n");
		pcs_free(slist.string1);
		return -1;
	}

	res = pcs_rename(context->pcs, &slist);
	if (!res) {
		fprintf(stderr, "Error: %s src=%s, dst=%s.\n", pcs_strerror(context->pcs), slist.string1, slist.string2);
		pcs_free(slist.string1);
		return -1;
	}
	info = res->info_list;
	if (info->info.error) {
		fprintf(stderr, "Error: unknow src=%s, dst=%s. \n", slist.string1, slist.string2);
		pcs_pan_api_res_destroy(res);
		pcs_free(slist.string1);
		return -1;
	}
	printf("Rename %s to %s Success.\n", slist.string1, slist.string2);
	pcs_pan_api_res_destroy(res);
	pcs_free(slist.string1);
	return 0;
}

/*更改上下文设置*/
static int cmd_set(ShellContext *context, struct args *arg)
{
	char *val = NULL;
	if (test_arg(arg, 0, 0, 
		"cookie_file", "captcha_file", 
		"list_page_size", "list_sort_name", "list_sort_direction",
		"secure_method", "secure_key", "secure_enable",
		"h", "help", NULL) && arg->optc == 0) {
		usage_set();
		return -1;
	}
	if (has_opts(arg, "h", "help", NULL)) {
		usage_set();
		return 0;
	}

	if (has_optEx(arg, "cookie_file", &val)) {
		if (set_cookiefile(context, val)) {
			usage_set();
			return -1;
		}
	}

	if (has_optEx(arg, "captcha_file", &val)) {
		if (set_captchafile(context, val)) {
			usage_set();
			return -1;
		}
	}

	if (has_optEx(arg, "list_page_size", &val)) {
		if (set_list_page_size(context, val)) {
			usage_set();
			return -1;
		}
	}

	if (has_optEx(arg, "list_sort_name", &val)) {
		if (set_list_sort_name(context, val)) {
			usage_set();
			return -1;
		}
	}

	if (has_optEx(arg, "list_sort_direction", &val)) {
		if (set_list_sort_direction(context, val)) {
			usage_set();
			return -1;
		}
	}

	if (has_optEx(arg, "secure_method", &val)) {
		if (set_secure_method(context, val)) {
			usage_set();
			return -1;
		}
	}

	if (has_optEx(arg, "secure_key", &val)) {
		if (set_secure_key(context, val)) {
			usage_set();
			return -1;
		}
	}

	if (has_optEx(arg, "secure_enable", &val)) {
		if (set_secure_enable(context, val)) {
			usage_set();
			return -1;
		}
	}
	printf("Success. You can view context by '%s context'\n", app_name);
	return 0;
}

/*搜索文件*/
static int cmd_search(ShellContext *context, struct args *arg)
{
	const char *opts[] = { "r", NULL };
	int is_recursive = 0;
	char *path = NULL;
	const char *relPath = NULL, *keyword = NULL;

	PcsFileInfoList *list = NULL;

	if (test_arg(arg, 1, 2, "r", "h", "help", NULL)) {
		usage_search();
		return -1;
	}
	if (has_opts(arg, "h", "help", NULL)) {
		usage_search();
		return 0;
	}

	is_recursive = has_opt(arg, "r");

	if (arg->argc == 1) {
		keyword = arg->argv[0];
	}
	else if (arg->argc == 2) {
		relPath = arg->argv[0];
		keyword = arg->argv[1];
	}
	else {
		usage_search();
		return 0;
	}

	if (!is_login(context, NULL)) return -1;

	if (!relPath) {
		path = pcs_utils_strdup(context->workdir);
	}
	else {
		path = combin_net_disk_path(context->workdir, relPath);
	}

	list = pcs_search(context->pcs, path, keyword, is_recursive ? PcsTrue : PcsFalse);
	if (!list) {
		if (pcs_strerror(context->pcs)) {
			fprintf(stderr, "Error: %s\n", pcs_strerror(context->pcs));
			pcs_free(path);
			return -1;
		}
		print_filelist_head(4);
		pcs_free(path);
		return 0;
	}
	print_filelist(list, NULL, NULL, NULL);
	pcs_filist_destroy(list);
	pcs_free(path);
	return 0;
}

#pragma region synch

static int synchDownload(MyMeta *meta, struct RBEnumerateState *s, void *state)
{
	if (s->dry_run) { /*演示操作，模拟成功*/
		meta->op_st = OP_ST_SUCC;
		return 0;
	}

	if (meta->remote_isdir) { /*跳过目录*/
		char *local_path;
		local_path = combin_path(s->local_basedir, -1, meta->path);
		/*创建目录*/
		if (CreateDirectoryRecursive(local_path) != MKDIR_OK) {
			if (meta->msg) pcs_free(meta->msg);
			meta->msg = pcs_utils_sprintf("Error: Can't create the directory: %s", local_path);
			meta->op_st = OP_ST_FAIL;
			pcs_free(local_path);
			return -1;
		}
		pcs_free(local_path);
		meta->op_st = OP_ST_SUCC;
		return 0;
	}

	return do_download(s->context, 
		meta->path, meta->remote_path, meta->remote_mtime, 
		s->local_basedir, s->remote_basedir,
		&meta->msg, &meta->op_st);
}

static int synchUpload(MyMeta *meta, struct RBEnumerateState *s, void *state)
{
	if (s->dry_run) { /*演示操作，模拟成功*/
		meta->op_st = OP_ST_SUCC;
		return 0;
	}

	if (meta->local_isdir) { /*跳过目录*/
		if (meta->local_filecount == 0 && !(meta->flag & FLAG_ON_REMOTE)) { /*其下没有文件，因此需手动创建目录*/
			/*创建目录*/
			char *remote_path, *dir;
			PcsRes res;

			dir = combin_net_disk_path(s->context->workdir, s->remote_basedir);
			remote_path = combin_net_disk_path(dir, meta->path);
			pcs_free(dir);
			res = pcs_mkdir(s->context->pcs, remote_path);
			if (res != PCS_OK) {
				if (meta->msg) pcs_free(meta->msg);
				meta->msg = pcs_utils_sprintf("Error: Can't create the directory %s: %s", remote_path, pcs_strerror(s->context->pcs));
				pcs_free(remote_path);
				meta->op_st = OP_ST_FAIL;
				return -1;
			}
			pcs_free(remote_path);
		}
		meta->op_st = OP_ST_SUCC;
		return 0;
	}

	return do_upload(s->context,
		meta->path, (meta->flag & FLAG_ON_REMOTE) ? meta->remote_path : meta->path, PcsTrue,
		s->local_basedir, s->remote_basedir,
		&meta->msg, &meta->op_st);
}

static int synchOnPrepare(MyMeta *meta, struct RBEnumerateState *s, void *state)
{
	if (meta->msg) {
		pcs_free(meta->msg);
		meta->msg = NULL;
	}
	switch (meta->op) {
	case OP_LEFT: {
		synchDownload(meta, s, state);
		break;
	}
	case OP_RIGHT: {
		synchUpload(meta, s, state);
		break;
	}
	case OP_EQ:
		meta->op_st = OP_ST_SKIP;
		break;
	case OP_CONFUSE:
		meta->op_st = OP_ST_CONFUSE;
		break;
	default:
		meta->op_st = OP_ST_NONE;
		break;
	}
	return 0;
}

static void synchOnRBEnumStatePrepared(ShellContext *context, compare_arg *arg, rb_red_blk_tree *rb, struct RBEnumerateState *state, void *st)
{
	/*state->print_op &= OP_NONE;
	if (!arg->print_eq && !arg->print_left && !arg->print_right && !arg->print_confuse)
		state->print_op = OP_RIGHT | OP_CONFUSE;
	if (arg->print_eq) state->print_op |= OP_EQ;
	if (arg->print_left) state->print_op |= OP_LEFT;
	if (arg->print_right) state->print_op |= OP_RIGHT;
	if (arg->print_confuse) state->print_op |= OP_CONFUSE;*/
	state->page_size = 0;
	state->page_index = 0;
	state->first = 6;
	state->process = &synchOnPrepare;
	state->processState = NULL;
	state->no_print_flag = 0;
	state->print_fail = 1;

	printf("\nDownload: %s, Upload: %s, Confuse: %s, Equal: %s\n",
		arg->print_left ? "on" : "off",
		arg->print_right ? "on" : "off",
		arg->print_confuse ? "on" : "off",
		arg->print_eq ? "on" : "off");
}

static int synchFile(ShellContext *context, compare_arg *arg, MyMeta *meta, void *state)
{
	int first, second, other;
	if (meta->msg) {
		pcs_free(meta->msg);
		meta->msg = NULL;
	}
	switch (meta->op) {
	case OP_LEFT: {
		if (arg->dry_run)
			meta->op_st = OP_ST_SUCC;
		else
			do_download(context,
				meta->path, meta->remote_path, meta->remote_mtime,
				arg->local_file, arg->remote_file,
				&meta->msg, &meta->op_st);
		break;
	}
	case OP_RIGHT: {
		if (arg->dry_run)
			meta->op_st = OP_ST_SUCC;
		else
			do_upload(context,
				meta->path, (meta->flag & FLAG_ON_REMOTE) ? meta->remote_path : meta->path, PcsTrue,
				arg->local_file, arg->remote_file,
				&meta->msg, &meta->op_st);
		break;
	}
	case OP_EQ:
		meta->op_st = OP_ST_SKIP;
		break;
	case OP_CONFUSE:
		meta->op_st = OP_ST_CONFUSE;
		break;
	default:
		meta->op_st = OP_ST_NONE;
		break;
	}
	first = 6; second = strlen(meta->path); other = 13;
	if (second < 10) second = 10;
	print_meta_list_head(first, second, other);
	print_meta_list_row(first, second, other, meta);
	print_meta_list_notes(first, second, other);
	return 0;
}

/*同步本地和网盘目录*/
static int cmd_synch(ShellContext *context, struct args *arg)
{
	compare_arg cmpArg = { 0 };

	if (test_arg(arg, 2, 2, "c", "d", "e", "n", "r", "u", "h", "help", NULL)) {
		usage_synch();
		return -1;
	}
	if (has_opts(arg, "h", "help", NULL)) {
		usage_synch();
		return 0;
	}
	if (parse_compare_args(arg, &cmpArg)) {
		usage_compare();
		return -1;
	}
	cmpArg.check_local_dir_exist = 0;
	cmpArg.onRBEnumerateStatePrepared = &synchOnRBEnumStatePrepared;

	return compare(context, &cmpArg, &synchFile, NULL, &on_compared_dir, NULL);
}

#pragma endregion

/*上传*/
static int cmd_upload(ShellContext *context, struct args *arg)
{
	const char *opts[] = { "f", NULL };
	int is_force = 0;
	char *path = NULL, *errmsg = NULL;
	const char *relPath = NULL, *locPath = NULL;

	LocalFileInfo *local;
	PcsFileInfo *meta;

	if (test_arg(arg, 2, 2, "f", "h", "help", NULL)) {
		usage_upload();
		return -1;
	}
	if (has_opts(arg, "h", "help", NULL)) {
		usage_upload();
		return 0;
	}

	is_force = has_opt(arg, "f");
	locPath = arg->argv[0];
	relPath = arg->argv[1];

	/*检查本地文件 - 开始*/
	local = GetLocalFileInfo(locPath);
	if (!local) {
		fprintf(stderr, "Error: The local file not exist.\n");
		return -1;
	}
	else if (local->isdir) {
		fprintf(stderr, "Error: The local file is directory. \nYou can use 'synch -cu' command to upload the directory.\n", locPath);
		DestroyLocalFileInfo(local);
		return -1;
	}
	DestroyLocalFileInfo(local);
	/*检查本地文件 - 结束*/

	//检查是否已经登录
	if (!is_login(context, NULL)) {
		return -1;
	}

	path = combin_net_disk_path(context->workdir, relPath);
	if (!path) {
		assert(path);
		return -1;
	}

	if (strcmp(path, "/") == 0) {
		fprintf(stderr, "Error: Can't specify root directory, you should specify the file name.\n");
		pcs_free(path);
		return -1;
	}

	/*检查网盘文件 - 开始*/
	meta = pcs_meta(context->pcs, path);
	if (meta && meta->isdir) {
		fprintf(stderr, "Error: The remote file exist, and it is directory. \n");
		pcs_fileinfo_destroy(meta);
		pcs_free(path);
		return -1;
	}
	else if (meta && !is_force){
		fprintf(stderr, "Error: The remote file exist. You can specify '-f' to force override.\n");
		pcs_fileinfo_destroy(meta);
		pcs_free(path);
		return -1;
	}
	else if (meta && is_force) {
		char *diskName = pcs_utils_filename(meta->path);
		if (diskName[0] == '.') { /*以点开头的文件在上传后点号会被自动去掉，
								  因此需要再次执行pcs_rename()函数重命名其为正确的名字。
								  此处预先删除该文件的目的是为了防止在do_upload()函数中重命名失败。*/
			PcsPanApiRes *res2;
			PcsSList sl2 = {
				meta->path, NULL
			};
			res2 = pcs_delete(context->pcs, &sl2);
			if (!res2 || res2->error != 0) {
				fprintf(stderr, "Error: Can't delete the %s, so can't override the file.\n", sl2.string);
				if (res2) { pcs_pan_api_res_destroy(res2); res2 = NULL; }
			}
			if (res2) { pcs_pan_api_res_destroy(res2); res2 = NULL; }
		}
		pcs_free(diskName);
	}
	/*检查网盘文件 - 结束*/

	/*开始上传*/
	if (do_upload(context,
		locPath, path, is_force ? PcsTrue : PcsFalse,
		"", context->workdir,
		&errmsg, NULL)) {
		fprintf(stderr, "Error: %s\n", errmsg);
		if (errmsg) pcs_free(errmsg);
		pcs_free(path);
		if (meta) pcs_fileinfo_destroy(meta);
		return -1;
	}
	printf("Upload %s to %s Success.\n", locPath, path);
	if (errmsg) pcs_free(errmsg);
	pcs_free(path);
	if (meta) pcs_fileinfo_destroy(meta);
	return 0;
}

/*打印版本*/
static int cmd_version(ShellContext *context, struct args *arg)
{
	if (test_arg(arg, 0, 0, "h", "help", NULL)) {
		usage_version();
		return -1;
	}
	if (has_opts(arg, "h", "help", NULL)) {
		usage_version();
		return 0;
	}
	version();
	return 0;
}

/*打印当前登录用户*/
static int cmd_who(ShellContext *context, struct args *arg)
{
	if (test_arg(arg, 0, 0, "h", "help", NULL)) {
		usage_who();
		return -1;
	}
	if (has_opts(arg, "h", "help", NULL)) {
		usage_who();
		return 0;
	}
	if (!is_login(context, NULL)) return -1;
	printf("%s\n", pcs_sysUID(context->pcs));
	return 0;
}

#pragma endregion

/*路由到具体的命令函数*/
static int exec_cmd(ShellContext *context, struct args *arg)
{
	int rc;
	char *cmd;

	if (!arg->cmd) {
		if (has_opt(arg, "v")
			|| has_opt(arg, "version")
			|| has_opt(arg, "version")) {
			rc = cmd_version(context, arg);
		}
		else {
			usage();
			rc = -1;
		}
		return rc;
	}

	cmd = arg->cmd;

	if (strcmp(cmd, "cat") == 0) {
		rc = cmd_cat(context, arg);
	}
	else if (strcmp(cmd, "cd") == 0) {
		rc = cmd_cd(context, arg);
	}
	else if (strcmp(cmd, "copy") == 0
		|| strcmp(cmd, "cp") == 0) {
		rc = cmd_copy(context, arg);
	}
	else if (strcmp(cmd, "compare") == 0
		|| strcmp(cmd, "c") == 0) {
		rc = cmd_compare(context, arg);
	}
	else if (strcmp(cmd, "context") == 0
		|| strcmp(cmd, "config") == 0) {
		rc = cmd_context(context, arg);
	}
	else if (strcmp(cmd, "download") == 0
		|| strcmp(cmd, "d") == 0) {
		rc = cmd_download(context, arg);
	}
	else if (strcmp(cmd, "echo") == 0) {
		rc = cmd_echo(context, arg);
	}
	else if (strcmp(cmd, "encode") == 0) {
		rc = cmd_encode(context, arg);
	}
	else if (strcmp(cmd, "list") == 0
		|| strcmp(cmd, "ls") == 0) {
		rc = cmd_list(context, arg);
	}
	else if (strcmp(cmd, "login") == 0) {
		rc = cmd_login(context, arg);
	}
	else if (strcmp(cmd, "logout") == 0) {
		rc = cmd_logout(context, arg);
	}
	else if (strcmp(cmd, "meta") == 0) {
		rc = cmd_meta(context, arg);
	}
	else if (strcmp(cmd, "mkdir") == 0) {
		rc = cmd_mkdir(context, arg);
	}
	else if (strcmp(cmd, "move") == 0
		|| strcmp(cmd, "mv") == 0) {
		rc = cmd_move(context, arg);
	}
	else if (strcmp(cmd, "pwd") == 0) {
		rc = cmd_pwd(context, arg);
	}
	else if (strcmp(cmd, "quota") == 0) {
		rc = cmd_quota(context, arg);
	}
	else if (strcmp(cmd, "remove") == 0
		|| strcmp(cmd, "rm") == 0) {
		rc = cmd_remove(context, arg);
	}
	else if (strcmp(cmd, "rename") == 0
		|| strcmp(cmd, "ren") == 0) {
		rc = cmd_rename(context, arg);
	}
	else if (strcmp(cmd, "set") == 0) {
		rc = cmd_set(context, arg);
	}
	else if (strcmp(cmd, "search") == 0) {
		rc = cmd_search(context, arg);
	}
	else if (strcmp(cmd, "synch") == 0
		|| strcmp(cmd, "s") == 0) {
		rc = cmd_synch(context, arg);
	}
	else if (strcmp(cmd, "upload") == 0
		|| strcmp(cmd, "u") == 0) {
		rc = cmd_upload(context, arg);
	}
	else if (strcmp(cmd, "version") == 0) {
		rc = cmd_version(context, arg);
	}
	else if (strcmp(cmd, "who") == 0) {
		rc = cmd_who(context, arg);
	}
	else if (strcmp(cmd, "help") == 0
		|| strcmp(cmd, "?") == 0) {
		rc = cmd_help(context, arg);
	}
	else {
		usage();
		rc = -1;
	}
	return rc;
}

int main(int argc, char *argv[])
{
	struct args arg = { 0 };
	ShellContext context = { 0 };
	int rc = 0;
	char *errmsg = NULL, *val = NULL;
	setlocale(LC_ALL, "chs");
	app_name = filename(argv[0]);
	if (parse_arg(&arg, argc, argv)) {
		usage();
		free_args(&arg);
		return -1;
	}
	hook_cjson();
	init_context(&context, &arg);
	if (has_optEx(&arg, "context", &val)) {
		if (restore_context(&context, val)) {
			rc = -1;
			goto exit_main;
		}
		remove_opt(&arg, "context", NULL);
	}
	else {
		restore_context(&context, NULL);
	}
	if (errmsg) {
		printf("%s\n", errmsg);
		pcs_free(errmsg);
	}
	init_pcs(&context);
	rc = exec_cmd(&context, &arg);
	save_context(&context);
exit_main:
	free_context(&context);
	free_args(&arg);
	pcs_mem_print_leak();
	return rc;
}
