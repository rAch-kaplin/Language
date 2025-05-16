#include <string.h>
#include <assert.h>
#include <ctype.h>

#include "lexical_analysis.h"
#include "logger.h"
#include "file_read.h"
#include "../common/colors.h"

/*********************************************************************************************************************************/

const size_t lexeme_array_size = 1000;

static void SkipSpaces          (const char **buffer);
static bool IsNum               (const char *cur);
static void AddLexeme           (Lexeme *lexeme_array, size_t *lexeme_count, LexemeType type, double value);
static bool GetOperatorType     (Lexeme *lexeme_array, size_t lexeme_count, const char **cur);
static bool GetOperationType    (Lexeme *lexeme_array, size_t lexeme_count, const char **cur);

Lexeme* StringToLexemes(const char *str, size_t *lexeme_count, NameTable *name_table);

/*********************************************************************************************************************************/

/*
        ======================
        |   VARS_FUNCTIONS   |
        ======================
*/
void FreeVarsTable(Variable *vars_table)
{
    for (size_t i = 0; i < MAX_VARS; i++)
    {
        LOG(LOGL_DEBUG, "free ---> %s ", vars_table[i].name);
        if (vars_table[i].name != nullptr)
        {
            free((vars_table[i].name));
            vars_table[i].name = nullptr;
        }
    }
}

size_t LookupVar(Variable *vars_table, const char* name, size_t len_name)
{
    assert(vars_table && name);

    size_t cur = 0;
    while (vars_table[cur].name && cur < MAX_VARS)
    {
        if (strncmp(name, vars_table[cur].name, len_name) == 0)
        {
            break;
        }

        cur++;
    }

    return cur;
}

size_t AddVartable(Variable *vars_table, const char* name, size_t len_name)
{
    assert(vars_table && name);

    size_t pos = LookupVar(vars_table, name, len_name);
    if (vars_table[pos].name == nullptr)
    {
        vars_table[pos].name = strdup(name);
        vars_table[pos].len_name = len_name;
    }

    return pos;
}

/*********************************************************************************************************************************/

/*
        ======================
        |   FUNC_FUNCTIONS   |
        ======================
*/

void FreeFuncTable(Function *func_table)
{
    for (size_t i = 0; i < MAX_FUNC; i++)
    {
        LOG(LOGL_DEBUG, "free function ---> %s ", func_table[i].name);
        if (func_table[i].name != nullptr)
        {
            free(func_table[i].name);
            func_table[i].name = nullptr;
        }
    }
}

size_t LookupFunc(Function *func_table, const char* name, size_t len_name)
{
    assert(func_table && name);

    size_t cur = 0;
    while (func_table[cur].name && cur < MAX_FUNC)
    {
        if (strncmp(name, func_table[cur].name, len_name) == 0)
        {
            break;
        }
        cur++;
    }

    return cur;
}

size_t AddFuncTable(Function *func_table, const char* name, size_t len_name)
{
    assert(func_table && name);

    size_t pos = LookupFunc(func_table, name, len_name);
    if (func_table[pos].name == nullptr)
    {
        func_table[pos].name = strdup(name);
        func_table[pos].len_name = len_name;
    }

    return pos;
}

void FreeNameTable(NameTable *name_table)
{
    assert(name_table);

    FreeVarsTable(name_table->vars_table);
    FreeFuncTable(name_table->func_table);
}

/*********************************************************************************************************************************/

static void SkipSpaces(const char **buffer)
{
    while (isspace(**buffer)) (*buffer)++;
}

Lexeme* StringToLexemes(const char *str, size_t *lexeme_count, NameTable *name_table)
{
    Lexeme *lexeme_array = (Lexeme*)calloc(lexeme_array_size, sizeof(Lexeme));
    if (lexeme_array == nullptr)
    {
        LOG(LOGL_ERROR, "lexeme_array memory was not allocated");
        return nullptr;
    }

    const char *cur = str;
    const char *end = strchr(str, '\0');

    while (cur < end)
    {
        SkipSpaces(&cur);
        LOG(LOGL_DEBUG, "curr: %c ---> {%d}, ip = [%d]", *cur, *cur, *lexeme_count);

        if (*cur == '(')
        {
            AddLexeme(lexeme_array, lexeme_count, LEX_LBRACKET, '(');
            cur++;
            continue;
        }
        else if (*cur == ')')
        {
            AddLexeme(lexeme_array, lexeme_count, LEX_RBRACKET, ')');
            cur++;
            continue;
        }
        else if (*cur == '{')
        {
            AddLexeme(lexeme_array, lexeme_count, LEX_LBRACE, '{');
            cur++;
            continue;
        }
        else if (*cur == '}')
        {
            AddLexeme(lexeme_array, lexeme_count, LEX_RBRACE, '}');
            cur++;
            continue;
        }
        else if (*cur == ';')
        {
            AddLexeme(lexeme_array, lexeme_count, LEX_SEMICOLON, ';');
            cur++;
            continue;
        }
        else if (*cur == '\0')
        {
            AddLexeme(lexeme_array, lexeme_count, LEX_END, 0);
            cur++;
            continue;
        }

        else if (IsNum(cur)) //TODO wrapper
        {
            lexeme_array[*lexeme_count].type = LEX_NUM;
            char *end_num = nullptr;
            lexeme_array[*lexeme_count].value.num = strtod(cur, &end_num);
            cur = (const char*)end_num;
            (*lexeme_count)++;
            continue;
        }
        else if (GetOperationType(lexeme_array, *lexeme_count, &cur))
        {
            (*lexeme_count)++;
            continue;
        }
        else if (GetOperatorType(lexeme_array, *lexeme_count, &cur))
        {
            (*lexeme_count)++;
            continue;
        }
//         else if (isalpha(*cur))
//         {
//             const char *start = cur;
//             while (isalpha(*cur) || *cur == '_') cur++;
//
//             size_t name_len = (size_t)(cur - start);
//
//             char *name = (char*)calloc(name_len + 1, sizeof(char));
//             if (name == nullptr)
//             {
//                 LOG(LOGL_ERROR, "Memory was not allocated");
//                 return nullptr;
//             }
//
//             if (*cur == '(')
//             {
//
//             }
//
//             size_t var_pos = AddVartable(name_table->vars_table, name, name_len);
//             LOG(LOGL_DEBUG, "VAR: <%s>", name);
//             lexeme_array[*lexeme_count].type = LEX_VAR;
//             lexeme_array[*lexeme_count].value.var = var_pos;
//             (*lexeme_count)++;
//
//             free(name);
//             continue;
//         }

        else if (isalpha(*cur) || *cur == '_')
        {
            const char *start = cur;
            while (isalpha(*cur) || *cur == '_') cur++;

            size_t name_len = (size_t)(cur - start);
            char *name = (char*)calloc(name_len + 1, sizeof(char));
            if (name == nullptr)
            {
                LOG(LOGL_ERROR, "Memory was not allocated");
                FreeNameTable(name_table);
                free(lexeme_array);
                return nullptr;
            }
            strncpy(name, start, name_len);
            name[name_len] = '\0';

            SkipSpaces(&cur);
            bool is_func_decl = (strncmp(name, "play", 4) == 0 && name_len == 4);

            bool is_func_call = (*cur == '(' && !is_func_decl);

            if (is_func_decl)
            {
                AddLexeme(lexeme_array, lexeme_count, LEX_FUNC_DEF, 0);
                free(name);

                SkipSpaces(&cur);

                start = cur;
                while (isalpha(*cur) || *cur == '_') cur++;
                name_len = (size_t)(cur - start);
                name = (char*)calloc(name_len + 1, sizeof(char));
                strncpy(name, start, name_len);
                name[name_len] = '\0';

                size_t func_pos = AddFuncTable(name_table->func_table, name, name_len);
                lexeme_array[*lexeme_count].type = LEX_FUNC;
                lexeme_array[*lexeme_count].value.func = func_pos;
                (*lexeme_count)++;
                free(name);
            }
            else if (is_func_call)
            {
                size_t func_pos = AddFuncTable(name_table->func_table, name, name_len);
                lexeme_array[*lexeme_count].type = LEX_FUNC;
                lexeme_array[*lexeme_count].value.func = func_pos;
                (*lexeme_count)++;
                free(name);
            }
            else
            {
                size_t var_pos = AddVartable(name_table->vars_table, name, name_len);
                LOG(LOGL_DEBUG, "VAR: <%s>", name);
                lexeme_array[*lexeme_count].type = LEX_VAR;
                lexeme_array[*lexeme_count].value.var = var_pos;
                (*lexeme_count)++;
                free(name);
            }
            continue;
        }
        else
        {
            LOG(LOGL_ERROR, "Unexpected character: %c", *cur);
            FreeNameTable(name_table);
            free(lexeme_array);
            return nullptr;
        }
    }
    return lexeme_array;
}

static bool IsNum(const char *cur)
{
    assert(cur);
    return (isdigit(*cur) || (*cur == '-' && isdigit(*(cur + 1))));
}

static void AddLexeme(Lexeme *lexeme_array, size_t *lexeme_count, LexemeType type, double value)
{
    assert(lexeme_array && lexeme_count);

    lexeme_array[*lexeme_count].type = type;
    lexeme_array[*lexeme_count].value.num = value;
    (*lexeme_count)++;
}

static bool GetOperationType(Lexeme *lexeme_array, size_t lexeme_count, const char **cur)
{
    assert(lexeme_array);
    assert(cur && *cur);

    for (size_t i = 0; i < size_of_operations; i++)
    {
        size_t op_len = strlen(operations[i].symbol);
        if (strncmp(*cur, operations[i].symbol, op_len) == 0)
        {
            lexeme_array[lexeme_count].type = LEX_OPERATION;
            lexeme_array[lexeme_count].value.oper = operations[i].op;

            (*cur) += op_len;
            return true;
        }
    }

    return false;
}

static bool GetOperatorType(Lexeme *lexeme_array, size_t lexeme_count, const char **cur)
{
    assert(lexeme_array);
    assert(cur && *cur);

    for (size_t i = 0; i < size_of_operators; i++)
    {
        size_t op_len = strlen(operators[i].keyword);
        if (strncmp(*cur, operators[i].keyword, op_len) == 0)
        {
            lexeme_array[lexeme_count].type = LEX_OPERATOR;
            lexeme_array[lexeme_count].value.optr = operators[i].optr;

            (*cur) += op_len;
            return true;
        }
    }

    return false;
}

void PrintLexemes(const Lexeme* lexeme_array, size_t lexeme_count, NameTable *name_table)
{
    assert(lexeme_array);
    assert(name_table);

    printf(BLUB " //***********************Lexemes***********************// " RESET "\n\n");

    for (size_t i = 0; i < lexeme_count; i++)
    {
        const Lexeme* lex = &lexeme_array[i];
        printf(GREEN "  [%2zu] " RESET, i);

        switch (lex->type)
        {
            case LEX_NUM:
                printf(CYAN "NUM        " RESET ": " BLUE "%lf" RESET "\n", lex->value.num);
                break;

            // case LEX_VAR:
            //     printf(GREEN "VAR        " RESET ": index " GREEN "%zu:<%s>" RESET "\n", lex->value.var, vars_table[lex->value.var].name);
            //     break;

            case LEX_VAR:
                printf(GREEN "VAR        " RESET ": index " GREEN "%zu:<%s>" RESET "\n",
                       lex->value.var, name_table->vars_table[lex->value.var].name);
                break;

            case LEX_FUNC:
                printf(MAGENTA "FUNCNAME   " RESET ": index " MAGENTA "%zu:<%s>" RESET "\n",
                       lex->value.func, name_table->func_table[lex->value.func].name);
                break;

            case LEX_FUNC_DEF:
                printf(MAGENTA "FUNC DEF       " RESET ": \"" MAGENTA "play" RESET "\"\n");
                break;

            case LEX_OPERATION:
                for (size_t j = 0; j < size_of_operations; j++)
                {
                    if (operations[j].op == lex->value.oper)
                    {
                        printf(MAGENTA "OPERATION  " RESET ": '" MAGENTA "%s" RESET "'\n", operations[j].symbol);
                        break;
                    }
                }
                break;

            case LEX_OPERATOR:
                for (size_t j = 0; j < size_of_operators; j++)
                {
                    if (operators[j].optr == lex->value.optr)
                    {
                        printf(YELLOW "OPERATOR   " RESET ": \"" YELLOW "%s" RESET "\"\n", operators[j].keyword);
                        break;
                    }
                }
                break;

            case LEX_LBRACKET:
                printf(YELLOW "LBRACKET   "     RESET ": '" YELLOW "('" RESET "\n");
                break;

            case LEX_RBRACKET:
                printf(YELLOW "RBRACKET   "     RESET ": '" YELLOW ")'" RESET "\n");
                break;

            case LEX_LBRACE:
                printf(YELLOW "LEX_LBRACE   "   RESET ": '" YELLOW "{'" RESET "\n");
                break;

            case LEX_RBRACE:
                printf(YELLOW "LEX_RBRACE   "   RESET ": '" YELLOW "}'" RESET "\n");
                break;

            case LEX_SEMICOLON:
                printf(RED    "SEMICOLON  "     RESET ": '" YELLOW ";'" RESET "\n");
                break;

            case LEX_END:
                printf(RED    "END        "     RESET ": '" RED "0'" RESET "\n");
                break;

            default:
                printf(REDB   " UNKNOWN   "     RESET " type: %d\n", lex->type);
                break;
        }
    }

    printf("\n");
}

Lexeme* InitLexemeArray(const char* file_expr, size_t *lexeme_count, NameTable *name_func)
{
    assert(file_expr);

    size_t file_size = 0;
    char *expr_buffer = ReadProgramToBuffer(file_expr, &file_size);
    if (!expr_buffer)
    {
        fprintf(stderr, "Failed to read input file.\n");
        return nullptr;
    }

    LOG(LOGL_DEBUG, "Start StringToLexemes()");
    Lexeme *lexeme_array = StringToLexemes(expr_buffer, lexeme_count, name_func);
    free(expr_buffer);

    LOG(LOGL_DEBUG, "Init Lexeme Array SUCCESS");
    return lexeme_array;
}

void DeinitLexemes(Lexeme* lexeme_array)
{
    if (!lexeme_array) return;

    free(lexeme_array);
}
