#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#ifdef WIN32
#  include <conio.h>
#  include <direct.h>
#else
#  include <inttypes.h>
#  include <termios.h>
#endif

#include "pcs/pcs_mem.h"
#include "pcs/cJSON.h"
#include "pcs/pcs_utils.h"
#include "pcs/pcs.h"
#include "rb_tree/red_black_tree.h"
#include "version.h"
#include "dir.h"
#include "utils.h"
#include "shell.h"

#define USAGE "Mozilla/5.0 (Windows NT 6.3; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/35.0.1916.153 Safari/537.36"

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

	int		printed_count;
	int		page_index;

	const char	*local_basedir;
	const char	*remote_basedir;

	/*在打印meta后调用一次*/
	int (*process)(MyMeta *meta, struct RBEnumerateState *s, void *state);
	void *processState;

	int dry_run;
};

static PcsBool is_login(ShellContext *context, const char *msg);

#pragma region 获取默认路径

/*获取上下文存储文件路径*/
static const char *contextfile()
{
	static char filename[1024] = { 0 };

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
	puts("File Name");
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
	puts(f->path);
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

/*打印字符串。如果len传入-1，则本函数等价于puts，否则，只打印len指定大小的字符串*/
//static void print_str(const char *str, int len)
//{
//	int i;
//	if (len == -1) {
//		puts(str);
//	}
//	else {
//		for (i = 0; i < len; i++) {
//			putchar(str[i]);
//		}
//	}
//}

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
		printf("Can't open the file: %s\n", filename);
		pcs_free(json);
		return;
	}
	fwrite(json, 1, strlen(json), pf);
	fclose(pf);
	pcs_free(json);
}

/*还原保存的上下文*/
static void restore_context(ShellContext *context)
{
	const char *filename;
	char *filecontent = NULL;
	int filesize = 0;
	cJSON *root, *item;

	filename = contextfile();
	filesize = read_file(filename, &filecontent);
	if (filesize <= 0) {
		if (filecontent) pcs_free(filecontent);
		return;
	}
	root = cJSON_Parse(filecontent);
	if (!root) {
		printf("Broken JSON string \n%s", filecontent);
		pcs_free(filecontent);
		return;
	}

	item = cJSON_GetObjectItem(root, "cookiefile");
	if (item && item->valuestring && item->valuestring[0]) {
		if (!is_absolute_path(item->valuestring)) {
			printf("warning: invalid context.cookiefile, the value should be absolute path, default to %s.\n", context->cookiefile);
		}
		else {
			if (context->cookiefile) pcs_free(context->cookiefile);
			context->cookiefile = pcs_utils_strdup(item->valuestring);
		}
	}

	item = cJSON_GetObjectItem(root, "captchafile");
	if (item && item->valuestring && item->valuestring[0]) {
		if (!is_absolute_path(item->valuestring)) {
			printf("warning: invalid context.captchafile, the value should be absolute path, default to %s.\n", context->captchafile);
		}
		else {
			if (context->captchafile) pcs_free(context->captchafile);
			context->captchafile = pcs_utils_strdup(item->valuestring);
		}
	}

	item = cJSON_GetObjectItem(root, "workdir");
	if (item && item->valuestring && item->valuestring[0]) {
		if (item->valuestring[0] != '/') {
			printf("warning: invalid context.workdir, the value should be absolute path, default to %s.\n", context->workdir);
		}
		else {
			if (context->workdir) pcs_free(context->workdir);
			context->workdir = pcs_utils_strdup(item->valuestring);
		}
	}

	item = cJSON_GetObjectItem(root, "list_page_size");
	if (item) {
		if (((int)item->valueint) < 1) {
			printf("warning: invalid context.list_page_size, the value should be great than 0, default to %d.\n", context->list_page_size);
		}
		else {
			context->list_page_size = (int)item->valueint;
		}
	}

	item = cJSON_GetObjectItem(root, "list_sort_name");
	if (item && item->valuestring && item->valuestring[0]) {
		if (strcmp(item->valuestring, "name") && strcmp(item->valuestring, "time") && strcmp(item->valuestring, "size")) {
			printf("warning: invalid context.list_sort_name, the value should be one of [name|time|size], default to '%s'.\n", context->list_sort_name);
		}
		else {
			if (context->list_sort_name) pcs_free(context->list_sort_name);
			context->list_sort_name = pcs_utils_strdup(item->valuestring);
		}
	}

	item = cJSON_GetObjectItem(root, "list_sort_direction");
	if (item && item->valuestring && item->valuestring[0]) {
		if (strcmp(item->valuestring, "asc") && strcmp(item->valuestring, "desc")) {
			printf("warning: invalid context.list_sort_direction, the value should be one of [asc|desc], default to '%s'.\n", context->list_sort_direction);
		}
		else {
			if (context->list_sort_direction) pcs_free(context->list_sort_direction);
			context->list_sort_direction = pcs_utils_strdup(item->valuestring);
		}
	}

	item = cJSON_GetObjectItem(root, "secure_method");
	if (item && item->valuestring && item->valuestring[0]) {
		if (strcmp(item->valuestring, "plaintext") && strcmp(item->valuestring, "aes-cbc-128") && strcmp(item->valuestring, "aes-cbc-192") && strcmp(item->valuestring, "aes-cbc-256")) {
			printf("warning: invalid context.secure_method, the value should be one of [plaintext|aes-cbc-128|aes-cbc-192|aes-cbc-256], default to '%s'.\n", context->secure_method);
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

	cJSON_Delete(root);
	pcs_free(filecontent);
}

/*初始化上下文*/
static void init_context(ShellContext *context, int argc, char *argv[])
{
	memset(context, 0, sizeof(ShellContext));
	context->name = pcs_utils_strdup(filename(argv[0]));
	context->cookiefile = pcs_utils_strdup(cookiefile());
	context->captchafile = pcs_utils_strdup(captchafile());
	context->workdir = pcs_utils_strdup("/");
	context->list_page_size = PRINT_PAGE_SIZE;
	context->list_sort_name = pcs_utils_strdup("name");
	context->list_sort_direction = pcs_utils_strdup("asc");
	
	context->secure_method = pcs_utils_strdup("plaintext");
	context->secure_key = pcs_utils_strdup("");
	context->secure_enable = 0;
}

/*释放上下文*/
static void free_context(ShellContext *context)
{
	if (context->name) pcs_free(context->name);
	if (context->cookiefile) pcs_free(context->cookiefile);
	if (context->captchafile) pcs_free(context->captchafile);
	if (context->workdir) pcs_free(context->workdir);
	if (context->list_sort_name) pcs_free(context->list_sort_name);
	if (context->list_sort_direction) pcs_free(context->list_sort_direction);
	if (context->pcs) pcs_destroy(context->pcs);
	if (context->secure_method) pcs_free(context->secure_method);
	if (context->secure_key) pcs_free(context->secure_key);
	memset(context, 0, sizeof(ShellContext));
}

/*初始化PCS的安全选项*/
static void init_pcs_secure(ShellContext *context)
{
	int method = 0;
	if (context->secure_method && context->secure_method[0]) {
		if (strcmp(context->secure_method, "aes-cbc-128")) {
			method = PCS_SECURE_AES_CBC_128;
		}
		else if (strcmp(context->secure_method, "aes-cbc-192")) {
			method = PCS_SECURE_AES_CBC_192;
		}
		else if (strcmp(context->secure_method, "aes-cbc-256")) {
			method = PCS_SECURE_AES_CBC_256;
		}
		else if (strcmp(context->secure_method, "plaintext")) {
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
		PCS_OPTION_END);
	init_pcs_secure(context);
}

#pragma endregion

#pragma region 打印用法相关函数

/*打印版本*/
static void version(ShellContext *context)
{
	printf(program_full_name "\n", context->name);
}

/*打印cat命令用法*/
static void usage_cat(ShellContext *context)
{
	version(context);
	printf("\nUsage: %s cat <path>\n", context->name);
	printf("\nDescription:\n");
	printf("  Print the file content\n");
	printf("\nSamples:\n");
	printf("  %s cat /music/list.txt\n", context->name);
	printf("  %s cat list.txt\n", context->name);
	printf("  %s cat \"/music/Europe and America/list.txt\"\n", context->name);
}

/*打印cd命令用法*/
static void usage_cd(ShellContext *context)
{
	version(context);
	printf("\nUsage: %s cd <path>\n", context->name);
	printf("\nDescription:\n");
	printf("  Change the work directory\n");
	printf("\nSamples:\n");
	printf("  %s cd /music\n", context->name);
	printf("  %s cd subdir\n", context->name);
	printf("  %s cd \"/music/Europe and America\"\n", context->name);
}

/*打印copy命令用法*/
static void usage_copy(ShellContext *context)
{
	version(context);
	printf("\nUsage: %s copy <src> <dst>\n", context->name);
	printf("\nDescription:\n");
	printf("  Copy the file|directory\n");
	printf("\nSamples:\n");
	printf("  %s copy src.txt dst.txt\n", context->name);
	printf("  %s copy /music/src.mp3 /music/mp3/dst.mp3\n", context->name);
	printf("  %s copy /music/src.mp3 \"/music/Europe and America/dst.mp3\"\n", context->name);
}

/*打印compare命令用法*/
static void usage_compare(ShellContext *context)
{
	version(context);
	printf("\nUsage: %s compare [-redu] <local path> <net disk path>\n", context->name);
	printf("\nDescription:\n");
	printf("  Print the differents between local and net disk. \n"
		   "  Default options is '-cdu'. \n");
	printf("\nOptions:\n");
	printf("  -c    Print the files that confuse, which is don't know how to do.\n");
	printf("  -d    Print the files that is old than the net disk.\n");
	printf("  -e    Print the files that is same between local and net disk.\n");
	printf("  -r    Recursive compare the sub directories.\n");
	printf("  -u    Print the files that is newer than the net disk.\n");
	printf("\nSamples:\n");
	printf("  %s compare ~/music /music\n", context->name);
	printf("  %s compare music /music\n", context->name);
	printf("  %s compare -r music /music\n", context->name);
}

/*打印context命令用法*/
static void usage_context(ShellContext *context)
{
	version(context);
	printf("\nUsage: %s context\n", context->name);
	printf("\nDescription:\n");
	printf("  Print the context\n");
	printf("\nSamples:\n");
	printf("  %s context\n", context->name);
}

/*打印download命令用法*/
static void usage_download(ShellContext *context)
{
	version(context);
	printf("\nUsage: %s download [-f] <file> <local file>\n", context->name);
	printf("\nDescription:\n");
	printf("  Download the file\n");
	printf("\nOptions:\n");
	printf("  -f    Force override the local file when the file exists on local file system.\n");
	printf("\nSamples:\n");
	printf("  %s download dst.txt ~/dst.txt\n", context->name);
	printf("  %s download dst.txt dst.txt\n", context->name);
	printf("  %s download -f dst.txt ~/dst.txt\n", context->name);
	printf("  %s download \"/music/dst.mp3\" \"/home/pcs/music/dst.mp3\"\n", context->name);
}

/*打印echo命令用法*/
static void usage_echo(ShellContext *context)
{
	version(context);
	printf("\nUsage: %s echo [-a] <path> <text>\n", context->name);
	printf("\nDescription:\n");
	printf("  Write the text into net disk file\n");
	printf("\nOptions:\n");
	printf("  -a    Append the text.\n");
	printf("\nSamples:\n");
	printf("  %s echo src.txt \"This is from 'echo' command.\" \n", context->name);
	printf("  %s echo /docs/src.txt \"This is from 'echo' command.\"\n", context->name);
	printf("  %s echo -a \"for test/src.txt\" \"This is from 'echo' command.\"\n", context->name);
}

/*打印help命令用法*/
static void usage_help(ShellContext *context)
{
	version(context);
	printf("\nUsage: %s help\n", context->name);
	printf("\nDescription:\n");
	printf("  Print the usage\n");
	printf("\nSamples:\n");
	printf("  %s help\n", context->name);
}

/*打印list命令用法*/
static void usage_list(ShellContext *context)
{
	version(context);
	printf("\nUsage: %s list [path]\n", context->name);
	printf("\nDescription:\n");
	printf("  List the directory\n");
	printf("\nSamples:\n");
	printf("  %s list\n", context->name);
	printf("  %s list /music\n", context->name);
	printf("  %s list \"/music/Europe and America\"\n", context->name);
}

/*打印login命令用法*/
static void usage_login(ShellContext *context)
{
	version(context);
	printf("\nUsage: %s login [user name] [password]\n", context->name);
	printf("\nDescription:\n");
	printf("  \n");
	printf("\nSamples:\n");
	printf("  %s login\n", context->name);
	printf("  %s login gang\n", context->name);
	printf("  %s login gang \"password\"\n", context->name);
}

/*打印logout命令用法*/
static void usage_logout(ShellContext *context)
{
	version(context);
	printf("\nUsage: %s logout\n", context->name);
	printf("\nDescription:\n");
	printf("  \n");
	printf("\nSamples:\n");
	printf("  %s logout\n", context->name);
}

/*打印meta命令用法*/
static void usage_meta(ShellContext *context)
{
	version(context);
	printf("\nUsage: %s meta [path]\n", context->name);
	printf("\nDescription:\n");
	printf("  Print the file|directory meta information\n");
	printf("\nSamples:\n");
	printf("  %s meta\n", context->name);
	printf("  %s meta /music\n", context->name);
	printf("  %s meta \"/music/Europe and America\"\n", context->name);
}

/*打印mkdir命令用法*/
static void usage_mkdir(ShellContext *context)
{
	version(context);
	printf("\nUsage: %s mkdir <path>\n", context->name);
	printf("\nDescription:\n");
	printf("  Make a new directory\n");
	printf("\nSamples:\n");
	printf("  %s mkdir subdir\n", context->name);
	printf("  %s mkdir /music\n", context->name);
	printf("  %s mkdir \"/music/Europe and America\"\n", context->name);
}

/*打印move命令用法*/
static void usage_move(ShellContext *context)
{
	version(context);
	printf("\nUsage: %s move <src> <dst>\n", context->name);
	printf("\nDescription:\n");
	printf("  Move the file|directory into other file|directory\n");
	printf("\nSamples:\n");
	printf("  %s move src.txt dst.txt\n", context->name);
	printf("  %s move /music/src.mp3 /music/mp3/dst.mp3\n", context->name);
	printf("  %s move /music/src.mp3 \"/music/Europe and America/dst.mp3\"\n", context->name);
}

/*打印pwd命令用法*/
static void usage_pwd(ShellContext *context)
{
	version(context);
	printf("\nUsage: %s pwd\n", context->name);
	printf("\nDescription:\n");
	printf("  Print the current work directory\n");
	printf("\nSamples:\n");
	printf("  %s pwd\n", context->name);
}

/*打印quota命令用法*/
static void usage_quota(ShellContext *context)
{
	version(context);
	printf("\nUsage: %s quota\n", context->name);
	printf("\nOptions:\n");
	printf("  -e    Print the exact size.\n");
	printf("\nDescription:\n");
	printf("  Print the quota\n");
	printf("\nSamples:\n");
	printf("  %s quota\n", context->name);
}

/*打印remove命令用法*/
static void usage_remove(ShellContext *context)
{
	version(context);
	printf("\nUsage: %s remove <path>\n", context->name);
	printf("\nDescription:\n");
	printf("  Remove the file|directory\n");
	printf("\nSamples:\n");
	printf("  %s remove src.txt\n", context->name);
	printf("  %s remove /music/src.mp3\n", context->name);
	printf("  %s remove \"/music/Europe and America/dst.mp3\"\n", context->name);
}

/*打印rename命令用法*/
static void usage_rename(ShellContext *context)
{
	version(context);
	printf("\nUsage: %s rename <src> <new name>\n", context->name);
	printf("\nDescription:\n");
	printf("  Rename the file|directory\n");
	printf("\nSamples:\n");
	printf("  %s rename src.txt dst.txt\n", context->name);
	printf("  %s rename /music/src.mp3 dst.mp3\n", context->name);
	printf("  %s rename \"/music/Europe and America/src.mp3\" \"dst 2.mp3\"\n", context->name);
}

/*打印list命令用法*/
static void usage_set(ShellContext *context)
{
	version(context);
	printf("\nUsage: %s set <option>=<value> ...\n", context->name);
	printf("\nDescription:\n");
	printf("  Change the context, you can print the context by 'context' command\n");
	printf("\nOptions:\n");
	printf("  Option Name         Type       Possible Values \n");
	printf("  -----------------------------------------------\n");
	printf("  captchafile         String     not null\n");
	printf("  cookiefile          String     not null\n");
	printf("  list_page_size      UInt       >0\n");
	printf("  list_sort_direction Enum       asc|desc\n");
	printf("  list_sort_name      Enum       name|time|size\n");
	printf("  secure_enable       Boolean    true|false\n");
	printf("  secure_key          String     not null when 'secure_method' is not 'plaintext'\n");
	printf("  secure_method       Enum       plaintext|aes-cbc-128|aes-cbc-192|aes-cbc-256\n");
	printf("\nSamples:\n");
	printf("  %s set cookiefile=\"/tmp/pcs.cookie\"\n", context->name);
	printf("  %s set cookiefile=\"/tmp/pcs.cookie\" captchafile=\"/tmp/vc.git\"\n", context->name);
	printf("  %s set list_page_size=20 list_sort_name=name list_sort_direction=desc\n", context->name);
}

/*打印search命令用法*/
static void usage_search(ShellContext *context)
{
	version(context);
	printf("\nUsage: %s search [-r] [target dir] <key word>\n", context->name);
	printf("\nDescription:\n");
	printf("  Search the files in the specify directory\n");
	printf("\nOptions:\n");
	printf("  -r    Recursive search the sub directories.\n");
	printf("\nSamples:\n");
	printf("  %s search dst.txt\n", context->name);
	printf("  %s search -r dst.txt\n", context->name);
	printf("  %s search /music dst.mp3\n", context->name);
	printf("  %s search -r /music dst.mp3\n", context->name);
	printf("  %s search \"/music/Europe and America\" \"dst 2.mp3\"\n", context->name);
}

/*打印synch命令用法*/
static void usage_synch(ShellContext *context)
{
	version(context);
	printf("\nUsage: %s synch [-redu] <local path> <net disk path>\n", context->name);
	printf("\nDescription:\n");
	printf("  Synch between local and net disk. You can 'compare' first. \n"
		   "  Default options is '-cdu'. \n");
	printf("\nOptions:\n");
	printf("  -c    Print the files that confuse, which is don't know how to do.\n");
	printf("  -d    Synch the new files from the net disk. \n"
		   "        This option will download the new files from the net disk.\n"
		   "        You can use 'compare -dr <local dir> <disk dir>' to view \n"
		   "        how many and which files will download.\n");
	printf("  -e    Print the files that is same between local and net disk.\n");
	printf("  -n    Dry run.\n");
	printf("  -r    Recursive synch the sub directories.\n");
	printf("  -u    Synch the new files to the net disk.\n \n"
		   "        This option will upload new files from the net disk.\n"
		   "        You can use 'compare -ur <local dir> <disk dir>' to view \n"
		   "        how many and which files will upload.\n");
	printf("\nSamples:\n");
	printf("  %s synch ~/music /music\n", context->name);
	printf("  %s synch music /music\n", context->name);
	printf("  %s synch -r music /music\n", context->name);
}

/*打印upload命令用法*/
static void usage_upload(ShellContext *context)
{
	version(context);
	printf("\nUsage: %s upload [-f] <local file> <file>\n", context->name);
	printf("\nDescription:\n");
	printf("  Upload the file\n");
	printf("\nOptions:\n");
	printf("  -f    Force override the local file when the file exists on local file system.\n");
	printf("\nSamples:\n");
	printf("  %s upload ~/dst.txt dst.txt\n", context->name);
	printf("  %s upload dst.txt dst.txt\n", context->name);
	printf("  %s upload -f ~/dst.txt dst.txt\n", context->name);
	printf("  %s upload \"/home/pcs/music/dst.mp3\" \"/music/dst.mp3\"\n", context->name);
}

/*打印version命令用法*/
static void usage_version(ShellContext *context)
{
	version(context);
	printf("\nUsage: %s version\n", context->name);
	printf("\nDescription:\n");
	printf("  Print the version\n");
	printf("\nSamples:\n");
	printf("  %s version\n", context->name);
}

/*打印who命令用法*/
static void usage_who(ShellContext *context)
{
	version(context);
	printf("\nUsage: %s who\n", context->name);
	printf("\nDescription:\n");
	printf("  Print the current user\n");
	printf("\nSamples:\n");
	printf("  %s who\n", context->name);
}

/*打印用法*/
static void usage(ShellContext *context)
{
	version(context);
	printf("\nUsage: %s command [options] [arg1|arg2...]\n", context->name);
	printf("\nDescription:\n");
	printf("  The %s is client of baidu net disk. It supplied many functions, \n"
		"  which can manage baidu net disk on terminal, such as ls, cp, rm, \n"
		"  mv, rename, download, upload, search and so on. \n"
		"  The %s provided AES encryption, which can protected your data.\n", 
		context->name, context->name, context->name);
	printf("\nCommands:\n"
		"  cat      Print the file content\n"
		"  cd       Change the work directory\n"
		"  copy     Copy the file|directory\n"
		"  compare  Print the differents between local and net disk\n"
		"  context  Print the context\n"
		"  download Download the file\n"
		"  echo     Write the text into net disk file\n"
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
	printf("Use '%s <command> -h' to print the details of the command. \n", context->name);
	printf("Sample: \n");
	printf("  %s cat -h\n", context->name);
	printf("  %s cd /temp\n", context->name);
}

#pragma endregion

/*判断是否需要打印usage*/
static inline int is_print_usage(int argc, char *argv[])
{
	return (argc == 1 && (strcmp(argv[0], "-h") == 0 || strcmp(argv[0], "-?") == 0 || strcmp(argv[0], "--help") == 0));
}

/*
 * 检查是否登录，如果未登录则登录
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
	if (strcmp(val, "true") == 0) {
		context->secure_enable = 1;
	}
	else if (strcmp(val, "false") == 0) {
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
	rb_red_blk_tree *rb = (rb_red_blk_tree *)state;
	MyMeta *meta;

	fix_unix_path(info->path);
	meta = meta_create(info->path);
	meta->flag |= FLAG_ON_LOCAL;
	meta->local_mtime = info->mtime;
	meta->local_isdir = info->isdir;
	meta->parent = (parent ? (MyMeta *)parent->userdata : NULL);
	info->userdata = meta;
	RBTreeInsert(rb, (void *)meta->path, (void *)meta);
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

	rb = RBTreeCreate(&rb_compare, &rb_destory_key, &rb_destory_info, &rb_print_key, &rb_print_info);

	cnt = GetDirectoryFiles(&link, dir, recursive, &onGotLocalFile, rb);
	if (cnt < 0) {
		RBTreeDestroy(rb);
		return NULL;
	}
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
		if ((meta->local_isdir && !meta->remote_isdir) || (!meta->local_isdir && meta->remote_isdir)) {
			meta->op = OP_CONFUSE;
		}
		else {
			if (meta->local_mtime < meta->remote_mtime)
				meta->op = OP_LEFT;
			else if (meta->local_mtime > meta->remote_mtime)
				meta->op = OP_RIGHT;
			else
				meta->op = OP_EQ;
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
static void print_meta_list_head(int first, int second, int other, int page_index)
{
	int i, total = 0;
	if (page_index) {
		printf("\nPAGE#%d\n", page_index);
	}
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
		printf("%s", meta->op_st == OP_ST_SUCC ? "[ ok ] "
			: (meta->op_st == OP_ST_FAIL ? "[fail] "
			: (meta->op_st == OP_ST_SKIP ? "[skip] "
			: (meta->op_st == OP_ST_CONFUSE ? "[confus]"
			: (meta->op_st == OP_ST_PROCESSING ? "[ing...]"
			: "[    ] ")))));
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
	printf("%s ", meta->op == OP_LEFT ? "<-"
		: (meta->op == OP_RIGHT ? "->"
		: (meta->op == OP_EQ ? "=="
		: (meta->op == OP_CONFUSE ? "><"
		: "  "))));
	if (meta->flag & FLAG_ON_REMOTE) {
		printf("%s%s%s\n", meta->remote_path, meta->msg ? "\t " : "", meta->msg ? meta->msg : "");
	}
	else {
		printf("%s%s\n", meta->msg ? "\t " : "", meta->msg ? meta->msg : "");
	}
}

/*
* 打印显示meta时的列表头
*   first  - 第一列宽度，第一列为操作成功还是失败的标记列。不存在时，传入0
*   second - 第二列宽度，第二列为本地文件的路径
*   other  - 剩下列的总宽度，不包括第三列。第三列宽度为固定值2
*/
static void print_meta_list_foot(int first, int second, int other)
{
	//int i;
	//int total = 0;
	//if (first > 0) total += first + 1;
	//total += second + 1;
	//total += 2 + 1; /*2为列头"OP"的长度，1为分隔空白*/
	//total += other;
	//for (i = 0; i < total; i++) putchar('-');
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
		print_meta_list_head(s->first, s->second, s->other, s->page_size > 0 ? s->page_index : 0);
	}
	else if (s->page_size > 0 && (s->printed_count % s->page_size) == 0) {
		s->page_index++;
		printf("\nPrint next page#%d [Y|N]? ", s->page_index);
		std_string(tmp, 8);
		if (strlen(tmp) != 0 && pcs_utils_strcmpi(tmp, "y") && pcs_utils_strcmpi(tmp, "yes")) {
			return -1;
		}
		print_meta_list_head(s->first, s->second, s->other, s->page_index);
	}

	if (s->process)
		meta->op_st = OP_ST_PROCESSING;

	print_meta_list_row(s->first, s->second, s->other, meta);
	
	if (s->process) {
		int rc;
		if ((rc = (*s->process)(meta, s, s->processState))) {
			clear_current_print_line();
			back_prev_print_line();
			if (meta->op_st == OP_ST_PROCESSING) meta->op_st = OP_ST_FAIL;
			print_meta_list_row(s->first, s->second, s->other, meta);
			return rc;
		}
		clear_current_print_line();
		back_prev_print_line();
		print_meta_list_row(s->first, s->second, s->other, meta);
	}

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
	return 0;
}

#pragma endregion

#pragma region 各命令函数体

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
				(*pErrMsg) = pcs_utils_strdup("Error: Can't create the directory.");
			}
			if (op_st) (*op_st) = OP_ST_FAIL;
			pcs_free(dir);
			pcs_free(local_path);
			return -1;
		}
		pcs_free(dir);
	}

	tmp_local_path = (char *)pcs_malloc(strlen(local_path) + 14);
	strcpy(tmp_local_path, local_path);
	strcat(tmp_local_path, ".pcs_download");

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

	dir = combin_unix_path(context->workdir, remote_basedir);
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
	remote_path = combin_unix_path(dir, remote_file);
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
		PCS_OPTION_END);
	res = pcs_download(context->pcs, remote_path);
	fclose(ds.pf);
	if (res != PCS_OK) {
		if (pErrMsg) {
			if (*pErrMsg) pcs_free(*pErrMsg);
			(*pErrMsg) = pcs_utils_sprintf("Error: %s. remote_path=%s\n", pcs_strerror(context->pcs), remote_path);
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
			(*pErrMsg) = pcs_utils_sprintf("Error: rename error. old=%s, new=%s\n", tmp_local_path, local_path);
		}
		if (op_st) (*op_st) = OP_ST_FAIL;
		DeleteFileRecursive(tmp_local_path);
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
	dir = combin_unix_path(context->workdir, remote_basedir);
	remote_path = combin_unix_path(dir, remote_file);
	pcs_free(dir);
	pcs_setopts(context->pcs,
		PCS_OPTION_PROGRESS_FUNCTION, &upload_progress,
		PCS_OPTION_PROGRESS_FUNCTION_DATE, NULL,
		PCS_OPTION_PROGRESS, (void *)((long)PcsTrue),
		PCS_OPTION_END);
	res = pcs_upload(context->pcs, remote_path, is_force, local_path);
	if (!res || !res->path || !res->path[0]) {
		if (pErrMsg) {
			if (*pErrMsg) pcs_free(*pErrMsg);
			(*pErrMsg) = pcs_utils_sprintf("Error: %s\n", pcs_strerror(context->pcs));
		}
		if (op_st) (*op_st) = OP_ST_FAIL;
		if (res) pcs_fileinfo_destroy(res);
		pcs_free(local_path);
		pcs_free(remote_path);
		return -1;
	}
	if (op_st) (*op_st) = OP_ST_SUCC;
	if (res) pcs_fileinfo_destroy(res);
	pcs_free(local_path);
	pcs_free(remote_path);
	return 0;
}

/*打印网盘文件内容*/
static int cmd_cat(ShellContext *context, int argc, char *argv[])
{
	char *path;
	const char *res;
	size_t sz;
	if (is_print_usage(argc, argv)) {
		usage_cat(context);
		return 0;
	}
	if (argc != 1) {
		usage_cat(context);
		return -1;
	}
	if (!is_login(context, NULL)) return -1;
	path = combin_unix_path(context->workdir, argv[0]);
	assert(path);
	res = pcs_cat(context->pcs, path, &sz);
	if (res == NULL) {
		printf("Error: %s path=%s.\n", pcs_strerror(context->pcs), path);
		pcs_free(path);
		return -1;
	}
	pcs_free(path);
	printf("%s\n\n", res);
	return 0;
}

/*更改网盘当前工作目录*/
static int cmd_cd(ShellContext *context, int argc, char *argv[])
{
	char *p;
	PcsFileInfo *meta;
	if (is_print_usage(argc, argv)) {
		usage_cd(context);
		return 0;
	}
	if (argc != 1) {
		usage_cd(context);
		return -1;
	}
	if (!is_login(context, NULL)) return -1;
	p = combin_unix_path(context->workdir, argv[0]);
	meta = pcs_meta(context->pcs, p);
	if (!meta) {
		printf("The target directory not exist, or have error: %s\n", pcs_strerror(context->pcs));
		pcs_free(p);
		return -1;
	}
	if (!meta->isdir) {
		printf("The target is not directory\n");
		pcs_free(p);
		pcs_fileinfo_destroy(meta);
		return -1;
	}
	pcs_fileinfo_destroy(meta);
	if (context->workdir) pcs_free(context->workdir);
	context->workdir = pcs_utils_strdup(p);
	pcs_free(p);
	printf("Work directory changed to %s\n", context->workdir);
	return 0;
}

/*复制网盘文件*/
static int cmd_copy(ShellContext *context, int argc, char *argv[])
{
	PcsPanApiRes *res;
	PcsPanApiResInfoList *info;
	PcsSList2 slist;
	if (is_print_usage(argc, argv)) {
		usage_copy(context);
		return 0;
	}
	if (argc != 2) {
		usage_copy(context);
		return -1;
	}
	if (!is_login(context, NULL)) return -1;

	slist.string1 = combin_unix_path(context->workdir, argv[0]); /* path */
	slist.string2 = combin_unix_path(context->workdir, argv[1]); /* new_name */
	slist.next = NULL;

	res = pcs_copy(context->pcs, &slist);
	if (!res) {
		printf("Error: %s src=%s, dst=%s.\n", pcs_strerror(context->pcs), slist.string1, slist.string2);
		pcs_free(slist.string1);
		pcs_free(slist.string2);
		return -1;
	}
	info = res->info_list;
	if (info->info.error) {
		printf("Error: unknow. src=%s, dst=%s. \n", slist.string1, slist.string2);
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
static int parse_compare_args(int argc, char *argv[], compare_arg *arg)
{
	int i;
	char *p;
	if (argc < 2) return -1;
	for (i = 0; i < argc; i++) {
		p = argv[i];
		if (*p == '-') {
			p++;
			while (*p) {
				switch (*p) {
				case 'c':
					if (arg->print_confuse) return -1; /*如果多次指定'-c'参数，则返回错误*/
					arg->print_confuse = 1;
					break;
				case 'd':
					if (arg->print_left) return -1;
					arg->print_left = 1;
					break;
				case 'e':
					if (arg->print_eq) return -1;
					arg->print_eq = 1;
					break;
				case 'n':
					if (arg->dry_run) return -1;
					arg->dry_run = 1;
					break;
				case 'r':
					if (arg->recursive) return -1;
					arg->recursive = 1;
					break;
				case 'u':
					if (arg->print_right) return -1;
					arg->print_right = 1;
					break;
				default:
					return -1;
				}
				p++;
			}
		}
		else if (!arg->local_file) arg->local_file = argv[i];
		else if (!arg->remote_file) arg->remote_file = argv[i];
		else return -1;
	}
	if (!arg->local_file || !arg->remote_file) return -1;
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
		page_size = 100;
	int cnt = 0;
	rb_red_blk_node *rbn;
	MyMeta *meta;

	while (1) {
		list = pcs_list(context->pcs, remote_dir,
			page_index, page_size,
			"name", PcsFalse);
		if (!list) {
			if (pcs_strerror(context->pcs)) {
				printf("Error: %s \n", pcs_strerror(context->pcs));
				return -1;
			}
			return 0;
		}

		cnt = list->count;
		if (total_cnt) (*total_cnt) += cnt;
		if (total_cnt && cnt > 0) {
			printf("Fetch %d Metas                     \r", *total_cnt);
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
	print_meta_list_head(first, second, other, 0);
	print_meta_list_row(first, second, other, mm);
	print_meta_list_foot(first, second, other);
	return 0;
}

static int on_compared_dir(ShellContext *context, compare_arg *arg, rb_red_blk_tree *rb, void *st)
{
	struct RBEnumerateState state = { 0 };
	if (!arg->print_eq && !arg->print_left && !arg->print_right && !arg->print_confuse)
		state.print_op = OP_LEFT | OP_RIGHT | OP_CONFUSE;
	if (arg->print_eq) state.print_op |= OP_EQ;
	if (arg->print_left) state.print_op |= OP_LEFT;
	if (arg->print_right) state.print_op |= OP_RIGHT;
	if (arg->print_confuse) state.print_op |= OP_CONFUSE;
	state.print_flag = FLAG_ON_LOCAL | FLAG_ON_REMOTE;
	state.no_print_flag = FLAG_PARENT_NOT_ON_REMOTE;
	state.tree = rb;
	state.context = context;
	state.page_size = context->list_page_size;
	state.page_index = 1;
	state.dry_run = arg->dry_run;
	state.local_basedir = arg->local_file;
	state.remote_basedir = arg->remote_file;
	rb->enumerateInfoState = &state;
	rb->EnumerateInfo = &rb_decide_op;
	RBTreeEnumerateInfo(rb);
	state.first = 0;
	if (state.second < 10) state.second = 10;
	state.other = 13;
	if (arg->onRBEnumerateStatePrepared)
		(*arg->onRBEnumerateStatePrepared)(context, arg, rb, &state, st);
	rb->EnumerateInfo = &rb_print_meta;
	RBTreeEnumerateInfo(rb);
	if (state.printed_count == 0)
		print_meta_list_head(state.first, state.second, state.other, 0);
	print_meta_list_foot(state.first, state.second, state.other);
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
		printf("Error: The local file not exist.\n");
		return -1;
	}
	/*检查本地文件 - 结束*/

	//检查是否已经登录
	if (!is_login(context, NULL)) {
		DestroyLocalFileInfo(local);
		return -1;
	}

	//拼接出网盘文件的完整路径
	path = combin_unix_path(context->workdir, arg->remote_file);
	if (!path) {
		DestroyLocalFileInfo(local);
		return -1;
	}

	/*获取网盘文件元数据*/
	remote = pcs_meta(context->pcs, path);
	if (!remote) {
		printf("Error: The remote file not exist, or have error: %s\n", pcs_strerror(context->pcs));
		DestroyLocalFileInfo(local);
		pcs_free(path);
		return -1;
	}

	/*本地是文件，远端是目录*/
	if (!local->isdir && remote->isdir) {
		printf("Error: Can't compare the file with directory.\n");
		DestroyLocalFileInfo(local);
		pcs_fileinfo_destroy(remote);
		pcs_free(path);
		return -1;
	}

	/*本地是目录，远端是文件*/
	if (local->isdir && !remote->isdir) {
		printf("Error: Can't compare the directory with file.\n");
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
		rb = meta_load(arg->local_file, arg->recursive);
		if (!rb) {
			printf("Error: Can't list the local directory.\n");
			DestroyLocalFileInfo(local);
			pcs_fileinfo_destroy(remote);
			pcs_free(path);
			return -1;
		}
		skip = strlen(remote->path);
		if (remote->path[skip - 1] != '/' && remote->path[skip - 1] != '\\') skip++;
		if (combin_with_remote_dir_files(context, rb, remote->path, arg->recursive, skip, &total_cnt, arg->check_local_dir_exist)) {
			printf("Error: Can't list the remote directory.\n");
			RBTreeDestroy(rb);
			DestroyLocalFileInfo(local);
			pcs_fileinfo_destroy(remote);
			pcs_free(path);
			return -1;
		}
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
static int cmd_compare(ShellContext *context, int argc, char *argv[])
{
	compare_arg arg = { 0 };

	if (is_print_usage(argc, argv) || parse_compare_args(argc, argv, &arg)) {
		usage_compare(context);
		return 0;
	}

	arg.check_local_dir_exist = 1;

	return compare(context, &arg, &on_compared_file, NULL, &on_compared_dir, NULL);
}

#pragma endregion

/*打印出上下文*/
static int cmd_context(ShellContext *context, int argc, char *argv[])
{
	char *json;
	if (is_print_usage(argc, argv)) {
		usage_context(context);
		return 0;
	}
	json = context2str(context);
	assert(json);
	printf("%s\n", json);
	pcs_free(json);
	return 0;
}

/*下载*/
static int cmd_download(ShellContext *context, int argc, char *argv[])
{
	int is_force = 0;
	char *path = NULL, *errmsg = NULL;
	const char *relPath = NULL, *locPath = NULL;
	int i;

	LocalFileInfo *local;
	PcsFileInfo *meta;

	if (is_print_usage(argc, argv)) {
		usage_download(context);
		return 0;
	}
	if (argc != 2 && argc != 3) {
		usage_download(context);
		return -1;
	}
	/*解析参数 - 开始*/
	if (argc == 2){
		if (strcmp(argv[0], "-f") == 0 || strcmp(argv[1], "-f") == 0) {
			usage_download(context);
			return -1;
		}
		relPath = argv[0];
		locPath = argv[1];
	}
	else if (argc == 3) {
		for (i = 0; i < argc; i++) {
			if (is_force == 0 && strcmp(argv[i], "-f") == 0) {
				is_force = 1;
			}
			else if (relPath == NULL) {
				relPath = argv[i];
			}
			else if (locPath == NULL) {
				locPath = argv[i];
			}
			else {
				usage_download(context);
				return -1;
			}
		}
		if (is_force == 0) {
			usage_download(context);
			return -1;
		}
	}
	if (!locPath || !relPath) {
		usage_download(context);
		return -1;
	}
	/*解析参数 - 结束*/

	/*检查本地文件 - 开始*/
	local = GetLocalFileInfo(locPath);
	if (local && local->isdir) {
		printf("Error: The local file exist, but it's a directory.\n");
		DestroyLocalFileInfo(local);
		return -1;
	}
	else if (local && !local->isdir) {
		if (!is_force) {
			printf("Error: The local file exist. You can specify '-f' to force override.\n", locPath);
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

	path = combin_unix_path(context->workdir, relPath);

	/*检查网盘文件 - 开始*/
	meta = pcs_meta(context->pcs, path);
	if (!meta) {
		printf("Error: The remote file not exist, or have error: %s\n", pcs_strerror(context->pcs));
		pcs_free(path);
		return -1;
	}
	if (meta->isdir) {
		printf("Error: The remote file is directory. \nYou can use 'synch -cd' command to download the directory.\n");
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
		printf("Error: %s\n", errmsg);
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
static int cmd_echo(ShellContext *context, int argc, char *argv[])
{
	int is_append = 0;
	char *path = NULL, *t = NULL;
	const char *relPath = NULL, *text = NULL;
	int i;
	size_t sz;
	PcsFileInfo *f;
	PcsFileInfo *meta;
	if (is_print_usage(argc, argv)) {
		usage_echo(context);
		return 0;
	}
	if (argc != 2 && argc != 3) {
		usage_echo(context);
		return -1;
	}
	/*解析参数 - 开始*/
	if (argc == 2) {
		relPath = argv[0];
		text = argv[1];
	}
	else {
		for (i = 0; i < argc; i++) {
			if (is_append == 0 && strcmp(argv[i], "-a") == 0) {
				is_append = 1;
			}
			else if (relPath == NULL) {
				relPath = argv[i];
			}
			else if (text == NULL) {
				text = argv[i];
			}
			else {
				usage_echo(context);
				return -1;
			}
		}
		if (!is_append) {
			usage_echo(context);
			return -1;
		}
	}
	/*解析参数 - 结束*/
	if (!is_login(context, NULL)) return -1;
	path = combin_unix_path(context->workdir, relPath);
	assert(path);
	sz = strlen(text);

	/*检查网盘文件 - 开始*/
	meta = pcs_meta(context->pcs, path);
	if (meta && meta->isdir) {
		printf("Error: The remote file is directory. \n");
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
			printf("Error: %s path=%s.\n", pcs_strerror(context->pcs), path);
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
		printf("Error: %s. \n", pcs_strerror(context->pcs), path);
		pcs_free(path);
		if (t != text) pcs_free(t);
		return -1;
	}
	printf("Success. You can use '%s cat %s' to print the file content.\n", context->name, path);
	pcs_fileinfo_destroy(f);
	pcs_free(path);
	if (t != text) pcs_free(t);
	return 0;
}

/*打印帮助信息*/
static int cmd_help(ShellContext *context, int argc, char *argv[])
{
	if (is_print_usage(argc, argv)) {
		usage_help(context);
		return 0;
	}
	if (argc != 0){
		usage_help(context);
		return -1;
	}
	usage(context);
	return 0;
}

/*列出目录*/
static int cmd_list(ShellContext *context, int argc, char *argv[])
{
	char *path;
	PcsFileInfoList *list = NULL;
	int page_index = 1;
	char tmp[64] = { 0 };
	int fileCount = 0, dirCount = 0;
	size_t totalSize = 0;

	if (is_print_usage(argc, argv)) {
		usage_list(context);
		return 0;
	}
	if (argc != 0 && argc != 1) {
		usage_list(context);
		return -1;
	}
	if (!is_login(context, NULL)) return -1;

	if (argc == 0) {
		path = pcs_utils_strdup(context->workdir);
	}
	else if (argc == 1) {
		path = combin_unix_path(context->workdir, argv[0]);
	}

	while (1) {
		list = pcs_list(context->pcs, path,
			page_index, context->list_page_size,
			context->list_sort_name,
			streq(context->list_sort_direction, "desc", -1) ? PcsTrue: PcsFalse);
		if (!list) {
			if (pcs_strerror(context->pcs)) {
				printf("Error: %s\n", pcs_strerror(context->pcs));
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
static int cmd_login(ShellContext *context, int argc, char *argv[])
{
	PcsRes pcsres;
	char str[50];
	if (is_print_usage(argc, argv)) {
		usage_login(context);
		return 0;
	}
	if (argc != 0 && argc != 1 && argc != 2) {
		usage_login(context);
		return -1;
	}
	if (is_login(context, "")) {
		printf("You have been logon, you can use 'who' command to print the UID. \n"
			"You can logout by 'logout' command and then relogin.\n");
		return -1;
	}
	if (argc > 1) {
		pcs_setopt(context->pcs, PCS_OPTION_USERNAME, argv[0]);
	}
	else {
		printf("User Name: ");
		std_string(str, 50);
		pcs_setopt(context->pcs, PCS_OPTION_USERNAME, str);
	}
	if (argc > 2) {
		pcs_setopt(context->pcs, PCS_OPTION_PASSWORD, argv[1]);
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
static int cmd_logout(ShellContext *context, int argc, char *argv[])
{
	PcsRes pcsres;
	if (is_print_usage(argc, argv)) {
		usage_logout(context);
		return 0;
	}
	if (argc != 0) {
		usage_logout(context);
		return -1;
	}
	if (!is_login(context, "")) {
		printf("You are not login, you can use 'login' command to login.\n");
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
static int cmd_meta(ShellContext *context, int argc, char *argv[])
{
	char *path;
	PcsFileInfo *fi;
	if (is_print_usage(argc, argv)) {
		usage_meta(context);
		return 0;
	}
	if (argc != 0 && argc != 1) {
		usage_meta(context);
		return -1;
	}
	if (!is_login(context, NULL)) return -1;
	if (argc == 1)
		path = combin_unix_path(context->workdir, argv[0]);
	else
		path = context->workdir;
	assert(path);
	fi = pcs_meta(context->pcs, path);
	if (!fi) {
		printf("Error: The target not exist, or have error: %s\n", pcs_strerror(context->pcs));
		if (path != context->workdir) pcs_free(path);
		return -1;
	}
	print_fileinfo(fi, " ");
	pcs_fileinfo_destroy(fi);
	if (path != context->workdir) pcs_free(path);
	return 0;
}

/*创建目录*/
static int cmd_mkdir(ShellContext *context, int argc, char *argv[])
{
	PcsRes res;
	char *path;
	if (is_print_usage(argc, argv)) {
		usage_mkdir(context);
		return 0;
	}
	if (argc != 1) {
		usage_mkdir(context);
		return -1;
	}
	if (!is_login(context, NULL)) return -1;
	path = combin_unix_path(context->workdir, argv[0]);
	assert(path);
	res = pcs_mkdir(context->pcs, path);
	if (res != PCS_OK) {
		printf("Error: %s\n", pcs_strerror(context->pcs));
		pcs_free(path);
		return -1;
	}
	printf("Success.\n");
	pcs_free(path);
	return 0;
}

/*移动文件*/
static int cmd_move(ShellContext *context, int argc, char *argv[])
{
	PcsPanApiRes *res;
	PcsSList2 slist;
	PcsPanApiResInfoList *info;
	if (is_print_usage(argc, argv)) {
		usage_move(context);
		return 0;
	}
	if (argc != 2) {
		usage_move(context);
		return -1;
	}
	if (!is_login(context, NULL)) return -1;
	slist.string1 = combin_unix_path(context->workdir, argv[0]); /* path */
	slist.string2 = combin_unix_path(context->workdir, argv[1]); /* new_name */
	slist.next = NULL;

	res = pcs_move(context->pcs, &slist);
	if (!res) {
		printf("Error: %s src=%s, dst=%s.\n", pcs_strerror(context->pcs), slist.string1, slist.string2);
		pcs_free(slist.string1);
		pcs_free(slist.string2);
		return -1;
	}
	info = res->info_list;
	if (info->info.error) {
		printf("Error: unknow src=%s, dst=%s. \n", slist.string1, slist.string2);
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
static int cmd_pwd(ShellContext *context, int argc, char *argv[])
{
	if (is_print_usage(argc, argv)) {
		usage_pwd(context);
		return 0;
	}
	if (argc != 0){
		usage_pwd(context);
		return -1;
	}
	//if (!is_login(context, NULL)) return -1;
	printf("%s\n", context->workdir);
	return 0;
}

/*打印配额*/
static int cmd_quota(ShellContext *context, int argc, char *argv[])
{
	PcsRes pcsres;
	size_t quota, used;
	int is_exact = 0;
	char str[32] = { 0 };
	if (is_print_usage(argc, argv)) {
		usage_quota(context);
		return 0;
	}
	if (argc != 0 && argc != 1){
		usage_quota(context);
		return -1;
	}
	if (argc == 1) {
		if (strcmp(argv[0], "-e")) {
			usage_quota(context);
			return -1;
		}
		is_exact = 1;
	}
	if (!is_login(context, NULL)) return -1;
	pcsres = pcs_quota(context->pcs, &quota, &used);
	if (pcsres != PCS_OK) {
		printf("Error: %s\n", pcs_strerror(context->pcs));
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
static int cmd_remove(ShellContext *context, int argc, char *argv[])
{
	PcsPanApiRes *res;
	PcsSList slist;
	PcsPanApiResInfoList *info;
	if (is_print_usage(argc, argv)) {
		usage_remove(context);
		return 0;
	}
	if (argc != 1) {
		usage_remove(context);
		return -1;
	}
	if (!is_login(context, NULL)) return -1;
	slist.string = combin_unix_path(context->workdir, argv[0]); /* path */
	slist.next = NULL;

	res = pcs_delete(context->pcs, &slist);
	if (!res) {
		printf("Error: %s path=%s.\n", pcs_strerror(context->pcs), slist.string);
		pcs_free(slist.string);
		return -1;
	}
	info = res->info_list;
	if (info->info.error) {
		printf("Error: unknow path=%s. \n", slist.string);
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
static int cmd_rename(ShellContext *context, int argc, char *argv[])
{
	PcsPanApiRes *res;
	PcsSList2 slist;
	PcsPanApiResInfoList *info;
	char *p;
	if (is_print_usage(argc, argv)) {
		usage_rename(context);
		return 0;
	}
	if (argc != 2) {
		usage_rename(context);
		return -1;
	}
	p = argv[1];
	while (*p) {
		if (*p == '/' || *p == '\\' || *p == ':' || *p == '*' || *p == '?' || *p == '"' || *p == '<' || *p == '>' || *p == '|') {
			printf("The file name can't include \"/\\:*?\"<>|\"\n");
			return -1;
		}
		p++;
	}
	if (!is_login(context, NULL)) return -1;
	slist.string1 = combin_unix_path(context->workdir, argv[0]); /* path */
	slist.string2 = argv[1]; /* new_name */
	slist.next = NULL;

	res = pcs_rename(context->pcs, &slist);
	if (!res) {
		printf("Error: %s src=%s, dst=%s.\n", pcs_strerror(context->pcs), slist.string1, slist.string2);
		pcs_free(slist.string1);
		return -1;
	}
	info = res->info_list;
	if (info->info.error) {
		printf("Error: unknow src=%s, dst=%s. \n", slist.string1, slist.string2);
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
static int cmd_set(ShellContext *context, int argc, char *argv[])
{
	int i, key_len;
	char *key, *val;
	if (is_print_usage(argc, argv)) {
		usage_set(context);
		return 0;
	}
	if (argc == 0) {
		usage_set(context);
		return -1;
	}
	for (i = 0; i < argc; i++) {
		key = val = argv[i];
		while (*val && *val != '=') val++;
		if (*val != '=') {
			usage_set(context);
			return -1;
		}
		key_len = val - key;
		val++;

		if (streq("cookiefile", key, key_len)) {
			if (set_cookiefile(context, val)) {
				usage_set(context);
				return -1;
			}
		}
		else if (streq("captchafile", key, key_len)) {
			if (set_captchafile(context, val)) {
				usage_set(context);
				return -1;
			}
		}
		else if (streq("list_page_size", key, key_len)) {
			if (set_list_page_size(context, val)) {
				usage_set(context);
				return -1;
			}
		}
		else if (streq("list_sort_name", key, key_len)) {
			if (set_list_sort_name(context, val)) {
				usage_set(context);
				return -1;
			}
		}
		else if (streq("list_sort_direction", key, key_len)) {
			if (set_list_sort_direction(context, val)) {
				usage_set(context);
				return -1;
			}
		}
		else if (streq("secure_method", key, key_len)) {
			if (set_secure_method(context, val)) {
				usage_set(context);
				return -1;
			}
		}
		else if (streq("secure_key", key, key_len)) {
			if (set_secure_key(context, val)) {
				usage_set(context);
				return -1;
			}
		}
		else if (streq("secure_enable", key, key_len)) {
			if (set_secure_enable(context, val)) {
				usage_set(context);
				return -1;
			}
		}
		else {
			printf("warning: unknow option %s\n", key);
		}
	}
	printf("Success. You can view context by '%s context'\n", context->name);
	return 0;
}

/*搜索文件*/
static int cmd_search(ShellContext *context, int argc, char *argv[])
{
	int is_recursive = 0;
	char *path = NULL;
	const char *relPath = NULL, *keyword = NULL;
	int i;

	PcsFileInfoList *list = NULL;

	if (is_print_usage(argc, argv)) {
		usage_search(context);
		return 0;
	}
	if (argc < 1 || argc > 3) {
		usage_search(context);
		return -1;
	}
	/*解析参数 - 开始*/
	if (argc == 1) {
		if (strcmp(argv[0], "-r") == 0) {
			usage_search(context);
			return -1;
		}
		else {
			keyword = argv[0];
		}
	}
	else if (argc == 2){
		if (strcmp(argv[0], "-r") == 0) {
			is_recursive = 1;
			keyword = argv[1];
		}
		else {
			relPath = argv[0];
			keyword = argv[1];
		}
	}
	else if (argc == 3) {
		for (i = 0; i < argc; i++) {
			if (is_recursive == 0 && strcmp(argv[i], "-r") == 0) {
				is_recursive = 1;
			}
			else if (relPath == NULL) {
				relPath = argv[i];
			}
			else if (keyword == NULL) {
				keyword = argv[i];
			}
			else {
				usage_search(context);
				return -1;
			}
		}
		if (is_recursive == 0) {
			usage_search(context);
			return -1;
		}
	}
	/*解析参数 - 结束*/

	if (!is_login(context, NULL)) return -1;

	if (!relPath) {
		path = pcs_utils_strdup(context->workdir);
	}
	else {
		path = combin_unix_path(context->workdir, relPath);
	}

	list = pcs_search(context->pcs, path, keyword, is_recursive ? PcsTrue : PcsFalse);
	if (!list) {
		if (pcs_strerror(context->pcs)) {
			printf("Error: %s\n", pcs_strerror(context->pcs));
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

	if (meta->remote_isdir) { /*跳过目录*/
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
	state->page_size = 0;
	state->page_index = 0;
	state->first = 6;
	state->process = &synchOnPrepare;
	state->processState = NULL;
	state->no_print_flag = 0;
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
	print_meta_list_head(first, second, other, 0);
	print_meta_list_row(first, second, other, meta);
	print_meta_list_foot(first, second, other);
	return 0;
}

/*同步本地和网盘目录*/
static int cmd_synch(ShellContext *context, int argc, char *argv[])
{
	compare_arg arg = { 0 };

	if (is_print_usage(argc, argv) || parse_compare_args(argc, argv, &arg)) {
		usage_synch(context);
		return 0;
	}

	arg.check_local_dir_exist = 0;
	arg.onRBEnumerateStatePrepared = &synchOnRBEnumStatePrepared;

	return compare(context, &arg, &synchFile, NULL, &on_compared_dir, NULL);
}

#pragma endregion

/*上传*/
static int cmd_upload(ShellContext *context, int argc, char *argv[])
{
	int is_force = 0;
	char *path = NULL, *errmsg = NULL;
	const char *relPath = NULL, *locPath = NULL;
	int i;

	LocalFileInfo *local;
	PcsFileInfo *meta;

	if (is_print_usage(argc, argv)) {
		usage_upload(context);
		return 0;
	}
	if (argc != 2 && argc != 3) {
		usage_upload(context);
		return -1;
	}
	/*解析参数 - 开始*/
	if (argc == 2){
		if (strcmp(argv[0], "-f") == 0 || strcmp(argv[1], "-f") == 0) {
			usage_upload(context);
			return -1;
		}
		locPath = argv[0];
		relPath = argv[1];
	}
	else if (argc == 3) {
		for (i = 0; i < argc; i++) {
			if (is_force == 0 && strcmp(argv[i], "-f") == 0) {
				is_force = 1;
			}
			else if (locPath == NULL) {
				locPath = argv[i];
			}
			else if (relPath == NULL) {
				relPath = argv[i];
			}
			else {
				usage_upload(context);
				return -1;
			}
		}
		if (is_force == 0) {
			usage_upload(context);
			return -1;
		}
	}
	if (!locPath || !relPath) {
		usage_upload(context);
		return -1;
	}
	/*解析参数 - 结束*/

	/*检查本地文件 - 开始*/
	local = GetLocalFileInfo(locPath);
	if (!local) {
		printf("Error: The local file not exist.\n");
		return -1;
	}
	else if (local->isdir) {
		printf("Error: The local file is directory. \nYou can use 'synch -cu' command to upload the directory.\n", locPath);
		DestroyLocalFileInfo(local);
		return -1;
	}
	DestroyLocalFileInfo(local);
	/*检查本地文件 - 结束*/

	//检查是否已经登录
	if (!is_login(context, NULL)) {
		return -1;
	}

	path = combin_unix_path(context->workdir, relPath);
	if (!path) {
		assert(path);
		return -1;
	}

	/*检查网盘文件 - 开始*/
	meta = pcs_meta(context->pcs, path);
	if (meta && meta->isdir) {
		printf("Error: The remote file exist, and it is directory. \n");
		pcs_fileinfo_destroy(meta);
		pcs_free(path);
		return -1;
	}
	else if (meta && !is_force){
		printf("Error: The remote file exist. You can specify '-f' to force override.\n");
		pcs_fileinfo_destroy(meta);
		pcs_free(path);
		return -1;
	}
	/*检查网盘文件 - 结束*/

	/*开始上传*/
	if (do_upload(context,
		locPath, path, is_force ? PcsTrue : PcsFalse,
		"", context->workdir,
		&errmsg, NULL)) {
		printf("Error: %s\n", errmsg);
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
static int cmd_version(ShellContext *context, int argc, char *argv[])
{
	if (is_print_usage(argc, argv)) {
		usage_version(context);
		return 0;
	}
	if (argc != 0){
		usage_version(context);
		return -1;
	}
	version(context);
	return 0;
}

/*打印当前登录用户*/
static int cmd_who(ShellContext *context, int argc, char *argv[])
{
	if (is_print_usage(argc, argv)) {
		usage_who(context);
		return 0;
	}
	if (argc != 0){
		usage_who(context);
		return -1;
	}
	if (!is_login(context, NULL)) return -1;
	printf("%s\n", pcs_sysUID(context->pcs));
	return 0;
}

/*路由到具体的命令函数*/
static int exec_cmd(ShellContext *context, int argc, char *argv[])
{
	const char *cmd;
	int cmd_argc;
	char **cmd_argv;
	int rc;
	if (argc < 2) {
		usage(context);
		return -1;
	}
	cmd = argv[1];
	cmd_argc = argc > 2 ? (argc - 2) : 0;
	cmd_argv = cmd_argc > 0 ? &argv[2] : NULL;

	if (strcmp(cmd, "cat") == 0) {
		rc = cmd_cat(context, cmd_argc, cmd_argv);
	}
	else if (strcmp(cmd, "cd") == 0) {
		rc = cmd_cd(context, cmd_argc, cmd_argv);
	}
	else if (strcmp(cmd, "copy") == 0 || strcmp(cmd, "cp") == 0) {
		rc = cmd_copy(context, cmd_argc, cmd_argv);
	}
	else if (strcmp(cmd, "compare") == 0 || strcmp(cmd, "c") == 0) {
		rc = cmd_compare(context, cmd_argc, cmd_argv);
	}
	else if (strcmp(cmd, "context") == 0 || strcmp(cmd, "config") == 0) {
		rc = cmd_context(context, cmd_argc, cmd_argv);
	}
	else if (strcmp(cmd, "download") == 0 || strcmp(cmd, "d") == 0) {
		rc = cmd_download(context, cmd_argc, cmd_argv);
	}
	else if (strcmp(cmd, "echo") == 0) {
		rc = cmd_echo(context, cmd_argc, cmd_argv);
	}
	else if (strcmp(cmd, "list") == 0 || strcmp(cmd, "ls") == 0) {
		rc = cmd_list(context, cmd_argc, cmd_argv);
	}
	else if (strcmp(cmd, "login") == 0) {
		rc = cmd_login(context, cmd_argc, cmd_argv);
	}
	else if (strcmp(cmd, "logout") == 0) {
		rc = cmd_logout(context, cmd_argc, cmd_argv);
	}
	else if (strcmp(cmd, "meta") == 0) {
		rc = cmd_meta(context, cmd_argc, cmd_argv);
	}
	else if (strcmp(cmd, "mkdir") == 0) {
		rc = cmd_mkdir(context, cmd_argc, cmd_argv);
	}
	else if (strcmp(cmd, "move") == 0 || strcmp(cmd, "mv") == 0) {
		rc = cmd_move(context, cmd_argc, cmd_argv);
	}
	else if (strcmp(cmd, "pwd") == 0) {
		rc = cmd_pwd(context, cmd_argc, cmd_argv);
	}
	else if (strcmp(cmd, "quota") == 0) {
		rc = cmd_quota(context, cmd_argc, cmd_argv);
	}
	else if (strcmp(cmd, "remove") == 0 || strcmp(cmd, "rm") == 0) {
		rc = cmd_remove(context, cmd_argc, cmd_argv);
	}
	else if (strcmp(cmd, "rename") == 0 || strcmp(cmd, "ren") == 0) {
		rc = cmd_rename(context, cmd_argc, cmd_argv);
	}
	else if (strcmp(cmd, "set") == 0) {
		rc = cmd_set(context, cmd_argc, cmd_argv);
	}
	else if (strcmp(cmd, "search") == 0) {
		rc = cmd_search(context, cmd_argc, cmd_argv);
	}
	else if (strcmp(cmd, "synch") == 0 || strcmp(cmd, "s") == 0) {
		rc = cmd_synch(context, cmd_argc, cmd_argv);
	}
	else if (strcmp(cmd, "upload") == 0 || strcmp(cmd, "u") == 0) {
		rc = cmd_upload(context, cmd_argc, cmd_argv);
	}
	else if (strcmp(cmd, "version") == 0
		|| strcmp(cmd, "ver") == 0
		|| strcmp(cmd, "-v") == 0 
		|| strcmp(cmd, "-V") == 0
		|| strcmp(cmd, "-version") == 0
		|| strcmp(cmd, "--version") == 0
		|| strcmp(cmd, "-ver") == 0
		|| strcmp(cmd, "--ver") == 0) {
		rc = cmd_version(context, cmd_argc, cmd_argv);
	}
	else if (strcmp(cmd, "who") == 0) {
		rc = cmd_who(context, cmd_argc, cmd_argv);
	}
	else if (strcmp(cmd, "help") == 0
		|| strcmp(cmd, "-h") == 0
		|| strcmp(cmd, "-H") == 0
		|| strcmp(cmd, "-?") == 0
		|| strcmp(cmd, "?") == 0
		|| strcmp(cmd, "-help") == 0
		|| strcmp(cmd, "--help") == 0) {
		rc = cmd_help(context, cmd_argc, cmd_argv);
	}
	else {
		usage(context);
		rc = -1;
	}
	return rc;
}

#pragma endregion

int main(int argc, char *argv[])
{
	ShellContext context;
	int rc = 0;
	hook_cjson();
	init_context(&context, argc, argv);
	restore_context(&context);
	init_pcs(&context);
	rc = exec_cmd(&context, argc, argv);
	save_context(&context);
	free_context(&context);
	pcs_mem_print_leak();
	return rc;
}
