#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include "stack.h"
#include "logger.h"
#include "color.h"
#include "debug.h"

static const size_t capacity_multiplier = 2;
static const size_t min_capacity = 16;

errorCode stackCtor(stack *stk, size_t capacity)
{
    stkNullCheck(stk);

    if (capacity <= min_capacity)
    {
        stk->capacity = min_capacity;
    }
    else
    {
        stk->capacity = capacity;
    }

    if (stk->capacity == 0)
    {
        DBG_FPRINTF(stderr, COLOR_RED "ERROR: BAD CAPACITY = %zu\n" COLOR_RESET, stk->capacity);
        assert(0);
    }

    stk->data = (stackElem*)calloc(stk->capacity + 2, sizeof(stackElem));
    if (stk->data == NULL)
    {
        fprintf(stderr, "memory allocation error\n");
        return STK_OUT_MEMORY;
    }

    stk->size = 0;

    putCanary(stk);
    putHash(stk);
    getStackState(LOGL_DEBUG, stk);
    stackAssert(stk);
    LOG(LOGL_DEBUG, "STACK Ctor: %p", *stk);

    DBG_PRINTF(COLOR_MAGENTA"Start: stk->data = %p\n" COLOR_RESET, stk->data);

    DBG_PRINTF(COLOR_MAGENTA "Start: stk->size = %zd\n" COLOR_RESET, stk->size);

    DBG_PRINTF(COLOR_MAGENTA"Start: stk->capacity = %zu\n\n" COLOR_RESET, stk->capacity);

    return STK_OK;
}

errorCode stackReallocUp(stack *stk)
{
    stackAssert(stk);
    stackElem *stk_tmptr = (stackElem*)realloc(stk->data, (stk->capacity * capacity_multiplier + 2) * sizeof(stackElem));
    if (stk_tmptr == NULL)
    {
        fprintf(stderr, "Realloc allocate failed\n");
        return STK_REALLOC_FAILED;
    }
    stk->data = stk_tmptr;
    stk->capacity *= capacity_multiplier;

    putCanary(stk);
    putHash(stk);

    getStackState(LOGL_DEBUG, stk);
    stackAssert(stk);
    return STK_OK;
}

errorCode stackPush(stack *stk, stackElem elem)
{
    DBG_PRINTF(COLOR_CYAN "Push " STACK_ELEM_FORMAT " elem to stack\n" COLOR_RESET, elem);
    stackAssert(stk);

    if ((size_t)stk->size >= stk->capacity)
        stackReallocUp(stk);

    stk->data[stk->size + 1] = elem;
    DBG_PRINTF(COLOR_YELLOW "Stack after push stk->data[%zd] = " STACK_ELEM_FORMAT "\n" COLOR_RESET, stk->size, elem);
    stk->size++;

    putHash(stk);
    getStackState(LOGL_DEBUG, stk);
    stackAssert(stk);
    LOG(LOGL_DEBUG, "STACK PUSH: " STACK_ELEM_FORMAT, elem);
    return STK_OK;
}

errorCode stackReallocDown(stack *stk)
{
    stackAssert(stk);
    stackElem *stk_tmptr = (stackElem*)realloc(stk->data, (stk->capacity / capacity_multiplier + 2) * sizeof(stackElem));
    if (stk_tmptr == NULL)
    {
        fprintf(stderr, "Realloc allocate failed\n");
        return STK_REALLOC_FAILED;
    }
    stk->data = stk_tmptr;
    stk->capacity /= capacity_multiplier;

    putCanary(stk);
    putHash(stk);
    //stk->data[0] = 1;
    getStackState(LOGL_DEBUG, stk);
    stackAssert(stk);
    return STK_OK;
}

errorCode stackPop(stack *stk, stackElem *elem_from_stack)
{
    stackAssert(stk);
    if (stk->size == 0)
        return STK_EMPTY_STACK;

    stk->size--;
    *elem_from_stack = stk->data[stk->size + 1];

    if ((stk->size >= (ssize_t)(min_capacity)) && ((size_t)stk->size <= stk->capacity / 4))
        stackReallocDown(stk);

    stackElem tmp = stk->data[stk->size + 1];
    stk->data[stk->size + 1] = POISON;
    getStackState(LOGL_DEBUG, stk);
    LOG(LOGL_DEBUG, "STACK POP: " STACK_ELEM_FORMAT, tmp);

    putHash(stk);
    stackAssert(stk);
    return STK_OK;
}

errorCode capacityOptimization(stack *stk)
{
    if (stk->size == 0)
    {
        stackElem *stk_tmptr = (stackElem*)realloc(stk->data, min_capacity + 2 * sizeof(stackElem));
        if (stk_tmptr == NULL)
        {
            fprintf(stderr, "Realloc allocate failed\n");
            return STK_REALLOC_FAILED;
        }
        stk->data = stk_tmptr;
        stk->capacity = min_capacity;

        putCanary(stk);
        putHash(stk);

        getStackState(LOGL_DEBUG, stk);
        stackAssert(stk);
        LOG(LOGL_DEBUG, "Optimization capacity:");
        return STK_OK;
    }
    return CANT_REALLOC_TO_FREE;
}

errorCode stackDtor(stack *stk)
{
    stkNullCheck(stk);
    free(stk->data);
    stk->data = NULL;
    stk->capacity = 0;
    stk->size = 0;

    DBG_PRINTF(COLOR_MAGENTA"Finished: stk->data = %p\n" COLOR_RESET, stk->data);
    DBG_PRINTF(COLOR_MAGENTA"Capacity: %zu\n" COLOR_RESET, stk->capacity);
    DBG_PRINTF(COLOR_MAGENTA"Size: %zd\n" COLOR_RESET, stk->size);
    DBG_PRINTF(COLOR_MAGENTA"Data pointer: %p\n" COLOR_RESET, stk->data);

    return STK_OK;
}


