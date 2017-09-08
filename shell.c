#include <inttypes.h>
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

# define snprintf _snprintf
# define vsnprintf _vsnprintf

#else
# include <unistd.h>
# include <termios.h>
# include <pthread.h>
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
#include "cache.h"
#ifdef WIN32
# include "pcs/utf8.h"
#ifndef __MINGW32__
# define lseek _lseek
# define fileno _fileno
# define fseeko _fseeki64
# define ftello _ftelli64
#endif
#endif
#include "shell.h"

#define USAGE "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/55.0.2883.87 Safari/537.36"
#define TIMEOUT						60
#define CONNECTTIMEOUT				1
#define DEFAULT_THREAD_NUM			5
#define MAX_THREAD_NUM				10000
#define MIN_SLICE_SIZE				(512 * 1024) /*最小分片大小*/
#define MAX_SLICE_SIZE				(10 * 1024 * 1024) /*最大分片大小*/
#define MAX_FFLUSH_SIZE				(10 * 1024 * 1024) /*最大缓存大小*/
#define MIN_UPLOAD_SLICE_SIZE		(512 * 1024) /*最小分片大小*/
#define MAX_UPLOAD_SLICE_SIZE		(10 * 1024 * 1024) /*最大分片大小*/
#define MAX_UPLOAD_SLICE_COUNT		1024

#define convert_to_real_speed(speed) ((speed) * 1024)

#define convert_to_real_cache_size(size) ((size) * 1024)

#define PCS_CONTEXT_ENV				"PCS_CONTEXT"
#define PCS_COOKIE_ENV				"PCS_COOKIE"
#define PCS_CAPTCHA_ENV				"PCS_CAPTCHA"
#define TEMP_FILE_SUFFIX			".pcs_temp"
#define DECRYPT_FILE_SUFFIX			".decrypt"
#define ENCRYPT_FILE_SUFFIX			".encrypt"
#define SLICE_FILE_SUFFIX			".slice"
//#define PCS_DEFAULT_CONTEXT_FILE	"/tmp/pcs_context.json"

#define THREAD_STATE_MAGIC			(((int)'T' << 24) | ((int)'S' << 16) | ((int)'H' << 8) | ((int)'T'))

#ifndef PCS_BUFFER_SIZE
#  define PCS_BUFFER_SIZE			(AES_BLOCK_SIZE * 1024)
#endif

#ifdef _WIN32
#  define NONE
#  define RED
#  define LIGHT_RED
#  define GREEN
#  define LIGHT_GREEN
#  define BLUE
#  define LIGHT_BLUE
#  define DARY_GRAY
#  define CYAN
#  define LIGHT_CYAN
#  define PURPLE
#  define LIGHT_PURPLE
#  define BROWN
#  define YELLOW
#  define LIGHT_GRAY
#  define WHITE
#else
#  define NONE         "\033[m"
#  define RED          "\033[0;32;31m"
#  define LIGHT_RED    "\033[1;31m"
#  define GREEN        "\033[0;32;32m"
#  define LIGHT_GREEN  "\033[1;32m"
#  define BLUE         "\033[0;32;34m"
#  define LIGHT_BLUE   "\033[1;34m"
#  define DARY_GRAY    "\033[1;30m"
#  define CYAN         "\033[0;36m"
#  define LIGHT_CYAN   "\033[1;36m"
#  define PURPLE       "\033[0;35m"
#  define LIGHT_PURPLE "\033[1;35m"
#  define BROWN        "\033[0;33m"
#  define YELLOW       "\033[1;33m"
#  define LIGHT_GRAY   "\033[0;37m"
#  define WHITE        "\033[1;37m"
#endif

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

#define PCS_SECURE_NONE				((int)0)
#define PCS_SECURE_PLAINTEXT		((int)1)
#define PCS_SECURE_AES_CBC_128		((int)128)
#define PCS_SECURE_AES_CBC_192		((int)192)
#define PCS_SECURE_AES_CBC_256		((int)256)

#define PCS_AES_MAGIC				(0x41455300)

#define DOWNLOAD_STATUS_OK				0
#define DOWNLOAD_STATUS_PENDDING		1
#define DOWNLOAD_STATUS_WRITE_FILE_FAIL	2
#define DOWNLOAD_STATUS_FAIL			3
#define DOWNLOAD_STATUS_DOWNLOADING		4

#define UPLOAD_STATUS_OK				0
#define UPLOAD_STATUS_PENDDING			1
#define UPLOAD_STATUS_WRITE_FILE_FAIL	2
#define UPLOAD_STATUS_FAIL				3
#define UPLOAD_STATUS_UPLOADING			4

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
	char		*md5;

	int			flag;

	int			op;				/*需要执行的操作*/
	int			op_st;			/*操作的结果*/
	char		*msg;			/*操作失败时的错误消息*/

	MyMeta		*parent;		/*父目录的元数据*/
	void		*userdata;		/*用户数据*/
};

struct DownloadThreadState;

struct DownloadState
{
	FILE *pf;
	int64_t downloaded_size; /*已经下载的字节数*/
	curl_off_t resume_from; /*断点续传时，从这个位置开始续传*/
	time_t time; /*最后一次在屏幕打印信息的时间*/
	size_t speed; /*用于统计下载速度*/
	int64_t file_size; /*完整的文件的字节大小*/
	ShellContext *context;
	void *mutex;
	int	num_of_running_thread; /*已经启动的线程数量*/
	int num_of_slice; /*分片数量*/
	char **pErrMsg;
	int	status;
	const char *remote_file;
	struct DownloadThreadState *threads;
	cathe_t cache;
};

struct DownloadThreadState
{
	struct DownloadState *ds;
	curl_off_t	start;
	curl_off_t	end;
	curl_off_t	postion;
	int		status;
	Pcs		*pcs;
	int		tid;
	struct DownloadThreadState *next;
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

struct UploadThreadState;
struct UploadState {
	FILE *pf;
	char *path;
	char *slice_file;
	int64_t uploaded_size; /*已经下载的字节数*/
	time_t time; /*最后一次在屏幕打印信息的时间*/
	size_t speed; /*用于统计下载速度*/
	int64_t file_size; /*完整的文件的字节大小*/
	ShellContext *context;
	void *mutex;
	int	num_of_running_thread; /*已经启动的线程数量*/
	int num_of_slice; /*分片数量*/
	char **pErrMsg;
	int	status;
	struct UploadThreadState *threads;
};

struct UploadThreadState {
	struct UploadState *us;
	curl_off_t	start;
	curl_off_t	end;
	int		status;
	size_t  uploaded_size;
	Pcs		*pcs;
	char	md5[33]; /*上传成功后的分片MD5值*/
	int		tid;
	struct UploadThreadState *next;
};

static const char *app_name = NULL;

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

char *mbs2utf8(const char *s)
{
	int sl = strlen(s);
	int sz = u8_mbs_toutf8_size(s, sl);
	char *res = 0;
	res = (char *)pcs_malloc(sz + 1);
	if (!res)
		return 0;
	memset(res, 0, sz + 1);
	u8_mbs_toutf8(res, sz, s, sl);
	return res;
}

char *utf82mbs(const char *s)
{
	int sl = strlen(s);
	int sz = u8_tombs_size(s, sl);
	char *res = 0;
	res = (char *)pcs_malloc(sz + 1);
	if (!res)
		return 0;
	memset(res, 0, sz + 1);
	u8_tombs(res, sz, s, sl);
	return res;
}

# define printf u8_printf

# define sleep(s) Sleep((s) * 1000)

int truncate(const char *file, int64_t size)
{
	HANDLE hFile;
	hFile = CreateFile(file, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile) {
		LARGE_INTEGER li = {0};
		li.QuadPart = (LONGLONG)size;
		SetFilePointer(hFile, li.LowPart, &li.HighPart, FILE_BEGIN);
		SetEndOfFile(hFile);
		CloseHandle(hFile);
		return 0;
	}
	return -1;
}

#else
/*判断当前操作系统编码是否是UTF-8编码*/
int u8_is_utf8_sys()
{
	return 1;
}

char *mbs2utf8(const char *s)
{
	return pcs_utils_strdup(s);
}

char *utf82mbs(const char *s)
{
	return pcs_utils_strdup(s);
}

#endif

#pragma region 加解密文件

static inline int get_secure_method(ShellContext *context)
{
	if (!context->secure_method) return -1;
	if (!strcmp(context->secure_method, "aes-cbc-128")) return PCS_SECURE_AES_CBC_128;
	if (!strcmp(context->secure_method, "aes-cbc-192")) return PCS_SECURE_AES_CBC_192;
	if (!strcmp(context->secure_method, "aes-cbc-256")) return PCS_SECURE_AES_CBC_256;
	return -1;
}

static inline void readToAesHead(const char *buf, struct PcsAesHead *head)
{
	head->magic = readInt(buf);
	head->bits = readInt(&buf[4]);
	head->polish = readInt(&buf[8]);
	head->reserve = readInt(&buf[12]);
}

static int get_file_secure_method(const char *src)
{
	FILE *srcFile;
	int64_t file_sz;
	size_t sz;
	unsigned char buf[PCS_BUFFER_SIZE];
	struct PcsAesHead head = { 0 };

	srcFile = fopen(src, "rb");
	if (!srcFile) {
		//fprintf(stderr, "Error: Can't open the source file: %s\n", src);
		return -1;
	}
	fseeko(srcFile, 0, SEEK_END);
	file_sz = ftello(srcFile);
	fseeko(srcFile, 0, SEEK_SET);

	if (file_sz < 32) {
		fclose(srcFile);
		return PCS_SECURE_PLAINTEXT;
	}

	sz = fread(buf, 1, 16, srcFile);
	if (sz != 16) {
		//fprintf(stderr, "Error: Can't read the source file: %s\n", src);
		fclose(srcFile);
		return -1;
	}

	readToAesHead(buf, &head);
	if (head.magic != PCS_AES_MAGIC || (head.bits != PCS_SECURE_AES_CBC_128 && head.bits != PCS_SECURE_AES_CBC_192 && head.bits != PCS_SECURE_AES_CBC_256)) {
		fclose(srcFile);
		return PCS_SECURE_PLAINTEXT;
	}
	fclose(srcFile);

	if (((file_sz - 32) % AES_BLOCK_SIZE) != 0) {
		//fprintf(stderr, "Error: The file is not a encrypt file or is broken.\n");
		return -1;
	}
	return head.bits;
}

static int encrypt_file(const char *src, const char *dst, int secure_method, const char *secure_key, char **pErrMsg)
{
	MD5_CTX md5;
	unsigned char md5_value[16], buf[PCS_BUFFER_SIZE], out_buf[PCS_BUFFER_SIZE];
	FILE *srcFile, *dstFile;
	char *tmp_local_path;
	int64_t file_sz;
	size_t buf_sz, sz;
	int polish;
	AES_KEY				aes = { 0 };
	unsigned char		key[AES_BLOCK_SIZE] = { 0 };
	unsigned char		iv[AES_BLOCK_SIZE] = { 0 };
	int rc;
	rc = AES_set_encrypt_key(pcs_md5_string_raw(secure_key), secure_method, &aes);
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
	tmp_local_path = (char *)pcs_malloc(strlen(src) + strlen(ENCRYPT_FILE_SUFFIX) + 1);
	strcpy(tmp_local_path, src);
	strcat(tmp_local_path, ENCRYPT_FILE_SUFFIX);
	dstFile = fopen(tmp_local_path, "wb");
	if (!dstFile) {
		fprintf(stderr, "Error: Can't create the temp file: %s\n", tmp_local_path);
		fclose(srcFile);
		pcs_free(tmp_local_path);
		return -1;
	}

	fseeko(srcFile, 0, SEEK_END);
	file_sz = ftello(srcFile);
	fseeko(srcFile, 0, SEEK_SET);

	polish = 0;
	if (file_sz % AES_BLOCK_SIZE == 0) {
		polish = 0;
	}
	else {
		polish = (int)((file_sz / AES_BLOCK_SIZE + 1) * AES_BLOCK_SIZE - file_sz);
	}
	int2Buffer(PCS_AES_MAGIC, buf);
	int2Buffer(secure_method, &buf[4]);
	int2Buffer(polish, &buf[8]);
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
			buf_sz = (size_t)((sz / AES_BLOCK_SIZE + 1) * AES_BLOCK_SIZE);
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
	//printf("Success\n");
	return 0;
}

static int decrypt_file(const char *src, const char *dst, const char *secure_key, char **pErrMsg)
{
	struct PcsAesHead head = { 0 };
	MD5_CTX md5;
	unsigned char md5_value[16], buf[PCS_BUFFER_SIZE], out_buf[PCS_BUFFER_SIZE];
	char *tmp_local_path;
	FILE *srcFile, *dstFile;
	int64_t file_sz;
	size_t sz, buf_sz;
	AES_KEY				aes = { 0 };
	unsigned char		key[AES_BLOCK_SIZE] = { 0 };
	unsigned char		iv[AES_BLOCK_SIZE] = { 0 };
	int rc, i;
	MD5_Init(&md5);
	srcFile = fopen(src, "rb");
	if (!srcFile) {
		if (pErrMsg) {
			if (*pErrMsg) pcs_free(*pErrMsg);
			(*pErrMsg) = pcs_utils_sprintf("Error: Can't open the source file: %s\n", src);
		}
		return -1;
	}
	tmp_local_path = (char *)pcs_malloc(strlen(src) + strlen(DECRYPT_FILE_SUFFIX) + 1);
	strcpy(tmp_local_path, src);
	strcat(tmp_local_path, DECRYPT_FILE_SUFFIX);
	dstFile = fopen(tmp_local_path, "wb");
	if (!dstFile) {
		if (pErrMsg) {
			if (*pErrMsg) pcs_free(*pErrMsg);
			(*pErrMsg) = pcs_utils_sprintf("Error: Can't create the temp file: %s\n", tmp_local_path);
		}
		fclose(srcFile);
		pcs_free(tmp_local_path);
		return -1;
	}

	fseeko(srcFile, 0, SEEK_END);
	file_sz = ftello(srcFile);
	fseeko(srcFile, 0, SEEK_SET);

	if (file_sz < 32 || ((file_sz - 32) % AES_BLOCK_SIZE) != 0) {
		if (pErrMsg) {
			if (*pErrMsg) pcs_free(*pErrMsg);
			(*pErrMsg) = pcs_utils_sprintf("Error: The file is not a encrypt file or is broken.\n");
		}
		fclose(srcFile);
		fclose(dstFile);
		DeleteFileRecursive(tmp_local_path);
		pcs_free(tmp_local_path);
		return -1;
	}

	sz = fread(buf, 1, 16, srcFile);
	if (sz != 16) {
		if (pErrMsg) {
			if (*pErrMsg) pcs_free(*pErrMsg);
			(*pErrMsg) = pcs_utils_sprintf("Error: Can't read the source file: %s\n", src);
		}
		fclose(srcFile);
		fclose(dstFile);
		DeleteFileRecursive(tmp_local_path);
		pcs_free(tmp_local_path);
		return -1;
	}

	readToAesHead(buf, &head);
	if (head.magic != PCS_AES_MAGIC || (head.bits != 128 && head.bits != 192 && head.bits != 256)) {
		if (pErrMsg) {
			if (*pErrMsg) pcs_free(*pErrMsg);
			(*pErrMsg) = pcs_utils_sprintf("Error: The file is not a encrypt file or is broken.\n");
		}
		fclose(srcFile);
		fclose(dstFile);
		DeleteFileRecursive(tmp_local_path);
		pcs_free(tmp_local_path);
		return -1;
	}

	rc = AES_set_decrypt_key(pcs_md5_string_raw(secure_key), head.bits, &aes);
	if (rc < 0) {
		if (pErrMsg) {
			if (*pErrMsg) pcs_free(*pErrMsg);
			(*pErrMsg) = pcs_utils_sprintf("Error: Can't set decrypt key.\n");
		}
		fclose(srcFile);
		fclose(dstFile);
		DeleteFileRecursive(tmp_local_path);
		pcs_free(tmp_local_path);
		return -1;
	}
	file_sz -= 32;
	if (file_sz < PCS_BUFFER_SIZE) {
		buf_sz = (size_t)file_sz;
	}
	else {
		buf_sz = PCS_BUFFER_SIZE;
	}
	while ((sz = fread(buf, 1, buf_sz, srcFile)) > 0) {
		if (sz != buf_sz) {
			if (pErrMsg) {
				if (*pErrMsg) pcs_free(*pErrMsg);
				(*pErrMsg) = pcs_utils_sprintf("Error: Can't read the source file: %s\n", src);
			}
			fclose(srcFile);
			fclose(dstFile);
			DeleteFileRecursive(tmp_local_path);
			pcs_free(tmp_local_path);
			return -1;
		}
		AES_cbc_encrypt(buf, out_buf, buf_sz, &aes, iv, AES_DECRYPT);
		if (file_sz == (int64_t)buf_sz) {
			buf_sz -= head.polish;
			file_sz = (int64_t)buf_sz;
		}
		MD5_Update(&md5, out_buf, buf_sz);
		sz = fwrite(out_buf, 1, buf_sz, dstFile);
		if (sz != buf_sz) {
			if (pErrMsg) {
				if (*pErrMsg) pcs_free(*pErrMsg);
				(*pErrMsg) = pcs_utils_sprintf("Error: Write data to %s error. \n", dst);
			}
			fclose(srcFile);
			fclose(dstFile);
			DeleteFileRecursive(tmp_local_path);
			pcs_free(tmp_local_path);
			return -1;
		}
		file_sz -= (int64_t)buf_sz;
		if (file_sz == 0) break;
		if (file_sz < PCS_BUFFER_SIZE) {
			buf_sz = (size_t)file_sz;
		}
		else {
			buf_sz = PCS_BUFFER_SIZE;
		}
	}
	MD5_Final(md5_value, &md5);
	sz = fread(buf, 1, 16, srcFile);
	if (sz != 16) {
		if (pErrMsg) {
			if (*pErrMsg) pcs_free(*pErrMsg);
			(*pErrMsg) = pcs_utils_sprintf("Error: Can't read the source file: %s\n", src);
		}
		fclose(srcFile);
		fclose(dstFile);
		DeleteFileRecursive(tmp_local_path);
		pcs_free(tmp_local_path);
		return -1;
	}
	for (i = 0; i < 16; i++) {
		if (md5_value[i] != buf[i]) {
			if (pErrMsg) {
				if (*pErrMsg) pcs_free(*pErrMsg);
				(*pErrMsg) = pcs_utils_sprintf("Error: Wrong key or broken file\n");
			}
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
		if (pErrMsg) {
			if (*pErrMsg) pcs_free(*pErrMsg);
			(*pErrMsg) = pcs_utils_sprintf(
				"Error: The file have been decrypted at %s, but can't rename to %s.\n You should be rename manual.\n", tmp_local_path, dst);
		}
		//DeleteFileRecursive(tmp_local_path);
		pcs_free(tmp_local_path);
		return -1;
	}
	pcs_free(tmp_local_path);
	//printf("Success\n");
	return 0;
}

static int get_data_secure_method(const char *buffer, size_t buffer_size)
{
	struct PcsAesHead head = { 0 };
	if (!buffer) {
		return -1;
	}
	if (buffer_size < 32) {
		return PCS_SECURE_PLAINTEXT;
	}

	readToAesHead((char *)buffer, &head);
	if (head.magic != PCS_AES_MAGIC || (head.bits != PCS_SECURE_AES_CBC_128 && head.bits != PCS_SECURE_AES_CBC_192 && head.bits != PCS_SECURE_AES_CBC_256)) {
		return PCS_SECURE_PLAINTEXT;
	}

	if (((buffer_size - 32) % AES_BLOCK_SIZE) != 0) {
		//fprintf(stderr, "Error: The file is not a encrypt file or is broken.\n");
		return -1;
	}
	return head.bits;
}

static int encrypt_data(const char *src, size_t src_size, char **dst, size_t *dst_size,
	int secure_method, const char *secure_key)
{
	int rc;
	MD5_CTX md5;
	AES_KEY				aes = { 0 };
	unsigned char		key[AES_BLOCK_SIZE] = { 0 };
	unsigned char		iv[AES_BLOCK_SIZE] = { 0 };
	int blockCount;
	size_t outsz;
	char *out_buf, *md5_value;
	int i;
	const char *pin;
	char *pout, buf[32];

	rc = AES_set_encrypt_key(pcs_md5_string_raw(secure_key), secure_method, &aes);
	if (rc < 0) {
		fprintf(stderr, "Error: Can't set encrypt key.\n");
		return -1;
	}

	blockCount = src_size / AES_BLOCK_SIZE;
	if ((src_size % AES_BLOCK_SIZE) != 0) {
		blockCount++;
	}
	outsz = blockCount * AES_BLOCK_SIZE + 32;
	out_buf = (unsigned char *)pcs_malloc(outsz);
	if (!out_buf) {
		fprintf(stderr, "Error: can't alloc memory.\n");
		return -1;
	}
	pout = out_buf;
	int2Buffer(PCS_AES_MAGIC, pout);
	int2Buffer(secure_method, pout + 4);
	int2Buffer((int)(outsz - src_size - 32), pout + 8);
	int2Buffer(0, pout + 12);
	pout += 16;
	pin = src;

	MD5_Init(&md5);
	for (i = 0; i < blockCount; i++) {
		if ((size_t)((i + 1) * AES_BLOCK_SIZE) > src_size) {
			MD5_Update(&md5, pin, src_size - i * AES_BLOCK_SIZE);
			memcpy(buf, pin, src_size - i * AES_BLOCK_SIZE);
			memset(buf + src_size - i * AES_BLOCK_SIZE, 0, AES_BLOCK_SIZE - (src_size - i * AES_BLOCK_SIZE));
			// encrypt (iv will change)
			AES_cbc_encrypt(buf, pout, AES_BLOCK_SIZE, &aes, iv, AES_ENCRYPT);
			pout += AES_BLOCK_SIZE;
		}
		else {
			MD5_Update(&md5, pin, AES_BLOCK_SIZE);
			// encrypt (iv will change)
			AES_cbc_encrypt(pin, pout, AES_BLOCK_SIZE, &aes, iv, AES_ENCRYPT);
			pin += AES_BLOCK_SIZE;
			pout += AES_BLOCK_SIZE;
		}
	}
	md5_value = pout;
	MD5_Final(md5_value, &md5);
	*dst = out_buf;
	*dst_size = outsz;
	return 0;
}

static int decrypt_data(const char *src, size_t src_size, char **dst, size_t *dst_size,
	const char *secure_key)
{
	int rc;
	struct PcsAesHead head = { 0 };
	MD5_CTX md5;
	AES_KEY				aes = { 0 };
	unsigned char		key[AES_BLOCK_SIZE] = { 0 };
	unsigned char		iv[AES_BLOCK_SIZE] = { 0 };
	char md5_value[16], *pout, *outbuf;
	const char *pin;
	int blockCount;
	int i;
	size_t outsz;

	MD5_Init(&md5);

	if (src_size < 32 || ((src_size - 32) % AES_BLOCK_SIZE) != 0) {
		return -1;
	}

	readToAesHead(src, &head);
	if (head.magic != PCS_AES_MAGIC || (head.bits != 128 && head.bits != 192 && head.bits != 256)) {
		return -1;
	}

	rc = AES_set_decrypt_key(pcs_md5_string_raw(secure_key), head.bits, &aes);
	if (rc < 0) {
		return -1;
	}

	outsz = src_size - 32 - head.polish;
	outbuf = (char *)pcs_malloc(src_size);

	pin = src + 16;
	pout = outbuf;

	blockCount = (src_size - 32) / AES_BLOCK_SIZE;

	for (i = 0; i < blockCount;i++) {
		AES_cbc_encrypt(pin, pout, AES_BLOCK_SIZE, &aes, iv, AES_DECRYPT);
		if ((size_t)(i + 1) * AES_BLOCK_SIZE > outsz)
			MD5_Update(&md5, pout, AES_BLOCK_SIZE - head.polish);
		else
			MD5_Update(&md5, pout, AES_BLOCK_SIZE);
		pin += AES_BLOCK_SIZE;
		pout += AES_BLOCK_SIZE;
	}
	MD5_Final(md5_value, &md5);
	for (i = 0; i < 16; i++) {
		if (md5_value[i] != pin[i]) {
			pcs_free(outbuf);
			return -1;
		}
	}
	*dst = outbuf;
	*dst_size = outsz;
	return 0;
}

#pragma endregion

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

#pragma region 线程 State 相关

static void init_download_state(struct DownloadState *ds)
{
	memset(ds, 0, sizeof(struct DownloadState));
	cache_init(&ds->cache);
#ifdef _WIN32
	ds->mutex = CreateMutex(NULL, FALSE, NULL);
#else
	ds->mutex = (pthread_mutex_t *)pcs_malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init((pthread_mutex_t *)ds->mutex, NULL);
#endif
}

static void uninit_download_state(struct DownloadState *ds)
{
	struct DownloadThreadState *ts = ds->threads, *ps = NULL;
#ifdef _WIN32
	CloseHandle(ds->mutex);
#else
	pthread_mutex_destroy((pthread_mutex_t *)ds->mutex);
	pcs_free(ds->mutex);
#endif
	while (ts) {
		ps = ts;
		ts = ts->next;
		pcs_free(ps);
	}
	cache_uninit(&ds->cache);
}

static void lock_for_download(struct DownloadState *ds)
{
#ifdef _WIN32
	WaitForSingleObject(ds->mutex, INFINITE);
#else
	pthread_mutex_lock((pthread_mutex_t *)ds->mutex);
#endif
}

static void unlock_for_download(struct DownloadState *ds)
{
#ifdef _WIN32
	ReleaseMutex(ds->mutex);
#else
	pthread_mutex_unlock((pthread_mutex_t *)ds->mutex);
#endif
}

static void init_upload_state(struct UploadState *us)
{
	memset(us, 0, sizeof(struct UploadState));
#ifdef _WIN32
	us->mutex = CreateMutex(NULL, FALSE, NULL);
#else
	us->mutex = (pthread_mutex_t *)pcs_malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init((pthread_mutex_t *)us->mutex, NULL);
#endif
}

static void uninit_upload_state(struct UploadState *us)
{
	struct UploadThreadState *ts = us->threads, *ps = NULL;
#ifdef _WIN32
	CloseHandle(us->mutex);
#else
	pthread_mutex_destroy((pthread_mutex_t *)us->mutex);
	pcs_free(us->mutex);
#endif
	while (ts) {
		ps = ts;
		ts = ts->next;
		pcs_free(ps);
	}
}

static void lock_for_upload(struct UploadState *us)
{
#ifdef _WIN32
	WaitForSingleObject(us->mutex, INFINITE);
#else
	pthread_mutex_lock((pthread_mutex_t *)us->mutex);
#endif
}

static void unlock_for_upload(struct UploadState *us)
{
#ifdef _WIN32
	ReleaseMutex(us->mutex);
#else
	pthread_mutex_unlock((pthread_mutex_t *)us->mutex);
#endif
}

#pragma endregion

#pragma region 三个回调： 输入验证码, 显示上传进度, 写下载文件

static int save_thread_states_to_file(FILE *pf, int64_t offset, struct DownloadThreadState *state_link)
{
	static int magic = THREAD_STATE_MAGIC;
	int rc;
	struct DownloadThreadState *pts;
	//lseek(fileno(pf), ds->file_size, SEEK_SET);
	rc = fseeko((pf), offset, SEEK_SET);
	if (rc) {
		return -1;
	}
	rc = fwrite(&magic, 4, 1, pf);
	if (rc != 1) return -1;
	pts = state_link;
	while (pts) {
		rc = fwrite(pts, sizeof(struct DownloadThreadState), 1, pf);
		if (rc != 1) return -1;
		pts = pts->next;
	}
	return 0;
}

static int save_upload_thread_states_to_file(const char *filename, struct UploadThreadState *state_link)
{
	static int magic = THREAD_STATE_MAGIC;
	FILE *pf;
	int rc;
	struct UploadThreadState *pts;
	pf = fopen(filename, "wb");
	if (!pf) return -1;
	rc = fwrite(&magic, 4, 1, pf);
	if (rc != 1) {
		fclose(pf);
		return -1;
	}
	pts = state_link;
	while (pts) {
		rc = fwrite(pts, sizeof(struct UploadThreadState), 1, pf);
		if (rc != 1) {
			fclose(pf);
			return -1;
		}
		pts = pts->next;
	}
	fclose(pf);
	return 0;
}

/*输出验证码图片，并等待用户输入识别结果*/
static PcsBool verifycode(unsigned char *ptr, size_t size, char *captcha, size_t captchaSize, void *state)
{
	static char filename[1024] = { 0 };
	ShellContext *context = (ShellContext *)state;
	const char *savedfile;
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

	if (context->captchafile)
		savedfile = context->captchafile;
	else
		savedfile = filename;

	pf = fopen(savedfile, "wb");
	if (!pf) {
		printf("Can't save the captcha image to %s.\n", savedfile);
		return PcsFalse;
	}
	fwrite(ptr, 1, size, pf);
	fclose(pf);

	printf("The captcha image at %s.\nPlease input the captcha code: ", savedfile);
#ifdef WIN32
	{
		char fmt[32];
		memset(captcha, 0, captchaSize);
		snprintf(fmt, sizeof(fmt), "%%%ds", captchaSize - 1);
		scanf(fmt, captcha);
	}
	if (!u8_is_utf8_sys()) {
		char *u8s = mbs2utf8(captcha);
		strncpy(captcha, u8s, captchaSize);
		pcs_free(u8s);
	}
#else
	std_string(captcha, captchaSize);
#endif
	return PcsTrue;
}


static PcsBool input_str(const char *tips, char *value, size_t valueSize, void *state)
{
    printf("%s", tips);
    std_string(value, valueSize);
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
	time_t tm;
	char tmp[64];
	int rc;
	tmp[63] = '\0';
	rc = cache_add(&ds->cache, ds->downloaded_size, ptr, size);
	if (rc)
		return 0;
	ds->downloaded_size += size;
	ds->speed += size;
	if (ds->cache.total_size >= convert_to_real_cache_size(ds->context->cache_size)) {
		rc = cache_flush(&ds->cache);
		if (rc)
			return 0;
	}

	tm = time(&tm);
	if (tm != ds->time) {
		int64_t left_size = ds->file_size - ds->downloaded_size;
		int64_t remain_tm = (ds->speed > 0) ? (left_size / ds->speed) : 0;
		ds->time = tm;
		printf("%s", pcs_utils_readable_size((double)ds->downloaded_size + (double)ds->resume_from, tmp, 63, NULL));
		printf("/%s \t", pcs_utils_readable_size((double)contentlength + (double)ds->resume_from, tmp, 63, NULL));
		printf("%s/s \t", pcs_utils_readable_size((double)ds->speed, tmp, 63, NULL));
		printf("%s            ", pcs_utils_readable_left_time(remain_tm, tmp, 63, NULL));
		printf("\r");
		fflush(stdout);
		ds->speed = 0;
	}
	return size;
}

static int download_write_for_multy_thread(char *ptr, size_t size, size_t contentlength, void *userdata)
{
	struct DownloadThreadState *ts = (struct DownloadThreadState *)userdata;
	ShellContext *context = ts->ds->context;
	struct DownloadState *ds = ts->ds;
	FILE *pf = ds->pf;
	time_t tm;
	char tmp[64];
	int rc;
	tmp[63] = '\0';
	lock_for_download(ds);
	if (ts->postion + size > ts->end) {
		if (ds->pErrMsg) {
			if (*(ds->pErrMsg)) pcs_free(*(ds->pErrMsg));
			(*(ds->pErrMsg)) = pcs_utils_sprintf("invalid size.");
		}
		ts->status = DOWNLOAD_STATUS_FAIL;
		unlock_for_download(ds);
		return 0;
	}
	if (size > 0) {
		rc = cache_add(&ds->cache, ts->postion, ptr, size);
		if (rc) {
			if (ds->pErrMsg) {
				if (*(ds->pErrMsg)) pcs_free(*(ds->pErrMsg));
				(*(ds->pErrMsg)) = pcs_utils_sprintf("cache_add() error.");
			}
			ds->status = DOWNLOAD_STATUS_WRITE_FILE_FAIL; /* Cause all threads to exit. */
			unlock_for_download(ds);
			return 0;
		}
	}
	ds->downloaded_size += size;
	ts->postion += size;
	ds->speed += size;

	if (ds->cache.total_size >= convert_to_real_cache_size(context->cache_size)) {
		rc = cache_flush(&ds->cache);
		if (rc) {
			if (ds->pErrMsg) {
				if (*(ds->pErrMsg)) pcs_free(*(ds->pErrMsg));
				(*(ds->pErrMsg)) = pcs_utils_sprintf("cache_flush() error.");
			}
			ds->status = DOWNLOAD_STATUS_WRITE_FILE_FAIL; /* Cause all threads to exit. */
			unlock_for_download(ds);
			return 0;
		}
		rc = save_thread_states_to_file(pf, ds->file_size, ds->threads);
		if (rc) {
			if (ds->pErrMsg) {
				if (*(ds->pErrMsg)) pcs_free(*(ds->pErrMsg));
				(*(ds->pErrMsg)) = pcs_utils_sprintf("save slices error.");
			}
			ds->status = DOWNLOAD_STATUS_WRITE_FILE_FAIL; /* Cause all threads to exit. */
			unlock_for_download(ds);
			return 0;
		}
		cache_reset(&ds->cache);
	}

	tm = time(&tm);
	if (tm != ds->time) {
		int64_t left_size = ds->file_size - ds->downloaded_size;
		int64_t remain_tm = (ds->speed > 0) ? (left_size / ds->speed) : 0;
		double percent = (ds->file_size > 0) ?
			(double)(100 * (ds->downloaded_size + ds->resume_from)) / (double)ds->file_size :
			0;
		ds->time = tm;
		printf("\r                                                \r");
		printf("%s", pcs_utils_readable_size((double)ds->downloaded_size + (double)ds->resume_from, tmp, 63, NULL));
		printf("/%s (%.2f%%)\t", pcs_utils_readable_size((double)ds->file_size, tmp, 63, NULL), (float)percent);
		printf("%s/s \t", pcs_utils_readable_size((double)ds->speed, tmp, 63, NULL));
		printf(" %s        ", pcs_utils_readable_left_time(remain_tm, tmp, 63, NULL));
		printf("\r");
		fflush(stdout);
		ds->speed = 0;
	}
	unlock_for_download(ds);
	return size;
}

/*显示上传进度*/
static int upload_progress_for_multy_thread(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow)
{
	static char tmp[64];
	struct UploadThreadState *ts = (struct UploadThreadState*)clientp;
	struct UploadState *us = ts->us;
	time_t tm;

	lock_for_upload(us);

	tmp[63] = '\0';
	if ((((size_t)ulnow) > 0) && (((size_t)ulnow) > ts->uploaded_size)) {
		us->speed += ((size_t)ulnow - ts->uploaded_size);
		us->uploaded_size += ((size_t)ulnow - ts->uploaded_size);
		ts->uploaded_size = (size_t)ulnow;
	}
	else {
		unlock_for_upload(us);
		return 0;
	}

	tm = time(&tm);
	if (tm != us->time) {
		int64_t left_size = us->file_size - us->uploaded_size;
		int64_t remain_tm = (us->speed > 0) ? (left_size / us->speed) : 0;
		us->time = tm;
		printf("\r                                                \r");
		printf("%s", pcs_utils_readable_size((double)us->uploaded_size, tmp, 63, NULL));
		printf("/%s \t", pcs_utils_readable_size((double)us->file_size, tmp, 63, NULL));
		printf("%s/s \t", pcs_utils_readable_size((double)us->speed, tmp, 63, NULL));
		printf(" %s        ", pcs_utils_readable_left_time(remain_tm, tmp, 63, NULL));
		printf("\r");
		fflush(stdout);
		us->speed = 0;
	}

	unlock_for_upload(us);
	return 0;
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
#ifdef _WIN32
	printf("\r");  //清除该行
#else
	//printf("\033[1A"); //先回到上一行
	printf("\033[K");  //清除该行
#endif
}

/*回到上一行*/
static inline void back_prev_print_line()
{
#ifdef _WIN32
	printf("\r"); //先回到上一行
	printf("                                        ");  //清除该行
	printf("\r"); //先回到上一行
#else
	printf("\033[1A"); //先回到上一行
	printf("\033[K");  //清除该行
#endif
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

static const char *uint64_tostr(int64_t size, int *fix_width, char ch)
{
	static char str[128], *p;
	int i;
	int j, cn, mod;
	int64_t sz;

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

static void print_uint64(const char *format, int64_t size)
{
	char tmp[64];
	tmp[63] = '\0';
	pcs_utils_readable_size((double)size, tmp, 63, NULL);
	printf(format, tmp);
}

/*打印文件列表的头*/
static void print_filelist_head(int size_width, int md5, int thumb)
{
	int i;
	putchar('D');
	putchar(' ');
	for (i = 0; i < size_width - 4; i++)
		putchar(' ');
	printf("Size");
	putchar(' ');
	putchar(' ');
	printf("Modify Date Time");
	if (md5) {
		putchar(' ');
		putchar(' ');
		printf("MD5");
	}
	putchar(' ');
	putchar(' ');
	printf("File Name");
    if (thumb) {
        putchar(' ');
        putchar(' ');
        printf("Thumb Url");
    }
    putchar('\n');
}

/*打印文件列表的数据行*/
static void print_filelist_row(PcsFileInfo *f, int size_width, int md5, int thumb)
{
	const char *p;

	if (f->isdir)
		putchar('d');
	else
		putchar('-');
	putchar(' ');

	p = uint64_tostr(f->size, &size_width, ' ');
	while (*p) {
		putchar(*p++);
	}
	putchar(' ');
	putchar(' ');
	print_time("%s", f->server_mtime);
	if (md5) {
		putchar(' ');
		putchar(' ');
		printf("%s", f->md5);
	}
	putchar(' ');
	putchar(' ');
	printf("%s", f->path);
    if (thumb && f->thumbs) {
        printf("  %s", f->thumbs->string2);
    }
    putchar('\n');
}

/*打印文件列表*/
static void print_filelist(PcsFileInfoList *list, int *pFileCount, int *pDirCount, int64_t *pTotalSize,
	int md5, int thumb)
{
	char tmp[64] = { 0 };
	int cnt_file = 0,
		cnt_dir = 0,
		size_width = 1,
		w;
	PcsFileInfo *file = NULL;
	int64_t total = 0;
	PcsFileInfoListIterater iterater;

	pcs_filist_iterater_init(list, &iterater, PcsFalse);
	while (pcs_filist_iterater_next(&iterater)) {
		file = iterater.current;
		w = -1;
		uint64_tostr(file->size, &w, ' ');
		if (size_width < w)
			size_width = w;
		total += file->size;
		if (file->isdir)
			cnt_dir++;
		else
			cnt_file++;
	}

	if (size_width < 4)
		size_width = 4;
	print_filelist_head(size_width, md5, thumb);
	puts("------------------------------------------------------------------------------");
	pcs_filist_iterater_init(list, &iterater, PcsFalse);
	while (pcs_filist_iterater_next(&iterater)) {
		file = iterater.current;
		print_filelist_row(file, size_width, md5, thumb);
	}
	puts("------------------------------------------------------------------------------");
	pcs_utils_readable_size((double)total, tmp, 63, NULL);
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
	printf("%sfs_id:\t%"PRIu64"\n", prex, f->fs_id);
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
		print_uint64("Size:\t\t%s\n", f->size);
		printf("%smd5:\t\t%s\n", prex, f->md5);
		printf("%sdlink:\t\t%s\n", prex, f->dlink);
	}
    if (f->thumbs) {
        PcsSList2 *list = f->thumbs;
        printf("%sthumbs:\n", prex);
        while (list) {
            printf("%s  %s: %s\n", prex, list->string1, list->string2);
            list = list->next;
        }
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

	item = cJSON_CreateNumber(context->max_thread);
	assert(item);
	cJSON_AddItemToObject(root, "max_thread", item);

	item = cJSON_CreateNumber(context->max_speed_per_thread);
	assert(item);
	cJSON_AddItemToObject(root, "max_speed_per_thread", item);

	item = cJSON_CreateNumber(context->max_upload_speed_per_thread);
	assert(item);
	cJSON_AddItemToObject(root, "max_upload_speed_per_thread", item);

	item = cJSON_CreateString(context->user_agent);
	assert(item);
	cJSON_AddItemToObject(root, "user_agent", item);

	item = cJSON_CreateNumber(context->cache_size);
	assert(item);
	cJSON_AddItemToObject(root, "cache_size", item);

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

	filename = context->contextfile;
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
#ifdef WIN32
		context->contextfile = pcs_utils_strdup(filename);
#else
		/* Can't open the path that start with '~/'. why? It's not good, but work. */
		if (filename[0] == '~' && filename[1] == '/') {
			static char tmp[1024] = { 0 };
			strcpy(tmp, getenv("HOME"));
			strcat(tmp, filename + 1);
			context->contextfile = pcs_utils_strdup(tmp);
		}
		else {
			context->contextfile = pcs_utils_strdup(filename);
		}
#endif
	}
	filesize = read_file(context->contextfile, &filecontent);
	if (filesize <= 0) {
		fprintf(stderr, "Error: Can't read the context file (%s).\n", context->contextfile);
		if (filecontent) pcs_free(filecontent);
		return -1;
	}
	root = cJSON_Parse(filecontent);
	if (!root) {
		fprintf(stderr, "Error: Broken context file (%s).\n", context->contextfile);
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

	item = cJSON_GetObjectItem(root, "max_thread");
	if (item) {
		if (((int)item->valueint) < 1) {
			printf("warning: Invalid context.max_thread, the value should be great than 0, use default value: %d.\n", context->max_thread);
		}
		else {
			context->max_thread = (int)item->valueint;
		}
	}

	item = cJSON_GetObjectItem(root, "max_speed_per_thread");
	if (item) {
		if (((int)item->valueint) < 0) {
			printf("warning: Invalid context.max_speed_per_thread, the value should be >= 0, use default value: %d.\n", context->max_speed_per_thread);
		}
		else {
			context->max_speed_per_thread = (int)item->valueint;
		}
	}

	item = cJSON_GetObjectItem(root, "max_upload_speed_per_thread");
	if (item) {
		if (((int)item->valueint) < 0) {
			printf("warning: Invalid context.max_upload_speed_per_thread, the value should be >= 0, use default value: %d.\n", context->max_upload_speed_per_thread);
		}
		else {
			context->max_upload_speed_per_thread = (int)item->valueint;
		}
	}

	item = cJSON_GetObjectItem(root, "user_agent");
	if (item && item->valuestring && item->valuestring[0]) {
		if (context->user_agent) pcs_free(context->user_agent);
		context->user_agent = pcs_utils_strdup(item->valuestring);
	}

	item = cJSON_GetObjectItem(root, "cache_size");
	if (item) {
		if (((int)item->valueint) < 0) {
			printf("warning: Invalid context.cache_size, the value should be >= 0, use default value: %d.\n", context->cache_size);
		}
		else {
			context->cache_size = (int)item->valueint;
		}
	}

	cJSON_Delete(root);
	pcs_free(filecontent);
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
	context->max_thread = DEFAULT_THREAD_NUM;
	context->max_speed_per_thread = 0;
	context->max_upload_speed_per_thread = 0;
	context->cache_size = MAX_CACHE_SIZE;

	context->user_agent = pcs_utils_strdup(USAGE);
}

/*释放上下文*/
static void free_context(ShellContext *context)
{
	if (context->cookiefile) pcs_free(context->cookiefile);
	if (context->captchafile) pcs_free(context->captchafile);
	if (context->workdir) pcs_free(context->workdir);
	if (context->list_sort_name) pcs_free(context->list_sort_name);
	if (context->list_sort_direction) pcs_free(context->list_sort_direction);
	if (context->secure_method) pcs_free(context->secure_method);
	if (context->secure_key) pcs_free(context->secure_key);
	if (context->contextfile) pcs_free(context->contextfile);
	if (context->user_agent) pcs_free(context->user_agent);
	memset(context, 0, sizeof(ShellContext));
}

/*初始化PCS*/
static Pcs *create_pcs(ShellContext *context)
{
	Pcs *pcs = pcs_create(context->cookiefile);
	if (!pcs) return NULL;
	pcs_setopt(pcs, PCS_OPTION_CAPTCHA_FUNCTION, (void *)&verifycode);
	pcs_setopt(pcs, PCS_OPTION_CAPTCHA_FUNCTION_DATA, (void *)context);
    pcs_setopt(pcs, PCS_OPTION_INPUT_FUNCTION, (void *)&input_str);
    pcs_setopt(pcs, PCS_OPTION_INPUT_FUNCTION_DATA, (void *)context);
	pcs_setopts(pcs,
		PCS_OPTION_PROGRESS_FUNCTION, (void *)&upload_progress,
		PCS_OPTION_PROGRESS, (void *)((long)PcsFalse),
		PCS_OPTION_USAGE, (void*)context->user_agent,
		//PCS_OPTION_TIMEOUT, (void *)((long)TIMEOUT),
		PCS_OPTION_CONNECTTIMEOUT, (void *)((long)CONNECTTIMEOUT),
		PCS_OPTION_END);
	return pcs;
}

static void destroy_pcs(Pcs *pcs)
{
	pcs_destroy(pcs);
}

#pragma endregion

#pragma region 打印用法相关函数

/*打印版本*/
static void version()
{
	printf(program_full_name "%s\n", app_name);
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
	printf("\nUsage: %s encode [-defh] <src> <dst>\n", app_name);
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

/*打印 fix 命令用法。*/
static void usage_fix()
{
	version();
	printf("\nUsage: %s fix [-fh] <md5> <length> <scrap> <remote path>\n", app_name);
	printf("\nDescription:\n");
	printf("  Fix file base md5 and scrap.\n");
	printf("  Some times, we download a file from foreign website, the speed is slow.\n");
	printf("  In the case, if we know the actual size and md5 of the file, we can \n");
	printf("  download first 256KB block from the foreign website, \n");
	printf("  and then use the \"pcs fix\" command to try to repair the file.\n");
	printf("  After success, the file will be saved in Baidu netdisk. \n");
	printf("  If somebody have been uploaded the file to Baidu netdisk, \n");
	printf("  it should be 100 % successful.\n");
	printf("\nOptions:\n");
	printf("  -f    Force override the remote file when the file exists on netdisk.\n");
	printf("  -h    Print the usage.\n");
	printf("\nSamples:\n");
	printf("  %s fix -h\n", app_name);
	printf("  %s fix 39d768542cd2420771f28b9a3652412f 5849513984 ~/xxx.iso xxx.iso\n", app_name);
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
	printf("  -md5   Print md5 if possiable.\n");
	printf("  -thumb Print thumb url if possiable.\n");
	printf("  -h     Print the usage.\n");
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
	printf("  list_page_size       UInt       > 0\n");
	printf("  list_sort_direction  Enum       asc|desc\n");
	printf("  list_sort_name       Enum       name|time|size\n");
	printf("  secure_enable        Boolean    true|false\n");
	printf("  secure_key           String     not null when 'secure_method' is not 'plaintext'\n");
	printf("  secure_method        Enum       plaintext|aes-cbc-128|aes-cbc-192|aes-cbc-256\n");
	printf("  timeout_retry        Boolean    true|false. \n");
	printf("  max_thread           UInt       > 0 and < %d. The max number of thread that allow create.\n", MAX_THREAD_NUM);
	printf("  max_speed_per_thread Int        >= 0. The max speed in KiB per thread.\n");
	printf("  max_upload_speed_per_thread Int >= 0. The max speed in KiB per thread.\n");
	printf("  user_agent           String     set user agent.\n");
	printf("  cache_size           Int        >= 0. The max cache size in KiB.\n");
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
	printf("  -md5   Print md5 if possiable.\n");
	printf("  -thumb Print thumb url if possiable.\n");
	printf("  -h     Print the usage.\n");
	printf("  -r     Recursive search the sub directories.\n");
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
		   "    e.g. A side of the target is file and another is directory.\n"
		   );
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
	printf("  The %s is client of baidu net disk. It supplied many functions, \n", app_name);
	printf("  which can manage baidu net disk on terminal, such as ls, cp, rm, \n");
	printf("  mv, rename, download, upload, search and so on. \n");
	printf("  The %s provided AES encryption, which can protected your data.\n", app_name);
	printf("  The %s is open source, and published on MIT. \n", app_name);
	printf("  Please see https://github.com/GangZhuo/baidupcs. \n");
	printf("\nOptions:\n");
	printf("  --context=<file path>  Specify context.\n");
	printf("\nCommands:\n");
	printf("  cat      Print the file content\n");
	printf("  cd       Change the work directory\n");
	printf("  copy     Copy the file|directory\n");
	printf("  compare  Print the differents between local and net disk\n");
	printf("  context  Print the context\n");
	printf("  download Download the file\n");
	printf("  echo     Write the text into net disk file\n");
	printf("  encode   Encrypt/decrypt the file\n");
	printf("  fix      Fix file base md5 and scrap\n");
	printf("  help     Print the usage\n");
	printf("  list     List the directory\n");
	printf("  login    Login\n");
	printf("  logout   Logout\n");
	printf("  meta     Print the file|directory meta information\n");
	printf("  mkdir    Make a new directory\n");
	printf("  move     Move the file|directory into other file|directory\n");
	printf("  pwd      Print the current work directory\n");
	printf("  quota    Print the quota\n");
	printf("  remove   Remove the file|directory\n");
	printf("  rename   Rename the file|directory\n");
	printf("  set      Change the context, you can print the context by 'context' command\n");
	printf("  search   Search the files in the specify directory\n");
	printf("  synch    Synch between local and net disk. You can 'compare' first.\n");
	printf("  upload   Upload the file\n");
	printf("  version  Print the version\n");
	printf("  who      Print the current user\n");
	printf("Use '%s <command> -h' to print command usage. \n", app_name);
	printf("Sample: \n");
	printf("  %s help\n", app_name);
	printf("  %s help cat\n", app_name);
	printf("  %s cat -h\n", app_name);
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
		Pcs *pcs = create_pcs(context);
		if (!pcs) return -1;
		destroy_pcs(context->pcs);
		context->pcs = pcs;
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
	return 0;
}

/*设置上下文中的secure_key值*/
static int set_secure_key(ShellContext *context, const char *val)
{
	if (!val || !val[0]) return -1;
	if (streq(context->secure_key, val, -1)) return 0;
	if (context->secure_key) pcs_free(context->secure_key);
	context->secure_key = pcs_utils_strdup(val);
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
	return 0;
}

/*设置上下文中的timeout_retry值*/
static int set_timeout_retry(ShellContext *context, const char *val)
{
	if (!val || !val[0]) return -1;
	if (strcmp(val, "true") == 0 || strcmp(val, "1") == 0) {
		context->timeout_retry = 1;
	}
	else if (strcmp(val, "false") == 0 || strcmp(val, "0") == 0) {
		context->timeout_retry = 0;
	}
	else {
		return -1;
	}
	return 0;
}

/*设置上下文中的max_thread值*/
static int set_max_thread(ShellContext *context, const char *val)
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
	if (v < 1 || v > MAX_THREAD_NUM) return -1;
	context->max_thread = v;
	return 0;
}

/*设置上下文中的max_speed_per_thread值*/
static int set_max_speed_per_thread(ShellContext *context, const char *val)
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
	if (v < 0) return -1;
	context->max_speed_per_thread = v;
	return 0;
}

/*设置上下文中的max_upload_speed_per_thread值*/
static int set_max_upload_speed_per_thread(ShellContext *context, const char *val)
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
	if (v < 0) return -1;
	context->max_upload_speed_per_thread = v;
	return 0;
}

/*设置上下文中的cache_size值*/
static int set_cache_size(ShellContext *context, const char *val)
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
	if (v < 0) return -1;
	context->cache_size = v;
	return 0;
}

/*设置上下文中的 user_agent 值*/
static int set_user_agent(ShellContext *context, const char *val)
{
	if (!val || !val[0]) return -1;
	if (streq(context->user_agent, val, -1)) return 0;
	if (context->user_agent) pcs_free(context->user_agent);
	context->user_agent = pcs_utils_strdup(val);
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
	printf("%s", (char *)a);
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
static MyMeta *meta_create(const char *path, const char *md5)
{
	MyMeta *meta;
	meta = (MyMeta *)pcs_malloc(sizeof(MyMeta));
	memset(meta, 0, sizeof(MyMeta));
	if (path) {
		meta->path = pcs_utils_strdup(path);
	}
	if (md5) {
		meta->md5 = pcs_utils_strdup(md5);
	}
	return meta;
}

/*释放掉一个MyMeta*/
static void meta_destroy(MyMeta *meta)
{
	if (!meta) return;
	if (meta->path) pcs_free(meta->path);
	if (meta->remote_path) pcs_free(meta->remote_path);
	if (meta->md5) pcs_free(meta->md5);
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
	meta = meta_create(info->path, NULL);
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
#ifdef _WIN32
			fprintf(stderr, "\r");  //清除该行
			fprintf(stderr, "                                        ");  //清除该行
			fprintf(stderr, "\r");  //清除该行
#else
			fprintf(stderr, "\033[K");  //清除该行
			fprintf(stderr, "\033[1A"); //先回到上一行
			fprintf(stderr, "\033[K");  //清除该行
#endif
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
	else if (pcsres == PCS_NOT_LOGIN) {
		printf("You are not logon or your session is time out. You can login by 'login' command.\n");
	}
	else {
		printf("Error: %s\n", pcs_strerror(context->pcs));
	}
	return PcsFalse;
}

static struct DownloadThreadState *pop_download_threadstate(struct DownloadState *ds)
{
	struct DownloadThreadState *ts = NULL;
	lock_for_download(ds);
	ts = ds->threads;
	while (ts && ts->status != DOWNLOAD_STATUS_PENDDING) {
		ts = ts->next;
	}
	if (ts && ts->status == DOWNLOAD_STATUS_PENDDING)
		ts->status = DOWNLOAD_STATUS_DOWNLOADING;
	else
		ts = NULL;
	unlock_for_download(ds);
	return ts;
}

#ifdef _WIN32
static DWORD WINAPI download_thread(LPVOID params)
#else
static void *download_thread(void *params)
#endif
{
	PcsRes res;
	int dsstatus;
	struct DownloadState *ds = (struct DownloadState *)params;
	ShellContext *context = ds->context;
	struct DownloadThreadState *ts = pop_download_threadstate(ds);
	Pcs *pcs;
	srand((unsigned int)time(NULL));
	if (ds->threads != ts) {
		sleep(rand() % 5);
	}
	if (ts == NULL) {
		lock_for_download(ds);
		ds->num_of_running_thread--;
		unlock_for_download(ds);
#ifdef _WIN32
		return (DWORD)0;
#else
		return NULL;
#endif
	}
	pcs = create_pcs(context);
	if (!pcs) {
		lock_for_download(ds);
		if (ds->pErrMsg) {
			if (*(ds->pErrMsg)) pcs_free(*(ds->pErrMsg));
			(*(ds->pErrMsg)) = pcs_utils_sprintf("Can't create pcs context.");
		}
		//ds->status = DOWNLOAD_STATUS_FAIL;
		ds->num_of_running_thread--;
		ts->status = DOWNLOAD_STATUS_PENDDING;
		unlock_for_download(ds);
#ifdef _WIN32
		return (DWORD)0;
#else
		return NULL;
#endif
	}
	pcs_clone_userinfo(pcs, context->pcs);
	while (ts) {
		ts->pcs = pcs;
		lock_for_download(ds);
		dsstatus = ds->status;
		unlock_for_download(ds);
		if (dsstatus != DOWNLOAD_STATUS_OK) {
			lock_for_download(ds);
			ts->status = DOWNLOAD_STATUS_PENDDING;
			unlock_for_download(ds);
			break;
		}
		pcs_setopts(pcs,
			PCS_OPTION_DOWNLOAD_WRITE_FUNCTION, &download_write_for_multy_thread,
			PCS_OPTION_DOWNLOAD_WRITE_FUNCTION_DATA, ts,
			PCS_OPTION_END);
		res = pcs_download(pcs, ds->remote_file, convert_to_real_speed(context->max_speed_per_thread), ts->postion, ts->end - ts->postion);
		if (res != PCS_OK) {
			int delay;
			lock_for_download(ds);
			if (!ds->pErrMsg) {
				(*(ds->pErrMsg)) = pcs_utils_sprintf("%s", pcs_strerror(pcs));
			}
			ts->postion = ts->start; /* retry from beginning of slice */
			unlock_for_download(ds);
			delay = rand();
			delay %= 10;
			sleep(delay); /*10秒后重试*/
			continue;
		}
		lock_for_download(ds);
		ts->status = DOWNLOAD_STATUS_OK;
		unlock_for_download(ds);
		ts = pop_download_threadstate(ds);
	}

	destroy_pcs(pcs);
	lock_for_download(ds);
	ds->num_of_running_thread--;
	unlock_for_download(ds);
#ifdef _WIN32
	return (DWORD)0;
#else
	return NULL;
#endif
}

static void start_download_thread(struct DownloadState *ds, void **pHandle)
{
#ifdef _WIN32
	DWORD tid;
	HANDLE thandle;
	/* hThread = CreateThread (&security_attributes, dwStackSize, ThreadProc, pParam, dwFlags, &idThread)
	WINBASEAPI HANDLE WINAPI CreateThread(LPSECURITY_ATTRIBUTES,DWORD,LPTHREAD_START_ROUTINE,PVOID,DWORD,PDWORD);
	第一个参数是指向SECURITY_ATTRIBUTES型态的结构的指针。在Windows 98中忽略该参数。在Windows NT中，它被设为NULL。
	第二个参数是用于新线程的初始堆栈大小，默认值为0。在任何情况下，Windows根据需要动态延长堆栈的大小。
	第三个参数是指向线程函数的指标。函数名称没有限制，但是必须以下列形式声明:DWORD WINAPI ThreadProc (PVOID pParam) ;
	第四个参数为传递给ThreadProc的参数。这样主线程和从属线程就可以共享数据。
	第五个参数通常为0，但当建立的线程不马上执行时为旗标
	第六个参数是一个指针，指向接受执行绪ID值的变量
	*/
	thandle = CreateThread(NULL, 0, download_thread, (LPVOID)ds, 0, &tid); // 建立线程
	if (pHandle) *pHandle = thandle;
	if (!thandle) {
		printf("Error: Can't create download thread.\n");
		lock_for_download(ds);
		ds->num_of_running_thread--;
		unlock_for_download(ds);
	}
#else
	int err;
	pthread_t main_tid;
	err = pthread_create(&main_tid, NULL, download_thread, ds);
	if (err) {
		printf("Error: Can't create download thread.\n");
		lock_for_download(ds);
		ds->num_of_running_thread--;
		unlock_for_download(ds);
	}
#endif
}

static int restore_download_state(struct DownloadState *ds, const char *tmp_local_path, int *pendding_count)
{
	LocalFileInfo *tmpFileInfo;
	int64_t tmp_file_size = 0;
	tmpFileInfo = GetLocalFileInfo(tmp_local_path);
	if (tmpFileInfo) {
		tmp_file_size = tmpFileInfo->size;
		DestroyLocalFileInfo(tmpFileInfo);
	}

	if ((tmp_file_size >= ds->file_size + 4 + sizeof(struct DownloadThreadState))) {
		FILE *pf;
		int magic;
		int thread_count = 0;
		struct DownloadThreadState *ts, *tail = NULL;
		curl_off_t left_size = 0;
		pf = fopen(tmp_local_path, "rb");
		if (!pf) return -1;
		if (fseeko(pf, ds->file_size, SEEK_SET)) {
			fclose(pf);
			return -1;
		}
		if (fread(&magic, 4, 1, pf) != 1) {
			fclose(pf);
			return -1;
		}
		if (magic != THREAD_STATE_MAGIC) {
			fclose(pf);
			return -1;
		}
		if (pendding_count) (*pendding_count) = 0;
		while(1) {
			ts = (struct DownloadThreadState *) pcs_malloc(sizeof(struct DownloadThreadState));
			if (fread(ts, sizeof(struct DownloadThreadState), 1, pf) != 1) {
				pcs_free(ts);
				break;
			}
			ts->ds = ds;
			//printf("%d: ", thread_count);
			if (ts->status != DOWNLOAD_STATUS_OK) {
				ts->status = DOWNLOAD_STATUS_PENDDING;
				ts->postion = ts->start;
				if (pendding_count) (*pendding_count)++;
				//printf("*");
			}
			//printf("%d/%d\n", (int)ts->start, (int)ts->end);
			left_size += (ts->end - ts->postion);
			if (tail == NULL) {
				ds->threads = tail = ts;
			}
			else {
				tail->next = ts;
				tail = ts;
			}
			thread_count++;
			ts->tid = thread_count;
			if (!ts->next) {
				ts->next = NULL;
				break;
			}
			ts->next = NULL;
		}
		fclose(pf);
		ds->resume_from = ds->file_size - left_size;
		ds->num_of_slice = thread_count;
		return 0;
	}
	return -1;
}

static int create_file(const char *file, int64_t fsize)
{
	FILE *pf;
	pf = fopen(file, "wb");
	if (!pf) return -1;
	fclose(pf);
	if (fsize > 0) {
		if (truncate(file, fsize)) {
			return -1;
		}
	}
	return 0;
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
	const char *local_basedir, const char *remote_basedir, const char *md5,
	char **pErrMsg, int *op_st)
{
	PcsRes res;
	struct DownloadState ds = { 0 };
	char *local_path, *remote_path, *dir,
		*tmp_local_path;
	const char *local_filename;
	int secure_method = 0;
	int64_t fsize = 0;

	init_download_state(&ds);

	ds.context = context;
	ds.pErrMsg = pErrMsg;
	ds.status = 0;

	local_filename = filename(local_file);
	if (strlen(local_filename) == 0) {
		char *new_local_file;
		dir = base_dir(local_file, -1);
		new_local_file = combin_path(dir, -1, filename(remote_file));
		local_path = combin_path(local_basedir, -1, new_local_file);
		pcs_free(new_local_file);
		pcs_free(dir);
	}
	else {
		local_path = combin_path(local_basedir, -1, local_file);
	}

	/*创建目录*/
	dir = base_dir(local_path, -1);
	if (dir) {
		if (CreateDirectoryRecursive(dir) != MKDIR_OK) {
			if (pErrMsg) {
				if (*pErrMsg) pcs_free(*pErrMsg);
				(*pErrMsg) = pcs_utils_sprintf("Can't create the directory: %s", dir);
			}
			if (op_st) (*op_st) = OP_ST_FAIL;
			pcs_free(dir);
			pcs_free(local_path);
			uninit_download_state(&ds);
			return -1;
		}
		pcs_free(dir);
	}

	tmp_local_path = (char *)pcs_malloc(strlen(local_path) + (md5 ? strlen(md5) + 1 : 0) + strlen(TEMP_FILE_SUFFIX) + 1);
	strcpy(tmp_local_path, local_path);
	if (md5) {
		strcat(tmp_local_path, ".");
		strcat(tmp_local_path, md5);
	}
	strcat(tmp_local_path, TEMP_FILE_SUFFIX);

	dir = combin_net_disk_path(context->workdir, remote_basedir);
	if (!dir) {
		if (pErrMsg) {
			if (*pErrMsg) pcs_free(*pErrMsg);
			(*pErrMsg) = pcs_utils_sprintf("Can't combin remote path\n");
		}
		if (op_st) (*op_st) = OP_ST_FAIL;
		DeleteFileRecursive(tmp_local_path);
		pcs_free(tmp_local_path);
		pcs_free(local_path);
		uninit_download_state(&ds);
		return -1;
	}
	remote_path = combin_net_disk_path(dir, remote_file);
	pcs_free(dir);
	if (!remote_path) {
		if (pErrMsg) {
			if (*pErrMsg) pcs_free(*pErrMsg);
			(*pErrMsg) = pcs_utils_sprintf("Can't combin remote path\n");
		}
		if (op_st) (*op_st) = OP_ST_FAIL;
		DeleteFileRecursive(tmp_local_path);
		pcs_free(tmp_local_path);
		pcs_free(local_path);
		uninit_download_state(&ds);
		return -1;
	}

	ds.remote_file = remote_path;

	fsize = pcs_get_download_filesize(context->pcs, remote_path);
	ds.file_size = fsize;
    if (fsize < 0) {
		if (pErrMsg) {
			if (*pErrMsg) pcs_free(*pErrMsg);
			(*pErrMsg) = pcs_utils_sprintf("Can't get the file size.\n");
		}
		if (op_st) (*op_st) = OP_ST_FAIL;
		DeleteFileRecursive(tmp_local_path);
		pcs_free(tmp_local_path);
		pcs_free(local_path);
        pcs_free(remote_path);
		uninit_download_state(&ds);
		return -1;
	}

	if (fsize <= MIN_SLICE_SIZE) {
		/*启动下载*/
		/*打开文件*/
		ds.pf = fopen(tmp_local_path, "wb");
		if (!ds.pf) {
			if (pErrMsg) {
				if (*pErrMsg) pcs_free(*pErrMsg);
				(*pErrMsg) = pcs_utils_sprintf("Can't open or create the temp file: %s\n", tmp_local_path);
			}
			if (op_st) (*op_st) = OP_ST_FAIL;
			DeleteFileRecursive(tmp_local_path);
			pcs_free(tmp_local_path);
			pcs_free(local_path);
			pcs_free(remote_path);
			uninit_download_state(&ds);
			return -1;
		}
		ds.cache.fp = ds.pf;
		pcs_setopts(context->pcs,
			PCS_OPTION_DOWNLOAD_WRITE_FUNCTION, &download_write,
			PCS_OPTION_DOWNLOAD_WRITE_FUNCTION_DATA, &ds,
			PCS_OPTION_END);
		res = pcs_download(context->pcs, remote_path, convert_to_real_speed(context->max_speed_per_thread), 0, 0);
		if (ds.cache.total_size > 0) {
			int rc = cache_flush(&ds.cache);
			if (rc) {
				if (pErrMsg) {
					if (*pErrMsg) pcs_free(*pErrMsg);
					(*pErrMsg) = pcs_utils_sprintf("%s\n", "cache_flush() error.");
				}
				if (op_st) (*op_st) = OP_ST_FAIL;
				fclose(ds.pf);
				pcs_free(tmp_local_path);
				pcs_free(local_path);
				pcs_free(remote_path);
				uninit_download_state(&ds);
				return -1;
			}
		}
		fclose(ds.pf);
		if (res != PCS_OK) {
			if (pErrMsg) {
				if (*pErrMsg) pcs_free(*pErrMsg);
				(*pErrMsg) = pcs_utils_sprintf("%s. local_path=%s, remote_path=%s\n",
					pcs_strerror(context->pcs), tmp_local_path, remote_path);
			}
			if (op_st) (*op_st) = OP_ST_FAIL;
			//if (!strstr(pcs_strerror(context->pcs), "Timeout"))
			//	DeleteFileRecursive(tmp_local_path);
			pcs_free(tmp_local_path);
			pcs_free(local_path);
			pcs_free(remote_path);
			uninit_download_state(&ds);
			return -1;
		}
	}
	else {
		struct DownloadThreadState *ts, *tail = NULL;
		curl_off_t start = 0;
		const char *mode = "rb+";
		int slice_count, pendding_slice_count = 0, thread_count, running_thread_count, i, is_success;
		int64_t slice_size;
#ifdef _WIN32
		HANDLE *handles = NULL;
#endif
		if (restore_download_state(&ds, tmp_local_path, &pendding_slice_count)) {
			//分片开始
			mode = "wb";
			ds.resume_from = 0;
			slice_size = MIN_SLICE_SIZE;
			slice_count = (int)(fsize / slice_size);
			if ((fsize % slice_size)) slice_count++;

			for (i = 0; i < slice_count; i++) {
				ts = (struct DownloadThreadState *) pcs_malloc(sizeof(struct DownloadThreadState));
				ts->ds = &ds;
				ts->start = start;
				ts->postion = start;
				start += slice_size;
				ts->end = start;
				if (ts->end > ((curl_off_t)fsize)) ts->end = (curl_off_t)fsize;
				ts->status = DOWNLOAD_STATUS_PENDDING;
				pendding_slice_count++;
				ts->tid = i + 1;
				ts->next = NULL;
				if (tail == NULL) {
					ds.threads = tail = ts;
				}
				else {
					tail->next = ts;
					tail = ts;
				}
			}
			ds.num_of_slice = slice_count;
			//分片结束
			//printf("fs: %" PRIu64 ", %" PRIu64 ", %" PRIu64 ", %d\n", ds.file_size, ds.file_size + 4 + sizeof(struct DownloadThreadState) * slice_count, slice_size, slice_count);
			if (create_file(tmp_local_path, ds.file_size + 4 + sizeof(struct DownloadThreadState) * slice_count)) {
				if (pErrMsg) {
					if (*pErrMsg) pcs_free(*pErrMsg);
					(*pErrMsg) = pcs_utils_sprintf("Can't create the temp file: %s, maybe have no disk space.\n", tmp_local_path);
				}
				if (op_st) (*op_st) = OP_ST_FAIL;
				DeleteFileRecursive(tmp_local_path);
				pcs_free(tmp_local_path);
				pcs_free(local_path);
				pcs_free(remote_path);
				uninit_download_state(&ds);
				return -1;
			}
			mode = "rb+";
		}
		/*打开文件*/
		ds.pf = fopen(tmp_local_path, mode);
		if (!ds.pf) {
			if (pErrMsg) {
				if (*pErrMsg) pcs_free(*pErrMsg);
				(*pErrMsg) = pcs_utils_sprintf("Can't open or create the temp file: %s\n", tmp_local_path);
			}
			if (op_st) (*op_st) = OP_ST_FAIL;
			DeleteFileRecursive(tmp_local_path);
			pcs_free(tmp_local_path);
			pcs_free(local_path);
			pcs_free(remote_path);
			uninit_download_state(&ds);
			return -1;
		}
		ds.cache.fp = ds.pf;
		//保存分片数据
		printf("Create/Load temporary file...\r");
		fflush(stdout);
		if (save_thread_states_to_file(ds.pf, ds.file_size, ds.threads)) {
			if (pErrMsg) {
				if (*pErrMsg) pcs_free(*pErrMsg);
				(*pErrMsg) = pcs_utils_sprintf("Can't save slices into temporary file: %s \n", tmp_local_path);
			}
			if (op_st) (*op_st) = OP_ST_FAIL;
			DeleteFileRecursive(tmp_local_path);
			pcs_free(tmp_local_path);
			pcs_free(local_path);
			pcs_free(remote_path);
			fclose(ds.pf);
			uninit_download_state(&ds);
			return -1;
		}
		fflush(ds.pf);

		printf("Starting threads...\r");
		fflush(stdout);

		thread_count = pendding_slice_count;
		if (thread_count > context->max_thread && context->max_thread > 0)
			thread_count = context->max_thread;
		ds.num_of_running_thread = thread_count;
		//printf("\nthread: %d, slice: %d\n", thread_count, ds.num_of_slice);
#ifdef _WIN32
		handles = (HANDLE *)pcs_malloc(sizeof(HANDLE) * thread_count);
		memset(handles, 0, sizeof(HANDLE) * thread_count);
#endif
		for (i = 0; i < thread_count; i++) {
#ifdef _WIN32
			start_download_thread(&ds, handles + i);
#else
			start_download_thread(&ds, NULL);
#endif
		}

		/*等待所有运行的线程退出*/
		while (1) {
			lock_for_download(&ds);
			running_thread_count = ds.num_of_running_thread;
			unlock_for_download(&ds);
			if (running_thread_count < 1) break;
			sleep(1);
		}

		if (ds.cache.total_size > 0) {
			int rc = cache_flush(&ds.cache);
			if (rc) {
				if (pErrMsg) {
					if (!(*pErrMsg))
						(*pErrMsg) = pcs_utils_sprintf("%s\n", "cache_flush() error.");
				}
				if (op_st) (*op_st) = OP_ST_FAIL;
				fclose(ds.pf);
				DeleteFileRecursive(tmp_local_path);
				pcs_free(tmp_local_path);
				pcs_free(local_path);
				pcs_free(remote_path);
				uninit_download_state(&ds);
				return -1;
			}
		}

		fclose(ds.pf);

#ifdef _WIN32
		for (i = 0; i < thread_count; i++) {
			if (handles[i]) {
				CloseHandle(handles[i]);
			}
		}
		pcs_free(handles);
#endif

		/*判断是否所有分片都下载完成了*/
		is_success = 1;
		ts = ds.threads;
		while (ts) {
			if (ts->status != DOWNLOAD_STATUS_OK) {
				is_success = 0;
				break;
			}
			ts = ts->next;
		}

		if (!is_success) {
			if (pErrMsg) {
				if (!(*pErrMsg))
					(*pErrMsg) = pcs_utils_sprintf("Download fail.\n");
			}
			if (op_st) (*op_st) = OP_ST_FAIL;
			DeleteFileRecursive(tmp_local_path);
			pcs_free(tmp_local_path);
			pcs_free(local_path);
			pcs_free(remote_path);
			uninit_download_state(&ds);
			return -1;
		}
		if (truncate(tmp_local_path, ds.file_size)) {
			if (pErrMsg) {
				if (!(*pErrMsg))
					(*pErrMsg) = pcs_utils_sprintf("Can't truncate the temp file %s.\n", tmp_local_path);
			}
			if (op_st) (*op_st) = OP_ST_FAIL;
			DeleteFileRecursive(tmp_local_path);
			pcs_free(tmp_local_path);
			pcs_free(local_path);
			pcs_free(remote_path);
			uninit_download_state(&ds);
			return -1;
		}
	}

	uninit_download_state(&ds);

	DeleteFileRecursive(local_path);

	if (context->secure_enable) {
		int context_secure_method = get_secure_method(context);
		if (context_secure_method == PCS_SECURE_AES_CBC_128
			|| context_secure_method == PCS_SECURE_AES_CBC_192
			|| context_secure_method == PCS_SECURE_AES_CBC_256) {
			secure_method = get_file_secure_method(tmp_local_path);
		}
	}

	if (secure_method == PCS_SECURE_AES_CBC_128 || secure_method == PCS_SECURE_AES_CBC_192 || secure_method == PCS_SECURE_AES_CBC_256) {
		if (decrypt_file(tmp_local_path, local_path, context->secure_key, pErrMsg)) {
			//if (pErrMsg) {
			//	if (*pErrMsg) pcs_free(*pErrMsg);
			//	(*pErrMsg) = pcs_utils_sprintf("Error: The file have been download at %s, but can't decrypted to %s.\n"
			//		" Maybe broken data.", tmp_local_path, local_path);
			//}
			if (pErrMsg) {
				if (!(*pErrMsg))
					(*pErrMsg) = pcs_utils_sprintf("Can't decrypt the file.\n");
			}
			if (op_st) (*op_st) = OP_ST_FAIL;
			//DeleteFileRecursive(tmp_local_path);
			pcs_free(tmp_local_path);
			pcs_free(local_path);
			pcs_free(remote_path);
			return -1;
		}
		DeleteFileRecursive(tmp_local_path);
	}
	else if (rename(tmp_local_path, local_path)) {
		if (pErrMsg) {
			if (*pErrMsg) pcs_free(*pErrMsg);
			(*pErrMsg) = pcs_utils_sprintf("The file have been download at %s, but can't rename to %s.\n"
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


static struct UploadThreadState *pop_upload_threadstate(struct UploadState *us)
{
	struct UploadThreadState *ts = NULL;
	lock_for_upload(us);
	ts = us->threads;
	while (ts && ts->status != UPLOAD_STATUS_PENDDING) {
		ts = ts->next;
	}
	if (ts && ts->status == UPLOAD_STATUS_PENDDING)
		ts->status = UPLOAD_STATUS_UPLOADING;
	else
		ts = NULL;
	unlock_for_upload(us);
	return ts;
}

/*see http://curl.haxx.se/libcurl/c/CURLOPT_READFUNCTION.html */
static size_t read_slice(void *ptr, size_t size, size_t nmemb, void *userdata)
{
	static char tmp[64];
	struct UploadThreadState *ts = (struct UploadThreadState*)userdata;
	struct UploadState *us = ts->us;
	time_t tm;
	size_t sz;

	lock_for_upload(us);

	if (ts->start + ts->uploaded_size >= ts->end) {
		unlock_for_upload(us);
		return 0;
	}

	if (fseeko(us->pf, ts->start + ts->uploaded_size, SEEK_SET)) {
		if (us->pErrMsg && !(*us->pErrMsg)) {
			(*(us->pErrMsg)) = pcs_utils_sprintf("Can't fseeko().");
		}
		us->status = UPLOAD_STATUS_FAIL;
		unlock_for_upload(us);
		return CURL_READFUNC_ABORT;
	}
	sz = size * nmemb;
	if (ts->start + ts->uploaded_size + sz > ts->end) {
		sz = (size_t)(ts->end - ts->start - ts->uploaded_size);
	}
	sz = fread(ptr, 1, sz, us->pf);
	if (sz == 0) {
		if (us->pErrMsg && !(*us->pErrMsg)) {
			(*(us->pErrMsg)) = pcs_utils_sprintf("Can't read the file.");
		}
		us->status = UPLOAD_STATUS_FAIL;
		unlock_for_upload(us);
		return CURL_READFUNC_ABORT;
	}
	//printf("%llu - %llu (%llu) \n", (long long)ts->start + ts->uploaded_size, (long long)ts->start + ts->uploaded_size + sz, (long long)sz);
	us->speed += sz;
	us->uploaded_size += sz;
	ts->uploaded_size += sz;


	tm = time(&tm);
	if (tm != us->time) {
		int64_t left_size = us->file_size - us->uploaded_size;
		int64_t remain_tm = (us->speed > 0) ? (left_size / us->speed) : 0;
		us->time = tm;
		tmp[63] = '\0';
		printf("\r                                                \r");
		printf("%s", pcs_utils_readable_size((double)us->uploaded_size, tmp, 63, NULL));
		printf("/%s \t", pcs_utils_readable_size((double)us->file_size, tmp, 63, NULL));
		printf("%s/s \t", pcs_utils_readable_size((double)us->speed, tmp, 63, NULL));
		printf(" %s        ", pcs_utils_readable_left_time(remain_tm, tmp, 63, NULL));
		printf("\r");
		fflush(stdout);
		us->speed = 0;
	}

	unlock_for_upload(us);
	return sz;
}

#ifdef _WIN32
static DWORD WINAPI upload_thread(LPVOID params)
#else
static void *upload_thread(void *params)
#endif
{
	PcsFileInfo *res;
	int dsstatus;
	struct UploadState *ds = (struct UploadState *)params;
	ShellContext *context = ds->context;
	struct UploadThreadState *ts = pop_upload_threadstate(ds);
	Pcs *pcs;

	if (ts == NULL) {
		lock_for_upload(ds);
		ds->num_of_running_thread--;
		unlock_for_upload(ds);
#ifdef _WIN32
		return (DWORD)0;
#else
		return NULL;
#endif
	}
	pcs = create_pcs(context);
	if (!pcs) {
		lock_for_upload(ds);
		if (ds->pErrMsg) {
			if (*(ds->pErrMsg)) pcs_free(*(ds->pErrMsg));
			(*(ds->pErrMsg)) = pcs_utils_sprintf("Can't create pcs context.");
		}
		//ds->status = DOWNLOAD_STATUS_FAIL;
		ds->num_of_running_thread--;
		ts->status = UPLOAD_STATUS_PENDDING;
		unlock_for_upload(ds);
#ifdef _WIN32
		return (DWORD)0;
#else
		return NULL;
#endif
	}
	pcs_clone_userinfo(pcs, context->pcs);
	while (ts) {
		ts->pcs = pcs;
		lock_for_upload(ds);
		dsstatus = ds->status;
		unlock_for_upload(ds);
		if (dsstatus != UPLOAD_STATUS_OK) {
			lock_for_upload(ds);
			ts->status = UPLOAD_STATUS_PENDDING;
			unlock_for_upload(ds);
			break;
		}
		ds->uploaded_size -= ts->uploaded_size;
		ts->uploaded_size = 0;

		res = pcs_upload_slicefile(pcs, &read_slice, ts, (size_t)(ts->end - ts->start),
			convert_to_real_speed(context->max_upload_speed_per_thread));
		if (!res) {
			lock_for_upload(ds);
			if (ds->pErrMsg && !(*ds->pErrMsg)) {
				(*(ds->pErrMsg)) = pcs_utils_sprintf("%s", pcs_strerror(pcs));
			}
			//ds->status = UPLOAD_STATUS_FAIL;
			unlock_for_upload(ds);
#ifdef _WIN32
			printf("Upload slice failed, retry delay 10 second, tid: %x. message: %s\n", GetCurrentThreadId(), pcs_strerror(pcs));
#else
			printf("Upload slice failed, retry delay 10 second, tid: %p. message: %s\n", pthread_self(), pcs_strerror(pcs));
#endif
			sleep(10); /*10秒后重试*/
			continue;
		}
		lock_for_upload(ds);
		ts->status = UPLOAD_STATUS_OK;
		strcpy(ts->md5, res->md5);
		pcs_fileinfo_destroy(res);
		save_upload_thread_states_to_file(ds->slice_file, ds->threads);
		unlock_for_upload(ds);
		ts = pop_upload_threadstate(ds);
	}

	destroy_pcs(pcs);
	lock_for_upload(ds);
	ds->num_of_running_thread--;
	unlock_for_upload(ds);
#ifdef _WIN32
	return (DWORD)0;
#else
	return NULL;
#endif
}

static void start_upload_thread(struct UploadState *us, void **pHandle)
{
#ifdef _WIN32
	DWORD tid;
	HANDLE thandle;
	/* hThread = CreateThread (&security_attributes, dwStackSize, ThreadProc, pParam, dwFlags, &idThread)
	WINBASEAPI HANDLE WINAPI CreateThread(LPSECURITY_ATTRIBUTES,DWORD,LPTHREAD_START_ROUTINE,PVOID,DWORD,PDWORD);
	第一个参数是指向SECURITY_ATTRIBUTES型态的结构的指针。在Windows 98中忽略该参数。在Windows NT中，它被设为NULL。
	第二个参数是用于新线程的初始堆栈大小，默认值为0。在任何情况下，Windows根据需要动态延长堆栈的大小。
	第三个参数是指向线程函数的指标。函数名称没有限制，但是必须以下列形式声明:DWORD WINAPI ThreadProc (PVOID pParam) ;
	第四个参数为传递给ThreadProc的参数。这样主线程和从属线程就可以共享数据。
	第五个参数通常为0，但当建立的线程不马上执行时为旗标
	第六个参数是一个指针，指向接受执行绪ID值的变量
	*/
	thandle = CreateThread(NULL, 0, upload_thread, (LPVOID)us, 0, &tid); // 建立线程
	if (pHandle) *pHandle = thandle;
	if (!thandle) {
		printf("Error: Can't create download thread.\n");
		lock_for_upload(us);
		us->num_of_running_thread--;
		unlock_for_upload(us);
	}
#else
	int err;
	pthread_t main_tid;
	err = pthread_create(&main_tid, NULL, upload_thread, us);
	if (err) {
		printf("Error: Can't create download thread.\n");
		lock_for_upload(us);
		us->num_of_running_thread--;
		unlock_for_upload(us);
	}
#endif
}

static int restore_upload_state(struct UploadState *us, const char *slice_local_path, int *pendding_count)
{
	LocalFileInfo *tmpFileInfo;
	int64_t slice_file_size = 0;
	FILE *pf;
	int magic;
	int thread_count = 0;
	struct UploadThreadState *ts, *tail = NULL;

	tmpFileInfo = GetLocalFileInfo(slice_local_path);
	if (!tmpFileInfo) {
		return -1;
	}
	slice_file_size = tmpFileInfo->size;
	DestroyLocalFileInfo(tmpFileInfo);

	pf = fopen(slice_local_path, "rb");
	if (!pf) return -1;
	if (fread(&magic, 4, 1, pf) != 1) {
		fclose(pf);
		return -1;
	}
	if (magic != THREAD_STATE_MAGIC) {
		fclose(pf);
		return -1;
	}
	if (pendding_count) (*pendding_count) = 0;
	us->uploaded_size = 0;
	while (1) {
		ts = (struct UploadThreadState *) pcs_malloc(sizeof(struct UploadThreadState));
		if (fread(ts, sizeof(struct UploadThreadState), 1, pf) != 1) {
			pcs_free(ts);
			break;
		}
		ts->us = us;
		//printf("%d: ", thread_count);
		if (ts->status != UPLOAD_STATUS_OK) {
			ts->status = UPLOAD_STATUS_PENDDING;
			if (pendding_count) (*pendding_count)++;
			ts->uploaded_size = 0;
			//printf("*");
		}
		us->uploaded_size += ts->uploaded_size;
		//printf("%d/%d\n", (int)ts->start, (int)ts->end);
		if (tail == NULL) {
			us->threads = tail = ts;
		}
		else {
			tail->next = ts;
			tail = ts;
		}
		thread_count++;
		ts->tid = thread_count;
		if (!ts->next) {
			ts->next = NULL;
			break;
		}
		ts->next = NULL;
	}
	fclose(pf);
	us->num_of_slice = thread_count;
	return 0;
}

static inline int do_upload(ShellContext *context, 
	const char *local_file, const char *remote_file, PcsBool is_force,
	const char *local_basedir, const char *remote_basedir,
	char **pErrMsg, int *op_st)
{
	PcsFileInfo *res = NULL;
	char *local_path, *remote_path, *dir;
	int del_local_file = 0;
	int64_t content_length;
	char content_md5[33] = { 0 };

	local_path = combin_path(local_basedir, -1, local_file);
	dir = combin_net_disk_path(context->workdir, remote_basedir);
	remote_path = combin_net_disk_path(dir, remote_file);
	pcs_free(dir);

	if (context->secure_enable) {
		int context_secure_method = get_secure_method(context);
		if (context_secure_method == PCS_SECURE_AES_CBC_128
			|| context_secure_method == PCS_SECURE_AES_CBC_192
			|| context_secure_method == PCS_SECURE_AES_CBC_256) {
			int secure_method = 0;
			secure_method = get_file_secure_method(local_path);
			if (secure_method != PCS_SECURE_AES_CBC_128
				&& secure_method != PCS_SECURE_AES_CBC_192
				&& secure_method != PCS_SECURE_AES_CBC_256) {
				char *tmp_local_path = (char *)pcs_malloc(strlen(local_path) + strlen(TEMP_FILE_SUFFIX) + 1);
				strcpy(tmp_local_path, local_path);
				strcat(tmp_local_path, TEMP_FILE_SUFFIX);
				if (encrypt_file(local_path, tmp_local_path, context_secure_method, context->secure_key, pErrMsg)) {
					//if (pErrMsg) {
					//	if (*pErrMsg) pcs_free(*pErrMsg);
					//	(*pErrMsg) = pcs_utils_sprintf("Error: can't encrypt the file %s.\n",
					//		local_path);
					//}
					if (op_st) (*op_st) = OP_ST_FAIL;
					if (res) pcs_fileinfo_destroy(res);
					pcs_free(tmp_local_path);
					pcs_free(local_path);
					pcs_free(remote_path);
					return -1;
				}
				pcs_free(local_path);
				local_path = tmp_local_path;
				del_local_file = 1;
			}
		}
	}

	content_length = pcs_local_filesize(context->pcs, local_path);
	if (content_length < 0) {
		if (pErrMsg) {
			if (*pErrMsg) pcs_free(*pErrMsg);
			(*pErrMsg) = pcs_utils_sprintf("%s. local_path=%s, remote_path=%s\n",
				pcs_strerror(context->pcs), local_path, remote_path);
		}
		if (op_st) (*op_st) = OP_ST_FAIL;
		if (del_local_file) DeleteFileRecursive(local_path);
		pcs_free(local_path);
		pcs_free(remote_path);
		return -1;
	}

	if (content_length > PCS_RAPIDUPLOAD_THRESHOLD)
		res = pcs_rapid_upload(context->pcs, remote_path, is_force, local_path, content_md5, NULL);
	if (!res && content_length <= MIN_UPLOAD_SLICE_SIZE) {
		pcs_setopts(context->pcs,
			PCS_OPTION_PROGRESS_FUNCTION, &upload_progress,
			PCS_OPTION_PROGRESS_FUNCTION_DATE, NULL,
			PCS_OPTION_PROGRESS, (void *)((long)PcsTrue),
			//PCS_OPTION_TIMEOUT, (void *)0L,
			PCS_OPTION_END);
		res = pcs_upload(context->pcs, remote_path, is_force, local_path,
			convert_to_real_speed(context->max_upload_speed_per_thread));
		//pcs_setopts(context->pcs,
		//	PCS_OPTION_TIMEOUT, (void *)((long)TIMEOUT),
		//	PCS_OPTION_END);
		if (!res || !res->path || !res->path[0]) {
			if (pErrMsg) {
				if (*pErrMsg) pcs_free(*pErrMsg);
				(*pErrMsg) = pcs_utils_sprintf("%s. local_path=%s, remote_path=%s\n",
					pcs_strerror(context->pcs), local_path, remote_path);
			}
			if (op_st) (*op_st) = OP_ST_FAIL;
			if (res) pcs_fileinfo_destroy(res);
			if (del_local_file) DeleteFileRecursive(local_path);
			pcs_free(local_path);
			pcs_free(remote_path);
			return -1;
		}
	}
	else if (!res) {
		struct UploadState us = { 0 };
		struct UploadThreadState *ts, *tail = NULL;
		curl_off_t start = 0;
		int slice_count, pendding_slice_count = 0, thread_count, running_thread_count, i, is_success;
		int64_t slice_size;
#ifdef _WIN32
		HANDLE *handles = NULL;
#endif
		char *slice_file;

		init_upload_state(&us);
		us.context = context;

		if (!(content_md5[0])) {
			if (!pcs_md5_file(context->pcs, local_path, content_md5)) {
				if (pErrMsg) {
					if (*pErrMsg) pcs_free(*pErrMsg);
					(*pErrMsg) = pcs_utils_sprintf("%s. local_path=%s, remote_path=%s\n",
						pcs_strerror(context->pcs), local_path, remote_path);
				}
				if (op_st) (*op_st) = OP_ST_FAIL;
				if (del_local_file) DeleteFileRecursive(local_path);
				pcs_free(local_path);
				pcs_free(remote_path);
				uninit_upload_state(&us);
				return -1;
			}
		}

		/*打开文件*/
		us.pf = fopen(local_path, "rb");
		if (!us.pf) {
			if (pErrMsg) {
				if (*pErrMsg) pcs_free(*pErrMsg);
				(*pErrMsg) = pcs_utils_sprintf("Can't open the file: %s\n", local_path);
			}
			if (op_st) (*op_st) = OP_ST_FAIL;
			if (del_local_file) DeleteFileRecursive(local_path);
			pcs_free(local_path);
			pcs_free(remote_path);
			uninit_upload_state(&us);
			return -1;
		}

		slice_file = (char *)pcs_malloc(strlen(local_path) + 33 + strlen(SLICE_FILE_SUFFIX) + 1);
		strcpy(slice_file, local_path);
		strcat(slice_file, ".");
		strcat(slice_file, content_md5);
		strcat(slice_file, SLICE_FILE_SUFFIX);

		us.slice_file = slice_file;
		us.pErrMsg = pErrMsg;
		us.file_size = content_length;
		slice_count = context->max_thread;
		if (slice_count < 1) slice_count = 1;
		if (restore_upload_state(&us, slice_file, &pendding_slice_count)) {
			//分片开始
			us.uploaded_size = 0;
			slice_size = content_length / slice_count;
			if ((content_length % slice_count))
				slice_size++;
			if (slice_size <= MIN_UPLOAD_SLICE_SIZE)
				slice_size = MIN_UPLOAD_SLICE_SIZE;
			if (slice_size > MAX_UPLOAD_SLICE_SIZE)
				slice_size = MAX_UPLOAD_SLICE_SIZE;
			slice_count = (int)(content_length / slice_size);
			if ((content_length % slice_size)) slice_count++;
			if (slice_count > MAX_UPLOAD_SLICE_COUNT) {
				slice_count = MAX_UPLOAD_SLICE_COUNT;
				slice_size = content_length / slice_count;
				if ((content_length % slice_count))
					slice_size++;
				slice_count = (int)(content_length / slice_size);
				if ((content_length % slice_size)) slice_count++;
			}

			for (i = 0; i < slice_count; i++) {
				ts = (struct UploadThreadState *) pcs_malloc(sizeof(struct UploadThreadState));
				memset(ts, 0, sizeof(struct UploadThreadState));
				ts->us = &us;
				ts->start = start;
				start += slice_size;
				ts->end = start;
				if (ts->end >((curl_off_t)content_length)) ts->end = (curl_off_t)content_length;
				ts->status = UPLOAD_STATUS_PENDDING;
				pendding_slice_count++;
				ts->tid = i + 1;
				ts->next = NULL;
				if (tail == NULL) {
					us.threads = tail = ts;
				}
				else {
					tail->next = ts;
					tail = ts;
				}
			}
			us.num_of_slice = slice_count;
			//分片结束
		}
		//保存分片数据
		printf("Saving slices...\r");
		fflush(stdout);
		if (save_upload_thread_states_to_file(slice_file, us.threads)) {
			if (pErrMsg) {
				if (*pErrMsg) pcs_free(*pErrMsg);
				(*pErrMsg) = pcs_utils_sprintf("Can't save slices into file: %s \n", slice_file);
			}
			if (op_st) (*op_st) = OP_ST_FAIL;
			if (del_local_file) DeleteFileRecursive(local_path);
			DeleteFileRecursive(slice_file);
			pcs_free(local_path);
			pcs_free(remote_path);
			pcs_free(slice_file);
			uninit_upload_state(&us);
			return -1;
		}

		printf("Starting threads...\r");
		fflush(stdout);

		thread_count = pendding_slice_count;
		if (thread_count > context->max_thread && context->max_thread > 0)
			thread_count = context->max_thread;
		us.num_of_running_thread = thread_count;
		//printf("\nthread: %d, slice: %d\n", thread_count, ds.num_of_slice);
#ifdef _WIN32
		handles = (HANDLE *)pcs_malloc(sizeof(HANDLE) * thread_count);
		memset(handles, 0, sizeof(HANDLE) * thread_count);
#endif
		for (i = 0; i < thread_count; i++) {
#ifdef _WIN32
			start_upload_thread(&us, handles + i);
#else
			start_upload_thread(&us, NULL);
#endif
		}

		/*等待所有运行的线程退出*/
		while (1) {
			lock_for_upload(&us);
			running_thread_count = us.num_of_running_thread;
			unlock_for_upload(&us);
			if (running_thread_count < 1) break;
			sleep(1);
		}
		fclose(us.pf);

#ifdef _WIN32
		for (i = 0; i < thread_count; i++) {
			if (handles[i]) {
				CloseHandle(handles[i]);
			}
		}
		pcs_free(handles);
#endif

		/*判断是否所有分片都下载完成了*/
		is_success = 1;
		ts = us.threads;
		while (ts) {
			if (ts->status != UPLOAD_STATUS_OK) {
				is_success = 0;
				break;
			}
			ts = ts->next;
		}

		if (!is_success) {
			if (pErrMsg) {
				if (!(*pErrMsg))
					(*pErrMsg) = pcs_utils_sprintf("Upload fail.\n");
			}
			if (op_st) (*op_st) = OP_ST_FAIL;
			if (del_local_file) DeleteFileRecursive(local_path);
			pcs_free(local_path);
			pcs_free(remote_path);
			pcs_free(slice_file);
			uninit_upload_state(&us);
			return -1;
		}
		else {
			//合并文件
			PcsSList *slist = NULL, *si, *si_tail;
			ts = us.threads;
			while (ts) {
				si = (PcsSList *)pcs_malloc(sizeof(PcsSList));
				si->string = ts->md5;
				si->next = NULL;
				if (slist == NULL) {
					slist = si_tail = si;
				}
				else {
					si_tail->next = si;
					si_tail = si;
				}
				ts = ts->next;
			}
			res = pcs_create_superfile(context->pcs, remote_path, is_force, slist);
			si = slist;
			while (si) {
				si_tail = si;
				si = si->next;
				pcs_free(si_tail);
			}
			if (!res) {
				if (pErrMsg) {
					if ((*pErrMsg)) pcs_free(*pErrMsg);
					(*pErrMsg) = pcs_utils_sprintf("%s", pcs_strerror(context->pcs));
				}
				if (op_st) (*op_st) = OP_ST_FAIL;
				if (del_local_file) DeleteFileRecursive(local_path);
				pcs_free(local_path);
				pcs_free(remote_path);
				pcs_free(slice_file);
				uninit_upload_state(&us);
				return -1;
			}
			uninit_upload_state(&us);
			DeleteFileRecursive(slice_file);
			pcs_free(slice_file);
		}

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
	if (del_local_file) DeleteFileRecursive(local_path);
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

	if (context->secure_enable) {
		int context_secure_method = get_secure_method(context);
		if (context_secure_method == PCS_SECURE_AES_CBC_128
			|| context_secure_method == PCS_SECURE_AES_CBC_192
			|| context_secure_method == PCS_SECURE_AES_CBC_256) {
			int secure_method = get_data_secure_method(res, sz);
			if (secure_method == PCS_SECURE_AES_CBC_128
				|| secure_method == PCS_SECURE_AES_CBC_192
				|| secure_method == PCS_SECURE_AES_CBC_256) {
				char *buf;
				size_t bufsz;
				if (decrypt_data(res, sz, &buf, &bufsz, context->secure_key)) {
					printf("Error: can't decrypt the data.");
					return -1;
				}
				buf[bufsz] = '\0';
				printf("%s\n\n", buf);
				pcs_free(buf);
				return 0;
			}
		}
	}

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

	char	*local_file;	/*本地路径*/
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

	cmpArg->local_file = u8_is_utf8_sys() ? g->argv[0] : utf82mbs(g->argv[0]);
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

	meta = meta_create(local->path, NULL);
	meta->flag |= FLAG_ON_LOCAL;
	meta->local_mtime = local->mtime;
	meta->local_isdir = local->isdir;

	if (remote) {
		meta->flag |= FLAG_ON_REMOTE;
		meta->remote_path = pcs_utils_strdup(remote->path);
		meta->remote_mtime = remote->server_mtime;
		meta->remote_isdir = remote->isdir;
		if (!remote->isdir && remote->md5) {
			meta->md5 = pcs_utils_strdup(remote->md5);
		}
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
				if (!info->isdir && info->md5) {
					meta->md5 = pcs_utils_strdup(info->md5);
				}
			}
			else {
				meta = meta_create(info->path + skip, NULL);
				meta->flag |= FLAG_ON_REMOTE;
				meta->remote_path = pcs_utils_strdup(info->path + skip);
				meta->remote_mtime = info->server_mtime;
				meta->remote_isdir = info->isdir;
				if (!info->isdir && info->md5) {
					meta->md5 = pcs_utils_strdup(info->md5);
				}
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
						if (rbn && ((meta = (MyMeta *)rbn->info)->flag & FLAG_ON_LOCAL)) {
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

	int rc = compare(context, &cmpArg, &on_compared_file, NULL, &on_compared_dir, NULL);
	if (!u8_is_utf8_sys()) {
		pcs_free(cmpArg.local_file);
	}
	return rc;
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
	char *relPath = NULL, *locPath = NULL;

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
	locPath = u8_is_utf8_sys() ? arg->argv[1] : utf82mbs(arg->argv[1]);

	/*检查本地文件 - 开始*/
	local = GetLocalFileInfo(locPath);
	if (local && local->isdir) {
		fprintf(stderr, "Error: The local file exist, but it's a directory.\n");
		DestroyLocalFileInfo(local);
		if (!u8_is_utf8_sys()) pcs_free(locPath);
		return -1;
	}
	else if (local && !local->isdir) {
		if (!is_force) {
			fprintf(stderr, "Error: The local file: %s exist. You can specify '-f' to force override.\n", arg->argv[1]);
			DestroyLocalFileInfo(local);
			if (!u8_is_utf8_sys()) pcs_free(locPath);
			return -1;
		}
	}
	if (local) DestroyLocalFileInfo(local);
	/*检查本地文件 - 结束*/

	//检查是否已经登录
	if (!is_login(context, NULL)) {
		if (!u8_is_utf8_sys()) pcs_free(locPath);
		return -1;
	}

	path = combin_net_disk_path(context->workdir, relPath);
	if (strcmp(path, "/") == 0) {
		fprintf(stderr, "Error: Can't download root directory. \nYou can use 'synch -cd' command to download the directory.\n");
		pcs_free(path);
		if (!u8_is_utf8_sys()) pcs_free(locPath);
		return -1;
	}

	/*检查网盘文件 - 开始*/
	meta = pcs_meta(context->pcs, path);
	if (!meta) {
		fprintf(stderr, "Error: The remote file not exist, or have error: %s\n", pcs_strerror(context->pcs));
		pcs_free(path);
		if (!u8_is_utf8_sys()) pcs_free(locPath);
		return -1;
	}
	if (meta->isdir) {
		fprintf(stderr, "Error: The remote file is directory. \nYou can use 'synch -cd' command to download the directory.\n");
		pcs_fileinfo_destroy(meta);
		pcs_free(path);
		if (!u8_is_utf8_sys()) pcs_free(locPath);
		return -1;
	}
	/*检查网盘文件 - 结束*/

	/*开始下载*/

	if (do_download(context,
		locPath, meta->path, meta->server_mtime,
		"", context->workdir, meta->md5,
		&errmsg, NULL)) {
		fprintf(stderr, "Error: %s\n", errmsg);
		pcs_fileinfo_destroy(meta);
		if (errmsg) pcs_free(errmsg);
		pcs_free(path);
		if (!u8_is_utf8_sys()) pcs_free(locPath);
		return -1;
	}
	printf("Download %s to %s Success.\n", meta->path, arg->argv[1]);
	pcs_fileinfo_destroy(meta);
	if (errmsg) pcs_free(errmsg);
	pcs_free(path);
	if (!u8_is_utf8_sys()) pcs_free(locPath);
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
	char *buf;
	size_t bufsz;
	int need_free = 0;

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
			//fprintf(stderr, "Error: %s path=%s.\n", pcs_strerror(context->pcs), path);
			//pcs_free(path);
			//return -1;
			org = "";
			len = 0;
		}
		buf = (char *)org;
		if (len > 0 && context->secure_enable) {
			int context_secure_method = get_secure_method(context);
			if (context_secure_method == PCS_SECURE_AES_CBC_128
				|| context_secure_method == PCS_SECURE_AES_CBC_192
				|| context_secure_method == PCS_SECURE_AES_CBC_256) {
				int secure_method = get_data_secure_method(org, len);
				if (secure_method == PCS_SECURE_AES_CBC_128
					|| secure_method == PCS_SECURE_AES_CBC_192
					|| secure_method == PCS_SECURE_AES_CBC_256) {
					if (decrypt_data(org, len, &buf, &bufsz, context->secure_key)) {
						printf("Error: can't decrypt the data.");
						return -1;
					}
					buf[bufsz] = '\0';
					len = bufsz;
					need_free = 1;
				}
			}
		}
		//拼接两个字符串
		t = (char *)pcs_malloc(sz + len + 1);
		assert(t);
		if (len > 0)
			memcpy(t, buf, len);
		memcpy(t + len, text, sz + 1);
		sz += len;
		if (need_free) pcs_free(buf);
	}
	else {
		t = (char *)text;
	}
	need_free = (t != text) ? 1 : 0;
	buf = t;
	if (context->secure_enable) {
		int context_secure_method = get_secure_method(context);
		if (context_secure_method == PCS_SECURE_AES_CBC_128
			|| context_secure_method == PCS_SECURE_AES_CBC_192
			|| context_secure_method == PCS_SECURE_AES_CBC_256) {
			if (encrypt_data(t, sz, &buf, &bufsz, context_secure_method, context->secure_key)) {
				printf("Error: can't encrypt the data.");
				return -1;
			}
			sz = bufsz;
			if (need_free) pcs_free(t);
			need_free = 1;
		}
	}

	f = pcs_upload_buffer(context->pcs, path, PcsTrue, buf, sz,
		convert_to_real_speed(context->max_upload_speed_per_thread));
	if (!f) {
		fprintf(stderr, "Error: %s. path = %s\n", pcs_strerror(context->pcs), path);
		pcs_free(path);
		if (need_free) pcs_free(buf);
		return -1;
	}
	printf("Success. You can use '%s cat %s' to print the file content.\n", app_name, path);
	pcs_fileinfo_destroy(f);
	pcs_free(path);
	if (need_free) pcs_free(buf);
	return 0;
}

static int cmd_encode(ShellContext *context, struct args *arg)
{
	int encrypt = 0, decrypt = 0, force = 0, secure_method;
	char *src = NULL, *dst = NULL;
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

	if (u8_is_utf8_sys()) {
		src = arg->argv[0];
		dst = arg->argv[1];
	}
	else {
		src = utf82mbs(arg->argv[0]);
		dst = utf82mbs(arg->argv[1]);
	}
	dstInfo = GetLocalFileInfo(dst);
	if (dstInfo) {
		if (dstInfo->isdir) {
			fprintf(stderr, "Error: The dest is directory. %s", arg->argv[1]);
			DestroyLocalFileInfo(dstInfo);
			if (!u8_is_utf8_sys()) {
				pcs_free(src);
				pcs_free(dst);
			}
			return -1;
		}
		else if (!force) {
			fprintf(stderr, "Error: The dest is exist. You can use '-f' to force override. %s", arg->argv[1]);
			DestroyLocalFileInfo(dstInfo);
			if (!u8_is_utf8_sys()) {
				pcs_free(src);
				pcs_free(dst);
			}
			return -1;
		}
		DestroyLocalFileInfo(dstInfo);
	}

	if (encrypt) {
		char *msg = NULL;
		secure_method = get_secure_method(context);
		if (secure_method != PCS_SECURE_AES_CBC_128 && secure_method != PCS_SECURE_AES_CBC_192 && secure_method != PCS_SECURE_AES_CBC_256) {
			fprintf(stderr, "Error: You have not set the encrypt method, the method should be one of [aes-cbc-128|aes-cbc-192|aes-cbc-256]. You can set it by '%s set'.\n", app_name);
			if (!u8_is_utf8_sys()) {
				pcs_free(src);
				pcs_free(dst);
			}
			return -1;
		}
		if (!context->secure_key || strlen(context->secure_key) == 0) {
			fprintf(stderr, "Error: You have not set the encrypt key. You can set it by '%s set'.", app_name);
			if (!u8_is_utf8_sys()) {
				pcs_free(src);
				pcs_free(dst);
			}
			return -1;
		}
		if (encrypt_file(src, dst, secure_method, context->secure_key, &msg)) {
			fprintf(stderr, "Error: %s\n", msg);
			if (!u8_is_utf8_sys()) {
				pcs_free(src);
				pcs_free(dst);
			}
			return -1;
		}
	}
	else if (decrypt) {
		char *msg = NULL;
		if (!context->secure_key || strlen(context->secure_key) == 0) {
			fprintf(stderr, "Error: You have not set the encrypt key. You can set it by '%s set'.", app_name);
			if (!u8_is_utf8_sys()) {
				pcs_free(src);
				pcs_free(dst);
			}
			return -1;
		}
		if (decrypt_file(src, dst, context->secure_key, &msg)) {
			fprintf(stderr, "Error: %s\n", msg);
			if (!u8_is_utf8_sys()) {
				pcs_free(src);
				pcs_free(dst);
			}
			return -1;
		}
	}
	else {
		usage_encode();
		if (!u8_is_utf8_sys()) {
			pcs_free(src);
			pcs_free(dst);
		}
		return -1;
	}
	if (!u8_is_utf8_sys()) {
		pcs_free(src);
		pcs_free(dst);
	}
	return 0;
}

/*根据 MD5 和 前 256 字节的文件碎片，来找回文件*/
static int cmd_fix(ShellContext *context, struct args *arg)
{
	int is_force = 0;
	const char *content_md5 = NULL;
	char *path = NULL, *errmsg = NULL;
	char *remotePath = NULL, *scrapFile = NULL;
	char slice_md5[33] = { 0 };
	int64_t content_length;

	LocalFileInfo *local;
	PcsFileInfo *meta;

	if (test_arg(arg, 4, 4, "f", "h", "help", NULL)) {
		usage_fix();
		return -1;
	}
	if (has_opts(arg, "h", "help", NULL)) {
		usage_fix();
		return 0;
	}

	is_force = has_opt(arg, "f");
	content_md5 = arg->argv[0];
	content_length = atoll(arg->argv[1]);
	scrapFile = u8_is_utf8_sys() ? arg->argv[1] : utf82mbs(arg->argv[2]);
	remotePath = arg->argv[3];

	if (strnlen(content_md5, 33) != 32) {
		fprintf(stderr, "Error: Invalid 'md5'.\n");
		if (!u8_is_utf8_sys()) pcs_free(scrapFile);
		return -1;
	}

	/*检查本地文件 - 开始*/
	local = GetLocalFileInfo(scrapFile);
	if (!local) {
		fprintf(stderr, "Error: The 'scrap' not exist.\n");
		if (!u8_is_utf8_sys()) pcs_free(scrapFile);
		return -1;
	}
	else if (local->isdir) {
		fprintf(stderr, "Error: The 'scrap' is directory: %s. \n", arg->argv[1]);
		DestroyLocalFileInfo(local);
		if (!u8_is_utf8_sys()) pcs_free(scrapFile);
		return -1;
	}
	if (local->size < PCS_RAPIDUPLOAD_THRESHOLD) {
		DestroyLocalFileInfo(local);
		if (!u8_is_utf8_sys()) pcs_free(scrapFile);
		fprintf(stderr, "The scrap size is not satisfied, the file must be great than %d.", PCS_RAPIDUPLOAD_THRESHOLD);
		return -1;
	}
	/*检查本地文件 - 结束*/

	//检查是否已经登录
	if (!is_login(context, NULL)) {
		DestroyLocalFileInfo(local);
		if (!u8_is_utf8_sys()) pcs_free(scrapFile);
		return -1;
	}

	path = combin_net_disk_path(context->workdir, remotePath);
	if (!path) {
		assert(path);
		DestroyLocalFileInfo(local);
		if (!u8_is_utf8_sys()) pcs_free(scrapFile);
		return -1;
	}

	if (strcmp(path, "/") == 0) {
		char *tmp = combin_net_disk_path(path, local->filename);
		pcs_free(path);
		path = tmp;
	}

	/*检查网盘文件 - 开始*/
	meta = pcs_meta(context->pcs, path);
	if (meta && meta->isdir) {
		char *tmp = combin_net_disk_path(path, local->filename);
		pcs_free(path);
		path = tmp;
		pcs_fileinfo_destroy(meta);
		meta = pcs_meta(context->pcs, path);
	}
	DestroyLocalFileInfo(local);
	if (meta && meta->isdir) {
		fprintf(stderr, "Error: The remote file exist, and it is directory. %s\n", path);
		pcs_fileinfo_destroy(meta);
		pcs_free(path);
		if (!u8_is_utf8_sys()) pcs_free(scrapFile);
		return -1;
	}
	else if (meta && !is_force) {
		fprintf(stderr, "Error: The remote file exist. You can specify '-f' to force override. %s\n", path);
		pcs_fileinfo_destroy(meta);
		pcs_free(path);
		if (!u8_is_utf8_sys()) pcs_free(scrapFile);
		return -1;
	}
	else if (meta && is_force) {
		char *diskName = pcs_utils_filename(meta->path);
		pcs_free(diskName);
	}
	if (meta) {
		pcs_fileinfo_destroy(meta);
		meta = NULL;
	}
	/*检查网盘文件 - 结束*/

	if (!pcs_md5_file_slice(context->pcs, scrapFile, 0, PCS_RAPIDUPLOAD_THRESHOLD, slice_md5)) {
		fprintf(stderr, "Error: Cannot get slice md5.\n");
		pcs_free(path);
		if (!u8_is_utf8_sys()) pcs_free(scrapFile);
		return -1;
	}

	/*开始修复*/
	meta = pcs_rapid_upload_r(context->pcs, path,
		is_force ? PcsTrue : PcsFalse,
		content_length, content_md5, slice_md5);
	if (meta == NULL) {
		fprintf(stderr, "Error: %s\n", errmsg);
		if (errmsg) pcs_free(errmsg);
		pcs_free(path);
		if (!u8_is_utf8_sys()) pcs_free(scrapFile);
		return -1;
	}
	printf("Fix Success, remote path: %s.\n", path);
	if (errmsg) pcs_free(errmsg);
	pcs_free(path);
	if (meta) pcs_fileinfo_destroy(meta);
	if (!u8_is_utf8_sys()) pcs_free(scrapFile);
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
	else if (strcmp(cmd, "fix") == 0) {
		usage_fix();
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
	int64_t totalSize = 0;
    int md5 = 0, thumb = 0;

	if (test_arg(arg, 0, 1, "md5", "thumb", "h", "help", NULL)) {
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

	md5 = has_opt(arg, "md5");
	thumb = has_opt(arg, "thumb");

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
		print_filelist(list, &fileCount, &dirCount, &totalSize, md5, thumb);
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
		pcs_utils_readable_size((double)totalSize, tmp, 63, NULL);
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
	else if (pcs_strerror(context->pcs)) {
		printf("Error: %s\n", pcs_strerror(context->pcs));
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
		if (pcs_strerror(context->pcs)) {
			printf("Error: %s\n", pcs_strerror(context->pcs));
			return -1;
		}
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
	int64_t quota, used;
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
		printf("%"PRIu64" Bytes ", used);
		putchar('/');
		printf("%" PRIu64 " Bytes  ", quota);
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
	if (!res || res->error != 0) {
		fprintf(stderr, "Error: %s path=%s.\n", pcs_strerror(context->pcs), slist.string);
		if (res)
			pcs_pan_api_res_destroy(res);
		pcs_free(slist.string);
		return -1;
	}
	info = res->info_list;
	if (info && info->info.error) {
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
	int count = 0;
	if (test_arg(arg, 0, 0, 
		"cookie_file", "captcha_file", 
		"list_page_size", "list_sort_name", "list_sort_direction",
		"secure_method", "secure_key", "secure_enable", "timeout_retry", "max_thread", "max_speed_per_thread",
		"user-agent", "cache_size",
		"h", "help", NULL) && arg->optc == 0) {
		usage_set();
		return -1;
	}
	if (has_opts(arg, "h", "help", NULL)) {
		usage_set();
		return 0;
	}

	if (has_optEx(arg, "cookie_file", &val)) {
		count++;
		if (set_cookiefile(context, val)) {
			usage_set();
			return -1;
		}
	}

	if (has_optEx(arg, "captcha_file", &val)) {
		count++;
		if (set_captchafile(context, val)) {
			usage_set();
			return -1;
		}
	}

	if (has_optEx(arg, "list_page_size", &val)) {
		count++;
		if (set_list_page_size(context, val)) {
			usage_set();
			return -1;
		}
	}

	if (has_optEx(arg, "list_sort_name", &val)) {
		count++;
		if (set_list_sort_name(context, val)) {
			usage_set();
			return -1;
		}
	}

	if (has_optEx(arg, "list_sort_direction", &val)) {
		count++;
		if (set_list_sort_direction(context, val)) {
			usage_set();
			return -1;
		}
	}

	if (has_optEx(arg, "secure_method", &val)) {
		count++;
		if (set_secure_method(context, val)) {
			usage_set();
			return -1;
		}
	}

	if (has_optEx(arg, "secure_key", &val)) {
		count++;
		if (set_secure_key(context, val)) {
			usage_set();
			return -1;
		}
	}

	if (has_optEx(arg, "secure_enable", &val)) {
		count++;
		if (set_secure_enable(context, val)) {
			usage_set();
			return -1;
		}
	}

	if (has_optEx(arg, "timeout_retry", &val)) {
		count++;
		if (set_timeout_retry(context, val)) {
			usage_set();
			return -1;
		}
	}

	if (has_optEx(arg, "max_thread", &val)) {
		count++;
		if (set_max_thread(context, val)) {
			usage_set();
			return -1;
		}
	}

	if (has_optEx(arg, "max_speed_per_thread", &val)) {
		count++;
		if (set_max_speed_per_thread(context, val)) {
			usage_set();
			return -1;
		}
	}

	if (has_optEx(arg, "max_upload_speed_per_thread", &val)) {
		count++;
		if (set_max_upload_speed_per_thread(context, val)) {
			usage_set();
			return -1;
		}
	}

	if (has_optEx(arg, "user_agent", &val)) {
		count++;
		if (set_user_agent(context, val)) {
			usage_set();
			return -1;
		}
	}

	if (has_optEx(arg, "cache_size", &val)) {
		count++;
		if (set_cache_size(context, val)) {
			usage_set();
			return -1;
		}
	}

	if (!count) {
		usage_set();
		return -1;
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
    int md5 = 0, thumb = 0;

	PcsFileInfoList *list = NULL;
	int fscount = 0;

	if (test_arg(arg, 1, 2, "md5", "thumb", "r", "h", "help", NULL)) {
		usage_search();
		return -1;
	}
	if (has_opts(arg, "h", "help", NULL)) {
		usage_search();
		return 0;
	}

	is_recursive = has_opt(arg, "r");
	md5 = has_opt(arg, "md5");
	thumb = has_opt(arg, "thumb");

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
		print_filelist_head(4, md5, thumb);
		pcs_free(path);
		return 1;
	}
	fscount = list->count;
	print_filelist(list, NULL, NULL, NULL, md5, thumb);
	pcs_filist_destroy(list);
	pcs_free(path);
	return fscount > 0 ? 0 : 1;
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
		s->local_basedir, s->remote_basedir, meta->md5,
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
				arg->local_file, arg->remote_file, meta->md5,
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

	int rc = compare(context, &cmpArg, &synchFile, NULL, &on_compared_dir, NULL);
	if (!u8_is_utf8_sys()) {
		pcs_free(cmpArg.local_file);
	}
	return rc;
}

#pragma endregion

/*上传*/
static int cmd_upload(ShellContext *context, struct args *arg)
{
	const char *opts[] = { "f", NULL };
	int is_force = 0;
	char *path = NULL, *errmsg = NULL;
	char *relPath = NULL, *locPath = NULL;

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
	locPath = u8_is_utf8_sys() ? arg->argv[0] : utf82mbs(arg->argv[0]);
	relPath = arg->argv[1];

	/*检查本地文件 - 开始*/
	local = GetLocalFileInfo(locPath);
	if (!local) {
		fprintf(stderr, "Error: The local file not exist.\n");
		if (!u8_is_utf8_sys()) pcs_free(locPath);
		return -1;
	}
	else if (local->isdir) {
		fprintf(stderr, "Error: The local file is directory: %s. \nYou can use 'synch -cu' command to upload the directory.\n", arg->argv[0]);
		DestroyLocalFileInfo(local);
		if (!u8_is_utf8_sys()) pcs_free(locPath);
		return -1;
	}
	/*检查本地文件 - 结束*/

	//检查是否已经登录
	if (!is_login(context, NULL)) {
		DestroyLocalFileInfo(local);
		if (!u8_is_utf8_sys()) pcs_free(locPath);
		return -1;
	}

	path = combin_net_disk_path(context->workdir, relPath);
	if (!path) {
		assert(path);
		DestroyLocalFileInfo(local);
		if (!u8_is_utf8_sys()) pcs_free(locPath);
		return -1;
	}

	if (strcmp(path, "/") == 0) {
		char *tmp = combin_net_disk_path(path, local->filename);
		pcs_free(path);
		path = tmp;
	}

	/*检查网盘文件 - 开始*/
	meta = pcs_meta(context->pcs, path);
	if (meta && meta->isdir) {
		char *tmp = combin_net_disk_path(path, local->filename);
		pcs_free(path);
		path = tmp;
		pcs_fileinfo_destroy(meta);
		meta = pcs_meta(context->pcs, path);
	}
	DestroyLocalFileInfo(local);
	if (meta && meta->isdir) {
		fprintf(stderr, "Error: The remote file exist, and it is directory. %s\n", path);
		pcs_fileinfo_destroy(meta);
		pcs_free(path);
		if (!u8_is_utf8_sys()) pcs_free(locPath);
		return -1;
	}
	else if (meta && !is_force){
		fprintf(stderr, "Error: The remote file exist. You can specify '-f' to force override. %s\n", path);
		pcs_fileinfo_destroy(meta);
		pcs_free(path);
		if (!u8_is_utf8_sys()) pcs_free(locPath);
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
		if (!u8_is_utf8_sys()) pcs_free(locPath);
		return -1;
	}
	printf("Upload %s to %s Success.\n", arg->argv[0], path);
	if (errmsg) pcs_free(errmsg);
	pcs_free(path);
	if (meta) pcs_fileinfo_destroy(meta);
	if (!u8_is_utf8_sys()) pcs_free(locPath);
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
	else if (strcmp(cmd, "fix") == 0) {
		rc = cmd_fix(context, arg);
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
	setlocale(LC_ALL, "");
	app_name = filename(argv[0]);
	if (parse_arg(&arg, argc, argv, u8_is_utf8_sys() ? NULL : mbs2utf8)) {
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
	context.pcs = create_pcs(&context);
	if (!context.pcs) {
		rc = -1;
		printf("Can't create pcs context.\n");
		goto exit_main;
	}
	rc = exec_cmd(&context, &arg);
	destroy_pcs(context.pcs);
	save_context(&context);
exit_main:
	free_context(&context);
	free_args(&arg);
	pcs_print_leak();
	return rc;
}
