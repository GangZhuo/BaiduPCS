
ver = release

OS_NAME = $(shell uname -s | cut -c1-6)
LC_OS_NAME = $(shell echo $(OS_NAME) | tr '[A-Z]' '[a-z]')

PCS_OBJS     =  bin/cJSON.o \
				bin/pcs.o \
				bin/pcs_fileinfo.o \
				bin/pcs_http.o \
				bin/pcs_mem.o \
				bin/pcs_pan_api_resinfo.o \
				bin/pcs_slist.o \
				bin/pcs_utils.o \
				bin/err_msg.o \
				bin/utf8.o \
				bin/pcs_buffer.o \
				bin/pcs_passport_dv.o

SHELL_OBJS   =  bin/shell_arg.o \
				bin/shell.o \
				bin/dir.o \
				bin/rb_tree_misc.o \
				bin/rb_tree_stack.o \
				bin/red_black_tree.o \
				bin/shell_utils.o \
				bin/hashtable.o

#CCFLAGS      = -DHAVE_ASPRINTF -DHAVE_ICONV
ifeq ($(LC_OS_NAME), cygwin)
CYGWIN_CCFLAGS = -largp
else
CYGWIN_CCFLAGS = 
endif

ifeq ($(LC_OS_NAME), $(filter $(LC_OS_NAME),mingw3 mingw6))
MINGW_CCFLAGS = -lshlwapi
else
MINGW_CCFLAGS = 
endif

ifeq ($(LC_OS_NAME), darwin)
APPLE_CCFLAGS = -I/usr/local/opt/openssl/include -L/usr/local/opt/openssl/lib
else
APPLE_CCFLAGS = 
endif

ifneq ($(ver), debug)
$(warning "Use 'make ver=debug' to build for gdb debug.")
CCFLAGS:=-D_FILE_OFFSET_BITS=64
else
CCFLAGS:=-g -D_FILE_OFFSET_BITS=64 -DDEBUG -D_DEBUG
endif

PCS_CCFLAGS = -fPIC $(CCFLAGS) $(CYGWIN_CCFLAGS) $(APPLE_CCFLAGS) $(MINGW_CCFLAGS)

all: bin/pcs

bin/pcs : pre $(PCS_OBJS) $(SHELL_OBJS)
	$(CC) -o $@ $(PCS_OBJS) $(SHELL_OBJS) $(CCFLAGS) $(CYGWIN_CCFLAGS) $(APPLE_CCFLAGS) $(MINGW_CCFLAGS) -lm -lcurl -lssl -lcrypto -lpthread

bin/shell_arg.o: arg.c arg.h
	$(CC) -o $@ -c $(PCS_CCFLAGS) arg.c
bin/shell.o: shell.c shell.h version.h dir.h
	$(CC) -o $@ -c $(PCS_CCFLAGS) shell.c
bin/dir.o: dir.c dir.h
	$(CC) -o $@ -c $(PCS_CCFLAGS) dir.c
bin/shell_utils.o: utils.c utils.h
	$(CC) -o $@ -c $(PCS_CCFLAGS) utils.c
bin/hashtable.o: hashtable.c hashtable.h
	$(CC) -o $@ -c $(PCS_CCFLAGS) hashtable.c
bin/rb_tree_misc.o: rb_tree/misc.c rb_tree/misc.h
	$(CC) -o $@ -c $(PCS_CCFLAGS) rb_tree/misc.c
bin/rb_tree_stack.o: rb_tree/stack.c rb_tree/stack.h
	$(CC) -o $@ -c $(PCS_CCFLAGS) rb_tree/stack.c
bin/red_black_tree.o: rb_tree/red_black_tree.c rb_tree/red_black_tree.h
	$(CC) -o $@ -c $(PCS_CCFLAGS) rb_tree/red_black_tree.c

bin/cJSON.o: pcs/cJSON.c pcs/cJSON.h
	$(CC) -o $@ -c $(PCS_CCFLAGS) pcs/cJSON.c
bin/pcs.o: pcs/pcs.c pcs/pcs_defs.h pcs/pcs_mem.h pcs/pcs_utils.h pcs/pcs_slist.h pcs/pcs_http.h pcs/cJSON.h pcs/pcs.h pcs/pcs_fileinfo.h pcs/pcs_pan_api_resinfo.h
	$(CC) -o $@ -c $(PCS_CCFLAGS) pcs/pcs.c
bin/pcs_fileinfo.o: pcs/pcs_fileinfo.c pcs/pcs_mem.h pcs/pcs_defs.h pcs/pcs_utils.h pcs/pcs_slist.h pcs/pcs_fileinfo.h
	$(CC) -o $@ -c $(PCS_CCFLAGS) pcs/pcs_fileinfo.c
bin/pcs_http.o: pcs/pcs_http.c pcs/pcs_mem.h pcs/pcs_defs.h pcs/pcs_utils.h pcs/pcs_slist.h pcs/pcs_http.h
	$(CC) -o $@ -c $(PCS_CCFLAGS) pcs/pcs_http.c
bin/pcs_mem.o: pcs/pcs_mem.c pcs/pcs_defs.h
	$(CC) -o $@ -c $(PCS_CCFLAGS) pcs/pcs_mem.c
bin/pcs_pan_api_resinfo.o: pcs/pcs_pan_api_resinfo.c pcs/pcs_mem.h pcs/pcs_defs.h pcs/pcs_pan_api_resinfo.h
	$(CC) -o $@ -c $(PCS_CCFLAGS) pcs/pcs_pan_api_resinfo.c
bin/pcs_slist.o: pcs/pcs_slist.c pcs/pcs_mem.h pcs/pcs_defs.h pcs/pcs_slist.h
	$(CC) -o $@ -c $(PCS_CCFLAGS) pcs/pcs_slist.c
bin/pcs_utils.o: pcs/pcs_utils.c pcs/pcs_mem.h pcs/pcs_defs.h pcs/pcs_utils.h pcs/pcs_slist.h
	$(CC) -o $@ -c $(PCS_CCFLAGS) pcs/pcs_utils.c
bin/err_msg.o: pcs/err_msg.c
	$(CC) -o $@ -c $(PCS_CCFLAGS) pcs/err_msg.c
bin/utf8.o: pcs/utf8.c
	$(CC) -o $@ -c $(PCS_CCFLAGS) pcs/utf8.c
bin/pcs_buffer.o: pcs/pcs_buffer.c
	$(CC) -o $@ -c $(PCS_CCFLAGS) pcs/pcs_buffer.c
bin/pcs_passport_dv.o: pcs/pcs_passport_dv.c
	$(CC) -o $@ -c $(PCS_CCFLAGS) pcs/pcs_passport_dv.c

bin/libpcs.so: pre $(PCS_OBJS)
	$(CC) -shared -fPIC -o $@ $(PCS_OBJS) $(CCFLAGS) $(CYGWIN_CCFLAGS) $(APPLE_CCFLAGS) $(MINGW_CCFLAGS) -lm -lcurl -lssl -lcrypto -lpthread

bin/libpcs.a : pre $(PCS_OBJS)
	$(AR) crv $@ $(PCS_OBJS)

.PHONY : install
install:
	cp ./bin/pcs /usr/local/bin

.PHONY : uninstall
uninstall:
	rm /usr/local/bin/pcs

.PHONY : clean
clean :
	-rm -f ./bin/*.o ./bin/*.so ./bin/pcs

.PHONY : pre
pre :
	mkdir -p bin/
