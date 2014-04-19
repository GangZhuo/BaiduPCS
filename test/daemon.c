#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <time.h>
#if defined(WIN32)
#  include <direct.h>
#  define mkdir _mkdir
#  define D_SLEEP(ms) Sleep(ms)
#else
#  include <unistd.h>
#  define D_SLEEP(ms) sleep(ms)
#endif

#include "../sqlite/sqlite3.h"

#include "../pcs/cJSON.h"
#include "../pcs/pcs.h"
#include "logger.h"

#define APP_NAME		(run_in_daemon ? "pcs daemon" : "pcs normal")

#define METHOD_UPDATE	1
#define METHOD_BACKUP	2
#define METHOD_RESTORE	3

#define ACTION_UPDATE	"update"
#define ACTION_BACKUP	"backup"
#define ACTION_RESTORE	"restore"

#define ACTION_STATUS_RUNNING	1
#define ACTION_STATUS_FINISHED	2
#define ACTION_STATUS_ERROR		3

#define PRINT_FATAL(...)	{ if (log_enabled) { log_write(LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__);   } if (printf_enabled) { printf("[FATAL]   ");    printf(__VA_ARGS__); printf("\n"); } }
#define PRINT_WARNING(...)  { if (log_enabled) { log_write(LOG_WARNING, __FILE__, __LINE__, __VA_ARGS__); } if (printf_enabled) { printf("[WARNING] ");    printf(__VA_ARGS__); printf("\n"); } }
#define PRINT_MONITOR(...)  { if (log_enabled) { log_write(LOG_MONITOR, __FILE__, __LINE__, __VA_ARGS__); } if (printf_enabled) { printf("[MONITOR] ");    printf(__VA_ARGS__); printf("\n"); } }
#define PRINT_NOTICE(...)	{ if (log_enabled) { log_write(LOG_NOTICE, __FILE__, __LINE__, __VA_ARGS__);  } if (printf_enabled) { printf("[NOTICE]  ");    printf(__VA_ARGS__); printf("\n"); } }
#define PRINT_TRACE(...)	{ if (log_enabled) { log_write(LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__);   } if (printf_enabled) { printf("[TRACE]   ");    printf(__VA_ARGS__); printf("\n"); } }
#define PRINT_DEBUG(...)	{ if (log_enabled) { log_write(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__);   } if (printf_enabled) { printf("[DEBUG]   ");    printf(__VA_ARGS__); printf("\n"); } }

#define TABLE_NAME_CACHE		"pcs_cache"
#define TABLE_NAME_ACTION		"pcs_action"
#define TABLE_CACHE_CREATOR		"CREATE TABLE [pcs_cache] (" \
								"  [server_fs_id]			INTEGER," \
								"  [server_path]			NTEXT," \
								"  [server_filename]		NTEXT," \
								"  [server_ctime]			INTEGER," \
								"  [server_mtime]			INTEGER," \
								"  [server_size]			INTEGER," \
								"  [server_category]		INTEGER," \
								"  [server_isdir]			INTEGER," \
								"  [server_dir_empty]		INTEGER," \
								"  [server_empty]			INTEGER," \
								"  [server_md5]				NTEXT," \
								"  [server_dlink]			NTEXT," \
								"  [server_if_has_sub_dir]	INTEGER," \
								"  [ctime]					INTEGER," \
								"  [mtime]					INTEGER," \
								"  [capp]					NVARCHAR," \
								"  [mapp]					NVARCHAR)"
#define TABLE_CACHE_INDEX_CREATOR "CREATE INDEX [ix_pcs_cache_path] ON [pcs_cache] ([server_path])"

#define TABLE_NAME_OP			"pcs_op"
#define TABLE_ACTION_CREATOR	"CREATE TABLE [pcs_action] (" \
								"  [action]					NVARCHAR, " \
								"  [status]					INTEGER, " \
								"  [start_time]				INTEGER, " \
								"  [end_time]				INTEGER, " \
								"  [capp]					NVARCHAR, " \
								"  [mapp]					NVARCHAR)"
#define TABLE_ACTION_INDEX_CREATOR "CREATE INDEX [ix_pcs_action_action] ON [pcs_action] ([action])"

#define SQL_TABLE_EXISTS		"SELECT name FROM sqlite_master WHERE type = 'table' AND name=?"

#define SQL_ACTION_EXISTS		"SELECT status FROM pcs_action WHERE action=?1"
#define SQL_ACTION_SELECT		"SELECT status FROM pcs_action WHERE action=?1"
#define SQL_ACTION_INSERT		"INSERT INTO pcs_action (action,status,start_time,end_time,capp,mapp) VALUES (?1,?2,?3,?4,?5,?5)"
#define SQL_ACTION_UPDATE		"UPDATE pcs_action SET status=?2,end_time=?3,mapp=?4 WHERE action=?1"
#define SQL_ACTION_DELETE		"DELETE FROM pcs_action WHERE action=?1"

#define SQL_CACHE_EXISTS		"SELECT server_path FROM pcs_cache WHERE server_path=?1"
#define SQL_CACHE_INSERT		"INSERT INTO pcs_cache (server_fs_id, server_path, server_filename, server_ctime, server_mtime, " \
								"server_size, server_category, server_isdir, server_dir_empty, server_empty, "\
								"server_md5, server_dlink, server_if_has_sub_dir, ctime, mtime, capp, mapp) " \
								"VALUES (?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8, ?9, ?10, ?11, ?12, ?13, ?14, ?14, ?15, ?15)"
#define SQL_CACHE_UPDATE		"UPDATE pcs_cache SET server_fs_id=?1, server_filename=?3, server_ctime=?4, server_mtime=?5, " \
								"server_size=?6, server_category=?7, server_isdir=?8, server_dir_empty=?9, server_empty=?10, "\
								"server_md5=?11, server_dlink=?12, server_if_has_sub_dir=?13, mtime=?14, mapp=?15 " \
								"WHERE server_path=?2"
#define SQL_CACHE_DELETE		"DELETE FROM pcs_cache WHERE server_path=?1"
#define SQL_CACHE_CLEAR			"DELETE FROM pcs_cache WHERE server_path LIKE ?1"
#define SQL_CACHE_CLEAR_ALL		"DELETE FROM pcs_cache"

typedef struct BackupItem {
	char	*localPath;
	char	*remotePath;
	int		method;
	int		schedule;  /*几点几分开始执行任务，值为一天从零点零分零秒开始到执行时间的秒数。例：6小时 = 6 * 60 * 60 = 21600秒，此处设置值为21600则表示，于凌晨6点开启任务。 */
	int		interval;  /*任务执行完成后，当前执行时间加上interval则为下次执行时间。86400秒 = 24小时，即：当凌晨6点执行完任务后，加上24小时的时间作为任务下次执行时间。*/
	time_t	next_run_time; /* 下次执行时间 */
} BackupItem;

typedef struct Config {
	char		*configFilePath; /*配置文件路径*/
	char		*cookieFilePath; /*从配置文件中读入的Cookie配置项，即使用的cookie文件路径*/
	char		*cacheFilePath; /*缓存文件的路径*/
	char		*logFilePath; /*日志文件的路径*/
	BackupItem	*items; /*需要备份|还原的项*/
	int			itemCount;


} Config;

typedef struct DbPrepare {
	sqlite3_stmt **stmts;
	int			 count;
} DbPrepare;

typedef struct DbPrepareList {
	sqlite3_stmt *stmt;
	struct DbPrepareList *next;
} DbPrepareList;

static Config config = { 0 };
static int run_in_daemon = 0;
static int log_enabled = 0;
static int printf_enabled = 0;
static sqlite3 *db = NULL;
static Pcs pcs = NULL;

/*返回数据库文件的路径*/
//static const char *db_file_path()
//{
//
//#ifdef WIN32
//	static char filename[1024] = { 0 };
//	strcpy(filename, getenv("UserProfile"));
//	strcat(filename, "\\.baidupcs");
//	mkdir(filename);
//	strcat(filename, "\\");
//	strcat(filename, "cache.db");
//	return filename;
//#else
//	strcpy(filename, "/var/baidupcs");
//	mkdir(filename, 0777);
//	strcat(filename, "/");
//	strcat(filename, "cache.db");
//#endif
//
//}
//
/*返回日志文件的路径*/
static const char *log_file_path()
{

#ifdef WIN32
	static char filename[1024] = { 0 };
	strcpy(filename, getenv("UserProfile"));
	strcat(filename, "\\.baidupcs");
	mkdir(filename);
	strcat(filename, "\\");
	strcat(filename, "baidupcs.log");
	return filename;
#else
	return "/var/log/baidupcs.log";
#endif

}

static void d_mem_printf(const char *fmt, ...)
{
    va_list ap;

	if (log_enabled) {
		va_start(ap, fmt);  
		log_writev(LOG_WARNING, __FILE__, __LINE__, fmt, ap);
		va_end(ap);   
	}
	if (printf_enabled) {
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
static int readConfigFile()
{
    long size_of_file;
	char *content = NULL;
	cJSON *json = NULL, *items, *item, *value;

	size_of_file = readFileContent(config.configFilePath, &content);
	if (size_of_file == -1) goto error;
	json = cJSON_Parse(content);
	pcs_free(content); content = NULL;
	if (!json) { PRINT_FATAL("Broken JSON string %s", config.configFilePath); goto error; }
	item = cJSON_GetObjectItem(json, "cookieFilePath");
	if (!item) { PRINT_FATAL("No \"cookieFilePath\" option (%s)", config.configFilePath); goto error; }
	config.cookieFilePath = pcs_utils_strdup(item->valuestring);
	item = cJSON_GetObjectItem(json, "cacheFilePath");
	if (!item) { PRINT_FATAL("No \"cacheFilePath\" option (%s)", config.configFilePath); goto error; }
	config.cacheFilePath = pcs_utils_strdup(item->valuestring);
	item = cJSON_GetObjectItem(json, "logFilePath");
	//if (!item) { PRINT_FATAL("No \"logFilePath\" option (%s)", config.configFilePath); goto error; }
	//config.logFilePath = pcs_utils_strdup(item->valuestring);
	if (item && item->valuestring[0])
		config.logFilePath = pcs_utils_strdup(item->valuestring);
	else
		config.logFilePath = NULL;
	items = cJSON_GetObjectItem(json, "items");
	if (!items) { PRINT_FATAL("No \"items\" option (%s)", config.configFilePath); goto error; }
	config.itemCount = cJSON_GetArraySize(items);
	if (config.itemCount > 0) {
		int i;
		config.items = (BackupItem *)pcs_malloc(sizeof(BackupItem) * config.itemCount);
		memset (config.items, 0, sizeof(BackupItem) * config.itemCount);
		for (i = 0; i < config.itemCount; i++) {
			item = cJSON_GetArrayItem(items, i);
			value = cJSON_GetObjectItem(item, "localPath");
			if (!value) { PRINT_FATAL("No \"localPath\" option in items[%d] (%s)", i, config.configFilePath); goto error; }
			config.items[i].localPath = pcs_utils_strdup(value->valuestring);
			value = cJSON_GetObjectItem(item, "remotePath");
			if (!value) { PRINT_FATAL("No \"remotePath\" option in items[%d] (%s)", i, config.configFilePath); goto error; }
			config.items[i].remotePath = pcs_utils_strdup(value->valuestring);
			value = cJSON_GetObjectItem(item, "method");
			if (!value) { PRINT_FATAL("No \"method\" option in items[%d] (%s)", i, config.configFilePath); goto error; }
			if (strcmp(value->valuestring, "update") == 0)
				config.items[i].method = METHOD_UPDATE;
			else if (strcmp(value->valuestring, "backup") == 0)
				config.items[i].method = METHOD_BACKUP;
			else if (strcmp(value->valuestring, "restore") == 0)
				config.items[i].method = METHOD_RESTORE;
			else { PRINT_FATAL("Wrong \"method\" option in items[%d], the method should be one of [\"update\", \"backup\", \"restore\"] (%s)", i, config.configFilePath); goto error; }
			value = cJSON_GetObjectItem(item, "schedule");
			if (!value) { PRINT_FATAL("No \"schedule\" option in items[%d] (%s)", i, config.configFilePath); goto error; }
			config.items[i].schedule = value->valueint;
			value = cJSON_GetObjectItem(item, "interval");
			if (!value) { PRINT_FATAL("No \"interval\" option in items[%d] (%s)", i, config.configFilePath); goto error; }
			config.items[i].interval = value->valueint;
		}
	}
	cJSON_Delete(json);
	return 0;
error:
	if (content) pcs_free(content);
	if (json) cJSON_Delete(json);
	return -1;
}

static void init_schedule()
{
	int i;
	time_t now;
	struct tm* ptm;
	time(&now);
	ptm = localtime(&now);  
	ptm->tm_hour = 0;
	ptm->tm_min = 0;
	ptm->tm_sec = 0;
	now = mktime(ptm);
	for(i = 0; i < config.itemCount; i++) {
		config.items[i].next_run_time = config.items[i].schedule + now;
	}
}

static int db_open()
{
	int rc;
	sqlite3_stmt *stmt = NULL;
	rc = sqlite3_open(config.cacheFilePath, &db);
	if(rc) {
		PRINT_FATAL("Can't open database(%s): %s", config.cacheFilePath, sqlite3_errmsg(db));
		return -1;
	}
	rc = sqlite3_prepare_v2(db, SQL_TABLE_EXISTS, -1, &stmt, NULL);
	if (rc) {
		PRINT_FATAL("Can't build the sql %s: %s", SQL_TABLE_EXISTS, sqlite3_errmsg(db));
		return -1;
	}
	rc = sqlite3_bind_text(stmt, 1, TABLE_NAME_CACHE, -1, SQLITE_STATIC);
	if (rc) {
		PRINT_FATAL("Can't bind the text into the statement %s: %s", SQL_TABLE_EXISTS, sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		return -1;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE) {
		PRINT_FATAL("Can't execute the statement %s: %s", SQL_TABLE_EXISTS, sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		return -1;
	}
	if (rc == SQLITE_DONE) {
		rc = sqlite3_exec(db, TABLE_CACHE_CREATOR, NULL, NULL, NULL);
		if (rc) {
			PRINT_FATAL("Can't create the table %s: %s", TABLE_NAME_CACHE, sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			return -1;
		}
		sqlite3_exec(db, TABLE_CACHE_INDEX_CREATOR, NULL, NULL, NULL);
	}

	sqlite3_reset(stmt);
	rc = sqlite3_bind_text(stmt, 1, TABLE_NAME_ACTION, -1, SQLITE_STATIC);
	if (rc) {
		PRINT_FATAL("Can't bind the text into the statement %s: %s", SQL_TABLE_EXISTS, sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		return -1;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE) {
		PRINT_FATAL("Can't execute the statement %s: %s", SQL_TABLE_EXISTS, sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		return -1;
	}
	if (rc == SQLITE_DONE) {
		rc = sqlite3_exec(db, TABLE_ACTION_CREATOR, NULL, NULL, NULL);
		if (rc) {
			PRINT_FATAL("Can't create the table %s: %s", TABLE_NAME_ACTION, sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			return -1;
		}
		sqlite3_exec(db, TABLE_ACTION_INDEX_CREATOR, NULL, NULL, NULL);
	}

	sqlite3_finalize(stmt);
	return 0;
error:
	return -1;
}

static int db_vprepare(DbPrepare *pre, va_list ap)
{
	const char *sql;
	int cnt = 0;
	sqlite3_stmt *stmt = NULL;
	DbPrepareList *root = NULL, *last = NULL, *node;
	while((sql = va_arg(ap, const char *)) != NULL) {
		if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL)) {
			PRINT_FATAL("Can't build the sql %s: %s", sql, sqlite3_errmsg(db));
			goto error;
		}
		node = (DbPrepareList *)pcs_malloc(sizeof(DbPrepareList));
		node->stmt = stmt;
		node->next = NULL;
		stmt = NULL;
		if (last) {
			last->next = node;
			last = node;
		}
		else {
			root = last = node;
		}
		cnt++;
	}
	if (cnt > 0) {
		int i = 0;
		pre->count = cnt;
		pre->stmts = (sqlite3_stmt **)pcs_malloc(sizeof(sqlite3_stmt *) * cnt);
		while(root){
			node = root;
			pre->stmts[i++] = node->stmt;
			root = root->next;
			pcs_free(node);
		}
		return 0;
	}
error:
	while(root){
		node = root;
		root = root->next;
		if (node->stmt) sqlite3_finalize(node->stmt);
		pcs_free(node);
	}
	return -1;
}

static int db_prepare(DbPrepare *pre, ...)
{
	int rc;
    va_list args;
    va_start(args, pre);
	rc = db_vprepare(pre, args);
    va_end(args);
	return rc;
}

static void db_prepare_destroy(DbPrepare *pre)
{
	if (pre->count > 0) {
		int i;
		for (i = 0; i < pre->count; i++) {
			if (pre->stmts[i]) sqlite3_finalize(pre->stmts[i]);
		}
		pcs_free(pre->stmts);
		pre->stmts = NULL;
		pre->count = 0;
	}
}

static int db_set_action(const char *action, int status)
{
	int rc;
	sqlite3_stmt *stmt = NULL;
	time_t now;
	rc = sqlite3_prepare_v2(db, SQL_ACTION_EXISTS, -1, &stmt, NULL);
	if (rc) {
		PRINT_FATAL("Can't build the sql %s: %s", SQL_ACTION_EXISTS, sqlite3_errmsg(db));
		return -1;
	}
	rc = sqlite3_bind_text(stmt, 1, action, -1, SQLITE_STATIC);
	if (rc) {
		PRINT_FATAL("Can't bind the text into the statement %s: %s", SQL_ACTION_EXISTS, sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		return -1;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE) {
		PRINT_FATAL("Can't execute the statement %s: %s", SQL_ACTION_EXISTS, sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		return -1;
	}
	sqlite3_finalize(stmt);

	if (rc == SQLITE_DONE) {
		rc = sqlite3_prepare_v2(db, SQL_ACTION_INSERT, -1, &stmt, NULL);
		if (rc) {
			PRINT_FATAL("Can't build the sql %s: %s", SQL_ACTION_INSERT, sqlite3_errmsg(db));
			return -1;
		}
		time(&now);
		sqlite3_bind_text(stmt, 1, action, -1, SQLITE_STATIC);
		sqlite3_bind_int(stmt, 2, status);
		sqlite3_bind_int64(stmt, 3, now);
		sqlite3_bind_int64(stmt, 4, now);
		sqlite3_bind_text(stmt, 5, APP_NAME, -1, SQLITE_STATIC);
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_ROW && rc != SQLITE_DONE) {
			PRINT_FATAL("Can't execute the statement %s: %s", SQL_ACTION_INSERT, sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			return -1;
		}
		sqlite3_finalize(stmt);
	}
	else {
		rc = sqlite3_prepare_v2(db, SQL_ACTION_UPDATE, -1, &stmt, NULL);
		if (rc) {
			PRINT_FATAL("Can't build the sql %s: %s", SQL_ACTION_UPDATE, sqlite3_errmsg(db));
			return -1;
		}
		time(&now);
		sqlite3_bind_text(stmt, 1, action, -1, SQLITE_STATIC);
		sqlite3_bind_int(stmt, 2, status);
		sqlite3_bind_int64(stmt, 3, now);
		sqlite3_bind_text(stmt, 4, APP_NAME, -1, SQLITE_STATIC);
		rc = sqlite3_step(stmt);
		if (rc != SQLITE_ROW && rc != SQLITE_DONE) {
			PRINT_FATAL("Can't execute the statement %s: %s", SQL_ACTION_UPDATE, sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			return -1;
		}
		sqlite3_finalize(stmt);
	}
	return 0;
}

static int db_remove_action(const char *action)
{
	int rc;
	sqlite3_stmt *stmt = NULL;
	rc = sqlite3_prepare_v2(db, SQL_ACTION_DELETE, -1, &stmt, NULL);
	if (rc) {
		PRINT_FATAL("Can't build the sql %s: %s", SQL_ACTION_DELETE, sqlite3_errmsg(db));
		return -1;
	}
	rc = sqlite3_bind_text(stmt, 1, action, -1, SQLITE_STATIC);
	if (rc) {
		PRINT_FATAL("Can't bind the text into the statement %s: %s", SQL_ACTION_DELETE, sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		return -1;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE) {
		PRINT_FATAL("Can't execute the statement %s: %s", SQL_ACTION_DELETE, sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		return -1;
	}
	sqlite3_finalize(stmt);
	return 0;
}

static int db_get_action_status(const char *action)
{
	int status;
	int rc;
	sqlite3_stmt *stmt = NULL;
	time_t now;
	rc = sqlite3_prepare_v2(db, SQL_ACTION_SELECT, -1, &stmt, NULL);
	if (rc) {
		PRINT_FATAL("Can't build the sql %s: %s", SQL_ACTION_SELECT, sqlite3_errmsg(db));
		return -1;
	}
	rc = sqlite3_bind_text(stmt, 1, action, -1, SQLITE_STATIC);
	if (rc) {
		PRINT_FATAL("Can't bind the text into the statement %s: %s", SQL_ACTION_SELECT, sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		return -1;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW) {
		PRINT_FATAL("Can't execute the statement %s: %s", SQL_ACTION_SELECT, sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		return -1;
	}
	status = sqlite3_column_int(stmt, 0);
	sqlite3_finalize(stmt);
	return status;
}

static int db_clear_cache(const char *path)
{
	int rc;
	sqlite3_stmt *stmt = NULL;
	int sz;
	char *val;
	rc = sqlite3_prepare_v2(db, SQL_CACHE_CLEAR, -1, &stmt, NULL);
	if (rc) {
		PRINT_FATAL("Can't build the sql %s: %s", SQL_CACHE_CLEAR, sqlite3_errmsg(db));
		return -1;
	}
	sz = strlen(path);
	val = (char *)pcs_malloc(sz + 3);
	memcpy(val, path, sz + 1);
	if (val[sz - 1] != '/') {
		val[sz] = '/'; val[sz + 1] = '%'; val[sz + 2] = '\0';
		sz += 2;
	}
	else {
		val[sz] = '%'; val[sz + 1] = '\0';
		sz++;
	}
	rc = sqlite3_bind_text(stmt, 1, val, sz, SQLITE_STATIC);
	if (rc) {
		PRINT_FATAL("Can't bind the text into the statement %s: %s", SQL_CACHE_CLEAR, sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		pcs_free(val);
		return -1;
	}
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE) {
		PRINT_FATAL("Can't execute the statement %s: %s", SQL_CACHE_CLEAR, sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		pcs_free(val);
		return -1;
	}
	sqlite3_finalize(stmt);
	pcs_free(val);
	return 0;
}

static int db_clear_all_cache()
{
	if (sqlite3_exec(db, SQL_CACHE_CLEAR_ALL, NULL, NULL, NULL)) {
		PRINT_FATAL("Can't clear all cache %s: %s", SQL_CACHE_CLEAR_ALL, sqlite3_errmsg(db));
		return -1;
	}
	return 0;
}

static int db_add_cache(PcsFileInfo *info, DbPrepare *pre)
{
	time_t now;
	int rc;
	sqlite3_stmt *stmt;
	time(&now);
	stmt = pre->stmts[1];
	sqlite3_bind_int64(stmt,  1, info->fs_id);
	sqlite3_bind_text(stmt,  2, info->path, -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt,  3, info->server_filename, -1, SQLITE_STATIC);
	sqlite3_bind_int64(stmt,  4, info->server_ctime);
	sqlite3_bind_int64(stmt,  5, info->server_mtime);
	sqlite3_bind_int64(stmt,  6, info->size);
	sqlite3_bind_int(stmt,  7, info->category);
	sqlite3_bind_int(stmt,  8, info->isdir);
	sqlite3_bind_int(stmt,  9, info->dir_empty);
	sqlite3_bind_int(stmt, 10, info->empty);
	sqlite3_bind_text(stmt, 11, info->md5, -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 12, info->dlink, -1, SQLITE_STATIC);
	sqlite3_bind_int(stmt, 13, info->ifhassubdir);
	sqlite3_bind_int64(stmt, 14, now);
	sqlite3_bind_text(stmt, 15, APP_NAME, -1, SQLITE_STATIC);

	rc = sqlite3_step(stmt);
	if (rc != SQLITE_ROW && rc != SQLITE_DONE) {
		PRINT_FATAL("Can't insert into the cache: %s", sqlite3_errmsg(db));
		sqlite3_reset(stmt);
		return -1;
	}
	sqlite3_reset(stmt);
	return 0;
}

static int quota(UInt64 *usedByteSize, UInt64 *totalByteSize)
{
	PcsRes pcsres;
	UInt64 total, used;
	pcsres = pcs_quota(pcs, &total, &used);
	if (pcsres != PCS_OK) {
		PRINT_WARNING("Get quota failed: %s", pcs_strerror(pcs, pcsres));
		return -1;
	}
	*usedByteSize = used;
	*totalByteSize = total;
	return 0;
}

/* 返回直接或间接文件数量 */
static int method_update_folder(const char *path, DbPrepare *pre)
{
	PcsFileInfoList *list = NULL;
	PcsFileInfoListIterater iterater;
	PcsFileInfo *info = NULL;
	int page_index = 1,
		page_size = 100;
	int cnt = 0, /*当前页文件数量*/
		cnt_total = 0, /*直接间接文件数量*/
		direct = 0, /*直接文件数量*/
		sub_total;
	while(1) {
		list = pcs_list(pcs, path, page_index, page_size, "name", PcsFalse);
		if (!list) {
			if (pcs_strerror(pcs, PCS_NONE)) {
				PRINT_FATAL("Cannot list the server directory %s: %s", path, pcs_strerror(pcs, PCS_NONE));
				return -1;
			}
			else {
				break;
			}
		}
		cnt = list->count;
		cnt_total += cnt;
		direct += cnt;

		pcs_filist_iterater_init(list, &iterater);
		while(pcs_filist_iterater_next(&iterater)) {
			info = iterater.current;
			if (db_add_cache(info, pre)) {
				pcs_filist_destroy(list);
				return -1;
			}
			if (info->isdir) {
				if ((sub_total = method_update_folder(info->path, pre)) < 0) {
					pcs_filist_destroy(list);
					return -1;
				}
				else {
					cnt_total += sub_total;
				}
			}
		}
		pcs_filist_destroy(list);
		if (cnt < page_size) {
			break;
		}
		page_index++;
	}
	PRINT_NOTICE("The server directory(%s) have %d direct files(or directories), have %d direct and indirect files(or directories)", path, direct, cnt_total);
	return cnt_total;
}

static int method_update(int itemIndex)
{
	BackupItem *item = &config.items[itemIndex];
	DbPrepare pre = { 0 };
	PRINT_NOTICE("Update meta data - Start");
	PRINT_NOTICE("Server Path: %s", item->remotePath);
	if (pcs_islogin(pcs) != PCS_LOGIN) {
		PRINT_FATAL("Not login or session time out");
		goto error;
	}
	PRINT_NOTICE("UID: %s", pcs_sysUID(pcs));
	if (db_prepare(&pre, SQL_CACHE_EXISTS, SQL_CACHE_INSERT, SQL_CACHE_UPDATE, SQL_CACHE_DELETE, NULL)) goto error;
	if (db_set_action(ACTION_UPDATE, ACTION_STATUS_RUNNING)) goto error;
	if (db_clear_cache(item->remotePath)) goto error;
	if (method_update_folder(item->remotePath, &pre) < 0) { db_set_action(ACTION_UPDATE, ACTION_STATUS_ERROR); goto error; }
	db_set_action(ACTION_UPDATE, ACTION_STATUS_FINISHED);
	db_prepare_destroy(&pre);
	PRINT_NOTICE("Update meta data - End");
	return 0;
error:
	db_prepare_destroy(&pre);
	PRINT_NOTICE("Update meta data - End");
	return -1;
}

static int task(int itemIndex)
{
	switch (config.items[itemIndex].method)
	{
	case METHOD_UPDATE:
		return method_update(itemIndex);
	case METHOD_BACKUP:
		break;
	case METHOD_RESTORE:
		break;
	}
	return 0;
}

static int run()
{
	int i;
	time_t now;
	if (run_in_daemon) {
		init_schedule();
		while(run_in_daemon) {
			time(&now);
			for(i = 0; i < config.itemCount; i++) {
				if (now >= config.items[i].next_run_time) {
					task(i);
					config.items[i].next_run_time += config.items[i].interval;
				}
			}
			D_SLEEP(1000);
		}
	}
	else {
		for(i = 0; i < config.itemCount; i++) {
			if (task(i)) {
				return -1;
			}
		}
	}
	return 0;
}

static int startup()
{
	PcsRes pcsres;
	UInt64 usedByteSize, totalByteSize;

	/* 创建一个Pcs对象 */
	pcs = pcs_create(config.cookieFilePath);

	if ((pcsres = pcs_islogin(pcs)) != PCS_LOGIN) {
		PRINT_FATAL("Not login or session time out");
		return -1;
	}
	PRINT_NOTICE("UID: %s", pcs_sysUID(pcs));
	if (!quota(&usedByteSize, &totalByteSize)) {
		char strTotal[32] = {0}, strUsed[32] = {0};
		pcs_utils_readable_size((double)usedByteSize, strUsed, 30, NULL);
		pcs_utils_readable_size((double)totalByteSize, strTotal, 30, NULL);
		PRINT_NOTICE("Quota: %s/%s", strUsed, strTotal);
		if (totalByteSize - usedByteSize < 10 * 1024 * 1024) {
			PRINT_FATAL("The remaining space is insufficient 10M");
			return -1;
		}
	}
	if (config.itemCount == 0) return -1;
	return run();
}

int run_daemon(int argc, char *argv[])
{
	int res, len;
	char *p, *src, *dst;

	/*解析参数 - 开始*/
	if (argc == 3 && pcs_utils_streq(argv[1], "daemon", 6) && pcs_utils_streq(argv[2], "--config=", 9)) {
		p = argv[2];
	}
	else if (argc == 2 && pcs_utils_streq(argv[1], "--config=", 9)) {
		p = argv[1];
	}
	else {
		PRINT_FATAL("Wrong arguments!\n    Usage: pcs --config=<config file>\n    Sample: pcs --config=/etc/pcs/default.json");
		goto exit;
	}
	/*解析参数 - 结束*/

	PRINT_NOTICE("Application start up");

	/*获取配置文件路径 - 开始*/
	p += 9;
	if (*p == '"') p++;
	len = strlen(p);
	config.configFilePath = (char *)pcs_malloc(len + 1);
	memset(config.configFilePath, 0, len + 1);
	src = p; dst = config.configFilePath;
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

	if (readConfigFile()) {
		PRINT_FATAL("Read config file error. %s", config.configFilePath);
		goto exit;
	}
	if (config.logFilePath && config.logFilePath[0] && strcmp(config.logFilePath, log_file_path())) {
		PRINT_NOTICE("Log file changed to %s", config.logFilePath);
		log_close();
		log_open(config.logFilePath);
		PRINT_NOTICE("The original log file is %s", log_file_path());
	}

	if (db_open()) goto exit;

	startup();

exit:
	if (config.configFilePath) pcs_free(config.configFilePath);
	if (config.cookieFilePath) pcs_free(config.cookieFilePath);
	if (config.cacheFilePath) pcs_free(config.cacheFilePath);
	if (config.logFilePath) pcs_free(config.logFilePath);
	if (config.items) {
		int ii;
		for(ii = 0; ii < config.itemCount; ii++) {
			if (config.items[ii].localPath) pcs_free(config.items[ii].localPath);
			if (config.items[ii].remotePath) pcs_free(config.items[ii].remotePath);
		}
		pcs_free(config.items);
	}
	memset(&config, 0, sizeof(Config));
	if (db) { sqlite3_close(db); db = NULL; }
	if (pcs) { pcs_destroy(pcs); pcs = NULL; }
	PRINT_NOTICE("Application end up");
	pcs_mem_print_leak();
	return 0;
}

int start_daemon(int argc, char *argv[])
{
	_pcs_mem_printf = &d_mem_printf;

	log_close();
	log_open(log_file_path());

	log_enabled = 1;
	printf_enabled = 1;

	/*检查传入参数 - 开始*/
	if (argc == 3 && pcs_utils_streq(argv[1], "daemon", 6) && pcs_utils_streq(argv[2], "--config=", 9)) {
		run_in_daemon = 1;
		printf_enabled = 0;
	}
	else if (argc == 2 && pcs_utils_streq(argv[1], "--config=", 9)) {
		run_in_daemon = 0;
		log_enabled = 1;
	}
	else {
		PRINT_FATAL("Wrong arguments!\n    Usage: pcs --config=<config file>\n    Sample: pcs --config=/etc/pcs/default.json");
		return -1;
	}
	/*检查传入参数 - 结束*/

	return run_daemon(argc, argv);
}
