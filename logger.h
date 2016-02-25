#ifndef _BASE_LOGGER_H_
#define _BASE_LOGGER_H_

#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include "thread.h"

#define LOG_BUFFER_SIZE (1024)
#define LOG_PREFIX_SIZE (1024)

#define LEVEL_DEBUG "DEBUG"
#define LEVEL_INFO  "INFO"
#define LEVEL_ERROR "ERROR"
#define LEVEL_WARN  "WARN"


#define ENABLE_LOG


#ifdef ENABLE_LOG

namespace base
{
    void log_print(char * prefix, char * msg);
}

#define LOG_PREFIX(L, buf) \
    do {time_t t = time(NULL); struct tm* lt = localtime(&t); if (lt) {\
        struct timeval tv; gettimeofday(&tv, NULL);\
        base::TID tid = base::Thread::CurrentThread();\
        sprintf(buf, "[%04d-%02d-%02d %02d:%02d:%02d.%03d.%03d][%s-%s:%d-%ld] ",\
                lt->tm_year + 1900, lt->tm_mon + 1, lt->tm_mday + 1,\
                lt->tm_hour + 1, lt->tm_min + 1, lt->tm_sec + 1, tv.tv_usec/1000,\
                tv.tv_usec%1000, L, __FILE__, __LINE__, (long)tid);}\
    }while(0)


#define LOG_DEBUG(...) \
    do { char __log_pre__[LOG_PREFIX_SIZE]; char __log_buf__[LOG_BUFFER_SIZE];  \
        LOG_PREFIX(LEVEL_DEBUG, __log_pre__); sprintf(__log_buf__, __VA_ARGS__); \
    base::log_print(__log_pre__, __log_buf__);}while(0)

#define LOG_INFO(...) \
    do { char __log_pre__[LOG_PREFIX_SIZE]; char __log_buf__[LOG_BUFFER_SIZE];  \
        LOG_PREFIX(LEVEL_INFO, __log_pre__); sprintf(__log_buf__, __VA_ARGS__); \
    base::log_print(__log_pre__, __log_buf__);}while(0)

#define LOG_ERROR(...) \
    do { char __log_pre__[LOG_PREFIX_SIZE]; char __log_buf__[LOG_BUFFER_SIZE];  \
        LOG_PREFIX(LEVEL_ERROR, __log_pre__); sprintf(__log_buf__, __VA_ARGS__); \
    base::log_print(__log_pre__, __log_buf__);}while(0)

#define LOG_WARN(...) \
    do { char __log_pre__[LOG_PREFIX_SIZE]; char __log_buf__[LOG_BUFFER_SIZE];  \
        LOG_PREFIX(LEVEL_WARN, __log_pre__); sprintf(__log_buf__, __VA_ARGS__); \
    base::log_print(__log_pre__, __log_buf__);}while(0)

#endif


#endif // ENABLE_LOG

#ifndef ENABLE_LOG

#define LOG_DEBUG
#define LOG_INFO
#define LOG_WARN
#define LOG_ERROR

#endif

