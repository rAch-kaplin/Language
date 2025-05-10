#ifndef _HLOGGER
#define _HLOGGER

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include "stack.h"
#include "common.h"

const size_t SIZE_BUFFER = 8192;

enum LogLevel
{
    LOGL_DEBUG = 30,
    LOGL_INFO  = 100,
    LOGL_ERROR = 200
};

enum OutputMode
{
    COLOR_MODE = 10,
    DEFAULT_MODE = 1
};

typedef struct Logger {
    LogLevel levelLogger;
    FILE *logFile;
    OutputMode color_mode;
} Logger;

Logger* GetLogger();
int CheckOutputMode(const char *log_file_name);
bool shouldLog(LogLevel levelMsg);
int loggerInit(LogLevel levelLogger, const char *log_file_name);
void loggerDeinit();
const char* ColorLogMsg(const enum LogLevel levelMsg);
void log(LogLevel levelMsg, const char *file, size_t line, const char *func,  const char *fmt, ...);

#define LOG(levelMsg, fmt, ...)                   \
    do {                                          \
        LOG_BEGIN(levelMsg, fmt, ##__VA_ARGS__);  \
        LOG_MSG(fmt);                             \
        LOG_END();                                \
    } while(0)

#define LOG_BEGIN(levelMsg, fmt, ...)                                         \
    do {                                                                      \
        if (shouldLog(levelMsg))                                              \
        {                                                                     \
            log(levelMsg, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__);  \
        }                                                                     \
    } while(0)
    
#define LOG_MSG(fmt, ...)                                                           \
    do {                                                                            \
        fprintf(GetLogger()->logFile, "%s", GetServiceLines()->proc_instruction);   \
        fprintf(GetLogger()->logFile, "\n%s", GetServiceLines()->stack_state);      \
    } while(0)

#define LOG_END()                                                                                     \
    do {                                                                                              \
        memset(GetServiceLines()->stack_state, 0, sizeof(GetServiceLines()->stack_state));            \
        memset(GetServiceLines()->proc_instruction, 0, sizeof(GetServiceLines()->proc_instruction));  \
        fflush(GetLogger()->logFile);                                                                 \
    } while(0)

#endif //_HLOGGER
