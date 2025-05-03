#ifndef _LEXICAL_ANALYSIS
#define _LEXICAL_ANALYSIS

#include "tree_func.h"

typedef enum LexemeType
{
    LEX_NUM,
    LEX_VAR,
    LEX_OPERATOR,
    LEX_OPERATION,
    LEX_LBRACKET,
    LEX_RBRACKET,
    LEX_RBRACE,
    LEX_LBRACE,
    LEX_SEMICOLON,
    LEX_END,
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
    } value;
} Lexeme;

Lexeme*     StringToLexemes(const char *str);
Lexeme*     InitLexemeArray(const char* file_expr, size_t *lexeme_count);
void        PrintLexemes(const Lexeme *lexeme_array, size_t lexeme_count);
void        DeinitLexemes(Lexeme* lexeme_array);

size_t LookupVar  (Variable *vars_table, const char* name, size_t len_name);
size_t AddVartable(Variable *vars_table, const char* name, size_t len_name);
Variable* GetVarsTable();
void FreeVarsTable();

#endif // _LEXICAL_ANALYSIS
