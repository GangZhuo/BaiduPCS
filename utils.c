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
#include "utils.h"


#if defined(WIN32)
# define snprintf _snprintf
#endif


static inline char *i_strdup(const char *str)
{
	char *res = 0;
	if (str) {
		res = (char *)pcs_malloc(strlen(str) + 1);
		if (!res)
			return 0;
		strcpy(res, str);
	}
	return res;
}

#pragma region std_string(), std_password(), is_absolute_path() ��������

#ifdef WIN32

/*
* �ӱ�׼�����������ַ���
* str  - ������ַ�������䵽 str ��
* size - ������� size ���ֽڡ�
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
* �ӱ�׼�������������룬������ַ�������
* password  - ��������뽫��䵽 password ��
* size      - �������size���ֽڡ�
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
* �ӱ�׼�����������ַ���
* str  - ������ַ�������䵽 str ��
* size - ������� size ���ֽڡ�
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
* �ӱ�׼�������������룬������ַ�������
* password  - ��������뽫��䵽 password ��
* size      - �������size���ֽڡ�
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

/*
* �ж������ַ����Ƿ���ȡ�
*  s1    - ��'\0'Ϊ������ǵ��ַ���
*  s2    - ���Ƚϵ��ַ���
*  s2len - s2�ַ������ֽڳ��ȡ��������-1�Ļ�����ʹ��'\0'��Ϊ��������
* �����ȣ��򷵻�1�����򷵻�0��
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
* �ж�arr�������Ƿ�����ַ���str����������򷵻����ţ����Ϊ [����] + 1�������򷵻�0��
* �Ƚ�ʱ���ִ�Сд��
* arr  - �洢�ܶ��ַ��������飬�������һ��Ԫ�ر���ΪNULL��
* str  - �ж��Ƿ���ڵ��ַ���
* len  - �ַ������ȡ� �������-1����'\0'��Ϊ�������ǡ�
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
* �ϲ�·�������filename������Ǿ���·������ֱ�ӷ���filename�Ŀ�����
*   base     - ��Ŀ¼
*   basesz   - base���ֽڳ��ȣ�����-1�Ļ�����ʹ��strlen()������ȡ��
*   filename - �ļ�����
* ʹ����������pcs_free���ͷŷ���ֵ
*/
char *combin_path(const char *base, int basesz, const char *filename)
{
	char *p, *p2;
	int filenamesz, sz;

	if (strcmp(filename, ".") == 0) {
		p = (char *)pcs_malloc(basesz + 1);
		assert(p);
		memset(p, 0, basesz + 1);
		memcpy(p, base, basesz);
		p[basesz] = '\0';
	}
	else if (strcmp(filename, "..") == 0) {
		p = (char *)pcs_malloc(basesz + 1);
		assert(p);
		memset(p, 0, basesz + 1);
		memcpy(p, base, basesz);
		p[basesz] = '\0';
		basesz--;
		if (p[basesz] == '/' || p[basesz] == '\\') p[basesz] = '\0';
		basesz--;
		while (basesz >= 0 && p[basesz] != '/' && p[basesz] != '\\') {
			basesz--;
		}
		if (basesz < 0) {
			p[0] = '\0';
		}
		else if (basesz == 0) {
			p[1] = '\0';
		}
		else {
			p[basesz] = '\0';
		}
	}
	else if (is_absolute_path(filename) || !base || basesz == 0 || !base[0]) {
		p = i_strdup(filename);
	}
	else {
		if (basesz == -1) basesz = strlen(base);
		filenamesz = strlen(filename);
		sz = basesz + filenamesz + 1;
		p = (char *)pcs_malloc(sz + 1);
		assert(p);
		memset(p, 0, sz + 1);
		memcpy(p, base, basesz);
		p[basesz] = '\0';
		if (filename[0] == '/' || filename[0] == '\\') {
			if (p[basesz - 1] == '/' || p[basesz - 1] == '\\') {
				p[basesz - 1] = '\0';
			}
		}
		else {
			if (p[basesz - 1] != '/' && p[basesz - 1] != '\\') {
#ifdef WIN32
				p[basesz] = '\\';
#else
				p[basesz] = '/';
#endif
				p[basesz + 1] = '\0';
			}
		}
		strcat(p, filename);
	}
	p2 = p;
	while (*p2) {
#ifdef WIN32
		if (*p2 == '/') *p2 = '\\';
#else
		if (*p2 == '\\') *p2 = '/';
#endif
		p2++;
	}
	return p;
}

/*
* �ϲ�unix��ʽ��·�������filename������Ǿ���·������ֱ�ӷ���filename�Ŀ�����
* ʹ����������pcs_free���ͷŷ���ֵ
*/
char *combin_unix_path(const char *base, const char *filename)
{
	char *p, *p2;
	int basesz, filenamesz, sz;

	if (strcmp(filename, ".") == 0) {
		basesz = strlen(base);
		p = (char *)pcs_malloc(basesz + 1);
		assert(p);
		memset(p, 0, basesz + 1);
		memcpy(p, base, basesz);
		p[basesz] = '\0';
	}
	else if (strcmp(filename, "..") == 0) {
		basesz = strlen(base);
		p = (char *)pcs_malloc(basesz + 1);
		assert(p);
		memset(p, 0, basesz + 1);
		memcpy(p, base, basesz);
		p[basesz] = '\0';
		basesz--;
		if (p[basesz] == '/' || p[basesz] == '\\') p[basesz] = '\0';
		basesz--;
		while (basesz >= 0 && p[basesz] != '/' && p[basesz] != '\\') {
			basesz--;
		}
		if (basesz < 0) {
			p[0] = '\0';
		}
		else if (basesz == 0) {
			p[1] = '\0';
		}
		else {
			p[basesz] = '\0';
		}
	}
	else if (filename[0] == '/' || filename[0] == '\\' || filename[0] == '~') { /*����Ǿ���·����ֱ�ӷ��ظ�ֵ*/
		p = i_strdup(filename);
	}
	else {
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
	}
	p2 = p;
	while (*p2) {
		if (*p2 == '\\') *p2 = '/';
		p2++;
	}
	return p;
}

/*
* ����·����
* ����·���з�б���滻Ϊ��б�ܡ�
* ������ɺ�ԭ������path
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
* ��ȡȫ���ļ�����
* file    - ����ȡ���ļ�
* pBuffer - �ļ����������ڵ��ڴ�ָ�뽫����pBufferָ�����ڴ���
* ���ض�ȡ�����ֽڴ�С��ʹ����ɺ������pcs_free(*pBuffer)
*/
int read_file(const char *file, char **pBuffer)
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

/*�ӳ���·�����ҵ��ļ�����ʼ��λ�ã����ؿ�ʼλ�õ�ָ��*/
const char *filename(char *path)
{
	const char *p;
	p = path;
	p += strlen(p);
	while (p > path && *p != '/' && *p != '\\') p--;
	if (*p == '/' || *p == '\\') p++;
	return p;
}

/*
* ��ȡ·���ĸ�·�������û�и�·���򷵻�NULL��
*   path  - ��ǰ·��
*   len   - path���ֽڳ��ȣ��������-1�����ڲ�ʹ��strlen()��ȡ�䳤��
* ����ֵ��Ҫ����pcs_free()
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
	*p = '\0';
	return dir;
}

/*
string to time_t
ʱ���ʽ 2009-3-24 0:00:08 �� 2009-3-24
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
	//Ϊ�˼�����Щû��ȷ��ʱ�����
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
time_t to string ʱ���ʽ 2009-3-24 0:00:08
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
