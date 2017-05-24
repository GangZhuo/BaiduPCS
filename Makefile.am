VERSION_INFO = 0:0:0

AM_CFLAGS = -g -Wall
#AM_CFLAGS += $(PTHREAD_CFLAGS)
AM_CFLAGS += -I$(top_srcdir)
AM_CFLAGS += -I$(top_srcdir)/pcs
AM_CFLAGS += -I$(top_srcdir)/rb_tree
ACLOCAL_AMFLAGS = -I m4

COMMON_LIBS = -lm

bin_PROGRAMS = baidupcs

pcs_src = pcs/cJSON.c \
		  pcs/pcs_fileinfo.c \
		  pcs/pcs_http.c \
		  pcs/pcs_mem.c \
		  pcs/pcs_pan_api_resinfo.c \
		  pcs/pcs_slist.c \
		  pcs/pcs_utils.c \
		  pcs/err_msg.c \
		  pcs/utf8.c \
		  pcs/pcs.c \
		  pcs/pcs_buffer.c \
		  pcs/pcs_passport_dv.c

shell_src = shell.c \
			arg.c \
			dir.c \
			rb_tree/misc.c \
			rb_tree/red_black_tree.c \
			rb_tree/stack.c \
			utils.c \
			hashtable.c

baidupcs_SOURCES = \
				   $(shell_src) \
				   $(pcs_src)

baidupcs_CFLAGS = $(AM_CFLAGS)
baidupcs_LDADD = $(COMMON_LIBS)

lib_LTLIBRARIES = libbaidupcs.la
libbaidupcs_la_SOURCES = $(pcs_src)
libbaidupcs_la_LDFLAGS = -version-info $(VERSION_INFO)
libbaidupcs_la_LIBADD = $(pcs_LDADD)
pcsincludedir = $(includedir)/pcs
pcsinclude_HEADERS = pcs/pcs.h \
					 pcs/pcs_buffer.h \
					 pcs/pcs_defs.h \
					 pcs/pcs_fileinfo.h \
					 pcs/pcs_http.h \
					 pcs/pcs_mem.h \
					 pcs/pcs_pan_api_resinfo.h \
					 pcs/pcs_passport_dv.h \
					 pcs/pcs_slist.h \
					 pcs/pcs_utils.h

noinst_HEADERS =  pcs/BaiduPCS.h \
				  pcs/cJSON.h \
				  pcs/openssl_aes.h \
				  pcs/openssl_md5.h \
				  pcs/openssl_rsa.h \
				  pcs/utf8.h \
				  shell.h \
				  utils.h \
				  version.h \
				  arg.h \
				  dir.h \
				  hashtable.h \
				  rb_tree/misc.h \
				  rb_tree/red_black_tree.h \
				  rb_tree/stack.h

EXTRA_DIST = CHANGES README.md docs
