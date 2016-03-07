#ifndef _PCS_SHELL_SHELL_H
#define _PCS_SHELL_SHELL_H

#include <time.h>
#include "pcs/pcs.h"

#define SORT_DIRECTION_ASC	0 /*正序*/
#define SORT_DIRECTION_DESC 1 /*倒序*/


/* shell 的上下文 */
typedef struct ShellContext
{
	char		*contextfile; /*上下文文件的路径*/
	char		*cookiefile; /*Cookie文件路径*/
	char		*captchafile; /*验证码图片路径*/
	char		*workdir; /*当前工作目录*/
	Pcs			pcs;

	int			list_page_size; /*执行list命令时，每页大小*/
	char		*list_sort_name; /*执行list命令时，排序字段，可选值：name|time|size*/
	char		*list_sort_direction; /*执行list命令时，排序字段，可选值：asc|desc*/

	char		*secure_method; /*加密方法，可选值：plaintext|aes-cbc-128|aes-cbc-192|aes-cbc-256*/
	char		*secure_key;    /*加密时的KEY*/
	int			secure_enable;  /*是否启用加密*/

	int			timeout_retry;  /*是否启用超时后重试*/
	int			max_thread; /*指定最大线程数量*/
	int			max_speed_per_thread; /*指定单个线程的最多下载速度*/
	int			max_upload_speed_per_thread; /*指定单个线程的最大上传速度*/
} ShellContext;

#endif
