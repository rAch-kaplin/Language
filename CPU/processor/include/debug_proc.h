#ifndef _HDEBUG_PROC
#define _HDEBUG_PROC

#include "logger.h"

int AddArgsToCommand(Logger *log, int *current_len, const char *fmt, ...);
void GetProcInstruction(int cmd, CPU *proc, ...);

#endif //_HDEBUG_PROC
