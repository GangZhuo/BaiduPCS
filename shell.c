#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

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
 * 从标准输入中输入字符串
 * str  - 输入的字符串将填充到 str 中
 * size - 最多输入 size 个字节。
*/
void std_string(char *str, int size)
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
 * 从标准输入中输入密码，输入的字符不回显
 * password  - 输入的密码将填充到 password 中
 * size      - 最多输入size个字节。
 */
void std_password(char *password, int size)
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

#else
#include <termios.h>
#include <unistd.h>

/*
* 从标准输入中输入字符串
* str  - 输入的字符串将填充到 str 中
* size - 最多输入 size 个字节。
*/
void std_string(char *str, int size)
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
* 从标准输入中输入密码，输入的字符不回显
* password  - 输入的密码将填充到 password 中
* size      - 最多输入size个字节。
*/
void std_password(char *password, int size)
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

#endif

/*返回COOKIE文件路径*/
static const char *cookiefile()
{
	static char filename[1024] = { 0 };
	if (!filename[0]){ /*如果已经处理过，则直接返回*/
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

/*返回验证码图片文件路径*/
static const char *captchafile()
{
	static char filename[1024] = { 0 };
	if (!filename[0]){ /*如果已经处理过，则直接返回*/
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

/*输出验证码图片，并等待用户输入识别结果*/
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

/*显示上传进度*/
int upload_progress(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow)
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
 * 读取全部文件内容
 * file    - 待读取的文件
 * pBuffer - 文件的内容所在的内存指针将存入pBuffer指定的内存中
 * 返回读取到的字节大小。使用完成后，需调用pcs_free(*pBuffer)
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

/*获取上下文存储文件路径*/
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

/*保存上下文*/
static void save_context(ShellContext *context)
{
	const char *filename;
	char *json;
	cJSON *root, *item;
	FILE *pf;

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

	json = cJSON_Print(root);
	assert(json);

	filename = contextfile();
	pf = fopen(filename, "wb");
	if (!pf) {
		printf("Can't open the file: %s\n", filename);
		pcs_free(json);
		cJSON_Delete(root);
		return;
	}
	fwrite(json, 1, strlen(json), pf);
	fclose(pf);
	pcs_free(json);
	cJSON_Delete(root);
}

/*还原保存的上下文*/
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
	if (item && item->valuestring[0]) {
		if (context->cookiefile) pcs_free(context->cookiefile);
		context->cookiefile = pcs_utils_strdup(item->valuestring);
	}

	item = cJSON_GetObjectItem(root, "captchafile");
	if (item && item->valuestring[0]) {
		if (context->captchafile) pcs_free(context->captchafile);
		context->captchafile = pcs_utils_strdup(item->valuestring);
	}

	item = cJSON_GetObjectItem(root, "workdir");
	if (item && item->valuestring[0]) {
		if (context->workdir) pcs_free(context->workdir);
		context->workdir = pcs_utils_strdup(item->valuestring);
	}
	cJSON_Delete(root);
	pcs_free(filecontent);
}

/*从程序路径中获取文件的名字*/
static const char *filename(char *path)
{
	const char *p;
	p = path;
	p += strlen(p);
	while (p > path && *p != '/' && *p != '\\') p--;
	if (*p == '/' || *p == '\\') p++;
	return p;
}

/*初始化PCS*/
static void init_pcs(ShellContext *context)
{
	context->pcs = pcs_create(context->cookiefile);
	pcs_setopt(context->pcs, PCS_OPTION_CAPTCHA_FUNCTION, (void *)&verifycode);
	pcs_setopts(context->pcs,
		PCS_OPTION_PROGRESS_FUNCTION, (void *)&upload_progress,
		PCS_OPTION_PROGRESS, (void *)((long)PcsFalse),
		PCS_OPTION_END);
	/*pcs_setopts(context->pcs,
		PCS_OPTION_SECURE_METHOD, (void *)((long)params->secure_method),
		PCS_OPTION_SECURE_KEY, params->secure_key,
		PCS_OPTION_SECURE_ENABLE, (void *)((long)PcsTrue),
		PCS_OPTION_END);*/

}

/*初始化上下文*/
static void init_context(ShellContext *context, int argc, char *argv[])
{
	memset(context, 0, sizeof(ShellContext));
	context->name = pcs_utils_strdup(filename(argv[0]));
	context->cookiefile = pcs_utils_strdup(cookiefile());
	context->captchafile = pcs_utils_strdup(captchafile());
	context->workdir = pcs_utils_strdup("/");
}

/*释放上下文*/
static void free_context(ShellContext *context)
{
	if (context->name) pcs_free(context->name);
	if (context->cookiefile) pcs_free(context->cookiefile);
	if (context->captchafile) pcs_free(context->captchafile);
	if (context->workdir) pcs_free(context->workdir);
	if (context->pcs) pcs_destroy(context->pcs);
	memset(context, 0, sizeof(ShellContext));
}

/*打印版本*/
static void version(ShellContext *context)
{
	printf(program_full_name "\n", context->name);
}

/*打印用法*/
static void usage(ShellContext *context)
{
	printf("Usage: %s command [options] [arg1|arg2...]\n", context->name);
	printf(
		"Commands:\n"
		"  cat      Print file content\n"
		"  copy     Copy file|directory\n"
		"  compare  Print different between local and net disk\n"
		"  download Download file|directory\n"
		"  echo     Write text into net disk\n"
		"  list     List files|directories\n"
		"  login    Login net disk\n"
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
		"  who      Print user ID\n"
		);
	printf("More detail see '%s command -h'. \n", context->name);
}

/*检查是否登录，如果未登录则登录*/
static int check_login(ShellContext *context)
{
	PcsRes pcsres;
	char str[50];
	pcsres = pcs_islogin(context->pcs);
	if (pcsres == PCS_LOGIN)
		return 0;
	printf("User Name: ");
	std_string(str, 50);
	pcs_setopt(context->pcs, PCS_OPTION_USERNAME, str);
	printf("Password: ");
	std_password(str, 50);
	pcs_setopt(context->pcs, PCS_OPTION_PASSWORD, str);
	pcsres = pcs_login(context->pcs);
	if (pcsres != PCS_OK) {
		printf("Login Failed: %s\n", pcs_strerror(context->pcs));
		return -1;
	}
	printf("Login Success. UID: %s\n", pcs_sysUID(context->pcs));
	return 0;
}

static void cmd_cat(ShellContext *context, int argc, char *argv[])
{

}

static void cmd_copy(ShellContext *context, int argc, char *argv[])
{
}

static void cmd_compare(ShellContext *context, int argc, char *argv[])
{
}

static void cmd_download(ShellContext *context, int argc, char *argv[])
{
}

static void cmd_echo(ShellContext *context, int argc, char *argv[])
{
}

static void cmd_list(ShellContext *context, int argc, char *argv[])
{
}

static void cmd_login(ShellContext *context, int argc, char *argv[])
{
}

static void cmd_meta(ShellContext *context, int argc, char *argv[])
{
}

static void cmd_mkdir(ShellContext *context, int argc, char *argv[])
{
}

static void cmd_move(ShellContext *context, int argc, char *argv[])
{
}

static void cmd_pwd(ShellContext *context, int argc, char *argv[])
{
}

static void cmd_quota(ShellContext *context, int argc, char *argv[])
{
}

static void cmd_remove(ShellContext *context, int argc, char *argv[])
{
}

static void cmd_rename(ShellContext *context, int argc, char *argv[])
{
}

static void cmd_set(ShellContext *context, int argc, char *argv[])
{
}

static void cmd_search(ShellContext *context, int argc, char *argv[])
{
}

static void cmd_upload(ShellContext *context, int argc, char *argv[])
{
}

static void cmd_who(ShellContext *context, int argc, char *argv[])
{
}

/*路由到具体的命令函数*/
static void exec_cmd(ShellContext *context, int argc, char *argv[])
{
	const char *cmd;
	int cmd_argc;
	char **cmd_argv;
	if (argc < 2) {
		usage(context);
		return;
	}
	cmd = argv[1];
	cmd_argc = argc > 2 ? (argc - 2) : 0;
	cmd_argv = cmd_argc > 0 ? &argv[2] : NULL;

	if (strcmp(cmd, "cat")) {
		cmd_cat(context, cmd_argc, cmd_argv);
	}
	else if (strcmp(cmd, "copy")) {
		cmd_copy(context, cmd_argc, cmd_argv);
	}
	else if (strcmp(cmd, "compare")) {
		cmd_compare(context, cmd_argc, cmd_argv);
	}
	else if (strcmp(cmd, "download")) {
		cmd_download(context, cmd_argc, cmd_argv);
	}
	else if (strcmp(cmd, "echo")) {
		cmd_echo(context, cmd_argc, cmd_argv);
	}
	else if (strcmp(cmd, "list")) {
		cmd_list(context, cmd_argc, cmd_argv);
	}
	else if (strcmp(cmd, "login")) {
		cmd_login(context, cmd_argc, cmd_argv);
	}
	else if (strcmp(cmd, "meta")) {
		cmd_meta(context, cmd_argc, cmd_argv);
	}
	else if (strcmp(cmd, "mkdir")) {
		cmd_mkdir(context, cmd_argc, cmd_argv);
	}
	else if (strcmp(cmd, "move")) {
		cmd_move(context, cmd_argc, cmd_argv);
	}
	else if (strcmp(cmd, "pwd")) {
		cmd_pwd(context, cmd_argc, cmd_argv);
	}
	else if (strcmp(cmd, "quota")) {
		cmd_quota(context, cmd_argc, cmd_argv);
	}
	else if (strcmp(cmd, "remove")) {
		cmd_remove(context, cmd_argc, cmd_argv);
	}
	else if (strcmp(cmd, "rename")) {
		cmd_rename(context, cmd_argc, cmd_argv);
	}
	else if (strcmp(cmd, "set")) {
		cmd_set(context, cmd_argc, cmd_argv);
	}
	else if (strcmp(cmd, "search")) {
		cmd_search(context, cmd_argc, cmd_argv);
	}
	else if (strcmp(cmd, "upload")) {
		cmd_upload(context, cmd_argc, cmd_argv);
	}
	else if (strcmp(cmd, "who")) {
		cmd_who(context, cmd_argc, cmd_argv);
	}
	else {
		usage(context);
	}
}

int main(int argc, char *argv[])
{
	ShellContext context;
	int rc = 0;
	hook_cjson();
	init_context(&context, argc, argv);
	restore_context(&context);
	init_pcs(&context);
	version(&context);
	exec_cmd(&context, argc, argv);
	save_context(&context);
	free_context(&context);
	pcs_mem_print_leak();
	return rc;
}
