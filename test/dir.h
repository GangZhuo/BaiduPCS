#ifndef _DIR_H_
#define _DIR_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct my_dirent my_dirent;

struct my_dirent
{
   char		 *path; /* File name */
   char		 *filename;
   int		 is_dir; /* File type */
   time_t	 mtime;
   size_t	 size;
   int		user_flag;
   my_dirent *next;
};

int get_file_ent(my_dirent **pEnt, const char *path);
int is_dir_or_file(const char *path);
my_dirent *list_dir(const char *path, int recursion);
void my_dirent_destroy(my_dirent *link);
time_t my_dirent_get_mtime(my_dirent *ent);

int my_dirent_remove(const char *path);

int my_dirent_utime(const char *path, time_t mtime);

#ifdef __cplusplus
}
#endif



#endif
