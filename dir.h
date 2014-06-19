#ifndef _PCS_SHELL_DIR_H_
#define _PCS_SHELL_DIR_H_

#define DEFAULT_MKDIR_ACCESS	0750	/*����Ŀ¼ʱĬ��Ȩ�ޣ�ֻ�з�Windowsϵͳʹ��*/

#define MKDIR_OK				0		/*Ŀ¼�����ɹ�*/
#define MKDIR_FAIL				1		/*Ŀ¼����ʧ��*/
#define MKDIR_TARGET_IS_FILE	2		/*���ش��ڣ������ļ�*/


typedef struct LocalFileInfo LocalFileInfo;

struct LocalFileInfo
{
   char				*path;
   const char		*filename;
   int				isdir;
   time_t			mtime;
   size_t			size;
   LocalFileInfo	*parent;
   LocalFileInfo	*next;

   void				*userdata;
};

/*�ͷŵ�LocalFileInfo����*/
void DestroyLocalFileInfo(LocalFileInfo *info);

/*�ͷŵ�LocalFileInfo����*/
void DestroyLocalFileInfoLink(LocalFileInfo *link);

/*
* ��ȡָ���ļ���Ŀ¼����Ϣ
*  file  - �ļ���Ŀ¼��·��
* ����ļ���Ŀ¼�����ڣ��򷵻�NULL�����򷵻���LocalFileInfo����
*/
LocalFileInfo *GetLocalFileInfo(const char *file);

/*
* ��ȡdirĿ¼�µ������ļ���Ŀ¼��
*   pLink     - ���ڽ��������ָ�롣
*   dir       - Ŀ��Ŀ¼·��
*   recursion - �Ƿ�ݹ��г�
*   on        - ����ȡ��һ�������Ļص�����
*      �ص�����������
*       info   - ��ǰ��ȡ�����ļ�
*       parent - ��ǰ��ȡ�����ļ��ĸ�Ŀ¼
*       state  - ״ֵ̬�����û�����
*   state     - Ҫ���ݵ��ص�����������������ֵ
* �ɹ��󷵻��ļ���Ŀ¼��������ʧ�ܷ��ظ���
*/
int GetDirectoryFiles(LocalFileInfo **pLink, const char *dir, int recursive,
	void(*on)(LocalFileInfo *info, LocalFileInfo *parent, void *state), void *state);

/*�����ļ�������޸�ʱ�䡣
���ִ�гɹ��򷵻�0��
���򷵻ط�0ֵ��*/
int SetFileLastModifyTime(const char *file, time_t mtime);

/*
* �ݹ鴴��Ŀ¼
*    path - ��������Ŀ¼
* ����ɹ��򷵻�MKDIR_OK�����򷵻ش����룬���ܴ�������: MKDIR_FAIL, MKDIR_TARGET_IS_FILE��
*/
int CreateDirectoryRecursive(const char *path);



#endif
