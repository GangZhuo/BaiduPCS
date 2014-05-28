#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(WIN32)
# include <argp/argp.h>
#else
# include <argp.h>
#endif

#include "../pcs/pcs_mem.h"
#include "../pcs/pcs_utils.h"
#include "shell_args.h"

#define program_actions "quota|list|rename|delete|download|upload|move|mkdir|echo|cat|search|copy|meta"

#define CONFIG_KEY_LEN		128
#define ARG_MSG_LEN			512

#define print_arg_err(s) printf(s)

const char *argp_program_version = program_full_name;
//const char *argp_program_bug_address = "<bug-gnu-utils@gnu.org>";

#define program_usage program_filename " [option...] <command> [<args>]"

#define OPT_SYNCH	1
#define OPT_COOKIE	2
#define OPT_RC4		3
#define OPT_CONFIG	4
#define OPT_CACHE	5
#define OPT_MD5		6

struct argp_option options[] = {
	{ 0,			0,	 0,						0,							"Options:", 0},
	{ "username",	'u', "<username>",			OPTION_ARG_OPTIONAL,		"Specify the account.", 0},
	{ "password",	'p', "<password>",			OPTION_ARG_OPTIONAL,		"Specify the password.", 0},
	{ "recursion",	'r', 0,						OPTION_ARG_OPTIONAL,		"Recursive execute the command.", 0},
	{ "force",		'f', 0,						OPTION_ARG_OPTIONAL,		"Force delete the files that not exists in source when download or upload.", 0},
	{ "sort",		's', "<field>",
												OPTION_ARG_OPTIONAL,		"Sort the result when list files. ", 0},
	{ "inverte",	'i', 0,						OPTION_ARG_OPTIONAL,		"Inverte the result when list files, if have no -s the -i option will be ignored.", 0},
	{ "append",		'a', 0,						OPTION_ARG_OPTIONAL,		"Append the text at the end of the file instead of override the file when execute echo command.", 0},
	{ 0,			0,	 0,						0,							"More Options:", 0},
	{ "synch",		OPT_SYNCH, 0,				OPTION_ARG_OPTIONAL,		"Synch the tree. "
																			"Delete the files or directories that not exists in the local file system when upload, "
																			"Delete the files or directories that not exists in the net disk when download.", 0},
	{ "urlc",		'U', 0,						OPTION_ARG_OPTIONAL,		"Use http://urlc.cn/g/ to hold the verify code image. "
																			"The default behavior write the image into ~/.baidupcs/ .", 0},
	{ "verbose",	'v', 0,						OPTION_ARG_OPTIONAL,		"Show the response text.", 0},
	{ "cookie",		OPT_COOKIE, "<cookiefile>", OPTION_ARG_OPTIONAL,		"Specify the cookie file.", 0},
	{ "rc4",		OPT_RC4, "<rc4-key>",		OPTION_ARG_OPTIONAL,		"Specify that use rc4 to encode/decode the content.", 0},
	{ "config",     OPT_CONFIG, "<config>",		OPTION_ARG_OPTIONAL,		"Specify the config file.", 0 },
	{ "cache",      OPT_CACHE, "<cache>",		OPTION_ARG_OPTIONAL,		"Specify the cache file.", 0 },
	{ "md5",		OPT_MD5, 0,					OPTION_ARG_OPTIONAL,		"Specify that whether use md5 when execute backup, restore, combin, compare.", 0 },

	{0, 0, 0, 0, 0, 0}
};

static const char args_doc[] = "<command> [<args>]";
static const char doc[] = "\nProvide some commands that can access the baidu network disk.\n"
	"\n"

	"Fields: \n"
	"  name                    Sort by name\n" 
	"  time                    Sort by last modify time\n" 
	"  size                    Sort by size\n"
	"\n"

	"Commands: \n"
	"  quota                    Show the quota\n"
	"  meta                     Show the meta data\n"
	"  list                     List files\n"
	"  rename                   Rename the file\n"
	"  move                     Move the file\n"
	"  copy                     Copy the file\n"
	"  mkdir                    Create a new directory\n"
	"  remove                   Remove the file\n"
	"  cat                      Show the content directly\n"
	"  echo                     Save the text into baidu network disk directly\n"
	"  search                   Search the baidu network disk\n"
	"  download                 Download the file\n"
	"  upload                   Upload the file\n"
	"  svc                      Run in service mode\n"
	"  reset                    Reset status for local cache\n"
	"  update                   Update local cache\n"
	"  backup                   Backup\n"
	"  restore                  Restore\n"
	"  combin                   Combin\n"
	"  compare                  Compare\n"
	"  time                     Print time\n"
	"  ls-op                    List OP\n"

	"\v\n"
	"If you are first use the program, or you session is time out, "
	"you should be use `" program_name " -u<username> quota` to login the disk. "
	"May be need input the verify code when login, there are two ways to view the image. "
	"One is save the image into ~/.baidupcs/verfy_code.gif, this is default behavior. "
	"Another is upload image to http://urlc.cn/g/ , "
	"you can use -U option to use this behavior. "
	"After upload, you can find the url from the response html, "
	"and then you can view the image by browser (i.e.g Firefox, Chrome, IE..), "
	"\n\n"
	"Simples:\n"
	"  Get the quota:\n"
	"      " program_name " -u<username> quota\n"
	"\n"
	"  List the files or directories that is in / folder:\n"
	"      " program_name " list /\n"
	"\n"
	"  Rename the /server_file.txt to new_server_file.txt:\n"
	"      " program_name " rename /server_file.txt new_server_file.txt\n"
	"  The second argument have no slash, if you want move the file into other folder, use command move.\n"
	"\n"
	"  Move the /server_file.txt into /sub_folder/new_name.txt:\n"
	"      " program_name " move /server_file.txt /sub_folder/new_name.txt\n"
	"\n"
	"  Create a new folder:\n"
	"      " program_name " mkdir /new_dir\n"
	"\n"
	"  Remove the file:\n"
	"      " program_name " remove /server_file1.txt\n"
	"\n"
	"  Download file:\n"
	"      " program_name " download /server_file.txt /home/user/download/local_file.txt\n"
	"\n"
	"  Upload file:\n"
	"      " program_name " upload /home/user/documents/local_file.txt /server_file.txt\n"
	"\n"
	"  Show text file content:\n"
	"      " program_name " cat /server_file.txt\n"
	"\n"
	"  Append the text at end of the file:\n"
	"      " program_name " echo -a /server_file.txt \"The text append by echo command.\\n\"\n"
	"\n"
	"  Search file in /temp folder:\n"
	"      " program_name " search /temp file.txt\n"
	"\n"
	"  Copy /temp/file.txt to /temp/2/new_file.txt:\n"
	"      " program_name " copy /temp/file.txt /temp/2/new_file.txt\n"
	"\n"
	;

static inline void free_params_args(struct params *params)
{
	int i;
	for (i = 0; i < params->args_count; i++) {
		if (params->args[i]) pcs_free(params->args[i]);
	}
	if (params->args) pcs_free(params->args);
	params->args = NULL;
	params->args_count = 0;
}

static error_t add_arg(struct params *params, char *arg)
{
	char **p;
	int i;
	p = (char **) pcs_malloc((params->args_count + 1) * sizeof(char *));
	if (!p) {
		print_arg_err("alloc memory failed\n");
		params->is_fail = PcsTrue;
		return EINVAL;
	}
	if (params->args_count > 0) {
		for (i = 0; i < params->args_count; i++) {
			p[i] = params->args[i];
		}
		pcs_free(params->args);
	}
	params->args = p;
	p = &params->args[params->args_count];
	params->args_count++;
	*p = pcs_utils_strdup(arg);
	return 0;
}

PcsBool shell_args_check_params(struct params *params)
{
	PcsBool res = PcsTrue;

	if (params->is_fail) {
		print_arg_err("unknown arguments\n");
		return PcsFalse;
	}

	switch(params->action)
	{
	case ACTION_NONE:
		print_arg_err("no command\n");
		res = PcsFalse;
		break;

	case ACTION_QUOTA:
		if (params->args_count != 0) {
			print_arg_err("usage: " program_name " quota\n");
			res = PcsFalse;
			break;
		}
		break;

	case ACTION_LIST:
		if (params->args_count != 1) {
			print_arg_err("usage: " program_name " list <remote path>\nSample: " program_name " list /backup\n");
			res = PcsFalse;
			break;
		}
		if (params->sort) {
			if (strcmp(params->sort, "name") != 0
				&& strcmp(params->sort, "time") != 0
				&& strcmp(params->sort, "size") != 0) {
				print_arg_err("wrong arguments\n");
				res = PcsFalse;
				break;
			}
		}
		break;

	case ACTION_RENAME:
		if (params->args_count != 2) {
			print_arg_err("usage: " program_name " rename <remote path> <new name>\nSample: " program_name " rename /backup/1.jpg 2.jpg\n");
			res = PcsFalse;
			break;
		}
		break;

	case ACTION_DELETE:
		if (params->args_count <= 0) {
			print_arg_err("usage: " program_name " delete <remote path> ...\nSample: " program_name " delete /backup/1.jpg /backup/2.jpg /backup/3.jpg\n");
			res = PcsFalse;
			break;
		}
		break;

	case ACTION_DOWNLOAD:
		if (params->args_count != 2) {
			print_arg_err("usage: " program_name " download <remote path> <local path>\nSample: " program_name " download -r /backup/upload_backup /var/www/upload\n");
			res = PcsFalse;
			break;
		}
		break;

	case ACTION_UPLOAD:
		if (params->args_count != 2) {
			print_arg_err("usage: " program_name " upload <local path> <local path>\nSample: " program_name " upload -r /var/www/upload /backup/upload_backup\n");
			res = PcsFalse;
			break;
		}
		break;

	case ACTION_MOVE:
		if (params->args_count != 2) {
			print_arg_err("usage: " program_name " move <remote path> <new path>\nSample: " program_name " move /backup/upload_backup /backup/xxx.com/upload_backup\n");
			res = PcsFalse;
			break;
		}
		break;

	case ACTION_MKDIR:
		if (params->args_count != 1) {
			print_arg_err("usage: " program_name " mkdir <remote path>\nSample: " program_name " mkdir /backup/xxx.com\n");
			res = PcsFalse;
			break;
		}
		break;

	case ACTION_ECHO:
		if (params->args_count != 2) {
			print_arg_err("usage: " program_name " echo <remote path> <text>\nSample: " program_name " echo -a /data/note.txt \"The text that append by 'echo' command\"\n");
			res = PcsFalse;
			break;
		}
		break;


	case ACTION_CAT:
		if (params->args_count != 1) {
			print_arg_err("usage: " program_name " cat <remote path>\nSample: " program_name " cat /data/note.txt\n");
			res = PcsFalse;
			break;
		}
		break;

	case ACTION_SEARCH:
		if (params->args_count != 2) {
			print_arg_err("usage: " program_name " search <search folder> <key word>\nSample: " program_name " search -r /backup config\n");
			res = PcsFalse;
			break;
		}
		break;

	case ACTION_COPY:
		if (params->args_count != 2) {
			print_arg_err("usage: " program_name " copy <remote path> <new path>\nSample: " program_name " copy /backup/data /backup/data_20140401\n");
			res = PcsFalse;
			break;
		}
		break;

	case ACTION_META:
		if (params->args_count != 1) {
			print_arg_err("usage: " program_name " meta <remote path>\nSample: " program_name " meta /backup/data/1.jpg\n");
			res = PcsFalse;
			break;
		}
		break;

	case ACTION_SVC:
		if (!params->config || !params->config[0]) {
			print_arg_err("usage: " program_name " svc --config=<config file>\n"
				"Sample: " program_name " svc --config=/etc/pcs/default.json\n");
			res = PcsFalse;
			break;
		}
		break;

	case ACTION_RESET:
		if (params->args_count != 0 || (!params->config && (!params->cache))) {
			print_arg_err("usage: " program_name " reset --cache=<cache file>\n"
				"Sample: " program_name " reset --cache=/etc/pcs/cache.db\n"
				"        " program_name " reset --config=/etc/pcs/default.json\n");
			res = PcsFalse;
			break;
		}
		break;

	case ACTION_UPDATE:
		if (params->args_count != 1 || (!params->config && (!params->cookie || !params->cache))) {
			print_arg_err("usage: " program_name " update --cookie=<cookie file> --cache=<cache file> <remote path>\n"
				"Sample: " program_name " update --cookie=/etc/pcs/default.cookie --cache=/etc/pcs/cache.db /backup/xxx.com\n"
				"        " program_name " update --config=/etc/pcs/default.json /backup/xxx.com\n");
			res = PcsFalse;
			break;
		}
		break;

	case ACTION_BACKUP:
		if (params->args_count != 2 || (!params->config && (!params->cookie || !params->cache))) {
			print_arg_err("usage: " program_name " backup --cookie=<cookie file> --cache=<cache file> <local path> <remote path>\n"
				"Sample: " program_name " backup --cookie=/etc/pcs/default.cookie --cache=/etc/pcs/cache.db /var/www/xxx.com /backup/xxx.com\n"
				"        " program_name " backup --config=/etc/pcs/default.json /var/www/xxx.com /backup/xxx.com\n");
			res = PcsFalse;
			break;
		}
		break;

	case ACTION_RESTORE:
		if (params->args_count != 2 || (!params->config && (!params->cookie || !params->cache))) {
			print_arg_err("usage: " program_name " restore --cookie=<cookie file> --cache=<cache file> <local path> <remote path>\n"
				"Sample: " program_name " restore --cookie=/etc/pcs/default.cookie --cache=/etc/pcs/cache.db /var/www/xxx.com /backup/xxx.com\n"
				"        " program_name " restore --config=/etc/pcs/default.json /var/www/xxx.com /backup/xxx.com\n");
			res = PcsFalse;
			break;
		}
		break;

	case ACTION_COMBIN:
		if (params->args_count != 2 || (!params->config && (!params->cookie || !params->cache))) {
			print_arg_err("usage: " program_name " combin --cookie=<cookie file> --cache=<cache file> <local path> <remote path>\n"
				"Sample: " program_name " combin --cookie=/etc/pcs/default.cookie --cache=/etc/pcs/cache.db /var/www/xxx.com /backup/xxx.com\n"
				"        " program_name " combin --config=/etc/pcs/default.json /var/www/xxx.com /backup/xxx.com\n");
			res = PcsFalse;
			break;
		}
		break;

	case ACTION_COMPARE:
		if (params->args_count != 2 || (!params->config && (!params->cookie || !params->cache))) {
			print_arg_err("usage: " program_name " compare --cookie=<cookie file> --cache=<cache file> <local path> <remote path>\n"
				"Sample: " program_name " compare --cookie=/etc/pcs/default.cookie --cache=/etc/pcs/cache.db /var/www/xxx.com /backup/xxx.com\n"
				"        " program_name " compare --config=/etc/pcs/default.json /var/www/xxx.com /backup/xxx.com\n");
			res = PcsFalse;
			break;
		}
		break;

	case ACTION_TIME:
		if (params->args_count != 0) {
			print_arg_err("usage: " program_name " time\n"
				"Sample: " program_name " time\n");
			res = PcsFalse;
			break;
		}
		break;

	case ACTION_LIST_ACTION:
		if (params->args_count != 0 || (!params->config && (!params->cache))) {
			print_arg_err("usage: " program_name " ls-op --cache=<cache file>\n"
				"Sample: " program_name " ls-op --cache=/etc/pcs/cache.db\n"
				"        " program_name " ls-op --config=/etc/pcs/default.json\n");
			res = PcsFalse;
			break;
		}
		break;

	case ACTION_MD5:
		if (params->args_count != 1) {
			print_arg_err("usage: " program_name " md5 <file>\n"
				"Sample: " program_name " md5 /etc/pcs/cache.db\n");
			res = PcsFalse;
			break;
		}
		break;

	default:
		//print_arg_err("wrong arguments\n");
		res = PcsFalse;
	}
	return res;
}

static error_t parse_opt (int key, char *arg, struct argp_state *state)
{
	struct params *params = (struct params *)state->input;
	int i;

	switch (key) {
	case ARGP_KEY_INIT:
		if (params->username) pcs_free(params->username);
		if (params->password) pcs_free(params->password);
		if (params->sort) pcs_free(params->sort);
		if (params->cookie) pcs_free(params->cookie);
		if (params->rc4_key) pcs_free(params->rc4_key);
		if (params->args) {
			for (i = 0; i < params->args_count; i++) {
				pcs_free(params->args[i]);
			}
			pcs_free(params->args);
		}
		if (params->config) pcs_free(params->config);
		if (params->cache) pcs_free(params->cache);
		params->is_fail = PcsFalse;
		params->username = NULL;
		params->password = NULL;
		params->cookie = NULL;
		params->rc4_key = NULL;
		params->sort = NULL;
		params->is_recursion = PcsFalse;
		params->is_force = PcsFalse;
		params->is_desc = PcsFalse;
		params->use_urlc = PcsFalse;
		params->is_append = PcsFalse;
		params->is_verbose = PcsFalse;
		params->is_synch = PcsFalse;
		params->is_rc4 = PcsFalse;
		params->action = ACTION_NONE;
		params->args = NULL;
		params->args_count = 0;
		params->config = NULL;
		params->cache = NULL;
		params->md5 = PcsFalse;
		break;

	case ARGP_KEY_NO_ARGS:
		break;

	case ARGP_KEY_ARG:
		if (state->arg_num == 0) {
			if (strcmp(arg, "quota") == 0 || strcmp(arg, "info") == 0)
				params->action = ACTION_QUOTA;
			else if (strcmp(arg, "list") == 0 || strcmp(arg, "ls") == 0)
				params->action = ACTION_LIST;
			else if (strcmp(arg, "rename") == 0)
				params->action = ACTION_RENAME;
			else if (strcmp(arg, "move") == 0 || strcmp(arg, "mv") == 0)
				params->action = ACTION_MOVE;
			else if (strcmp(arg, "copy") == 0 || strcmp(arg, "cp") == 0)
				params->action = ACTION_COPY;
			else if (strcmp(arg, "mkdir") == 0)
				params->action = ACTION_MKDIR;
			else if (strcmp(arg, "delete") == 0 || strcmp(arg, "remove") == 0 || strcmp(arg, "rm") == 0)
				params->action = ACTION_DELETE;
			else if (strcmp(arg, "download") == 0 || strcmp(arg, "down") == 0)
				params->action = ACTION_DOWNLOAD;
			else if (strcmp(arg, "upload") == 0 || strcmp(arg, "up") == 0)
				params->action = ACTION_UPLOAD;
			else if (strcmp(arg, "cat") == 0)
				params->action = ACTION_CAT;
			else if (strcmp(arg, "echo") == 0)
				params->action = ACTION_ECHO;
			else if (strcmp(arg, "search") == 0)
				params->action = ACTION_SEARCH;
			else if (strcmp(arg, "meta") == 0)
				params->action = ACTION_META;
			else if (strcmp(arg, "svc") == 0)
				params->action = ACTION_SVC;
			else if (strcmp(arg, "reset") == 0)
				params->action = ACTION_RESET;
			else if (strcmp(arg, "update") == 0)
				params->action = ACTION_UPDATE;
			else if (strcmp(arg, "backup") == 0)
				params->action = ACTION_BACKUP;
			else if (strcmp(arg, "restore") == 0)
				params->action = ACTION_RESTORE;
			else if (strcmp(arg, "combin") == 0)
				params->action = ACTION_COMBIN;
			else if (strcmp(arg, "compare") == 0)
				params->action = ACTION_COMPARE;
			else if (strcmp(arg, "time") == 0)
				params->action = ACTION_TIME;
			else if (strcmp(arg, "ls-op") == 0)
				params->action = ACTION_LIST_ACTION;
			else if (strcmp(arg, "md5") == 0)
				params->action = ACTION_MD5;
			else {
				print_arg_err("unknown command\n");
				return EINVAL;
			}
			break;
		}
		if (add_arg(params, arg) != 0)
			return EINVAL;
		break;

	case 'u':
		if (arg && arg[0]) {
			params->username = pcs_utils_strdup(arg);
		}
		//else {
		//	print_arg_err("Not specify username\n");
		//	return EINVAL;
		//}
		break;

	case 'p':
		if (arg && arg[0]) {
			params->password = pcs_utils_strdup(arg);
		}
		break;

	case 'r':
		params->is_recursion = PcsTrue;
		break;

	case 'f':
		params->is_force = PcsTrue;
		break;

	case 's':
		if (arg) {
			params->sort = pcs_utils_strdup(arg);
		}
		break;

	case 'i':
		params->is_desc = PcsTrue;
		break;

	case 'U':
		params->use_urlc = PcsTrue;
		break;

	case 'a':
		params->is_append = PcsTrue;
		break;

	case 'v':
		params->is_verbose = PcsTrue;
		break;

	case OPT_SYNCH:
		params->is_synch = PcsTrue;
		break;

	case OPT_COOKIE:
		if (arg && arg[0]) {
			params->cookie = pcs_utils_strdup(arg);
		}
		break;

	case OPT_RC4:
		params->is_rc4 = PcsTrue;
		if (arg && arg[0]) {
			params->rc4_key = pcs_utils_strdup(arg);
		}
		break;

	case OPT_CONFIG:
		if (arg && arg[0]) {
			params->config = pcs_utils_strdup(arg);
		}
		break;

	case OPT_CACHE:
		if (arg && arg[0]) {
			params->cache = pcs_utils_strdup(arg);
		}
		break;

	case OPT_MD5:
		params->md5 = PcsTrue;
		break;

	case ARGP_KEY_END:
	case ARGP_KEY_ARGS:
	case ARGP_KEY_SUCCESS:
	default:
		
		return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

static struct argp_child argp_children[] = {
	{ 0, 0, 0, 0 }
};

static struct argp argp = {
	options, parse_opt, args_doc, doc, argp_children, 0, 0
};

void shell_args_parse(int argc, char *argv[], struct params *params)
{
	argp_parse (&argp, argc, argv, 0, 0, params);
	if (!shell_args_check_params(params)) {
		params->is_fail = PcsTrue;
	}
}


struct params *shell_args_create_params()
{
	struct params *p = (struct params *)pcs_malloc(sizeof(struct params));
	if (p)
		memset(p, 0, sizeof(struct params));
	return p;
}

void shell_args_destroy_params(struct params *params)
{
	int i;
	if (params->username) pcs_free(params->username);
	if (params->password) pcs_free(params->password);
	if (params->sort) pcs_free(params->sort);
	if (params->cookie) pcs_free(params->cookie);
	if (params->rc4_key) pcs_free(params->rc4_key);
	if (params->args) {
		for (i = 0; i < params->args_count; i++) {
			pcs_free(params->args[i]);
		}
		pcs_free(params->args);
	}
	if (params->config) pcs_free(params->config);
	if (params->cache) pcs_free(params->cache);
	pcs_free(params);
}

void shell_print_version() {
	printf("%s\n", argp_program_version);
}
