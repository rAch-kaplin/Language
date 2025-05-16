#ifndef _HBACKEND
#define _HBACKEND

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

#include "tree_func.h"

typedef struct AsmFile
{
    char *asm_buffer;
    int cur_len;
    size_t endif;
    size_t endwhile;
} AsmFile;

CodeError AssemblyTree(const Node *root, NameTable *name_table, const char *file_asm);

#endif //_HBACKEND
