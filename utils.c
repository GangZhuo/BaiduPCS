#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#ifdef WIN32
#  include <conio.h>
#  include <direct.h>
# include <WinSock2.h>
# include <Windows.h>
# include <Shlwapi.h>

# define snprintf _snprintf
# define vsnprintf _vsnprintf

#else
#  include <inttypes.h>
#  include <termios.h>
#endif

#include "pcs/pcs_mem.h"
#include "utils.h"


static inline char *i_strdup(const char *str, int len)
{
	char *res = 0;
	if (str) {
		if (len == -1) len = strlen(str);
		res = (char *)pcs_malloc(len + 1);
		if (!res) return 0;
		memcpy(res, str, len);
		res[len] = '\0';
	}
	return res;
}

#pragma region std_string(), std_password(), is_absolute_path() 三个函数

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

int is_absolute_path(const char *path)
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

int is_absolute_path(const char *path)
{
	if (!path) return 0;
	if (strlen(path) < 1) return 0;
	if (path[0] != '/' && path[0] != '~') return 0;
	return 1;
}

#endif

#pragma endregion

/** detecting whether base is starts with str
*/
int startsWith(char* base, char* str)
{
	return (strstr(base, str) - base) == 0;
}

/** detecting whether base is ends with str
*/
int endsWith(char* base, char* str)
{
	int blen = strlen(base);
	int slen = strlen(str);
	return (blen >= slen) && (strcmp(base + blen - slen, str) == 0);
}

/*
* 判断两个字符串是否相等。
*  s1    - 以'\0'为结束标记的字符串
*  s2    - 待比较的字符串
*  s2len - s2字符串的字节长度。如果传入-1的话，则使用'\0'作为其结束标记
* 如果相等，则返回1，否则返回0。
*/
int streq(const char *s1, const char *s2, int s2len)
{
	const char *p1 = s1, *p2 = s2;
	int i = 0, rc = 1;
	if (s2len == -1) {
		for (i = 0;; i++) {
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
* 判断arr数组中是否存在字符串str，如果存在则返回其标号（标号为 [索引] + 1），否则返回0。
* 比较时区分大小写。
* arr  - 存储很多字符串的数组，数组最后一个元素必须为NULL。
* str  - 判断是否存在的字符串
* len  - 字符串长度。 如果传入-1，则'\0'作为其结束标记。
*/
int str_in_array(const char **arr, const char *str, int len)
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

/*
* 合并路径，如果filename传入的是绝对路径，则直接返回filename的拷贝。
*   base     - 基目录
*   basesz   - base的字节长度，传入-1的话，将使用strlen()函数读取。
*   filename - 文件名字
* 使用完后，需调用pcs_free来释放返回值
*/
char *combin_path(const char *base, int basesz, const char *filename)
{
	char *result = NULL;
#ifdef WIN32
	char buf[MAX_PATH] = "";
	char *p = i_strdup(base, basesz),
		*f = i_strdup(filename, -1);
	p = fix_win_path(p);
	f = fix_win_path(f);
	if (PathCombine(buf, p, f)) {
		result = i_strdup(buf, -1);
	}
	pcs_free(p);
	pcs_free(f);
#else
	if (basesz == -1) {
		result = combin_net_disk_path(base, filename);
	}
	else {
		char *p = i_strdup(base, basesz);
		result = combin_net_disk_path(p, filename);
		pcs_free(p);
	}
#endif
	return result;
}

/*
把path根据斜杠拆分成一个个的目录名，然后把这些目录按顺序组合到buf中。
该函数能正确处理"."和".."。
*/
static inline void fill_unix_true_path_to_buf(char *buf, const char *path)
{
	const char *start, *end;
	char *p;
	p = buf;
	while (*p) p++;
	if (path && path[0]) {
		start = end = path;
		if (*end == '/' || *end == '\\') { /*path是绝对路径,则清掉buf原来的值*/
			/*清掉buf原来值，并设置新值为斜杠*/
			p = buf;
			*p = '/';
			p++;
			*p = '\0';
			end++;
			start = end;
		}
		else if (p > buf && p[-1] != '/' && p[-1] != '\\') { /*检查buf的最后一个字符是否是目录分隔符*/
			/*末尾添加斜杠*/
			*p = '/';
			p++;
			*p = '\0';
		}
		while (1) {
			if (*end == '/' || *end == '\\' || !(*end)) {
				if (end == start) { /*连续的目录分隔符*/
					if (!(*end)) break;
					end++;
					start = end;
					continue;
				}
				else if (streq(".", start, end - start)) {
					if (!(*end)) break;
					end++;
					start = end;
					continue;
				}
				else if (streq("..", start, end - start)) {
					if ((buf[0] == '/' || buf[0] == '\\') && buf[1] == '\0') { /*如果buf == "/"，
																				即buf是根目录，则其没有父目录。直接跳过。
																				*/
						if (!(*end)) break;
						end++;
						start = end;
						continue;
					}
					p--; *p = '\0'; /*清掉最后一个斜杠*/
					while (p >= buf) {
						if (*p == '/' || *p == '\\') {
							break;
						}
						p--;
					}
					if (p < buf) { /*未找到斜杠，则清空buf*/
						p = buf;
						*p = '\0';
					}
					else {
						p++; /*找到斜杠，则清除斜杠后边的值*/
						*p = '\0';
					}
					if (!(*end)) break;
					end++;
					start = end;
					continue;
				}
				else {
					memcpy(p, start, end - start);
					p += end - start;
					*p = '/';
					p++;
					*p = '\0';
					if (!(*end)) break;
					end++;
					start = end;
					continue;
				}
			}
			end++;
		}
	}
	/*如果buf != "/"的话，即如果buf不是根目录的话，清除最后一个斜杠*/
	if (p > buf && (p[-1] == '/' || p[-1] == '\\')) { /*检查到buf的最后一个字符是目录分隔符*/
		if (p > buf + 1) {
			p[-1] = '\0';
		}
	}
}

/*
* 合并unix格式的路径，如果filename传入的是绝对路径，则直接返回filename的拷贝。
* 使用完后，需调用pcs_free来释放返回值
*/
char *combin_net_disk_path(const char *base, const char *filename)
{
	char *result;
	int basesz = 0, filenamesz = 0, sz;

	if (base) basesz = strlen(base);
	if (filename) filenamesz = strlen(filename);
	sz = basesz + filenamesz + 2;
	result = (char *)pcs_malloc(sz + 1);
	result[0] = '\0';
	fill_unix_true_path_to_buf(result, base);
	fill_unix_true_path_to_buf(result, filename);
	return result;
}

/*
* 修正路径。
* 即把路径中反斜杠替换为正斜杠。
* 修正完成后，原样返回path
*/
char *fix_unix_path(char *path)
{
	char *p = path;
	while (*p) {
		if (*p == '\\') *p = '/';
		p++;
	}
	return path;
}

/*
* 修正路径。
* 即把路径中斜杠替换为反斜杠。
* 修正完成后，原样返回path
*/
char *fix_win_path(char *path)
{
	char *p = path;
	while (*p) {
		if (*p == '/') *p = '\\';
		p++;
	}
	return path;
}

/*
* 读取全部文件内容
* file    - 待读取的文件
* pBuffer - 文件的内容所在的内存指针将存入pBuffer指定的内存中
* 返回读取到的字节大小。使用完成后，需调用pcs_free(*pBuffer)
*/
int read_file(const char *file, char **pBuffer)
{
	FILE *fp;
	long size_of_file, sz;
	char *content;

	fp = fopen(file, "rb");
	if (!fp) {
		//printf("Open file fail: %s\n", file);
		return -1;
	}
	fseek(fp, 0L, SEEK_END);
	size_of_file = ftell(fp);
	if (size_of_file < 3) {
		printf("Wrong file size: Size=%ld, %s\n", size_of_file, file);
		fclose(fp);
		return -1;
	}
	fseek(fp, 0L, SEEK_SET);
	content = (char *)pcs_malloc(size_of_file + 1);
	sz = fread(content, 1, size_of_file, fp);
	fclose(fp);
	if (sz != size_of_file) {
		printf("Read file error: %s\n", file);
		pcs_free(content);
		return -1;
	}
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

/*从程序路径中找到文件名开始的位置，返回开始位置的指针*/
const char *filename(const char *path)
{
	const char *p;
	p = path;
	p += strlen(p);
	while (p > path && *p != '/' && *p != '\\') p--;
	if (*p == '/' || *p == '\\') p++;
	return p;
}

/*
* 获取路径的父路径，如果没有父路径则返回NULL。
*   path  - 当前路径
*   len   - path的字节长度，如果传入-1，则内部使用strlen()获取其长度
* 返回值需要调用pcs_free()
*/
char *base_dir(const char *path, int len)
{
	char *dir, *p;
	if (!path) return NULL;
	if (len == -1) len = strlen(path);
	if (len == 0) return NULL;
	dir = (char *)pcs_malloc(len + 1);
	strcpy(dir, path);
	p = dir + len - 1;
	while (p > dir && *p != '/' && *p != '\\') p--;
	if (p == dir) {
		if (*p != '/' && *p != '\\') {
			pcs_free(dir);
			return NULL;
		}
		p[1] = '\0';
	}
	else {
#ifdef _WIN32
		if (*(p - 1) == ':' && ((p - dir) == 2))
			*(p + 1) = '\0';
		else
			*p = '\0';
#else
		*p = '\0';
#endif
	}
	return dir;
}

/*
string to time_t
时间格式 2009-3-24 0:00:08 或 2009-3-24
*/
int str2time(const char *str, time_t *timeData)
{
	char *pBeginPos = (char*)str;
	char *pPos = strstr(pBeginPos, "-");
	int iYear, iMonth, iDay, iHour, iMin, iSec;
	struct tm sourcedate = { 0 };
	if (pPos == NULL) {
		printf("strDateStr[%s] err \n", str);
		return -1;
	}
	iYear = atoi(pBeginPos);
	iMonth = atoi(pPos + 1);
	pPos = strstr(pPos + 1, "-");
	if (pPos == NULL) {
		printf("strDateStr[%s] err \n", str);
		return -1;
	}
	iDay = atoi(pPos + 1);
	iHour = 0;
	iMin = 0;
	iSec = 0;
	pPos = strstr(pPos + 1, " ");
	//为了兼容有些没精确到时分秒的
	if (pPos != NULL) {
		iHour = atoi(pPos + 1);
		pPos = strstr(pPos + 1, ":");
		if (pPos != NULL) {
			iMin = atoi(pPos + 1);
			pPos = strstr(pPos + 1, ":");
			if (pPos != NULL) {
				iSec = atoi(pPos + 1);
			}
		}
	}

	sourcedate.tm_sec = iSec;
	sourcedate.tm_min = iMin;
	sourcedate.tm_hour = iHour;
	sourcedate.tm_mday = iDay;
	sourcedate.tm_mon = iMonth - 1;
	sourcedate.tm_year = iYear - 1900;
	*timeData = mktime(&sourcedate);
	return 0;
}

/*
time_t to string 时间格式 2009-3-24 0:00:08
*/
char *time2str(char *buf, const time_t *t)
{
	char chTmp[100] = { 0 };
	struct tm *p;
	p = localtime(t);
	p->tm_year = p->tm_year + 1900;
	p->tm_mon = p->tm_mon + 1;

	snprintf(chTmp, sizeof(chTmp), "%04d-%02d-%02d %02d:%02d:%02d",
		p->tm_year, p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
	strcpy(buf, chTmp);
	return 0;
}
