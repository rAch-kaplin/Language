#ifndef _SYNTAX_ANALYSIS
#define _SYNTAX_ANALYSIS

#include "lexical_analysis.h"

Node* General(Lexeme *lexeme_array, size_t *cur, NameTable *name_table);

#endif // _SYNTAX_ANALYSIS
