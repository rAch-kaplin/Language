#ifndef _LEXICAL_ANALYSIS
#define _LEXICAL_ANALYSIS

#include "tree_func.h"

typedef enum LexemeType
{
    LEX_NUM            = 1,
    LEX_VAR            = 2,
    LEX_FUNC           = 3,
    LEX_FUNC_DEF       = 4,
    // LEX_FUNC_CALL,
    LEX_RET            = 5,
    LEX_OPERATOR       = 6,
    LEX_OPERATION      = 7,
    LEX_LBRACKET       = 8,
    LEX_RBRACKET       = 9,
    LEX_RBRACE         = 10,
    LEX_LBRACE         = 11,
    LEX_SEMICOLON      = 12,
    LEX_COMMA          = 13,
    LEX_END            = 14,
} LexemeType;

typedef struct Lexeme
{
    LexemeType type;

    union
    {
        double num;
        Operator optr;
        Operation oper;
        size_t var;
        size_t func;
    } value;
} Lexeme;

Lexeme*     StringToLexemes(const char *str);
Lexeme*     InitLexemeArray(const char *file_expr, size_t *lexeme_count,  NameTable *name_table);
void        PrintLexemes(const Lexeme *lexeme_array, size_t lexeme_count, NameTable *name_table);
void        DeinitLexemes(Lexeme* lexeme_array);

#endif // _LEXICAL_ANALYSIS
