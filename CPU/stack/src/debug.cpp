#include <assert.h>
#include "debug.h"
#include "color.h"
#include "logger.h"
#include "common.h"

OutputMode CheckArgs(int argc, char *argv[]);

int putCanary(stack *stk)
{
    stkNullCheck(stk);
    stk->data[0] = CANARY;
    stk->data[stk->capacity + 1] = CANARY;

    return 0;
}

int putHash(stack *stk)
{
    stkNullCheck(stk);
    stk->hash_buffer = Hash(stk->data, (stk->capacity + 2) * sizeof(stackElem));
    DBG_FPRINTF(stderr, "Updated Hashes: hash_buffer = %lu\n", stk->hash_buffer);
    return 0;
}

uint64_t Hash(const void *ptr, size_t size, uint64_t seed)
{
    uint64_t hash = seed;
    const char *data = (const char*)ptr;
    if (data == nullptr)
    {
        printf("data == nullptr in Hash()\n");
        assert(0);
    }
    for (size_t i = 0; i < size; ++i)
    {
        hash = hash * 33 ^ (uint64_t)data[i];
    }
    return hash;
}

void stkNullCheck(const stack *stk)
{
    if (stk == NULL)
    {
        DBG_FPRINTF(stderr, COLOR_RED "ERROR: STK POINTER IS NULL\n" COLOR_RESET);
        assert(0);
    }
}

int verify(stack *stk)
{
    int error = 0;
    stkNullCheck(stk);

    if (stk->data == NULL)
        error = error | STK_OUT_MEMORY;
    else
    {
        if (stk->data[0] != CANARY)
            error = error | BAD_CANARY_1;
        if (stk->data[stk->capacity + 1] != CANARY)
            error = error | BAD_CANARY_2;
    }

    if ((size_t)stk->size > stk->capacity)
        error = error | STK_SIZE_LARGER_CAPACITY;

    if (stk->size < 0)
        error = error | BAD_SIZE;

    if (stk->capacity == 0)
        error = error | STK_CAPACITY_NOT_EXSIST;

    if (stk->hash_buffer != Hash(stk->data, (stk->capacity + 2) * sizeof(stackElem)))
        error |= BAD_HASH_BUF;

    return error;
}


void stackAssert(stack *stk)
{
    int error = verify(stk);
    if (error)
    {
        LOG(LOGL_ERROR, "Stack verification failed: %s", stk, decoderError(error));
        loggerDeinit();
        assert(0);
    }
}

const char* decoderError(int error)
{
    if (error & STK_STRUCT_NULL_POINTER)
    {
        return "STK_STRUCT_NULL_POINTER";
    }

    if (error & STK_OUT_MEMORY)
    {
        return "STK_OUT_MEMORY";
    }

    if (error & STK_SIZE_LARGER_CAPACITY)
    {
        return "STK_SIZE_LARGER_CAPACITY";
    }

    if (error & BAD_SIZE)
    {
        return "BAD_SIZE";
    }

    if (error & STK_CAPACITY_NOT_EXSIST)
    {
        return "STK_CAPACITY_NOT_EXSIST";
    }

    if (error & BAD_CANARY_1)
    {
        return "BAD_CANARY_1";
    }

    if (error & BAD_CANARY_2)
    {
        return "BAD_CANARY_2";
    }

    if (error & BAD_HASH_BUF)
    {
        return "BAD_HASH_BUF";
    }

    return "Unknow Error :(";
}

void getStackState(LogLevel DepthMsg, stack* stk)
{
    stkNullCheck(stk);

    ServiceLines *serv_lines = GetServiceLines();

    if (DepthMsg >= 50)
    {
        int current_len = snprintf(serv_lines->stack_state, SIZE_BUFFER,
                    "\tstack pointer = %p\n"
                    "\tCapacity: %zu\n"
                    "\tSize: %zd\n"
                    "\tData pointer: %p\n"
                    "\tData: ",
                    stk, stk->capacity, stk->size, stk->data);

        for (size_t i = 0; i < stk->capacity + 2; i++)
        {
            if (GetLogger()->color_mode == COLOR_MODE)
            {
                current_len += snprintf(serv_lines->stack_state + current_len, SIZE_BUFFER - (size_t)current_len,
                                        COLOR_MAGENTA " " STACK_ELEM_FORMAT COLOR_RESET, stk->data[i]);
            }
            else
            {
                current_len += snprintf(serv_lines->stack_state + current_len, SIZE_BUFFER - (size_t)current_len,
                                         " " STACK_ELEM_FORMAT, stk->data[i]);
            }
        }
    }

    else
    {
        int current_len = 0;
        if (GetLogger()->color_mode == COLOR_MODE)
        {
            current_len += snprintf(serv_lines->stack_state, SIZE_BUFFER, COLOR_MAGENTA "\tData: " COLOR_RESET);
        }
        else
        {
            current_len += snprintf(serv_lines->stack_state, SIZE_BUFFER, "\tData: ");
        }

        for (size_t i = 0; i < stk->capacity + 2; i++)
        {
            if (GetLogger()->color_mode == COLOR_MODE)
            {
                current_len += snprintf(serv_lines->stack_state + current_len, SIZE_BUFFER - (size_t)current_len,
                                        COLOR_MAGENTA " " STACK_ELEM_FORMAT COLOR_RESET, stk->data[i]);
            }
            else
            {
                current_len += snprintf(serv_lines->stack_state + current_len, SIZE_BUFFER - (size_t)current_len,
                                         " " STACK_ELEM_FORMAT, stk->data[i]);
            }
        }
        snprintf(serv_lines->stack_state + current_len, SIZE_BUFFER - (size_t)current_len, "\n\n");
    }
}
