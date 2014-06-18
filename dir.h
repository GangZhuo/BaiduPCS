#ifndef _PCS_SHELL_DIR_H_
#define _PCS_SHELL_DIR_H_

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
*   dir       - Ŀ��Ŀ¼·��
*   recursion - �Ƿ�ݹ��г�
*   on        - ����ȡ��һ�������Ļص�����
*      �ص�����������
*       info   - ��ǰ��ȡ�����ļ�
*       parent - ��ǰ��ȡ�����ļ��ĸ�Ŀ¼
*       state  - ״ֵ̬�����û�����
*   state     - Ҫ���ݵ��ص�����������������ֵ
* �ɹ��󷵻�һ��LocalFileInfo����
*/
LocalFileInfo *GetDirectoryFiles(const char *dir, int recursive, 
	void(*on)(LocalFileInfo *info, LocalFileInfo *parent, void *state), void *state);

/*�����ļ�������޸�ʱ�䡣
���ִ�гɹ��򷵻�0��
���򷵻ط�0ֵ��*/
int SetFileLastModifyTime(const char *file, time_t mtime);

#endif
