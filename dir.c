#ifdef WIN32
# include <stdio.h>
# include <stdlib.h>
# include <io.h>
# include <direct.h>
# include <sys/utime.h>
# include <WinSock2.h>
# include <Windows.h>
#else
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <time.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <alloca.h>
# include <unistd.h>
# include <dirent.h>
# ifdef HAVE_UTIME_H
#   ifdef WIN32
#     include <sys/utime.h>
#   endif
#   ifdef LINUX
#     include <utime.h>
#   endif
# else
 struct utimbuf {
	time_t actime;
	time_t modtime;
 };
 int utime(const char *filename, const struct utimbuf *times);
# endif

#endif

#include "pcs/pcs_mem.h"
#include "dir.h"

#ifdef WIN32
# define utimbuf	_utimbuf
# define utime		_utime
# define rmdir		_rmdir
#endif

#ifdef WIN32

time_t FileTimeToTime_t(FILETIME ft, time_t *t)
{
	ULONGLONG  ll;
	ULARGE_INTEGER ui;
	time_t rc;
	ui.LowPart = ft.dwLowDateTime;
	ui.HighPart = ft.dwHighDateTime;
	ll = (((ULONGLONG)ft.dwHighDateTime) << 32) + ft.dwLowDateTime;
	rc = ((ULONGLONG)(ui.QuadPart - 116444736000000000) / 10000000);
	if (t) *t = rc;
	return rc;
}

#endif

static char *combin_path(const char *base, const char *filename, const char **pName)
{
	char *p;
	int sz_base;

	sz_base = strlen(base);
	p = (char *)pcs_malloc(sz_base + strlen(filename) + 2);
	strcpy(p, base);
#ifdef WIN32
	p[sz_base] = '\\';
#else
	p[sz_base] = '/';
#endif
	sz_base++;
	p[sz_base] = '\0';
	if (pName) (*pName) = &p[sz_base];
	strcat(p, filename);
	return p;
}

static char *path_dup(const char *path, const char **pName)
{
	char *result, *p;
	int len = strlen(path);
	result = (char *)pcs_malloc(len + 1);
	strcpy(result, path);
	p = result;
	p += len - 1;
	if (*p == '/' || *p == '\\') *p = '\0'; //如果最后一个字符是'/'或者'\\'的话，则清掉
	if (pName) {
		while (p > result && *p != '/' && *p != '\\') p--; /*找到第一个'/'或'\\'的地方，用于设定pName值*/
		if (*p == '/' || *p == '\\') {
			p++;
			(*pName) = p;
		}
		else {
			(*pName) = p;
		}
	}
#ifdef WIN32
	while (p >= result) { if (*p == '/') *p = '\\'; p--; } /*继续修正斜杠*/
#else
	while (p >= result) { if (*p == '\\') *p = '/'; p--; }
#endif
	return result;
}

static char *filename_dup(const char *filename, const char **pName)
{
	char *result;
	int len = strlen(filename);
	result = (char *)pcs_malloc(len + 1);
	strcpy(result, filename);
	if (pName) (*pName) = result;
	return result;
}

static LocalFileInfo *CreateLocalFileInfo(const char *parentPath, const char *filename, int isdir, time_t mtime, int64_t size, LocalFileInfo *parent)
{
	LocalFileInfo *info;
	info = (LocalFileInfo *)pcs_malloc(sizeof(LocalFileInfo));
	if (!parentPath || !parentPath[0]) {
		if (filename && filename[0])
			info->path = filename_dup(filename, &info->filename);
		else 
			info->filename = info->path = NULL;
	}
	else {
		if (filename && filename[0])
			info->path = combin_path(parentPath, filename, &info->filename);
		else
			info->path = path_dup(parentPath, &info->filename);
	}
	info->isdir = isdir;
	info->mtime = mtime;
	info->size = size;
	info->parent = parent;
	info->next = NULL;
	info->filecount = 0;
	info->userdata = NULL;
	while (parent) {
		parent->filecount++;
		parent = parent->parent;
	}
	return info;
}

void DestroyLocalFileInfo(LocalFileInfo *info)
{
	if (info->path) pcs_free(info->path);
	pcs_free(info);
}

void DestroyLocalFileInfoLink(LocalFileInfo *link)
{
	LocalFileInfo *cusor, *info;
	cusor = link;
	while (cusor) {
		info = cusor;
		cusor = cusor->next;
		DestroyLocalFileInfo(info);
	}
}

LocalFileInfo *GetLocalFileInfo(const char *file)
{
	LocalFileInfo *info = NULL;
#ifdef WIN32
	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile(file, &fd);
	FindClose(hFind);
	if (hFind != INVALID_HANDLE_VALUE) {
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) /*为目录*/
			info = CreateLocalFileInfo(file, NULL, 1, FileTimeToTime_t(fd.ftLastWriteTime, NULL), 0, NULL);
		else { /*为文件*/
			int64_t fsize = fd.nFileSizeHigh;
			fsize <<= 32;
			fsize |= fd.nFileSizeLow;
			info = CreateLocalFileInfo(file, NULL, 0, FileTimeToTime_t(fd.ftLastWriteTime, NULL), fsize, NULL);
		}
	}
#else
	struct stat st;
	if (stat(file, &st))
		return NULL;
	if (S_ISDIR(st.st_mode)) /*为目录*/
		info = CreateLocalFileInfo(file, NULL, 1, st.st_mtime, 0, NULL);
	else if (S_ISREG(st.st_mode)) /*为文件*/
		info = CreateLocalFileInfo(file, NULL, 0, st.st_mtime, st.st_size, NULL);
#endif
	return info;
}

static int GetDirectoryFilesHelp(const char *dir, LocalFileInfo **pCusor, int recursive, int skip, int *pCnt, LocalFileInfo *parent,
	void(*on)(LocalFileInfo *info, LocalFileInfo *parent, void *state), void *state)
{
#ifdef WIN32
	struct _finddata_t filefind;
	char *curr, *subdir;
	int done = 0, handle, len;
	LocalFileInfo *cusor, *info;

	len = strlen(dir);
	curr = (char *)pcs_malloc(len + 5);
	strcpy(curr, dir);
	if (dir[len - 1] == '/' || dir[len - 1] == '\\')
		strcat(curr, "*.*");
	else
		strcat(curr, "\\*.*");
	if ((handle = _findfirst(curr, &filefind)) == -1) {
		pcs_free(curr);
		return -1;
	}
	cusor = *pCusor;
	while (!(done = _findnext(handle, &filefind))) {
		if (!strcmp(filefind.name, ".") || !strcmp(filefind.name, ".."))
			continue;
		if ((_A_SUBDIR == filefind.attrib)) {
			info = CreateLocalFileInfo(dir + skip, filefind.name, 1, filefind.time_write, 0, parent);
			cusor->next = info;
			cusor = info;
			if (pCnt) (*pCnt)++;
			if (on) (*on)(info, parent, state);
			if (recursive) {
				subdir = (char *)pcs_malloc(len + strlen(filefind.name) + 2);
				strcpy(subdir, dir);
				if (subdir[len - 1] != '/' && subdir[len - 1] != '\\') strcat(subdir, "\\");
				strcat(subdir, filefind.name);
				if (GetDirectoryFilesHelp(subdir, &cusor, recursive, skip, pCnt, info, on, state)) {
					_findclose(handle);
					*pCusor = cusor;
					pcs_free(subdir);
					return -1;
				}
				pcs_free(subdir);
			}
		}
		else {
			info = CreateLocalFileInfo(dir + skip, filefind.name, 0, filefind.time_write, filefind.size, parent);
			cusor->next = info;
			cusor = info;
			if (pCnt) (*pCnt)++;
			if (on) (*on)(info, parent, state);
		}
	}
	_findclose(handle);
	*pCusor = cusor;
	pcs_free(curr);
#else
	struct dirent* ent = NULL;
	DIR* pDir;
	LocalFileInfo *cusor, *info;
	struct stat st;
	char *subdir;
	int len;

	len = strlen(dir);
	pDir = opendir(dir);
	if (pDir == NULL) return -1;
	cusor = *pCusor;
	while ((ent = readdir(pDir)) != NULL) {
		if (ent->d_type == 4) {
			if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, ".."))
				continue;
			info = CreateLocalFileInfo(dir + skip, ent->d_name, 1, 0, 0, parent);
			cusor->next = info;
			cusor = info;
			subdir = (char *)pcs_malloc(len + strlen(ent->d_name) + 2);
			strcpy(subdir, dir);
			if (subdir[len - 1] != '/' && subdir[len - 1] != '\\') strcat(subdir, "/");
			strcat(subdir, ent->d_name);
			if (!stat(subdir, &st))
				info->mtime = st.st_mtime;
			if (pCnt) (*pCnt)++;
			if (on) (*on)(info, parent, state);
			if (recursive) {
				if (GetDirectoryFilesHelp(subdir, &cusor, recursive, skip, pCnt, info, on, state)) {
					*pCusor = cusor;
					pcs_free(subdir);
					return -1;
				}
			}
			pcs_free(subdir);
		}
		else if (ent->d_type == 8){
			info = CreateLocalFileInfo(dir + skip, ent->d_name, 0, 0, 0, parent);
			cusor->next = info;
			cusor = info;
			subdir = (char *)pcs_malloc(len + strlen(ent->d_name) + 2);
			strcpy(subdir, dir);
			if (subdir[len - 1] != '/' && subdir[len - 1] != '\\') strcat(subdir, "/");
			strcat(subdir, ent->d_name);
			if (!stat(subdir, &st))
				info->mtime = st.st_mtime;
			info->size = st.st_size;
			pcs_free(subdir);
			if (pCnt) (*pCnt)++;
			if (on) (*on)(info, parent, state);
		}
	}
	closedir(pDir);
	*pCusor = cusor;
#endif
	return 0;
}

int GetDirectoryFiles(LocalFileInfo **pLink, const char *dir, int recursive, void(*on)(LocalFileInfo *info, LocalFileInfo *parent, void *state), void *state)
{
	LocalFileInfo root = { 0 }, *cusor = &root;
	int skip;
	char *p;
	int cnt = 0;
	skip = strlen(dir);
	if (dir[skip - 1] != '/' && dir[skip - 1] != '\\') {
		p = (char *)alloca(skip + 2);
		strcpy(p, dir);
#ifdef WIN32
		p[skip++] = '\\';
#else
		p[skip++] = '/';
#endif
		p[skip] = '\0';
		dir = p;
	}
	if (GetDirectoryFilesHelp(dir, &cusor, recursive, skip, &cnt, NULL, on, state)) {
		DestroyLocalFileInfoLink(root.next);
		root.next = NULL;
		return -1;
	}
	if (pLink) {
		(*pLink) = root.next;
	}
	else {
		DestroyLocalFileInfoLink(root.next);
	}
	return cnt;
}

int SetFileLastModifyTime(const char *file, time_t mtime)
{
	struct utimbuf ut;
	ut.actime = mtime;
	ut.modtime = mtime;
	return utime(file, &ut);
}


/*
* 递归创建目录
*    path - 待创建的目录
* 如果成功则返回0，否则返回错误码。
*
*/
int CreateDirectoryRecursive(const char *path)
{
	char *tmp, *p;
	int len;
	LocalFileInfo *info;

	if (!path || (len = strlen(path)) == 0)
		return MKDIR_OK;
#ifdef _WIN32
	if (len == 2 && *(path + 1) == ':' && ((*path >= 'a' && *path <= 'z') || (*path >= 'A' && *path <= 'Z')))
		return MKDIR_OK;
	else if (len == 3 && *(path + 1) == ':' && (*(path + 2) == '\\' || *(path + 2) == '/') && ((*path >= 'a' && *path <= 'z') || (*path >= 'A' && *path <= 'Z')))
		return MKDIR_OK;
#else
	if (len == 1 && *path == '/')
		return MKDIR_OK;
#endif
	info = GetLocalFileInfo(path);
	if (info) {
		if (!info->isdir) {
			DestroyLocalFileInfo(info);
			return MKDIR_TARGET_IS_FILE;
		}
		if (info->isdir) {
			DestroyLocalFileInfo(info);
			return MKDIR_OK;
		}
		DestroyLocalFileInfo(info);
	}
	tmp = (char *)alloca(len + 1);
	strcpy(tmp, path);
	p = tmp;
	if (*p == '/' || *p == '\\') p++;
	while (*p) {
		if (*p == '/' || *p == '\\') {
			*p = '\0';
			if (!strcmp(tmp, ".") || !strcmp(tmp, "..")) {
#ifdef WIN32
				*p = '\\';
#else
				*p = '/';
#endif
				p++;
				continue;
			}
			info = GetLocalFileInfo(tmp);
			if (info) {
				if (!info->isdir) {
					DestroyLocalFileInfo(info);
					return MKDIR_TARGET_IS_FILE;
				}
				DestroyLocalFileInfo(info);
			}
			else {
#ifdef WIN32
				if (_mkdir(tmp))
#else
				if (mkdir(tmp, DEFAULT_MKDIR_ACCESS))
#endif
					return MKDIR_FAIL;
			}
#ifdef WIN32
			*p = '\\';
#else
			*p = '/';
#endif
		}
		p++;
	}
	if (p[-1] != '/' && p[-1] != '\\') {
#ifdef WIN32
		if (_mkdir(tmp))
#else
		if (mkdir(tmp, DEFAULT_MKDIR_ACCESS))
#endif
			return MKDIR_FAIL;
	}
	return MKDIR_OK;
}

/*递归删除文件或目录
成功返回0，否则返回非0值
*/
int DeleteFileRecursive(const char *path)
{
	LocalFileInfo *info, *p;
	
	info = GetLocalFileInfo(path);
	if (!info) return 0;

	if (info->isdir) {
		p = info;
		if (GetDirectoryFilesHelp(path, &p, 0, 0, NULL, info, NULL, NULL) < 0){
			DestroyLocalFileInfoLink(info);
			return -1;
		}
		p = info;
		while (p) {
			if (DeleteFileRecursive(p->path)) {
				DestroyLocalFileInfoLink(info);
				return -1;
			}
			p = p->next;
		}
		DestroyLocalFileInfoLink(info);
		if (!p) {
			if (rmdir(path))
				return -1;
		}
	}
	else {
		if (remove(path)) {
			DestroyLocalFileInfo(info);
			return -1;
		}
	}
	DestroyLocalFileInfo(info);
	return 0;

}
