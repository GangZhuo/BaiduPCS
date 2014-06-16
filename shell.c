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
#include "version.h"
#include "shell.h"

#if defined(WIN32)
# define mkdir _mkdir
#endif

#ifdef WIN32

/*
 * �ӱ�׼�����������ַ���
 * str  - ������ַ�������䵽 str ��
 * size - ������� size ���ֽڡ�
*/
static void std_string(char *str, int size)
{
	char c;
	int i = 0;

	while ((c = _getch()) != '\r' && c != '\n') {
		if (c == '\b') {
			if (i > 0) {
				i--;
				putchar(c);
				putchar(' ');
				putchar(c);
			}
		}
		else if (isprint(c)) {
			str[i] = c;
			putchar(c);
			i++;
			if (i >= size) {
				break;
			}
		}
	}
	str[i >= size ? (size - 1) : i] = '\0';
	printf("\n");
}

/*
 * �ӱ�׼�������������룬������ַ�������
 * password  - ��������뽫��䵽 password ��
 * size      - �������size���ֽڡ�
 */
static void std_password(char *password, int size)
{
	char c;
	int i = 0;

	while ((c = _getch()) != '\r' && c != '\n') {
		if (c == '\b') {
			if (i > 0)
				i--;
		}
		else if (isprint(c)) {
			password[i] = c;
			//putchar('*');
			i++;
			if (i >= size) {
				break;
			}
		}
	}
	password[i >= size ? (size - 1) : i] = '\0';
	printf("\n");
}

static int is_absolute_path(const char *path)
{
	if (!path) return 0;
	if (strlen(path) < 2) return 0;
	if (path[1] != ':') return 0;
	if (!((path[0] > 'a' && path[0] < 'z') || (path[0] > 'A' && path[0] < 'Z'))) return 0;
	return 1;
}

#else
#include <termios.h>
#include <unistd.h>

/*
* �ӱ�׼�����������ַ���
* str  - ������ַ�������䵽 str ��
* size - ������� size ���ֽڡ�
*/
static void std_string(char *str, int size)
{
	struct termios oflags, nflags;

	/* disabling echo */
	tcgetattr(fileno(stdin), &oflags);
	nflags = oflags;
	//nflags.c_lflag &= ~ECHO;
	//nflags.c_lflag |= ECHONL;

	if (tcsetattr(fileno(stdin), TCSANOW, &nflags) != 0) {
		perror("tcsetattr");
		return;
	}

	//printf("user name: ");
	fgets(str, size, stdin);
	str[size - 1] = 0;
	str[strlen(str) - 1] = 0;
	//printf("you typed '%s'\n", str);

	/* restore terminal */
	if (tcsetattr(fileno(stdin), TCSANOW, &oflags) != 0) {
		perror("tcsetattr");
		return;
	}
}

/*
* �ӱ�׼�������������룬������ַ�������
* password  - ��������뽫��䵽 password ��
* size      - �������size���ֽڡ�
*/
static void std_password(char *password, int size)
{
	struct termios oflags, nflags;

	/* disabling echo */
	tcgetattr(fileno(stdin), &oflags);
	nflags = oflags;
	nflags.c_lflag &= ~ECHO;
	nflags.c_lflag |= ECHONL;

	if (tcsetattr(fileno(stdin), TCSANOW, &nflags) != 0) {
		perror("tcsetattr");
		return;
	}

	//printf("password: ");
	fgets(password, size, stdin);
	password[size - 1] = 0;
	password[strlen(password) - 1] = 0;
	//printf("you typed '%s'\n", password);

	/* restore terminal */
	if (tcsetattr(fileno(stdin), TCSANOW, &oflags) != 0) {
		perror("tcsetattr");
		return;
	}

}

static int is_absolute_path(const char *path)
{
	if (!path) return 0;
	if (strlen(path) < 1) return 0;
	if (path[0] != '/' && path[0] != '~') return 0;
	return 1;
}

#endif

/*����COOKIE�ļ�·��*/
static const char *cookiefile()
{
	static char filename[1024] = { 0 };
	if (!filename[0]){ /*����Ѿ����������ֱ�ӷ���*/
#ifdef WIN32
		strcpy(filename, getenv("UserProfile"));
		strcat(filename, "\\.pcs");
		mkdir(filename);
		strcat(filename, "\\");
		strcat(filename, "default.cookie");
#else
		strcpy(filename, getenv("HOME"));
		strcat(filename, "/.pcs");
		mkdir(filename, 0777);
		strcat(filename, "/");
		strcat(filename, "default.cookie");
#endif
	}
	return filename;
}

/*������֤��ͼƬ�ļ�·��*/
static const char *captchafile()
{
	static char filename[1024] = { 0 };
	if (!filename[0]){ /*����Ѿ����������ֱ�ӷ���*/
#ifdef WIN32
		strcpy(filename, getenv("UserProfile"));
		strcat(filename, "\\.pcs");
		mkdir(filename);
		strcat(filename, "\\");
		strcat(filename, "captcha.gif");
#else
		strcpy(filename, getenv("HOME"));
		strcat(filename, "/.pcs");
		mkdir(filename, 0777);
		strcat(filename, "/");
		strcat(filename, "captcha.gif");
#endif
	}
	return filename;
}

/*�����֤��ͼƬ�����ȴ��û�����ʶ����*/
static PcsBool verifycode(unsigned char *ptr, size_t size, char *captcha, size_t captchaSize, void *state)
{
	static char filename[1024] = { 0 };
	FILE *pf;

	if (!filename[0]) {
#ifdef WIN32
		strcpy(filename, getenv("UserProfile"));
		strcat(filename, "\\.pcs");
		mkdir(filename);
		strcat(filename, "\\vc.gif");
#else
		strcpy(filename, getenv("HOME"));
		strcat(filename, "/.pcs");
		mkdir(filename, 0777);
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

/*��ʾ�ϴ�����*/
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

/*
 * ��ȡȫ���ļ�����
 * file    - ����ȡ���ļ�
 * pBuffer - �ļ����������ڵ��ڴ�ָ�뽫����pBufferָ�����ڴ���
 * ���ض�ȡ�����ֽڴ�С��ʹ����ɺ������pcs_free(*pBuffer)
*/
static int readFileContent(const char *file, char **pBuffer)
{
	FILE *fp;
	long int save_pos;
	long size_of_file;
	char *content;

	fp = fopen(file, "rb");
	if (!fp) {
		//printf("Open file fail: %s\n", file);
		return -1;
	}
	save_pos = ftell(fp);
	fseek(fp, 0L, SEEK_END);
	size_of_file = ftell(fp);
	if (size_of_file < 3) {
		printf("Wrong file size: Size=%d, %s\n", size_of_file, file);
		fclose(fp);
		return -1;
	}
	fseek(fp, save_pos, SEEK_SET);
	content = (char *)pcs_malloc(size_of_file + 1);
	save_pos = fread(content, 1, size_of_file, fp);
	fclose(fp);
	content[size_of_file] = '\0';
	if ((((int)content[0]) & 0xEF) == 0xEF) {
		if ((((int)content[1]) & 0xBB) == 0xBB) {
			if ((((int)content[2]) & 0xBF) == 0xBF) {
				content[0] = content[1] = content[2] = ' ';
			}
			else {
				printf("Wrong UTF-8 BOM: BOM=0x%2X%2X%2X %s\n", content[0] & 0xFF, content[1] & 0xFF, content[2] & 0xFF, file);
				return -1;
			}
		}
		else {
			printf("Wrong UTF-8 BOM: BOM=0x%2X%2X%2X %s\n", content[0] & 0xFF, content[1] & 0xFF, content[2] & 0xFF, file);
			return -1;
		}
	}
	*pBuffer = content;
	return size_of_file;
}

/*��ȡ�����Ĵ洢�ļ�·��*/
static const char *contextfile()
{
	static char filename[1024] = { 0 };

	if (!filename[0]) {
#ifdef WIN32
		strcpy(filename, getenv("UserProfile"));
		strcat(filename, "\\.pcs");
		mkdir(filename);
		strcat(filename, "\\pcs.context");
#else
		strcpy(filename, getenv("HOME"));
		strcat(filename, "/.pcs");
		mkdir(filename, 0777);
		strcat(filename, "/pcs.context");
#endif
	}
	return filename;
}

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

/*��������ת��Ϊ�ַ���*/
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

/*����������*/
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

/*��ԭ�����������*/
static void restore_context(ShellContext *context)
{
	const char *filename;
	char *filecontent = NULL;
	int filesize = 0;
	cJSON *root, *item;

	filename = contextfile();
	filesize = readFileContent(filename, &filecontent);
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

/*�ӳ���·���л�ȡ�ļ�������*/
static const char *filename(char *path)
{
	const char *p;
	p = path;
	p += strlen(p);
	while (p > path && *p != '/' && *p != '\\') p--;
	if (*p == '/' || *p == '\\') p++;
	return p;
}

/*
 * �ϲ�unix��ʽ��·�������filename������Ǿ���·������ֱ�ӷ���filename�Ŀ�����
 * ʹ����������pcs_free���ͷŷ���ֵ
 */
static char *combin_unix_path(const char *base, const char *filename)
{
	char *p;
	size_t basesz, filenamesz, sz;

	if (filename[0] == '/') { /*����Ǿ���·����ֱ�ӷ��ظ�ֵ*/
		return pcs_utils_strdup(filename);
	}

	basesz = strlen(base);
	filenamesz = strlen(filename);
	sz = basesz + filenamesz + 1;
	p = (char *)pcs_malloc(sz + 1);
	assert(p);
	memset(p, 0, sz + 1);
	strcpy(p, base);
	if (p[basesz - 1] != '/') {
		p[basesz] = '/';
		p[basesz + 1] = '\0';
	}
	strcat(p, filename);
	return p;
}

/*���ļ���Сת�����ַ���*/
static const char *size_tostr(UInt64 size, int *fix_width, char ch)
{
	static char str[128], *p;
	UInt64 i;
	int j, cn, mod;

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

	i = size;
	j = 127;
	str[j] = '\0';
	cn = 0;
	while (i != 0) {
		mod = i % 10;
		i = i / 10;
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

/*��ӡ�ļ�ʱ��*/
static void print_time(const char *format, UInt64 time)
{
	struct tm *tm;
	time_t t = time;
	char tmp[64];

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

/*��ӡ�ɶ����ļ���С*/
static void print_size(const char *format, UInt64 size)
{
	char tmp[64];
	tmp[63] = '\0';
	pcs_utils_readable_size(size, tmp, 63, NULL);
	printf(format, tmp);
}

/*��ӡ�ļ��б��ͷ*/
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

/*��ӡ�ļ��б��������*/
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

/*��ӡ�ļ��б�*/
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

/*��ӡ�ַ��������len����-1���򱾺����ȼ���puts������ֻ��ӡlenָ����С���ַ���*/
static void print_str(const char *str, int len)
{
	int i;
	if (len == -1) {
		puts(str);
	}
	else {
		for (i = 0; i < len; i++) {
			putchar(str[i]);
		}
	}
}

/*��ӡ�ļ���Ŀ¼��Ԫ����*/
static void print_fileinfo(PcsFileInfo *f, const char *prex)
{
	if (!prex) prex = "";
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

/*
 * �ж������ַ����Ƿ���ȡ�
 *  s1    - ��'\0'Ϊ������ǵ��ַ���
 *  s2    - ���Ƚϵ��ַ���
 *  s2len - s2�ַ������ֽڳ��ȡ��������-1�Ļ�����ʹ��'\0'��Ϊ��������
 * �����ȣ��򷵻�1�����򷵻�0��
 */
static int streq(const char *s1, const char *s2, int s2len)
{
	const char *p1 = s1, *p2 = s2;
	int i = 0, rc = 1;
	if (s2len == -1) {
		for (i = 0; ; i++) {
			if (!s1[i]) {
				if (!s2[i]){
					break;
				}
				else{
					rc = 0;
					break;
				}
			}
			else if (!s2[i]) {
				rc = 0;
				break;
			}
			else if (s1[i] != s2[i]) {
				rc = 0;
				break;
			}
		}
	}
	else {
		for (i = 0; i < s2len; i++) {
			if (!s1[i] || s1[i] != s2[i]) {
				rc = 0;
				break;
			}
		}
	}
	return rc;
}

/*
 * �ж�arr�������Ƿ�����ַ���str����������򷵻����ţ����Ϊ [����] + 1�������򷵻�0��
 * �Ƚ�ʱ���ִ�Сд��
 * arr  - �洢�ܶ��ַ��������飬�������һ��Ԫ�ر���ΪNULL��
 * str  - �ж��Ƿ���ڵ��ַ���
 * len  - �ַ������ȡ� �������-1����'\0'��Ϊ�������ǡ�
*/
static int str_in_array(const char **arr, const char *str, int len)
{
	int i = 0, rc = 0;
	const char *p;
	while ((p = arr[i++])) {
		if (streq(p, str, len)) {
			rc = i;
			break;
		}
	}
	return rc;
}

/*��ʼ��������*/
static void init_context(ShellContext *context, int argc, char *argv[])
{
	memset(context, 0, sizeof(ShellContext));
	context->name = pcs_utils_strdup(filename(argv[0]));
	context->cookiefile = pcs_utils_strdup(cookiefile());
	context->captchafile = pcs_utils_strdup(captchafile());
	context->workdir = pcs_utils_strdup("/");
	context->list_page_size = 20;
	context->list_sort_name = pcs_utils_strdup("name");
	context->list_sort_direction = pcs_utils_strdup("asc");
	
	context->secure_method = pcs_utils_strdup("plaintext");
	context->secure_key = pcs_utils_strdup("");
	context->secure_enable = 0;
}

/*�ͷ�������*/
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

/*��ʼ��PCS*/
static void init_pcs(ShellContext *context)
{
	context->pcs = pcs_create(context->cookiefile);
	pcs_setopt(context->pcs, PCS_OPTION_CAPTCHA_FUNCTION, (void *)&verifycode);
	pcs_setopts(context->pcs,
		PCS_OPTION_PROGRESS_FUNCTION, (void *)&upload_progress,
		PCS_OPTION_PROGRESS, (void *)((long)PcsFalse),
		PCS_OPTION_END);
	init_pcs_secure(context);
}

/*��ӡ�汾*/
static void version(ShellContext *context)
{
	printf(program_full_name "\n", context->name);
}

/*��ӡcat�����÷�*/
static void usage_cat(ShellContext *context)
{
	version(context);
	printf("Usage: %s cat <path>\n", context->name);
	printf("Samples:\n");
	printf("  %s cat /music/list.txt\n", context->name);
	printf("  %s cat list.txt\n", context->name);
	printf("  %s cat \"/music/Europe and America/list.txt\"\n", context->name);
}

/*��ӡcd�����÷�*/
static void usage_cd(ShellContext *context)
{
	version(context);
	printf("Usage: %s cd <path>\n", context->name);
	printf("Samples:\n");
	printf("  %s cd /music\n", context->name);
	printf("  %s cd subdir\n", context->name);
	printf("  %s cd \"/music/Europe and America\"\n", context->name);
}

/*��ӡcopy�����÷�*/
static void usage_copy(ShellContext *context)
{
	version(context);
	printf("Usage: %s copy <src> <dst>\n", context->name);
	printf("Samples:\n");
	printf("  %s copy src.txt dst.txt\n", context->name);
	printf("  %s copy /music/src.mp3 /music/mp3/dst.mp3\n", context->name);
	printf("  %s copy /music/src.mp3 \"/music/Europe and America/dst.mp3\"\n", context->name);
}

/*��ӡecho�����÷�*/
static void usage_echo(ShellContext *context)
{
	version(context);
	printf("Usage: %s echo [-a] <path> <text>\n", context->name);
	printf("Options:\n");
	printf("  -a    Append the text.\n");
	printf("Samples:\n");
	printf("  %s echo src.txt \"This is from 'echo' command.\" \n", context->name);
	printf("  %s echo /docs/src.txt \"This is from 'echo' command.\"\n", context->name);
	printf("  %s echo -a \"for test/src.txt\" \"This is from 'echo' command.\"\n", context->name);
}

/*��ӡlist�����÷�*/
static void usage_list(ShellContext *context)
{
	version(context);
	printf("Usage: %s list [path]\n", context->name);
	printf("Samples:\n");
	printf("  %s list\n", context->name);
	printf("  %s list /music\n", context->name);
	printf("  %s list \"/music/Europe and America\"\n", context->name);
}

/*��ӡlist�����÷�*/
static void usage_set(ShellContext *context)
{
	version(context);
	printf("Usage: %s set <option>=<value> ...\n", context->name);
	printf("Options:\n");
	printf("  Option Name         Type       Possible Values \n");
	printf("  ------------------------------------------\n");
	printf("  cookiefile          String     not null\n");
	printf("  captchafile         String     not null\n");
	printf("  list_page_size      UInt       >0\n");
	printf("  list_sort_name      Enum       name|time|size\n");
	printf("  list_sort_direction Enum       asc|desc\n");
	printf("  secure_method       Enum       plaintext|aes-cbc-128|aes-cbc-192|aes-cbc-256\n");
	printf("  secure_key          String     not null when 'secure_method' is not 'plaintext'\n");
	printf("  secure_enable       Boolean    true|false\n");
	printf("\nSamples:\n");
	printf("  %s set cookiefile=\"/tmp/pcs.cookie\"\n", context->name);
	printf("  %s set cookiefile=\"/tmp/pcs.cookie\" captchafile=\"/tmp/vc.git\"\n", context->name);
	printf("  %s set list_page_size=20 list_sort_name=name list_sort_direction=desc\n", context->name);
}

/*��ӡlogin�����÷�*/
static void usage_login(ShellContext *context)
{
	version(context);
	printf("Usage: %s login [user name] [password]\n", context->name);
	printf("Samples:\n");
	printf("  %s login\n", context->name);
	printf("  %s login gang\n", context->name);
	printf("  %s login gang \"password\"\n", context->name);
}

/*��ӡlogout�����÷�*/
static void usage_logout(ShellContext *context)
{
	version(context);
	printf("Usage: %s logout\n", context->name);
	printf("Samples:\n");
	printf("  %s logout\n", context->name);
}

/*��ӡpwd�����÷�*/
static void usage_pwd(ShellContext *context)
{
	version(context);
	printf("Usage: %s pwd\n", context->name);
	printf("Samples:\n");
	printf("  %s pwd\n", context->name);
}

/*��ӡquota�����÷�*/
static void usage_quota(ShellContext *context)
{
	version(context);
	printf("Usage: %s quota\n", context->name);
	printf("Samples:\n");
	printf("  %s quota\n", context->name);
}

/*��ӡversion�����÷�*/
static void usage_version(ShellContext *context)
{
	version(context);
	printf("Usage: %s version\n", context->name);
	printf("Samples:\n");
	printf("  %s version\n", context->name);
}

/*��ӡwho�����÷�*/
static void usage_who(ShellContext *context)
{
	version(context);
	printf("Usage: %s who\n", context->name);
	printf("Samples:\n");
	printf("  %s who\n", context->name);
}

/*��ӡhelp�����÷�*/
static void usage_help(ShellContext *context)
{
	version(context);
	printf("Usage: %s help\n", context->name);
	printf("Samples:\n");
	printf("  %s help\n", context->name);
}

/*��ӡmeta�����÷�*/
static void usage_meta(ShellContext *context)
{
	version(context);
	printf("Usage: %s meta [path]\n", context->name);
	printf("Samples:\n");
	printf("  %s meta\n", context->name);
	printf("  %s meta /music\n", context->name);
	printf("  %s meta \"/music/Europe and America\"\n", context->name);
}

/*��ӡmkdir�����÷�*/
static void usage_mkdir(ShellContext *context)
{
	version(context);
	printf("Usage: %s mkdir <path>\n", context->name);
	printf("Samples:\n");
	printf("  %s mkdir subdir\n", context->name);
	printf("  %s mkdir /music\n", context->name);
	printf("  %s mkdir \"/music/Europe and America\"\n", context->name);
}

/*��ӡ�÷�*/
static void usage(ShellContext *context)
{
	version(context);
	printf("Usage: %s command [options] [arg1|arg2...]\n", context->name);
	printf(
		"Commands:\n"
		"  cat      Print file content\n"
		"  cd       Change work directory\n"
		"  copy     Copy file|directory\n"
		"  compare  Print different between local and net disk\n"
		"  context  Print the context\n"
		"  download Download file|directory\n"
		"  echo     Write text into net disk\n"
		"  list     List files|directories\n"
		"  login    Login net disk\n"
		"  logout   Logout net disk\n"
		"  meta     Print meta for file|directory\n"
		"  mkdir    Make new directory\n"
		"  move     Move file|directory\n"
		"  pwd      Print work directory\n"
		"  quota    Print quota\n"
		"  remove   Remove file|directory\n"
		"  rename   Rename file or directory\n"
		"  set      Change configuration\n"
		"  search   Search files\n"
		"  upload   Upload file|directory\n"
		"  version  Print version ID\n"
		"  who      Print user ID\n"
		);
	printf("More detail see '%s command -h'. \n", context->name);
}

/*
 * ����Ƿ��¼�����δ��¼���¼
 *   msg   - ��⵽δ��¼ʱ�Ĵ�ӡ��Ϣ������NULL�Ļ�����ʹ��Ĭ����Ϣ��
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

/*��ӡ�����ļ�����*/
static int cmd_cat(ShellContext *context, int argc, char *argv[])
{
	char *path;
	const char *res;
	size_t sz;
	if (argc != 1) {
		usage_cat(context);
		return -1;
	}
	if (!is_login(context, NULL)) return -1;
	path = combin_unix_path(context->workdir, argv[0]);
	assert(path);
	res = pcs_cat(context->pcs, path, &sz);
	if (res == NULL) {
		printf("Error: path=%s. %s\n", path, pcs_strerror(context->pcs));
		pcs_free(path);
		return -1;
	}
	pcs_free(path);
	printf("%s\n\n", res);
	return 0;
}

/*�������̵�ǰ����Ŀ¼*/
static int cmd_cd(ShellContext *context, int argc, char *argv[])
{
	char *p;
	PcsFileInfo *meta;
	if (argc != 1) {
		usage_cd(context);
		return -1;
	}
	if (!is_login(context, NULL)) return -1;
	p = combin_unix_path(context->workdir, argv[0]);
	meta = pcs_meta(context->pcs, p);
	if (!meta) {
		printf("Can't get the meta for %s: %s\n", p, pcs_strerror(context->pcs));
		pcs_free(p);
		return -1;
	}
	if (!meta->isdir) {
		printf("The target (%s) is not directory\n", p);
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

/*���������ļ�*/
static int cmd_copy(ShellContext *context, int argc, char *argv[])
{
	PcsPanApiRes *res;
	PcsPanApiResInfoList *info;
	PcsSList2 slist;
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
		printf("Error: src=%s, dst=%s. %s\n", slist.string1, slist.string2, pcs_strerror(context->pcs));
		pcs_free(slist.string1);
		pcs_free(slist.string2);
		return -1;
	}
	info = res->info_list;
	if (info->info.error) {
		printf("Error: src=%s, dst=%s. \n", slist.string1, slist.string2);
		pcs_free(slist.string1);
		pcs_free(slist.string2);
		return -1;
	}
	else {
		printf("Copy %s to %s Success.\n", slist.string1, slist.string2);
	}
	pcs_pan_api_res_destroy(res);
	pcs_free(slist.string1);
	pcs_free(slist.string2);
	return 0;
}

/*�Ƚϱ��غ�����Ŀ¼����ͬ*/
static int cmd_compare(ShellContext *context, int argc, char *argv[])
{
	if (!is_login(context, NULL)) return -1;
	printf("not implement\n");
	return 0;
}

/*��ӡ��������*/
static int cmd_context(ShellContext *context, int argc, char *argv[])
{
	char *json;
	json = context2str(context);
	assert(json);
	printf("%s\n", json);
	pcs_free(json);
	return 0;
}

/*����*/
static int cmd_download(ShellContext *context, int argc, char *argv[])
{
	if (!is_login(context, NULL)) return -1;
	printf("not implement\n");
	return 0;
}

/*����ı�������ĳһ���ļ�*/
static int cmd_echo(ShellContext *context, int argc, char *argv[])
{
	int is_append = 0;
	char *path = NULL, *t = NULL;
	const char *relPath = NULL, *text = NULL;
	int i;
	size_t sz;
	PcsFileInfo *f;
	if (argc != 2 && argc != 3) {
		usage_echo(context);
		return -1;
	}
	/*�������� - ��ʼ*/
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
	/*�������� - ����*/
	if (!is_login(context, NULL)) return -1;
	path = combin_unix_path(context->workdir, relPath);
	assert(path);
	sz = strlen(text);
	if (is_append) {
		const char *org;
		size_t len;
		//��ȡ�ļ�������
		org = pcs_cat(context->pcs, path, &len);
		if (org == NULL) {
			printf("Error: path=%s. %s\n", path, pcs_strerror(context->pcs));
			pcs_free(path);
			return -1;
		}
		//ƴ�������ַ���
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
		printf("Error: %s. The path is %s\n", pcs_strerror(context->pcs), path);
		pcs_free(path);
		if (t != text) pcs_free(t);
		return -1;
	}
	printf("Success. path is %s\n", path);
	pcs_fileinfo_destroy(f);
	pcs_free(path);
	if (t != text) pcs_free(t);
	return 0;
}

/*�г�Ŀ¼*/
static int cmd_list(ShellContext *context, int argc, char *argv[])
{
	char *path;
	PcsFileInfoList *list = NULL;
	int page_index = 1;
	char tmp[64] = { 0 };
	int fileCount = 0, dirCount = 0;
	size_t totalSize = 0;

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
		printf("[%s] %d\n", tmp, strlen(tmp));
		if (strlen(tmp) != 0 && pcs_utils_strcmpi(tmp, "y") && pcs_utils_strcmpi(tmp, "yes"))
			break;
		page_index++;
	}
	puts("\n------------------------------------------------------------------------------");
	pcs_utils_readable_size(totalSize, tmp, 63, NULL);
	tmp[63] = '\0';
	printf("Total Page: %d, Total Size: %s, File Count: %d, Directory Count: %d\n", page_index, tmp, fileCount, dirCount);
	putchar('\n');
	pcs_free(path);
	return 0;
}

/*��¼*/
static int cmd_login(ShellContext *context, int argc, char *argv[])
{
	PcsRes pcsres;
	char str[50];
	if (argc != 0 && argc != 1 && argc != 2) {
		usage_login(context);
		return -1;
	}
	if (is_login(context, "")) {
		printf("You have been logon, UID is %s. You can 'logout' and then relogin.\n", pcs_sysUID(context->pcs));
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

/*ע��*/
static int cmd_logout(ShellContext *context, int argc, char *argv[])
{
	PcsRes pcsres;
	if (argc != 0) {
		usage_logout(context);
		return -1;
	}
	if (!is_login(context, NULL)) return -1;
	pcsres = pcs_logout(context->pcs);
	if (pcsres != PCS_OK) {
		printf("Logout Fail: %s\n", pcs_strerror(context->pcs));
		return -1;
	}
	printf("Logout Success.\n");
	return 0;
}

/*��ӡ�ļ���Ŀ¼��Ԫ����*/
static int cmd_meta(ShellContext *context, int argc, char *argv[])
{
	char *path;
	PcsFileInfo *fi;
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
		printf("Error: %s\n", pcs_strerror(context->pcs));
		if (path != context->workdir) pcs_free(path);
		return;
	}
	print_fileinfo(fi, " ");
	pcs_fileinfo_destroy(fi);
	if (path != context->workdir) pcs_free(path);
	return 0;
}

/*����Ŀ¼*/
static int cmd_mkdir(ShellContext *context, int argc, char *argv[])
{
	PcsRes res;
	char *path;
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
		return;
	}
	printf("Success.\n");
	pcs_free(path);
	return 0;
}

/*�ƶ��ļ�*/
static int cmd_move(ShellContext *context, int argc, char *argv[])
{
	if (!is_login(context, NULL)) return -1;
	printf("not implement\n");
	return 0;
}

/*��ӡ��ǰ����Ŀ¼*/
static int cmd_pwd(ShellContext *context, int argc, char *argv[])
{
	if (argc != 0){
		usage_pwd(context);
		return -1;
	}
	//if (!is_login(context, NULL)) return -1;
	printf("%s\n", context->workdir);
	return 0;
}

/*��ӡ���*/
static int cmd_quota(ShellContext *context, int argc, char *argv[])
{
	PcsRes pcsres;
	UInt64 quota, used;
	char str[32] = { 0 };
	if (argc != 0){
		usage_quota(context);
		return -1;
	}
	if (!is_login(context, NULL)) return -1;
	pcsres = pcs_quota(context->pcs, &quota, &used);
	if (pcsres != PCS_OK) {
		printf("Error: %s\n", pcs_strerror(context->pcs));
		return -1;
	}
	//printf("%lluBytes", used);
	//putchar('/');
	//printf("%lluBytes  ", quota);
	pcs_utils_readable_size((double)used, str, 30, NULL);
	printf("%s", str);
	putchar('/');
	pcs_utils_readable_size((double)quota, str, 30, NULL);
	printf("%s", str);
	printf("\n");
	return 0;
}

/*ɾ���ļ�*/
static int cmd_remove(ShellContext *context, int argc, char *argv[])
{
	if (!is_login(context, NULL)) return -1;
	printf("not implement\n");
	return 0;
}

/*�������ļ�*/
static int cmd_rename(ShellContext *context, int argc, char *argv[])
{
	if (!is_login(context, NULL)) return -1;
	printf("not implement\n");
	return 0;
}

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

static int set_captchafile(ShellContext *context, const char *val)
{
	if (!val || !val[0]) return -1;
	if (streq(context->captchafile, val, -1)) return 0;
	if (context->captchafile) pcs_free(context->captchafile);
	context->captchafile = pcs_utils_strdup(val);
	return 0;
}

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

/*��������������*/
static int cmd_set(ShellContext *context, int argc, char *argv[])
{
	int i, key_len;
	char *key, *val;
	//if (!is_login(context, NULL)) return -1;
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

/*�����ļ�*/
static int cmd_search(ShellContext *context, int argc, char *argv[])
{
	if (!is_login(context, NULL)) return -1;
	printf("not implement\n");
	return 0;
}

/*�ϴ�*/
static int cmd_upload(ShellContext *context, int argc, char *argv[])
{
	if (!is_login(context, NULL)) return -1;
	printf("not implement\n");
	return 0;
}

/*��ӡ�汾*/
static int cmd_version(ShellContext *context, int argc, char *argv[])
{
	if (argc != 0){
		usage_version(context);
		return -1;
	}
	version(context);
	return 0;
}

/*��ӡ��ǰ��¼�û�*/
static int cmd_who(ShellContext *context, int argc, char *argv[])
{
	if (argc != 0){
		usage_who(context);
		return -1;
	}
	if (!is_login(context, NULL)) return -1;
	printf("%s\n", pcs_sysUID(context->pcs));
	return 0;
}

/*��ӡ������Ϣ*/
static int cmd_help(ShellContext *context, int argc, char *argv[])
{
	if (argc != 0){
		usage_help(context);
		return -1;
	}
	usage(context);
	return 0;
}

/*·�ɵ�����������*/
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
	else if (strcmp(cmd, "compare") == 0) {
		rc = cmd_compare(context, cmd_argc, cmd_argv);
	}
	else if (strcmp(cmd, "context") == 0) {
		rc = cmd_context(context, cmd_argc, cmd_argv);
	}
	else if (strcmp(cmd, "download") == 0) {
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
	else if (strcmp(cmd, "upload") == 0) {
		rc = cmd_upload(context, cmd_argc, cmd_argv);
	}
	else if (strcmp(cmd, "version") == 0 
		|| strcmp(cmd, "-v") == 0 
		|| strcmp(cmd, "-V") == 0) {
		rc = cmd_version(context, cmd_argc, cmd_argv);
	}
	else if (strcmp(cmd, "who") == 0) {
		rc = cmd_who(context, cmd_argc, cmd_argv);
	}
	else if (strcmp(cmd, "help") == 0
		|| strcmp(cmd, "-h") == 0
		|| strcmp(cmd, "-H") == 0
		|| strcmp(cmd, "-?") == 0
		|| strcmp(cmd, "?") == 0) {
		rc = cmd_help(context, cmd_argc, cmd_argv);
	}
	else {
		usage(context);
		rc = -1;
	}
	return rc;
}

int main(int argc, char *argv[])
{
	ShellContext context;
	int rc = 0;
	hook_cjson();
	init_context(&context, argc, argv);
	restore_context(&context);
	init_pcs(&context);
	exec_cmd(&context, argc, argv);
	save_context(&context);
	free_context(&context);
	pcs_mem_print_leak();
	return rc;
}
