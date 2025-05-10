#ifndef _HDEBUG
#define _HDEBUG
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include "stack.h"
#include "logger.h"

const stackElem CANARY = (stackElem)0xDEADBABE;

#define DEBUG

#ifdef DEBUG
    #define DBG(...) __VA_ARGS__
#else
    #define DBG(...)
#endif

//#define ONDBG

#ifdef ONDBG
    #define DBG_PRINTF(...) printf(__VA_ARGS__)
    #define DBG_FPRINTF(fp, ...) fprintf(fp, __VA_ARGS__)
    #define DBG_FPRINTF_NO_ARGS(fp, format) fprintf(fp, format)
#else
    #define DBG_PRINTF(...) ((void)0)
    #define DBG_FPRINTF(fp, ...) ((void)0)
    #define DBG_FPRINTF_NO_ARGS(fp, format) ((void)0)
#endif


void getStackState(LogLevel DepthMsg, stack* stk);
int putCanary(stack *stk);
int putHash(stack *stk);
uint64_t Hash(const void *ptr, size_t size, uint64_t seed = 5381);
void stkNullCheck(const stack *stk);
int verify(stack *stk);
void stackAssert(stack *stk);
const char* decoderError(int error);

#endif
