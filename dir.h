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
 * ��ȡָ���ļ���Ŀ¼��my_dirent����
 *  pEnt  - ���ڽ����ļ���my_dirent���󡣿ɴ���NULL����ʾ������my_dirent����
 *  path  - �ļ���Ŀ¼��·��
 * ����ļ���Ŀ¼�����ڣ��򷵻�0�����Ŀ����Ŀ¼�򷵻�2�����򷵻�1��
 */
int get_file_ent(my_dirent **pEnt, const char *path);

/*
* �жϱ����ļ��Ƿ���ڣ�������ڵĻ����ж��Ƿ��ļ�����Ŀ¼��
* ��������ڣ��򷵻�0�������Ŀ¼�򷵻�2�����򷵻�1��
*/
int is_dir_or_file(const char *path);

/*
 * �г�pathָ��Ŀ¼�µ������ļ���Ŀ¼��
 *   path      - ָ��Ŀ��Ŀ¼
 *   recursion - ָ���Ƿ�ݹ��г�Ŀ¼
 * �ɹ��󷵻�һ��my_dirent����
*/
my_dirent *list_dir(const char *path, int recursion);

/*�ͷŵ�my_dirent����*/
void my_dirent_destroy(my_dirent *link);

/*����entָ�����������޸�ʱ�䡣
�ú������ж�ent->mtime�Ƿ��Ѿ���ֵ��
����Ѿ���ֵ����ֱ�ӷ��أ��������ϵͳ������ȡ������޸�ʱ�䡣*/
time_t my_dirent_get_mtime(my_dirent *ent);

/*�ݹ�ɾ��pathָ�����ļ���Ŀ¼�����ִ�гɹ��򷵻�0�����򷵻ط�0ֵ��*/
int my_dirent_remove(const char *path);

/*����Ŀ�������޸�ʱ�䡣���ִ�гɹ��򷵻�0�����򷵻ط�0ֵ��*/
int my_dirent_utime(const char *path, time_t mtime);

#ifdef __cplusplus
}
#endif



#endif
