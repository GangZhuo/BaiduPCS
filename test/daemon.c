#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#if defined(WIN32)
#  include <direct.h>
#  define mkdir _mkdir
#endif

#include "../pcs/cJSON.h"
#include "../pcs/pcs.h"
#include "logger.h"

#define METHOD_UPDATE	1
#define METHOD_BACKUP	2
#define METHOD_RESTORE	3

#define PRINT_FATAL(...)	{ if (run_in_deamon) { log_write(LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__);   } else { printf("[FATAL]   \x7"); printf(__VA_ARGS__); printf("\n"); } }
#define PRINT_WARNING(...)  { if (run_in_deamon) { log_write(LOG_WARNING, __FILE__, __LINE__, __VA_ARGS__); } else { printf("[WARNING] ");    printf(__VA_ARGS__); printf("\n"); } }
#define PRINT_MONITOR(...)  { if (run_in_deamon) { log_write(LOG_MONITOR, __FILE__, __LINE__, __VA_ARGS__); } else { printf("[MONITOR] ");    printf(__VA_ARGS__); printf("\n"); } }
#define PRINT_NOTICE(...)	{ if (run_in_deamon) { log_write(LOG_NOTICE, __FILE__, __LINE__, __VA_ARGS__);  } else { printf("[NOTICE]  ");    printf(__VA_ARGS__); printf("\n"); } }
#define PRINT_TRACE(...)	{ if (run_in_deamon) { log_write(LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__);   } else { printf("[TRACE]   ");    printf(__VA_ARGS__); printf("\n"); } }
#define PRINT_DEBUG(...)	{ if (run_in_deamon) { log_write(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__);   } else { printf("[DEBUG]   ");    printf(__VA_ARGS__); printf("\n"); } }

typedef struct BackupItem {
	char	*localPath;
	char	*remotePath;
	int		method;
} BackupItem;

typedef struct Config {
	char		*configFile; /*配置文件路径*/
	char		*cookie; /*从配置文件中读入的Cookie配置项，即使用的cookie文件路径*/
	BackupItem	*items; /*需要备份|还原的项*/
	int			itemCount;


} Config;

static int run_in_deamon = 0;

/*返回日志文件的路径*/
static const char *log_file_path()
{

#ifdef WIN32
	static char filename[1024] = { 0 };
	strcpy(filename, getenv("UserProfile"));
	strcat(filename, "\\.baidupcs");
	mkdir(filename);
	strcat(filename, "\\");
	strcat(filename, "pcs.log");
	return filename;
#else
	return "/var/log/pcs.log";
#endif

}

static void d_mem_printf(const char *fmt, ...)
{
    va_list ap;

	if (run_in_deamon) {
		va_start(ap, fmt);  
		log_writev(LOG_WARNING, __FILE__, __LINE__, fmt, ap);
		va_end(ap);   
	}
	else {
		va_start(ap, fmt);  
		vprintf(fmt, ap);
		va_end(ap);
	}
}

/*读取文件内容*/
static int readFileContent(const char *file, char **pBuffer)
{
	FILE *fp;
	long int save_pos;
    long size_of_file;
	char *content;

	fp = fopen(file, "rb");
	if (!fp) {
		PRINT_FATAL("Open file fail: %s", file);
		return -1;
	}
	save_pos = ftell(fp);
	fseek(fp, 0L, SEEK_END);
	size_of_file = ftell(fp);
	if (size_of_file < 3) {
		PRINT_FATAL("Wrong file size: Size=%d, %s", size_of_file, file);
		fclose(fp);
		return -1;
	}
	fseek(fp, save_pos, SEEK_SET);
	content = (char *)pcs_malloc(size_of_file + 1);
	save_pos = fread(content, 1, size_of_file, fp);
	fclose(fp);
	content[size_of_file] = '\0';
	if ((((int)content[0]) & 0xEF) == 0xEF) {
		if ((((int)content[1]) & 0xBB) == 0xBB) {
			if ((((int)content[2]) & 0xBF) == 0xBF) {
				content[0] = content[1] = content[2] = ' ';
			}
			else {
				PRINT_FATAL("Wrong UTF-8 BOM: BOM=0x%2X%2X%2X %s", content[0] & 0xFF, content[1] & 0xFF, content[2] & 0xFF, file);
				return -1;
			}
		}
		else {
			PRINT_FATAL("Wrong UTF-8 BOM: BOM=0x%2X%2X%2X %s", content[0] & 0xFF, content[1] & 0xFF, content[2] & 0xFF, file);
			return -1;
		}
	}
	*pBuffer = content;
	return size_of_file;
}

/*读取并解析配置文件*/
static int readConfigFile(Config *config)
{
    long size_of_file;
	char *content = NULL;
	cJSON *json = NULL, *items, *item, *value;

	size_of_file = readFileContent(config->configFile, &content);
	if (size_of_file == -1) goto error;
	json = cJSON_Parse(content);
	pcs_free(content); content = NULL;
	if (!json) { PRINT_FATAL("Broken JSON string %s", config->configFile); goto error; }
	item = cJSON_GetObjectItem(json, "cookie");
	if (!item) { PRINT_FATAL("No \"cookie\" option (%s)", config->configFile); goto error; }
	config->cookie = pcs_utils_strdup(item->valuestring);
	items = cJSON_GetObjectItem(json, "items");
	if (!items) { PRINT_FATAL("No \"items\" option (%s)", config->configFile); goto error; }
	config->itemCount = cJSON_GetArraySize(items);
	if (config->itemCount > 0) {
		int i;
		config->items = (BackupItem *)pcs_malloc(sizeof(BackupItem) * config->itemCount);
		memset (config->items, 0, sizeof(BackupItem) * config->itemCount);
		for (i = 0; i < config->itemCount; i++) {
			item = cJSON_GetArrayItem(items, i);
			value = cJSON_GetObjectItem(item, "localPath");
			if (!value) { PRINT_FATAL("No \"localPath\" option in items[%d] (%s)", i, config->configFile); goto error; }
			config->items[i].localPath = pcs_utils_strdup(value->valuestring);
			value = cJSON_GetObjectItem(item, "remotePath");
			if (!value) { PRINT_FATAL("No \"remotePath\" option in items[%d] (%s)", i, config->configFile); goto error; }
			config->items[i].remotePath = pcs_utils_strdup(value->valuestring);
			value = cJSON_GetObjectItem(item, "method");
			if (!value) { PRINT_FATAL("No \"method\" option in items[%d] (%s)", i, config->configFile); goto error; }
			if (strcmp(value->valuestring, "update") == 0)
				config->items[i].method = METHOD_UPDATE;
			else if (strcmp(value->valuestring, "backup") == 0)
				config->items[i].method = METHOD_BACKUP;
			else if (strcmp(value->valuestring, "restore") == 0)
				config->items[i].method = METHOD_RESTORE;
			else { PRINT_FATAL("Wrong \"method\" option in items[%d], the method should be one of [\"update\", \"backup\", \"restore\"] (%s)", i, config->configFile); goto error; }
		}
	}
	cJSON_Delete(json);
	return 0;
error:
	if (content) pcs_free(content);
	if (json) cJSON_Delete(json);
	return -1;
}

static int method_update(Config *config, BackupItem *item, Pcs pcs)
{
	PcsRes pcsres;
	if ((pcsres = pcs_islogin(pcs)) != PCS_LOGIN) {
		pcs_destroy(pcs);
		PRINT_FATAL("Not login or session time out");
		return -1;
	}
	PRINT_NOTICE("UID: %s", pcs_sysUID(pcs));

	return 0;
}

static int startup(Config *config)
{
	int i;
	PcsRes pcsres;
	Pcs pcs = NULL;

	/* 创建一个Pcs对象 */
	pcs = pcs_create(config->cookie);

	if ((pcsres = pcs_islogin(pcs)) != PCS_LOGIN) {
		pcs_destroy(pcs);
		PRINT_FATAL("Not login or session time out");
		return -1;
	}
	PRINT_NOTICE("UID: %s", pcs_sysUID(pcs));

	if (config->itemCount == 0) return -1;

	for(i = 0; i < config->itemCount; i++) {
		switch (config->items[i].method)
		{
		case METHOD_UPDATE:
			break;
		case METHOD_BACKUP:
			break;
		case METHOD_RESTORE:
			break;
		}
	}
	pcs_destroy(pcs);
	return 0;
error:
	if (pcs) pcs_destroy(pcs);

	return -1;
}

int daemon(int argc, char *argv[])
{
	Config config = { 0 };
	int res, len;
	char *p, *src, *dst;
	_pcs_mem_printf = &d_mem_printf;

	/*检查传入参数 - 开始*/
	if (argc == 3 && pcs_utils_streq(argv[1], "daemon", 6) && pcs_utils_streq(argv[2], "--config=", 9)) {
		run_in_deamon = 1;
		log_open(log_file_path());
		p = argv[2];
	}
	else if (argc == 2 && pcs_utils_streq(argv[1], "--config=", 9)) {
		run_in_deamon = 0;
		p = argv[1];
	}
	else {
		PRINT_FATAL("Wrong arguments!\n    Usage: pcs --config=<config file>\n    Sample: pcs --config=/etc/pcs/default.json");
		goto exit;
	}
	/*检查传入参数 - 结束*/

	PRINT_NOTICE("Application start up");

	/*获取配置文件路径 - 开始*/
	p += 9;
	if (*p == '"') p++;
	len = strlen(p);
	config.configFile = (char *)pcs_malloc(len + 1);
	memset(config.configFile, 0, len + 1);
	src = p; dst = config.configFile;
	while (*src) {
		*dst = *src;
		src++;
		if (*dst == '"') {
			if (*src == '\0') {
				*dst = '\0';
				break;
			}
			else {
				PRINT_FATAL("Wrong path of the config file.");
				goto exit;
			}
		}
		dst++;
	}
	/*获取配置文件路径 - 结束*/

	if (readConfigFile(&config)) {
		PRINT_FATAL("Read config file error. %s", config.configFile);
		goto exit;
	}

	startup(&config);

exit:
	if (config.configFile) pcs_free(config.configFile);
	if (config.cookie) pcs_free(config.cookie);
	if (config.items) {
		int ii;
		for(ii = 0; ii < config.itemCount; ii++) {
			if (config.items[ii].localPath) pcs_free(config.items[ii].localPath);
			if (config.items[ii].remotePath) pcs_free(config.items[ii].remotePath);
		}
		pcs_free(config.items);
	}
	memset(&config, 0, sizeof(Config));
	PRINT_NOTICE("Application end up");
	pcs_mem_print_leak();
	return 0;
}
