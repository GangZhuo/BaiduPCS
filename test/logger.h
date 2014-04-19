/*
 *
 * LOGGER v0.0.3
 * A simple logger for c/c++ under linux, multiprocess-safe
 *
 * ---- CopyLeft by Felix021 @ http://www.felix021.com ----
 *
 * LOG Format:
 * --LEVEL_NOTE--\x7 [Y-m-d H:m:s]\x7 [FILE:LINE]\x7 [EXTRA_INFO]\x7 log_info
 *   // LEVEL_NOTE stands for one of DEBUG/TRACE/NOTICE...
 *   // \x7 is a special character to separate logged fields.
 *
 * Usage:
 *   //Open log file first. Supply a log file name.
 *   log_open("log.txt"); 
 *   
 *   //use it just as printf
 *   FM_LOG_TRACE("some info %d", 123); 
 *   
 *   //6 level: DEBUG, TRACE, NOTICE, MONITOR, WARNING, FATAL
 *   FM_LOG_DEBUG("hi there"); 
 *   
 *   //Need EXTRA_INFO to be logged automatically?
 *   log_add_info("pid:123");
 *   
 *   //You don't need to call log_close manually, it'll be called at exit
 *   log_close();
 *
 */

#ifndef _LOGGER_H
#define _LOGGER_H


int log_open(const char *filename);
void log_close();
void log_write(int, const char *, const int, const char *, ...);
void log_writev(int level, const char *file,
        const int line, const char *fmt, va_list ap);
void log_add_info(const char *info);

#define LOG_FATAL         0
#define LOG_WARNING       1
#define LOG_MONITOR       2
#define LOG_NOTICE        3
#define LOG_TRACE         4
#define LOG_DEBUG         5

#define FM_LOG_DEBUG(...)   log_write(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define FM_LOG_TRACE(...)   log_write(LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define FM_LOG_NOTICE(...)  log_write(LOG_NOTICE, __FILE__, __LINE__, __VA_ARGS__)
#define FM_LOG_MOITOR(...)  log_write(LOG_MONITOR, __FILE__, __LINE__, __VA_ARGS__)
#define FM_LOG_WARNING(...) log_write(LOG_WARNING, __FILE__, __LINE__, __VA_ARGS__)
#define FM_LOG_FATAL(...)   log_write(LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)



#endif
