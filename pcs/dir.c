
#ifdef WIN32

#include "pcs_mem.h"

#include <io.h>
#include <malloc.h>
#include <Windows.h>
#include <sys/utime.h>

#include "dir.h"

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

my_dirent *create_dirent(const char *basepath, const char *filename, int is_dir, time_t mtime)
{
	my_dirent *ent;
	ent = (my_dirent *)pcs_malloc(sizeof(my_dirent));
	if (!ent)
		return NULL;
	ent->path = (char *)pcs_malloc(strlen(basepath) + strlen(filename) + 2);
	if (!ent->path) {
		pcs_free(ent);
		return NULL;
	}
	strcpy(ent->path, basepath);
	strcat(ent->path, "\\");
	strcat(ent->path, filename);
	ent->filename = ent->path + strlen(basepath) + 1;
	ent->is_dir = is_dir;
	ent->mtime = mtime;
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
			ent = create_dirent(path, filefind.name, 1, filefind.time_write);
			if (!ent)
				return -1;
			cusor->next = ent;
			cusor = ent;
			if (recursion) {
				if (filesearch(ent->path, cusor, recursion))
					return -1;
			}
		}
		else {
			ent = create_dirent(path, filefind.name, 0, filefind.time_write);
			if (!ent)
				return -1;
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
#include <sys/utime.h>
#include <alloca.h>
#include <unistd.h>
#include <dirent.h>

#include "pcs_mem.h"
#include "dir.h"

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

my_dirent *create_dirent(const char *basepath, const char *filename, int is_dir, time_t mtime)
{
	my_dirent *ent;
	ent = (my_dirent *)pcs_malloc(sizeof(my_dirent));
	if (!ent)
		return NULL;
	ent->path = (char *)pcs_malloc(strlen(basepath) + strlen(filename) + 2);
	if (!ent->path) {
		pcs_free(ent);
		return NULL;
	}
	strcpy(ent->path, basepath);
	strcat(ent->path, "/");
	strcat(ent->path, filename);
	ent->filename = ent->path + strlen(basepath) + 1;
	ent->is_dir = is_dir;
	ent->mtime = mtime;
	ent->user_flag = 0;
	ent->next = NULL;
	return ent;
}

int filesearch(const char *path, my_dirent *root, int recursion)
{
    struct dirent* ent = NULL;
    DIR* pDir;
	my_dirent *cusor, *p;

	pDir = opendir(path);
	if (pDir == NULL)
		return -1;
	cusor = root;
    while((ent = readdir(pDir)) != NULL) {
        if (ent->d_type == 4) {
			if(!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, ".."))
				continue;
			p = create_dirent(path, ent->d_name, 1, 0);
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
			p = create_dirent(path, ent->d_name, 0, 0);
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
