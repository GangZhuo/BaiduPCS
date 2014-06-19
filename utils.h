#ifndef _PCS_SHELL_UTILS_H
#define _PCS_SHELL_UTILS_H

/*
* �ӱ�׼�����ж����ַ��� str �У�ֱ�������س����ߴﵽ size �޶���
* �ַ����ԡ�
*   str  - ������ַ�������䵽 str ��
*   size - ������� size ���ֽڡ�
* ���ﵽ size �޶��󣬺��������� str[size - 1] = '\0'��
*/
void std_string(char *str, int size);

/*
* �ӱ�׼�����ж����ַ��� password �У�ֱ�������س����ߴﵽ size �޶���
* �ַ������ԡ�
*   password  - ��������뽫��䵽 password ��
*   size      - �������size���ֽڡ�
* ���ﵽ size �޶��󣬺��������� str[size - 1] = '\0'��
*/
void std_password(char *password, int size);

/*
 * ����path�Ƿ���һ������·����
 * ����Windowsϵͳ�� ��������: ^[a-zA-Z]\:
 * ���ڷ�Windowsϵͳ�� ��������^[~|/]
*/
int is_absolute_path(const char *path);


/*
* �ж������ַ����Ƿ���ȡ�
*  s1    - ��'\0'Ϊ������ǵ��ַ���
*  s2    - ���Ƚϵ��ַ���
*  s2len - s2�ַ������ֽڳ��ȡ��������-1�Ļ�����ʹ��'\0'��Ϊ��������
* �����ȣ��򷵻�1�����򷵻�0��
*/
int streq(const char *s1, const char *s2, int s2len);

/*
* �ж�arr�������Ƿ�����ַ���str����������򷵻����ţ����Ϊ [����] + 1�������򷵻�0��
* �Ƚ�ʱ���ִ�Сд��
* arr  - �洢�ܶ��ַ��������飬�������һ��Ԫ�ر���ΪNULL��
* str  - �ж��Ƿ���ڵ��ַ���
* len  - �ַ������ȡ� �������-1����'\0'��Ϊ�������ǡ�
*/
int str_in_array(const char **arr, const char *str, int len);

/*
* �ϲ�·�������filename������Ǿ���·������ֱ�ӷ���filename�Ŀ�����
*  ����ʱ���������WIN32����ʹ��'\\'��ΪĿ¼�ָ���������ʹ��'/'��ΪĿ¼�ָ�����
*   base     - ��Ŀ¼
*   basesz   - base���ֽڳ��ȣ�����-1�Ļ�����ʹ��strlen()������ȡ��
*   filename - �ļ�����
* ʹ����������pcs_free���ͷŷ���ֵ
*/
char *combin_path(const char *base, int basesz, const char *filename);

/*
* �ϲ�unix��ʽ��·�������filename������Ǿ���·������ֱ�ӷ���filename�Ŀ�����
* �ú�����ʹ��'/'�滻����'\\'��
* ʹ����������pcs_free���ͷŷ���ֵ
*/
char *combin_unix_path(const char *base, const char *filename);

/*
* ����·��Ϊ*nix��ʽ��
* ����·����'\\'�滻Ϊ'/'��
* ������ɺ�ԭ������pathָ��
*/
char *fix_unix_path(char *path);

/*
* ��ȡ�ļ�ȫ������
* file    - ����ȡ���ļ�
* pBuffer - �ļ����������ڵ��ڴ�ָ�뽫����pBufferָ�����ڴ���
* ���ض�ȡ�����ֽڴ�С��ʹ����ɺ������pcs_free(*pBuffer)
*/
int read_file(const char *file, char **pBuffer);

/*
�ӳ���·�����ҵ��ļ�����ʼ��λ�ã�
���ؿ�ʼλ�õ�ָ��
*/
const char *filename(char *path);

/*
* ��ȡ·���ĸ�·�������û�и�·���򷵻�NULL��
*   path  - ��ǰ·��
*   len   - path���ֽڳ��ȣ��������-1�����ڲ�ʹ��strlen()��ȡ�䳤��
* ����ֵ��Ҫ����pcs_free()
*/
char *base_dir(const char *path, int len);

/*
string to time_t
ʱ���ʽ 2009-3-24 0:00:08 �� 2009-3-24
*/
int str2time(const char *str, time_t *timeData);

/*
time_t to string ʱ���ʽ 2009-3-24 0:00:08
*/
char *time2str(char *buf, const time_t *t);


#endif
