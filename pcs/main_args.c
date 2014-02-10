#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(WIN32)
# include <argp/argp.h>
#else
# include <argp.h>
#endif

#include "pcs_mem.h"
#include "pcs_utils.h"
#include "main_args.h"

#define program_name	"pcs"
#define program_version "0.0.1"
#define program_full_name program_name " v" program_version
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

struct argp_option options[] = {
	{ 0,			0,	 0,				0,							"Options:", 0},
	{ "username",	'u', "<username>",	OPTION_ARG_OPTIONAL,		"Specify the account.", 0},
	{ "password",	'p', "<password>",	OPTION_ARG_OPTIONAL,		"Specify the password.", 0},
	{ "recursion",	'r', 0,				OPTION_ARG_OPTIONAL,		"Recursive execute the command.", 0},
	{ "force",		'f', 0,				OPTION_ARG_OPTIONAL,		"Force delete the files that not exists in source when download or upload.", 0},
	{ "sort",		's', "<field>",
										OPTION_ARG_OPTIONAL,		"Sort the result when list files. ", 0},
	{ "inverte",	'i', 0,				OPTION_ARG_OPTIONAL,		"Inverte the result when list files, if have no -s the -i option will be ignored.", 0},
	{ "append",		'a', 0,				OPTION_ARG_OPTIONAL,		"Append the text at the end of the file instead of override the file when execute echo command.", 0},
	{ 0,			0,	 0,				0,							"More Options:", 0},
	{ "synch",		OPT_SYNCH, 0,		OPTION_ARG_OPTIONAL,		"Synch the tree. "
																	"Delete the files or directories that not exists in the local file system when upload, "
																	"Delete the files or directories that not exists in the net disk when download.", 0},
	{ "urlc",		'U', 0,				OPTION_ARG_OPTIONAL,		"Use http://urlc.cn/g/ to hold the verify code image. "
																	"The default behavior write the image into ~/.baidupcs/ .", 0},
	{ "verbose",	'v', 0,				OPTION_ARG_OPTIONAL,		"Show the response text.", 0},
	{ "cookie",		OPT_COOKIE, "<cookiefile>", OPTION_ARG_OPTIONAL,"Specify the cookie file.", 0},
	{ "rc4",		OPT_RC4, "<rc4-key>", OPTION_ARG_OPTIONAL,		"Specify that use rc4 to encode/decode the content.", 0},

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

	"Commands£º\n"
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

inline void free_params_args(struct params *params)
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

PcsBool main_args_check_params(struct params *params)
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
			print_arg_err("wrong arguments\n");
			res = PcsFalse;
			break;
		}
		break;

	case ACTION_LIST:
		if (params->args_count != 1) {
			print_arg_err("wrong arguments\n");
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
			print_arg_err("wrong arguments\n");
			res = PcsFalse;
			break;
		}
		break;

	case ACTION_MOVE:
		if (params->args_count != 2) {
			print_arg_err("wrong arguments\n");
			res = PcsFalse;
			break;
		}
		break;

	case ACTION_COPY:
		if (params->args_count != 2) {
			print_arg_err("wrong arguments\n");
			res = PcsFalse;
			break;
		}
		break;

	case ACTION_MKDIR:
		if (params->args_count != 1) {
			print_arg_err("wrong arguments\n");
			res = PcsFalse;
			break;
		}
		break;

	case ACTION_DELETE:
		if (params->args_count == 0) {
			print_arg_err("wrong arguments\n");
			res = PcsFalse;
			break;
		}
		break;

	case ACTION_CAT:
		if (params->args_count != 1) {
			print_arg_err("wrong arguments\n");
			res = PcsFalse;
			break;
		}
		break;

	case ACTION_ECHO:
		if (params->args_count != 2) {
			print_arg_err("wrong arguments\n");
			res = PcsFalse;
			break;
		}
		break;

	case ACTION_SEARCH:
		if (params->args_count != 2) {
			print_arg_err("wrong arguments\n");
			res = PcsFalse;
			break;
		}
		break;

	case ACTION_META:
		if (params->args_count != 1) {
			print_arg_err("wrong arguments\n");
			res = PcsFalse;
			break;
		}
		break;

	case ACTION_DOWNLOAD:
		if (params->args_count != 2) {
			print_arg_err("wrong arguments\n");
			res = PcsFalse;
			break;
		}
		break;

	case ACTION_UPLOAD:
		if (params->args_count != 2) {
			print_arg_err("wrong arguments\n");
			res = PcsFalse;
			break;
		}
		break;

	default:
			print_arg_err("wrong arguments\n");
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
		if (arg) {
			params->username = pcs_utils_strdup(arg);
		}
		//else {
		//	print_arg_err("Not specify username\n");
		//	return EINVAL;
		//}
		break;

	case 'p':
		if (arg) {
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
		if (arg) {
			params->cookie = pcs_utils_strdup(arg);
		}
		break;

	case OPT_RC4:
		params->is_rc4 = PcsTrue;
		if (arg) {
			params->rc4_key = pcs_utils_strdup(arg);
		}
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

void main_args_parse(int argc, char *argv[], struct params *params)
{
	argp_parse (&argp, argc, argv, 0, 0, params);
}


struct params *main_args_create_params()
{
	struct params *p = (struct params *)pcs_malloc(sizeof(struct params));
	if (p)
		memset(p, 0, sizeof(struct params));
	return p;
}

void main_args_destroy_params(struct params *params)
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
	pcs_free(params);
}

