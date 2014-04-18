#ifndef _SHELL_ARGS_H
#define _SHELL_ARGS_H

#include "../pcs/pcs_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#define program_name	"pcs"
#define program_version "0.0.1"
#if defined(DEBUG) || defined(_DEBUG)
#  define program_full_name program_name "(debug) v" program_version
#else
#  define program_full_name program_name " v" program_version
#endif


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
};

/* Structure used to communicate with the parsing functions.  */
struct params {
	PcsBool		is_fail;
	char		*username;
	char		*password;
	char		*cookie;
	char		*sort;
	char		*rc4_key;
	PcsBool		is_recursion;
	PcsBool		is_force;
	PcsBool		is_desc;
	PcsBool		is_append;
	PcsBool		use_urlc;
	PcsBool		is_verbose;
	PcsBool		is_synch;
	PcsBool		is_rc4;
	enum action action;
	char		**args;
	int			args_count;
};

struct params *shell_args_create_params();
void shell_args_destroy_params(struct params *params);

PcsBool shell_args_check_params(struct params *params);
void shell_args_parse(int argc, char *argv[], struct params *params);

#ifdef __cplusplus
}
#endif


#endif
