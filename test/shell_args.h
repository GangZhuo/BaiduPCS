#ifndef _SHELL_ARGS_H
#define _SHELL_ARGS_H

#include "../pcs/pcs_defs.h"
#include "../pcs/pcs.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "version.h"

enum action {
	ACTION_NONE = 0,
	ACTION_QUOTA = 1,
	ACTION_LIST = 2,
	ACTION_RENAME = 3,
	ACTION_DELETE = 4,
	ACTION_DOWNLOAD = 5,
	ACTION_UPLOAD = 6,
	ACTION_MOVE = 7,
	ACTION_MKDIR = 8,
	ACTION_ECHO = 9,
	ACTION_CAT = 10,
	ACTION_SEARCH = 11,
	ACTION_COPY = 12,
	ACTION_META = 13,
	ACTION_SVC = 14,
	ACTION_RESET = 15,
	ACTION_UPDATE = 16,
	ACTION_BACKUP = 17,
	ACTION_RESTORE = 18,
	ACTION_COMBIN = 19,
	ACTION_COMPARE = 20,
	ACTION_TIME = 21,
	ACTION_LIST_ACTION = 22,
	ACTION_MD5 = 23,
};

/* Structure used to communicate with the parsing functions.  */
struct params {
	PcsBool		is_fail;
	char		*username;
	char		*password;
	char		*cookie;
	char		*sort;
	char		*rc4_key;
	char		*config;
	char		*cache;
	PcsBool		is_recursion;
	PcsBool		is_force;
	PcsBool		is_desc;
	PcsBool		is_append;
	PcsBool		use_urlc;
	PcsBool		is_verbose;
	PcsBool		is_synch;
	PcsBool		is_rc4;
	PcsBool		md5;
	enum action action;
	char		**args;
	int			args_count;
};

struct params *shell_args_create_params();
void shell_args_destroy_params(struct params *params);

PcsBool shell_args_check_params(struct params *params);
void shell_args_parse(int argc, char *argv[], struct params *params);

void shell_print_version();

#ifdef __cplusplus
}
#endif


#endif
