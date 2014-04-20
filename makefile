
ver = debug

OS_NAME = $(shell uname -o)
LC_OS_NAME = $(shell echo $(OS_NAME) | tr '[A-Z]' '[a-z]')

PCS_OBJS     = bin/cJSON.o bin/pcs.o bin/pcs_fileinfo.o bin/pcs_http.o bin/pcs_mem.o bin/pcs_pan_api_resinfo.o bin/pcs_slist.o bin/pcs_utils.o
SHELL_OBJS   = bin/shell.o bin/pcs_io.o bin/shell_args.o bin/dir.o bin/hashtable.o bin/md5.o bin/rc4.o bin/daemon.o bin/logger.o bin/sqlite3.o
#CCFLAGS      = -DHAVE_ASPRINTF -DHAVE_ICONV
ifeq ($(LC_OS_NAME), cygwin)
CYGWIN_CCFLAGS = -largp
else
CYGWIN_CCFLAGS = -largp
endif

ifeq ($(ver), debug)
$(warning " Build for debug, if you used in production, please use 'make clean & make ver=release'.")
CC = gcc -g -DDEBUG -D_DEBUG
else
CC = gcc
endif

all: bin/libpcs.a bin/pcs

bin/pcs : bin/main.o bin/libpcs.a $(SHELL_OBJS)
	$(CC) -o $@ bin/main.o $(SHELL_OBJS) $(CCFLAGS) $(CYGWIN_CCFLAGS) -L./bin -lpcs -lm -lcurl -lssl

bin/main.o: test/main.c test/shell.h
	$(CC) -o $@ -c test/main.c $(PCS_CCFLAGS)

bin/shell.o: test/shell.c \
         test/shell.h \
		 test/shell_args.h \
		 test/pcs_io.h \
		 test/md5.h \
		 test/rc4.h
	$(CC) -o $@ -c $(PCS_CCFLAGS) test/shell.c
bin/pcs_io.o: test/pcs_io.c test/pcs_io.h
	$(CC) -o $@ -c $(PCS_CCFLAGS) test/pcs_io.c
bin/shell_args.o: test/shell_args.c test/shell_args.h
	$(CC) -o $@ -c $(PCS_CCFLAGS) test/shell_args.c
bin/dir.o: test/dir.c test/dir.h
	$(CC) -o $@ -c $(PCS_CCFLAGS)test/dir.c
bin/hashtable.o: test/hashtable.c test/hashtable.h
	$(CC) -o $@ -c $(PCS_CCFLAGS) test/hashtable.c
bin/md5.o: test/md5.c test/md5.h
	$(CC) -o $@ -c $(PCS_CCFLAGS) test/md5.c
bin/rc4.o: test/rc4.c test/rc4.h test/md5.h
	$(CC) -o $@ -c $(PCS_CCFLAGS) test/rc4.c
bin/daemon.o: test/daemon.c test/daemon.h test/sql.h
	$(CC) -o $@ -c $(PCS_CCFLAGS) test/daemon.c
bin/logger.o: test/logger.c test/logger.h
	$(CC) -o $@ -c $(PCS_CCFLAGS) test/logger.c
bin/sqlite3.o: sqlite/sqlite3.c sqlite/sqlite3.h
	$(CC) -o $@ -c $(PCS_CCFLAGS) sqlite/sqlite3.c

bin/libpcs.a : $(PCS_OBJS)
	$(AR) crv $@ $^

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

.PHONY : clean
clean :
	-rm bin/*.o bin/libpcs.a bin/pcs
