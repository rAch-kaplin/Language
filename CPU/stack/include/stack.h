#ifndef _HSTACK
#define _HSTACK

#include <stdint.h>

typedef int stackElem;
const stackElem POISON = 0xBAD1ABA;
#define STACK_ELEM_FORMAT "%d"

enum errorCode
{
    STK_OK = 0,
    STK_OUT_MEMORY =            (1 << 0),
    STK_REALLOC_FAILED =        (1 << 1),
    STK_EMPTY_STACK =           (1 << 2),
    STK_SIZE_LARGER_CAPACITY =  (1 << 3),
    STK_CAPACITY_NOT_EXSIST =   (1 << 4),
    BAD_CAPACITY =              (1 << 5),
    BAD_SIZE =                  (1 << 6),
    STK_STRUCT_NULL_POINTER =   (1 << 7),
    FILE_POINTER_IS_NULL =      (1 << 8),
    CANT_REALLOC_TO_FREE =      (1 << 9),
    BAD_CANARY_1 =              (1 << 10),
    BAD_CANARY_2 =              (1 << 11),
    BAD_HASH_BUF =              (1 << 12)
};

typedef struct stack
{
    stackElem *data;
    ssize_t size;
    size_t capacity;
    uint64_t hash_buffer;
} stack;


errorCode stackCtor(stack *stk, size_t capacity);
errorCode stackPush(stack *stk, stackElem elem);
errorCode stackDtor(stack *stk);
errorCode stackPop(stack *stk, stackElem *elem_from_stack);
errorCode stackReallocUp(stack *stk);
errorCode stackReallocDown(stack *stk);
errorCode capacityOptimization(stack *stk);


#endif
