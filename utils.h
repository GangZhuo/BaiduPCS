#ifndef _PCS_SHELL_UTILS_H
#define _PCS_SHELL_UTILS_H

/*
* 从标准输入中读入字符到 str 中，直到遇到回车或者达到 size 限定。
* 字符回显。
*   str  - 输入的字符串将填充到 str 中
*   size - 最多输入 size 个字节。
* 当达到 size 限定后，函数会设置 str[size - 1] = '\0'。
*/
void std_string(char *str, int size);

/*
* 从标准输入中读入字符到 password 中，直到遇到回车或者达到 size 限定。
* 字符不回显。
*   password  - 输入的密码将填充到 password 中
*   size      - 最多输入size个字节。
* 当达到 size 限定后，函数会设置 str[size - 1] = '\0'。
*/
void std_password(char *password, int size);

/*
 * 返回path是否是一个绝对路径。
 * 对于Windows系统： 满足正则: ^[a-zA-Z]\:
 * 对于非Windows系统： 满足正则：^[~|/]
*/
int is_absolute_path(const char *path);

/** detecting whether base is starts with str
*/
int startsWith(char* base, char* str);

/** detecting whether base is ends with str
*/
int endsWith(char* base, char* str);

/*
* 判断两个字符串是否相等。
*  s1    - 以'\0'为结束标记的字符串
*  s2    - 待比较的字符串
*  s2len - s2字符串的字节长度。如果传入-1的话，则使用'\0'作为其结束标记
* 如果相等，则返回1，否则返回0。
*/
int streq(const char *s1, const char *s2, int s2len);

/*
* 判断arr数组中是否存在字符串str，如果存在则返回其标号（标号为 [索引] + 1），否则返回0。
* 比较时区分大小写。
* arr  - 存储很多字符串的数组，数组最后一个元素必须为NULL。
* str  - 判断是否存在的字符串
* len  - 字符串长度。 如果传入-1，则'\0'作为其结束标记。
*/
int str_in_array(const char **arr, const char *str, int len);

/*
* 合并路径，如果filename传入的是绝对路径，则直接返回filename的拷贝。
*  编译时，如果定义WIN32，则使用'\\'作为目录分隔符，否则使用'/'作为目录分隔符。
*   base     - 基目录
*   basesz   - base的字节长度，传入-1的话，将使用strlen()函数读取。
*   filename - 文件名字
* 使用完后，需调用pcs_free来释放返回值
*/
char *combin_path(const char *base, int basesz, const char *filename);

/*
* 合并网盘路径，如果filename传入的是绝对路径，则直接返回filename的拷贝。
* 该函数会使用'/'替换所有'\\'。
* 使用完后，需调用pcs_free来释放返回值
*/
char *combin_net_disk_path(const char *base, const char *filename);

/*
* 修正路径为*nix格式。
* 即把路径中'\\'替换为'/'。
* 修正完成后，原样返回path指针
*/
char *fix_unix_path(char *path);

/*
* 修正路径。
* 即把路径中斜杠替换为反斜杠。
* 修正完成后，原样返回path
*/
char *fix_win_path(char *path);

/*
* 读取文件全部内容
* file    - 待读取的文件
* pBuffer - 文件的内容所在的内存指针将存入pBuffer指定的内存中
* 返回读取到的字节大小。使用完成后，需调用pcs_free(*pBuffer)
*/
int read_file(const char *file, char **pBuffer);

/*
从程序路径中找到文件名开始的位置，
返回开始位置的指针
*/
const char *filename(const char *path);

/*
* 获取路径的父路径，如果没有父路径则返回NULL。
*   path  - 当前路径
*   len   - path的字节长度，如果传入-1，则内部使用strlen()获取其长度
* 返回值需要调用pcs_free()
*/
char *base_dir(const char *path, int len);

/*
string to time_t
时间格式 2009-3-24 0:00:08 或 2009-3-24
*/
int str2time(const char *str, time_t *timeData);

/*
time_t to string 时间格式 2009-3-24 0:00:08
*/
char *time2str(char *buf, const time_t *t);


#endif
