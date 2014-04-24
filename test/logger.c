
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>
//#include <unistd.h>
//#include <error.h>
//#include <sys/file.h>

#include "logger.h"

#ifndef NULL
#  define NULL ((void *)0)
#endif
#ifdef WIN32
//#  include <stdint.h>
#  define snprintf _snprintf
#else
#  include <alloca.h>
#endif

static char LOG_LEVEL_NOTE[][10] = 
{ "FATAL", "WARNING", "MONITOR", "NOTICE", "TRACE", "DEBUG" };

static FILE *log_fp                 = NULL;
static char *log_filename           = NULL;
static int  log_opened              = 0;

#define log_buffer_size 8192
static char log_buffer[log_buffer_size];
static char log_extra_info[log_buffer_size];

int log_open(const char* filename)
{
	int len;
    if (log_opened == 1) {
        fprintf(stderr, "logger: log already opened\n");
        return 0;
    }
    len = strlen(filename);
    log_filename = (char *)malloc(sizeof(char) * len + 1);
    strcpy(log_filename, filename);
    log_fp = fopen(log_filename, "a");
    if (log_fp == NULL) {
        perror("can't not open log file");
        exit(1);
    }

    atexit(log_close);
    log_opened = 1;
    log_extra_info[0] = 0;
    FM_LOG_NOTICE("log_open");
    return 1;
}

void log_close(void)
{
    if (log_opened) {
        FM_LOG_NOTICE("log_close\n");
        fclose(log_fp);
        free(log_filename);
        log_fp       = NULL;
        log_filename = NULL;
        log_opened   = 0;
    }
}

void log_write(int level, const char *file,
        const int line, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);   
    log_writev(level, file, line, fmt, ap);
    va_end(ap);   
}

void log_writev(int level, const char *file,
        const int line, const char *fmt, va_list ap)
{
    static char buffer[log_buffer_size];
    static char datetime[100];
    static char line_str[20];
    static time_t now;
	size_t count;

    if (log_opened == 0) {
        fprintf(stderr, "log_open not called yet\n");
        exit(1);
    }
    now = time(NULL);

    strftime(datetime, 99, "%Y-%m-%d %H:%M:%S", localtime(&now));
    snprintf(line_str, 19, "%d", line);
    vsnprintf(log_buffer, log_buffer_size, fmt, ap);   

    count = snprintf(buffer, log_buffer_size,
            "--%s-- [%s] [%s:%d]%s %s\n", 
            LOG_LEVEL_NOTE[level], datetime, file, line, log_extra_info, log_buffer);
    if (fwrite(buffer, 1, count, log_fp) < 0) {
        perror("write error");
        exit(1);
    }
}

void log_add_info(const char *info)
{
    int len = strlen(log_extra_info);
    snprintf(log_extra_info + len, log_buffer_size - len, " [%s]", info);
}
