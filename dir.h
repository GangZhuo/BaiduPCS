#ifndef _PCS_SHELL_DIR_H_
#define _PCS_SHELL_DIR_H_

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

/*
 * 获取指定文件或目录的my_dirent对象
 *  pEnt  - 用于接收文件的my_dirent对象。可传入NULL，表示不接收my_dirent对象
 *  path  - 文件或目录的路径
 * 如果文件或目录不存在，则返回0；如果目标是目录则返回2；否则返回1。
 */
int get_file_ent(my_dirent **pEnt, const char *path);

/*
* 判断本地文件是否存在，如果存在的话，判断是否文件还是目录。
* 如果不存在，则返回0；如果是目录则返回2；否则返回1。
*/
int is_dir_or_file(const char *path);

/*
 * 列出path指定目录下的所有文件或目录。
 *   path      - 指定目标目录
 *   recursion - 指定是否递归列出目录
 * 成功后返回一个my_dirent链表
*/
my_dirent *list_dir(const char *path, int recursion);

/*释放掉my_dirent链表*/
void my_dirent_destroy(my_dirent *link);

/*返回ent指定对象的最后修改时间。
该函数会判断ent->mtime是否已经设值，
如果已经设值，则直接返回，否则调用系统函数获取其最后修改时间。*/
time_t my_dirent_get_mtime(my_dirent *ent);

/*递归删除path指定的文件或目录。如果执行成功则返回0，否则返回非0值。*/
int my_dirent_remove(const char *path);

/*设置目标的最后修改时间。如果执行成功则返回0，否则返回非0值。*/
int my_dirent_utime(const char *path, time_t mtime);

#ifdef __cplusplus
}
#endif



#endif
