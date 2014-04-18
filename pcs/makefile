
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

main.o: main.c shell.h
	$(CC) -o $@ -c main.c $(PCS_CCFLAGS)

shell.o: shell.c shell.h shell_args.h pcs_io.h md5.h rc4.h
	$(CC) -o $@ -c shell.c $(PCS_CCFLAGS)
pcs_io.o: pcs_io.c pcs_io.h
	$(CC) -o $@ -c pcs_io.c $(PCS_CCFLAGS)
shell_args.o: shell_args.c shell_args.h
	$(CC) -o $@ -c shell_args.c $(PCS_CCFLAGS)
dir.o: dir.c dir.h
	$(CC) -o $@ -c dir.c $(PCS_CCFLAGS)
hashtable.o: hashtable.c hashtable.h
	$(CC) -o $@ -c hashtable.c $(PCS_CCFLAGS)
md5.o: md5.c md5.h
	$(CC) -o $@ -c md5.c $(PCS_CCFLAGS)
rc4.o: rc4.c rc4.h md5.h
	$(CC) -o $@ -c rc4.c $(PCS_CCFLAGS)

libpcs.a : $(PCS_OBJS)
	$(AR) crv $@ $^

cJSON.o: cJSON.c cJSON.h
	$(CC) -o $@ -c cJSON.c $(PCS_CCFLAGS)
pcs.o: pcs.c pcs_defs.h pcs_mem.h pcs_utils.h pcs_slist.h pcs_http.h cJSON.h pcs.h pcs_fileinfo.h pcs_pan_api_resinfo.h
	$(CC) -o $@ -c pcs.c $(PCS_CCFLAGS)
pcs_fileinfo.o: pcs_fileinfo.c pcs_mem.h pcs_defs.h pcs_utils.h pcs_slist.h pcs_fileinfo.h
	$(CC) -o $@ -c pcs_fileinfo.c $(PCS_CCFLAGS)
pcs_http.o: pcs_http.c pcs_mem.h pcs_defs.h pcs_utils.h pcs_slist.h pcs_http.h
	$(CC) -o $@ -c pcs_http.c $(PCS_CCFLAGS)
pcs_mem.o: pcs_mem.c pcs_defs.h
	$(CC) -o $@ -c pcs_mem.c $(PCS_CCFLAGS)
pcs_pan_api_resinfo.o: pcs_pan_api_resinfo.c pcs_mem.h pcs_defs.h pcs_pan_api_resinfo.h
	$(CC) -o $@ -c pcs_pan_api_resinfo.c $(PCS_CCFLAGS)
pcs_slist.o: pcs_slist.c pcs_mem.h pcs_defs.h pcs_slist.h
	$(CC) -o $@ -c pcs_slist.c $(PCS_CCFLAGS)
pcs_utils.o: pcs_utils.c pcs_mem.h pcs_defs.h pcs_utils.h pcs_slist.h
	$(CC) -o $@ -c pcs_utils.c $(PCS_CCFLAGS)

.PHONY : clean
clean :
	-rm pcs libpcs.a *.o
