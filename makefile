
ver = debug

OS_NAME = $(shell uname -o)
LC_OS_NAME = $(shell echo $(OS_NAME) | tr '[A-Z]' '[a-z]')

PCS_OBJS     = cJSON.o pcs.o pcs_fileinfo.o pcs_http.o pcs_mem.o pcs_pan_api_resinfo.o pcs_slist.o pcs_utils.o
SHELL_OBJS   = shell.o pcs_io.o shell_args.o dir.o hashtable.o md5.o rc4.o
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

all: libpcs.a pcs

pcs : main.o libpcs.a $(SHELL_OBJS)
	$(CC) -o $@ main.o $(SHELL_OBJS) $(CCFLAGS) $(CYGWIN_CCFLAGS) -L. -lpcs -lm -lcurl -lssl

main.o: test/main.c test/shell.h
	$(CC) -o $@ -c test/main.c $(PCS_CCFLAGS)

shell.o: test/shell.c \
         test/shell.h \
		 test/shell_args.h \
		 test/pcs_io.h \
		 test/md5.h \
		 test/rc4.h
	$(CC) -o $@ -c $(PCS_CCFLAGS) test/shell.c
pcs_io.o: test/pcs_io.c test/pcs_io.h
	$(CC) -o $@ -c $(PCS_CCFLAGS) test/pcs_io.c
shell_args.o: test/shell_args.c test/shell_args.h
	$(CC) -o $@ -c $(PCS_CCFLAGS) test/shell_args.c
dir.o: test/dir.c test/dir.h
	$(CC) -o $@ -c $(PCS_CCFLAGS)test/dir.c
hashtable.o: test/hashtable.c test/hashtable.h
	$(CC) -o $@ -c $(PCS_CCFLAGS) test/hashtable.c
md5.o: test/md5.c test/md5.h
	$(CC) -o $@ -c $(PCS_CCFLAGS) test/md5.c
rc4.o: test/rc4.c test/rc4.h test/md5.h
	$(CC) -o $@ -c $(PCS_CCFLAGS) test/rc4.c

libpcs.a : $(PCS_OBJS)
	$(AR) crv $@ $^

cJSON.o: pcs/cJSON.c pcs/cJSON.h
	$(CC) -o $@ -c $(PCS_CCFLAGS) pcs/cJSON.c
pcs.o: pcs/pcs.c pcs/pcs_defs.h pcs/pcs_mem.h pcs/pcs_utils.h pcs/pcs_slist.h pcs/pcs_http.h pcs/cJSON.h pcs/pcs.h pcs/pcs_fileinfo.h pcs/pcs_pan_api_resinfo.h
	$(CC) -o $@ -c $(PCS_CCFLAGS) pcs/pcs.c
pcs_fileinfo.o: pcs/pcs_fileinfo.c pcs/pcs_mem.h pcs/pcs_defs.h pcs/pcs_utils.h pcs/pcs_slist.h pcs/pcs_fileinfo.h
	$(CC) -o $@ -c $(PCS_CCFLAGS) pcs/pcs_fileinfo.c
pcs_http.o: pcs/pcs_http.c pcs/pcs_mem.h pcs/pcs_defs.h pcs/pcs_utils.h pcs/pcs_slist.h pcs/pcs_http.h
	$(CC) -o $@ -c $(PCS_CCFLAGS) pcs/pcs_http.c
pcs_mem.o: pcs/pcs_mem.c pcs/pcs_defs.h
	$(CC) -o $@ -c $(PCS_CCFLAGS) pcs/pcs_mem.c
pcs_pan_api_resinfo.o: pcs/pcs_pan_api_resinfo.c pcs/pcs_mem.h pcs/pcs_defs.h pcs/pcs_pan_api_resinfo.h
	$(CC) -o $@ -c $(PCS_CCFLAGS) pcs/pcs_pan_api_resinfo.c
pcs_slist.o: pcs/pcs_slist.c pcs/pcs_mem.h pcs/pcs_defs.h pcs/pcs_slist.h
	$(CC) -o $@ -c $(PCS_CCFLAGS) pcs/pcs_slist.c
pcs_utils.o: pcs/pcs_utils.c pcs/pcs_mem.h pcs/pcs_defs.h pcs/pcs_utils.h pcs/pcs_slist.h
	$(CC) -o $@ -c $(PCS_CCFLAGS) pcs/pcs_utils.c

.PHONY : clean
clean :
	-rm pcs libpcs.a *.o
