#ifndef _READ_TREE
#define _READ_TREE

#include "tree_func.h"

size_t GetSizeFile(FILE *name_base);
char *ReadProgramToBuffer(const char *name_base, size_t *file_size);
Node* ReadProgram(const char *file_expr);

#endif //_READ_TREE
