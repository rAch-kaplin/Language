#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "syntax_analysis.h"
#include "lexical_analysis.h"
#include "logger.h"
#include "DSL.h"

static Node* ParseBlock         (Lexeme* lexemes, size_t* pos, NameTable *name_table);
static Node* GetStatement       (Lexeme* lexemes, size_t* pos, NameTable *name_table);
static Node* GetIf              (Lexeme* lexemes, size_t* pos, NameTable *name_table);
static Node* GetWhile           (Lexeme* lexemes, size_t* pos, NameTable *name_table);
static Node* GetAssignment      (Lexeme* lexemes, size_t* pos, NameTable *name_table);
static Node* GetCondition       (Lexeme* lexemes, size_t* pos, NameTable *name_table);
static Node* GetExpression      (Lexeme* lexemes, size_t* pos, NameTable *name_table);
static Node* GetTerm            (Lexeme* lexemes, size_t* pos, NameTable *name_table);
static Node* GetFactor          (Lexeme* lexemes, size_t* pos, NameTable *name_table);
static Node* GetPrint           (Lexeme* lexemes, size_t* pos, NameTable *name_table);
static Node* GetScan            (Lexeme* lexemes, size_t* pos);
static Node* GetSqrt            (Lexeme* lexemes, size_t* pos, NameTable* name_table);

static Node* GetFuncDef         (Lexeme* lexemes, size_t* pos, NameTable* name_table);
static Node* GetFuncCall        (Lexeme* lexemes, size_t* pos, NameTable* name_table);
static Node* GetParamList       (Lexeme* lexemes, size_t* pos, NameTable* name_table);
static Node* GetArgList         (Lexeme* lexemes, size_t* pos, NameTable* name_table);
static Node* GetReturn          (Lexeme* lexemes, size_t* pos, NameTable* name_table);
static Node* GetVar             (Lexeme* lexemes, size_t* pos, NameTable* name_table);

static void SyntaxERROR(const char* message, size_t pos, LexemeType expected, LexemeType found);

static void SyntaxERROR(const char* message, size_t pos, LexemeType expected, LexemeType found)
{
    fprintf(stderr, "Syntax error at position %zu: %s (expected %d, found %d)\n",
            pos, message, expected, found);
}

Node* General(Lexeme* lexemes, size_t* pos, NameTable *name_table)
{
    assert(lexemes && pos);

    Node* program_root = nullptr;
    Node* current_node = nullptr;
    _DLOG("General()");
    while (lexemes[*pos].type != LEX_END)
    {
        Node* new_node = GetStatement(lexemes, pos, name_table);
        _DLOG("Exit from GetStatement()");

        if (program_root == nullptr)
        {
            program_root = new_node;
            current_node = new_node;
        }
        else
        {
            current_node->right = new_node;
            current_node = new_node;
        }
    }

    return program_root;
}

Node* ParseBlock(Lexeme* lexemes, size_t* pos, NameTable *name_table)
{
    assert(lexemes && pos);

    Node* block_root = nullptr;
    Node* current_node = nullptr;

    while (lexemes[*pos].type != LEX_RBRACE && lexemes[*pos].type != LEX_END)
    {
        Node* new_node = GetStatement(lexemes, pos, name_table);

        if (block_root == nullptr)
        {
            block_root = new_node;
            current_node = new_node;
        }
        else
        {
            current_node->right = new_node;
            current_node = new_node;
        }
    }

    return block_root;
}

Node* GetIf(Lexeme* lexemes, size_t* pos, NameTable *name_table)
{
    assert(lexemes);
    assert(pos);

    (*pos)++;

    if (lexemes[*pos].type != LEX_LBRACKET)
    {
        SyntaxERROR("Expected '(' after 'block'", *pos, LEX_LBRACKET, lexemes[*pos].type);
        return nullptr;
    }
    (*pos)++;

    Node* cond = GetCondition(lexemes, pos, name_table);
    if (!cond)
    {
        return nullptr;
    }

    if (lexemes[*pos].type != LEX_RBRACKET)
    {
        SyntaxERROR("Expected ')' after condition", *pos, LEX_RBRACKET, lexemes[*pos].type);
        return nullptr;
    }
    (*pos)++;

    if (lexemes[*pos].type != LEX_LBRACE)
    {
        SyntaxERROR("Expected '{' after if condition", *pos, LEX_LBRACE, lexemes[*pos].type);
        return nullptr;
    }
    (*pos)++;

    Node* body = ParseBlock(lexemes, pos, name_table);
    if (!body)
    {
        return nullptr;
    }

    if (lexemes[*pos].type != LEX_RBRACE)
    {
        SyntaxERROR("Expected '}' after if body", *pos, LEX_RBRACE, lexemes[*pos].type);
        return nullptr;
    }
    (*pos)++;

    return _IF(cond, body);
}

Node* GetWhile(Lexeme* lexemes, size_t* pos, NameTable *name_table) //FIXME
{
    assert(lexemes);
    assert(pos);

    (*pos)++;

    if (lexemes[*pos].type != LEX_LBRACKET)
    {
        SyntaxERROR("Expected '(' after 'rally'", *pos, LEX_LBRACKET, lexemes[*pos].type);
        return nullptr;
    }
    (*pos)++;

    Node* cond = GetCondition(lexemes, pos, name_table);
    if (!cond)
    {
        return nullptr;
    }

    if (lexemes[*pos].type != LEX_RBRACKET)
    {
        SyntaxERROR("Expected ')' after condition", *pos, LEX_RBRACKET, lexemes[*pos].type);
        return nullptr;
    }
    (*pos)++;

    if (lexemes[*pos].type != LEX_LBRACE)
    {
        SyntaxERROR("Expected '{' after while condition", *pos, LEX_LBRACE, lexemes[*pos].type);
        return nullptr;
    }
    (*pos)++;

    Node* body = ParseBlock(lexemes, pos, name_table);
    if (!body)
    {
        return nullptr;
    }

    if (lexemes[*pos].type != LEX_RBRACE)
    {
        SyntaxERROR("Expected '}' after while body", *pos, LEX_RBRACE, lexemes[*pos].type);
        return nullptr;
    }
    (*pos)++;

    Node *node = _WHILE(cond, body);
    if (node == nullptr)
    {
        _DLOG("_WHILE returned nullptr node");
    }

    return node;
}

Node* GetPrint(Lexeme* lexemes, size_t* pos, NameTable *name_table)
{
    assert(lexemes);
    assert(pos);

    (*pos)++;

    if (lexemes[*pos].type != LEX_LBRACKET)
    {
        SyntaxERROR("Expected '(' after 'serve'", *pos, LEX_LBRACKET, lexemes[*pos].type);
        return nullptr;
    }
    (*pos)++;

    Node* expr = GetExpression(lexemes, pos, name_table);
    if (!expr)
    {
        SyntaxERROR("Expected expression in print", *pos, LEX_NUM, lexemes[*pos].type);
        return nullptr;
    }

    if (lexemes[*pos].type != LEX_RBRACKET)
    {
        SyntaxERROR("Expected ')' after print expression", *pos, LEX_RBRACKET, lexemes[*pos].type);
        return nullptr;
    }
    (*pos)++;

    return _PRINT(expr);
}

Node* GetScan(Lexeme* lexemes, size_t* pos)
{
    assert(lexemes);
    assert(pos);

    (*pos)++;

    if (lexemes[*pos].type != LEX_LBRACKET)
    {
        SyntaxERROR("Expected '(' after 'receive'", *pos, LEX_LBRACKET, lexemes[*pos].type);
        return nullptr;
    }
    (*pos)++;

    if (lexemes[*pos].type != LEX_VAR)
    {
        SyntaxERROR("Expected variable in scan", *pos, LEX_VAR, lexemes[*pos].type);
        return nullptr;
    }
    Node* var = _VAR(lexemes[*pos].value.var);
    (*pos)++;

    if (lexemes[*pos].type != LEX_RBRACKET)
    {
        SyntaxERROR("Expected ')' after scan variable", *pos, LEX_RBRACKET, lexemes[*pos].type);
        return nullptr;
    }
    (*pos)++;

    return _SCAN(var);
}

Node* GetAssignment(Lexeme* lexemes, size_t* pos, NameTable *name_table)
{
    assert(lexemes);
    assert(pos);

    Node* var = _VAR(lexemes[*pos].value.var);
    (*pos)++;

    if (lexemes[*pos].type != LEX_OPERATOR || lexemes[*pos].value.optr != OP_ASSIGN)
    {
        SyntaxERROR("Expected '=' in assignment", *pos, LEX_OPERATOR, lexemes[*pos].type);
        return nullptr;
    }
    (*pos)++;

    if (lexemes[*pos].value.optr == OP_SQRT)
    {
        Node *node = GetSqrt(lexemes, pos, name_table);
        return _ASSIGN(var, node);
    }

    Node* expr = GetExpression(lexemes, pos, name_table);
    if (!expr)
    {
        SyntaxERROR("Expected expression in assignment", *pos, LEX_NUM, lexemes[*pos].type);
        return nullptr;
    }

    return _ASSIGN(var, expr);
}

Node* GetCondition(Lexeme* lexemes, size_t* pos, NameTable *name_table)
{
    assert(lexemes);
    assert(pos);

    Node* left = GetExpression(lexemes, pos, name_table);
    if (!left)
    {
        SyntaxERROR("Expected left expression in condition", *pos, LEX_NUM, lexemes[*pos].type);
        return nullptr;
    }

    if (lexemes[*pos].type != LEX_OPERATION)
    {
        SyntaxERROR("Expected comparison operator", *pos, LEX_OPERATION, lexemes[*pos].type);
        return nullptr;
    }

    Operation op = lexemes[*pos].value.oper;
    (*pos)++;

    Node* right = GetExpression(lexemes, pos, name_table);
    if (!right)
    {
        SyntaxERROR("Expected right expression in condition", *pos, LEX_NUM, lexemes[*pos].type);
        return nullptr;
    }

    switch (op)
    {
        case EQ:  return _EQ (left, right);
        case NEQ: return _NEQ(left, right);
        case LT:  return _LT (left, right);
        case LE:  return _LE (left, right);
        case GT:  return _GT (left, right);
        case GE:  return _GE (left, right);

        case ADD:
        case SUB:
        case MUL:
        case DIV:
        default:
            SyntaxERROR("Invalid comparison operator", *pos, LEX_OPERATION, lexemes[*pos].type);
            return nullptr;
    }
}

Node* GetExpression(Lexeme* lexemes, size_t* pos, NameTable *name_table)
{
    assert(lexemes);
    assert(pos);

    Node* node = GetTerm(lexemes, pos, name_table);
    if (!node)
    {
        return nullptr;
    }

    while (lexemes[*pos].type == LEX_OPERATION &&
          (lexemes[*pos].value.oper == ADD || lexemes[*pos].value.oper == SUB))
    {
        Operation op = lexemes[*pos].value.oper;
        (*pos)++;
        Node* right = GetTerm(lexemes, pos, name_table);
        if (!right)
        {
            return nullptr;
        }

        node = (op == ADD) ? _ADD(node, right) : _SUB(node, right);
    }

    return node;
}

Node* GetTerm(Lexeme* lexemes, size_t* pos, NameTable *name_table)
{
    assert(lexemes);
    assert(pos);

    Node* node = GetFactor(lexemes, pos, name_table);
    if (!node)
    {
        return nullptr;
    }

    while (lexemes[*pos].type == LEX_OPERATION &&
          (lexemes[*pos].value.oper == MUL || lexemes[*pos].value.oper == DIV))
    {
        Operation op = lexemes[*pos].value.oper;
        (*pos)++;
        Node* right = GetFactor(lexemes, pos, name_table);
        if (!right)
        {
            return nullptr;
        }

        node = (op == MUL) ? _MUL(node, right) : _DIV(node, right);
    }

    return node;
}

/******************************************************************************************************************************* */

#define _FUNC_DEF(name, params, body)   NewNode(OPERATOR, OPERATOR_VALUE(OP_FUNC_DEF), name, body)
#define _FUNC_CALL(name, args)          NewNode(OPERATOR, OPERATOR_VALUE(OP_FUNC_CALL), name, args)
#define _RETURN(expr)                   NewNode(OPERATOR, OPERATOR_VALUE(OP_RET), expr, nullptr)

Node* GetStatement(Lexeme* lexemes, size_t* pos, NameTable* name_table)
{
    assert(lexemes && pos && name_table);

    _DLOG("Enter GetStatement()");
    Node* node = nullptr;

    if (lexemes[*pos].type == LEX_OPERATOR)
    {
        switch (lexemes[*pos].value.optr)
        {
            case OP_IF:
                node = GetIf(lexemes, pos, name_table);
                break;
            case OP_WHILE:
                node = GetWhile(lexemes, pos, name_table);
                break;
            case OP_PRINT:
                node = GetPrint(lexemes, pos, name_table);
                if (lexemes[*pos].type != LEX_SEMICOLON)
                {
                    SyntaxERROR("Expected ';' after print", *pos, LEX_SEMICOLON, lexemes[*pos].type);
                    return nullptr;
                }
                (*pos)++;
                break;
            case OP_SCAN:
                node = GetScan(lexemes, pos);
                if (lexemes[*pos].type != LEX_SEMICOLON)
                {
                    SyntaxERROR("Expected ';' after scan", *pos, LEX_SEMICOLON, lexemes[*pos].type);
                    return nullptr;
                }
                (*pos)++;
                break;
            case OP_FUNC_DEF:
                node = GetFuncDef(lexemes, pos, name_table);
                _DLOG("Exit GetFuncDef()");
                break;
            case OP_RET:
                node = GetReturn(lexemes, pos, name_table);
                if (lexemes[*pos].type != LEX_SEMICOLON)
                {
                    SyntaxERROR("Expected ';' after return", *pos, LEX_SEMICOLON, lexemes[*pos].type);
                    return nullptr;
                }
                (*pos)++;
                break;
            default:
                break;
        }
    }
    else if (lexemes[*pos].type == LEX_FUNC)
    {
        node = GetFuncCall(lexemes, pos, name_table);
        _DLOG("Exit GetFuncCall()");
        if (lexemes[*pos].type != LEX_SEMICOLON)
        {
            SyntaxERROR("Expected ';' after function call", *pos, LEX_SEMICOLON, lexemes[*pos].type);
            return nullptr;
        }
        (*pos)++;
    }
    else if (lexemes[*pos].type == LEX_VAR &&
             lexemes[*pos+1].type == LEX_OPERATOR &&
             lexemes[*pos+1].value.optr == OP_ASSIGN)
             {
        node = GetAssignment(lexemes, pos, name_table);
        if (lexemes[*pos].type != LEX_SEMICOLON)
        {
            SyntaxERROR("Expected ';' after assignment", *pos, LEX_SEMICOLON, lexemes[*pos].type);
            return nullptr;
        }
        (*pos)++;
    }

    return node ? NewNode(OPERATOR, OPERATOR_VALUE(OP_SEMICOLON), node, nullptr) : nullptr;
}

static Node* GetSqrt(Lexeme* lexemes, size_t* pos, NameTable* name_table)
{
    assert(lexemes && pos && name_table);

    (*pos)++;

    if (lexemes[*pos].type != LEX_LBRACKET)
    {
        SyntaxERROR("Expected '(' after 'sqrt'", *pos, LEX_LBRACKET, lexemes[*pos].type);
        return nullptr;
    }
    (*pos)++;

    Node* expr = GetExpression(lexemes, pos, name_table);
    if (!expr)
    {
        SyntaxERROR("Expected expression in sqrt", *pos, LEX_NUM, lexemes[*pos].type);
        return nullptr;
    }

    if (lexemes[*pos].type != LEX_RBRACKET)
    {
        SyntaxERROR("Expected ')' after sqrt argument", *pos, LEX_RBRACKET, lexemes[*pos].type);
        return nullptr;
    }
    (*pos)++;

    return _SQRT(expr);
}

Node* GetFactor(Lexeme* lexemes, size_t* pos, NameTable* name_table)
{
    assert(lexemes && pos && name_table);

    switch (lexemes[*pos].type)
    {
        case LEX_NUM:
        {
            return _NUM(lexemes[(*pos)++].value.num);
        }
        case LEX_VAR:
        {
            return _VAR(lexemes[(*pos)++].value.var);
        }
        case LEX_LBRACKET:
        {
            (*pos)++;
            Node* expr = GetExpression(lexemes, pos, name_table);
            if (!expr) return nullptr;
            if (lexemes[*pos].type != LEX_RBRACKET)
            {
                SyntaxERROR("Expected ')'", *pos, LEX_RBRACKET, lexemes[*pos].type);
                return nullptr;
            }
            (*pos)++;
            return expr;
        }
        default:
        {
            SyntaxERROR("Expected number, variable or '('", *pos, LEX_NUM, lexemes[*pos].type);
            return nullptr;
        }
    }
    return nullptr;
}

static Node* GetFuncDef(Lexeme* lexemes, size_t* pos, NameTable* name_table)
{
    assert(lexemes && pos && name_table);

    _DLOG("Enter GetFuncDef()");

    (*pos)++;

    if (lexemes[*pos].type != LEX_FUNC)
    {
        SyntaxERROR("Expected function name", *pos, LEX_FUNC, lexemes[*pos].type);
        return nullptr;
    }
    Node* func_name = _FUNC(lexemes[*pos].value.func);
    (*pos)++;

    if (lexemes[*pos].type != LEX_LBRACKET)
    {
        SyntaxERROR("Expected '('", *pos, LEX_LBRACKET, lexemes[*pos].type);
        return nullptr;
    }
    (*pos)++;

    Node* params = nullptr;
    if (lexemes[*pos].type != LEX_RBRACKET)
    {
        params = GetParamList(lexemes, pos, name_table);
        if (!params) return nullptr;
    }

    if (lexemes[*pos].type != LEX_RBRACKET)
    {
        SyntaxERROR("Expected ')'", *pos, LEX_RBRACKET, lexemes[*pos].type);
        return nullptr;
    }
    (*pos)++;

    if (lexemes[*pos].type != LEX_LBRACE)
    {
        SyntaxERROR("Expected '{'", *pos, LEX_LBRACE, lexemes[*pos].type);
        return nullptr;
    }
    (*pos)++;

    Node* body = ParseBlock(lexemes, pos, name_table);
    if (!body) return nullptr;

    if (lexemes[*pos].type != LEX_RBRACE)
    {
        SyntaxERROR("Expected '}'", *pos, LEX_RBRACE, lexemes[*pos].type);
        return nullptr;
    }
    (*pos)++;

    //Node* header = NewNode(OPERATOR, OPERATOR_VALUE(OP_SEMICOLON), func_name, params);

    return NewNode(OPERATOR, OPERATOR_VALUE(OP_FUNC_DEF), func_name, body);
}

static Node* GetFuncCall(Lexeme* lexemes, size_t* pos, NameTable* name_table)
{
    assert(lexemes && pos && name_table);

    _DLOG("Enter GetFuncCall()");
    if (lexemes[*pos].type != LEX_FUNC)
    {
        SyntaxERROR("Expected function name", *pos, LEX_FUNC, lexemes[*pos].type);
        return nullptr;
    }
    Node* func_name = _FUNC(lexemes[*pos].value.func);
    (*pos)++;

    if (lexemes[*pos].type != LEX_LBRACKET)
        {
            SyntaxERROR("Expected '('", *pos, LEX_LBRACKET, lexemes[*pos].type);
            return nullptr;
        }
        (*pos)++;

        Node* args = nullptr;
        if (lexemes[*pos].type != LEX_RBRACKET)
        {
            args = GetParamList(lexemes, pos, name_table);
            if (!args) return nullptr;
        }

        if (lexemes[*pos].type != LEX_RBRACKET)
        {
            printf("%p\n", args);
            SyntaxERROR("Expected ')'", *pos, LEX_RBRACKET, lexemes[*pos].type);
            return nullptr;
        }
        (*pos)++;

    return _FUNC_CALL(func_name, args);
}

static Node* GetParamList(Lexeme* lexemes, size_t* pos, NameTable* name_table)
{
    assert(lexemes && pos && name_table);

    _DLOG("Enter GetParamList()");
    Node* first_param = GetVar(lexemes, pos, name_table);
    if (!first_param) return nullptr;

    Node* current = first_param;
    while (lexemes[*pos].type == LEX_COMMA)
    {
        (*pos)++;
        Node* next_param = GetVar(lexemes, pos, name_table);
        if (!next_param) return nullptr;

        current->right = next_param;
        current = next_param;
    }

    return first_param;
}

static Node* GetArgList(Lexeme* lexemes, size_t* pos, NameTable* name_table)
{
    assert(lexemes && pos && name_table);

    _DLOG("Enter GetArgList()");
    Node* first_arg = GetExpression(lexemes, pos, name_table);
    if (!first_arg) return nullptr;

    Node* current = first_arg;
    while (lexemes[*pos].type == LEX_COMMA)
    {
        (*pos)++;
        Node* next_arg = GetExpression(lexemes, pos, name_table);
        if (!next_arg) return nullptr;

        current->right = next_arg;
        current = next_arg;
    }

    return first_arg;
}

static Node* GetReturn(Lexeme* lexemes, size_t* pos, NameTable* name_table)
{
    assert(lexemes && pos && name_table);

    (*pos)++;

    Node* expr = GetExpression(lexemes, pos, name_table);
    if (!expr) return nullptr;

    return _RETURN(expr);
}

static Node* GetVar(Lexeme* lexemes, size_t* pos, NameTable* name_table)
{
    assert(lexemes && pos && name_table);

    if (lexemes[*pos].type != LEX_VAR)
    {
        SyntaxERROR("Expected variable", *pos, LEX_VAR, lexemes[*pos].type);
        return nullptr;
    }

    size_t var_index = lexemes[*pos].value.var;
    if (var_index >= MAX_VARS || name_table->vars_table[var_index].name == nullptr)
    {
        SyntaxERROR("Undefined variable", *pos, LEX_VAR, lexemes[*pos].type);
        return nullptr;
    }

    Node* var_node = _VAR(var_index);
    (*pos)++;

    return var_node;
}
