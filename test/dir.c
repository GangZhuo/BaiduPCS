
#ifdef WIN32

#include "../pcs/pcs_mem.h"

#include <io.h>
#include <malloc.h>
#include <direct.h>
#include <Windows.h>
#include <sys/utime.h>

#include "dir.h"

my_dirent *create_dirent(const char *basepath, const char *filename, int is_dir, time_t mtime, size_t size);

time_t FileTimeToTime_t(FILETIME ft, time_t *t)  
{  
	ULONGLONG  ll;
	ULARGE_INTEGER ui;
	time_t rc;
	ui.LowPart =  ft.dwLowDateTime;
	ui.HighPart =  ft.dwHighDateTime;
	ll = (((ULONGLONG)ft.dwHighDateTime) << 32) + ft.dwLowDateTime;
	rc = ((ULONGLONG)(ui.QuadPart - 116444736000000000) / 10000000);
	if (t) *t = rc;
	return rc;
}

int get_file_ent(my_dirent **pEnt, const char *path)
{
	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile(path, &fd);
	FindClose(hFind);
	//不存在同名的文件或文件夹
	if (hFind == INVALID_HANDLE_VALUE) {
		return 0;
	}
	else if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
		//判断是否为目录
		if (pEnt) *pEnt = create_dirent(path, NULL, 1, FileTimeToTime_t(fd.ftLastWriteTime, NULL), 0);
		return 2;
	}
	else {
		//判断为文件
		if (pEnt) *pEnt = create_dirent(path, NULL, 0, FileTimeToTime_t(fd.ftLastWriteTime, NULL), fd.nFileSizeLow);
		return 1;
	}
}

int is_dir_or_file(const char *path)
{
	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile(path, &fd);
	FindClose(hFind);
	//不存在同名的文件或文件夹
	if (hFind == INVALID_HANDLE_VALUE) {
		return 0;
	}
	else if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
		//判断是否为目录
		return 2;
	}
	else {
		//判断为文件
		return 1;
	}
}

void my_dirent_destroy(my_dirent *link)
{
	my_dirent *cusor, *ent;
	cusor = link;
	while(cusor) {
		ent = cusor;
		cusor = cusor->next;
		if (ent->path)
			pcs_free(ent->path);
		pcs_free(ent);
	}
}

my_dirent *create_dirent(const char *basepath, const char *filename, int is_dir, time_t mtime, size_t size)
{
	my_dirent *ent;
	ent = (my_dirent *)pcs_malloc(sizeof(my_dirent));
	if (!ent)
		return NULL;
	ent->path = (char *)pcs_malloc(strlen(basepath) + (filename ? strlen(filename) : 0) + 2);
	if (!ent->path) {
		pcs_free(ent);
		return NULL;
	}
	strcpy(ent->path, basepath);
	if (filename) {
		strcat(ent->path, "\\");
		strcat(ent->path, filename);
		ent->filename = ent->path + strlen(basepath) + 1;
	}
	else {
		char *p = ent->path + strlen(basepath);
		while (p >= ent->path) {
			if (*p == '\\' || *p == '/') {
				p++;
				break;
			}
			p--;
		}
		ent->filename = p;
	}
	ent->is_dir = is_dir;
	ent->mtime = mtime;
	ent->size = size;
	ent->user_flag = 0;
	ent->next = NULL;
	return ent;
}

int filesearch(const char *path, my_dirent *root, int recursion)
{
	struct _finddata_t filefind;
	char *curr;
	int done = 0, handle;
	my_dirent *cusor, *ent;

	curr = (char *)alloca(strlen(path) + 5);
	if (!curr)
		return -1;
	strcpy(curr, path);
	strcat(curr, "\\*.*");
	if((handle = _findfirst(curr, &filefind)) == -1)
		return -1;
	cusor = root;
	while(!(done = _findnext(handle, &filefind))) {
		if(!strcmp(filefind.name, ".."))
			continue;
		if ((_A_SUBDIR == filefind.attrib)) {
			ent = create_dirent(path, filefind.name, 1, filefind.time_write, 0);
			if (!ent) {
				_findclose(handle);
				return -1;
			}
			cusor->next = ent;
			cusor = ent;
			if (recursion) {
				if (filesearch(ent->path, cusor, recursion)) {
					_findclose(handle);
					return -1;
				}
			}
		}
		else {
			ent = create_dirent(path, filefind.name, 0, filefind.time_write, filefind.size);
			if (!ent) {
				_findclose(handle);
				return -1;
			}
			cusor->next = ent;
			cusor = ent;
		}
	}    
	_findclose(handle);
	return 0;
}

my_dirent *list_dir(const char *path, int recursion)
{
	my_dirent root;
	memset(&root, 0, sizeof(my_dirent));
	if (filesearch(path, &root, recursion)) {
		my_dirent_destroy(root.next);
	}
	return root.next;
}

time_t my_dirent_get_mtime(my_dirent *ent)
{
	return ent->mtime;
}

int my_dirent_remove(const char *path) 
{
	my_dirent root, *i;
	int res = 0;
	int ft = is_dir_or_file(path);
	memset(&root, 0, sizeof(my_dirent));
	if (ft == 2) {
		if (filesearch(path, &root, 0)) {
			res = -1;
		}
		else {
			i = root.next;
			while(i) {
				if (my_dirent_remove(i->path)) {
					res = -1;
					break;
				}
				i = i->next;
			}
		}
		my_dirent_destroy(root.next);
		if (!i) {
			res = _rmdir(path);
		}
	}
	else {
		res = remove(path);
	}
	return res;

}

int my_dirent_utime(const char *path, time_t mtime)
{
	struct _utimbuf ut;
	ut.actime = mtime;
	ut.modtime = mtime;
	return _utime(path, &ut);
}

#else

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <alloca.h>
#include <unistd.h>
#include <dirent.h>
#ifdef HAVE_UTIME_H
#    include <utime.h>
#else
#  ifdef HAVE_SYS_UTIME_H
#    include <sys/utime.h>
#  else
	  struct utimbuf {
        time_t actime;
        time_t modtime;
	  };
#  endif
#endif
#include "../pcs/pcs_mem.h"
#include "dir.h"

my_dirent *create_dirent(const char *basepath, const char *filename, int is_dir, time_t mtime, size_t size);

int get_file_ent(my_dirent **pEnt, const char *path)
{
	struct stat st;
	stat(path, &st);
	if (S_ISDIR(st.st_mode)) {
		//为目录
		if (pEnt) *pEnt = create_dirent(path, NULL, 1, st.st_mtime, 0);
		return 2;
	}
	else if (S_ISREG(st.st_mode)) {
		//为文件
		if (pEnt) *pEnt = create_dirent(path, NULL, 0, st.st_mtime, st.st_size);
		return 1;
	}
	return 0;
}

int is_dir_or_file(const char *path)
{
	struct stat st;
	stat(path, &st);
	if (S_ISDIR(st.st_mode))
		return 2;
	else if (S_ISREG(st.st_mode))
		return 1;
	return 0;
}

void my_dirent_destroy(my_dirent *link)
{
	my_dirent *cusor, *ent;
	cusor = link;
	while(cusor) {
		ent = cusor;
		cusor = cusor->next;
		if (ent->path)
			pcs_free(ent->path);
		pcs_free(ent);
	}
}

my_dirent *create_dirent(const char *basepath, const char *filename, int is_dir, time_t mtime, size_t size)
{
	my_dirent *ent;
	ent = (my_dirent *)pcs_malloc(sizeof(my_dirent));
	if (!ent)
		return NULL;
	ent->path = (char *)pcs_malloc(strlen(basepath) + (filename ? strlen(filename) : 0) + 2);
	if (!ent->path) {
		pcs_free(ent);
		return NULL;
	}
	strcpy(ent->path, basepath);
	if (filename) {
		strcat(ent->path, "/");
		strcat(ent->path, filename);
		ent->filename = ent->path + strlen(basepath) + 1;
	}
	else {
		char *p = ent->path + strlen(basepath);
		while (p >= ent->path) {
			if (*p == '/' || *p == '\\') {
				p++;
				break;
			}
			p--;
		}
		ent->filename = p;
	}
	ent->is_dir = is_dir;
	ent->mtime = mtime;
	ent->size = size;
	ent->user_flag = 0;
	ent->next = NULL;
	return ent;
}

int filesearch(const char *path, my_dirent *root, int recursion)
{
    struct dirent* ent = NULL;
    DIR* pDir;
	my_dirent *cusor, *p;
	struct stat st;

	pDir = opendir(path);
	if (pDir == NULL)
		return -1;
	cusor = root;
    while((ent = readdir(pDir)) != NULL) {
        if (ent->d_type == 4) {
			if(!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, ".."))
				continue;
			stat(path, &st);
			p = create_dirent(path, ent->d_name, 1, st.st_mtime, 0);
            if (!ent)
				return -1;
			cusor->next = p;
			cusor = p;
			if (recursion) {
				if (filesearch(p->path, cusor, recursion))
					return -1;
			}
        }
        else if (ent->d_type == 8){
			stat(path, &st);
			p = create_dirent(path, ent->d_name, 0, st.st_mtime, st.st_size);
            if (!p)
				return -1;
			cusor->next = p;
			cusor = p;
        }
    }    
    closedir(pDir);
	return 0;
}

my_dirent *list_dir(const char *path, int recursion)
{
	my_dirent root;
	memset(&root, 0, sizeof(my_dirent));
	if (filesearch(path, &root, recursion)) {
		my_dirent_destroy(root.next);
	}
	return root.next;
}

time_t my_dirent_get_mtime(my_dirent *ent)
{
	struct stat st;
	if (ent->mtime)
		return ent->mtime;
	if (stat(ent->path, &st))
		return 0;
	ent->mtime = st.st_mtime;
	return ent->mtime;
}


int my_dirent_remove(const char *path) 
{
	my_dirent root, *i;
	int res = 0;
	int ft = is_dir_or_file(path);
	memset(&root, 0, sizeof(my_dirent));
	if (ft == 2) {
		if (filesearch(path, &root, 0)) {
			res = -1;
		}
		else {
			i = root.next;
			while(i) {
				if (my_dirent_remove(i->path)) {
					res = -1;
					break;
				}
				i = i->next;
			}
		}
		my_dirent_destroy(root.next);
		if (!i) {
			res = remove(path);
		}
	}
	else {
		res = remove(path);
	}
	return res;

}

int my_dirent_utime(const char *path, time_t mtime)
{
	struct utimbuf ut;
	ut.actime = mtime;
	ut.modtime = mtime;
	return utime(path, &ut);
}

#endif
