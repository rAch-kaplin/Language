#ifndef COMMON_H
#define COMMON_H

const int SIZE_BUFFER2 = 8192;

typedef struct ServiceLines
{
    char stack_state[SIZE_BUFFER2];
    char proc_instruction[SIZE_BUFFER2];
} ServiceLines;

ServiceLines* GetServiceLines();

#endif // COMMON_H

