#include <string.h>
#include <assert.h>
#include <ctype.h>

#include "lexical_analysis.h"
#include "logger.h"
#include "file_read.h"
#include "../common/colors.h"

/*********************************************************************************************************************************/

const size_t lexeme_array_size = 1000;
const size_t MAX_VARS = 10;

static void SkipSpaces(const char **buffer);
static void AddLexeme(Lexeme *lexeme_array, size_t *lexeme_count, LexemeType type, double value);
static bool IsNum(const char *cur);
//static bool IsOperator(const char *op);
static bool GetOperatorType (Lexeme *lexeme_array, size_t lexeme_count, const char **cur);
static bool GetOperationType(Lexeme *lexeme_array, size_t lexeme_count, const char **cur);

Lexeme* StringToLexemes(const char *str, size_t *lexeme_count);

/*********************************************************************************************************************************/

Variable* GetVarsTable() //TODO struct
{
    static Variable vars_table[MAX_VARS] = {};
    return vars_table;
}

void FreeVarsTable()
{
    Variable* vars_table = GetVarsTable();

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

static void SkipSpaces(const char **buffer)
{
    while (isspace(**buffer)) (*buffer)++;
}

Lexeme* StringToLexemes(const char *str, size_t *lexeme_count)
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
        else if (isalpha(*cur))
        {
            const char *start = cur;
            while (isalpha(*cur)) cur++;

            size_t name_len = (size_t)(cur - start);

            char *name = (char*)calloc(name_len + 1, sizeof(char));
            if (name == nullptr)
            {
                LOG(LOGL_ERROR, "Memory was not allocated");
                return nullptr;
            }
            strncpy(name, start, name_len);
            name[name_len] = '\0';

            Variable* vars_table = GetVarsTable();
            size_t var_pos = AddVartable(vars_table, name, name_len);
            LOG(LOGL_DEBUG, "VAR: <%s>", name);
            lexeme_array[*lexeme_count].type = LEX_VAR;
            lexeme_array[*lexeme_count].value.var = var_pos;
            (*lexeme_count)++;

            free(name);
            continue;
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

void PrintLexemes(const Lexeme* lexeme_array, size_t lexeme_count)
{
    assert(lexeme_array);

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

            case LEX_VAR:
                printf(GREEN "VAR        " RESET ": index " GREEN "%zu" RESET "\n", lex->value.var);
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
                printf(YELLOW "LBRACKET   " RESET ": '" YELLOW "('" RESET "\n");
                break;

            case LEX_RBRACKET:
                printf(YELLOW "RBRACKET   " RESET ": '" YELLOW ")'" RESET "\n");
                break;

            case LEX_LBRACE:
                printf(YELLOW "LEX_LBRACE   " RESET ": '" YELLOW "{'" RESET "\n");
                break;

            case LEX_RBRACE:
                printf(YELLOW "LEX_RBRACE   " RESET ": '" YELLOW "}'" RESET "\n");
                break;

            case LEX_SEMICOLON:
                printf(RED "SEMICOLON  " RESET ": '" YELLOW ";'" RESET "\n");
                break;

            case LEX_END:
                printf(RED "END        " RESET ": '" RED "0'" RESET "\n");
                break;

            default:
                printf(REDB " UNKNOWN   " RESET " type: %d\n", lex->type);
                break;
        }
    }

    printf("\n");
}

Lexeme* InitLexemeArray(const char* file_expr, size_t *lexeme_count)
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
    Lexeme *lexeme_array = StringToLexemes(expr_buffer, lexeme_count);
    free(expr_buffer);

    LOG(LOGL_DEBUG, "Init Lexeme Array SUCCESS");
    return lexeme_array;
}

void DeinitLexemes(Lexeme* lexeme_array)
{
    if (!lexeme_array) return;

    free(lexeme_array);
}
