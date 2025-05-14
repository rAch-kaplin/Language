#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "syntax_analysis.h"
#include "lexical_analysis.h"
#include "logger.h"
#include "DSL.h"

static Node* ParseBlock         (Lexeme* lexemes, size_t* pos, Variable *vars_table);
static Node* GetStatement       (Lexeme* lexemes, size_t* pos, Variable *vars_table);
static Node* GetIf              (Lexeme* lexemes, size_t* pos, Variable *vars_table);
static Node* GetWhile           (Lexeme* lexemes, size_t* pos, Variable *vars_table);
static Node* GetPrint           (Lexeme* lexemes, size_t* pos, Variable *vars_table);
static Node* GetScan            (Lexeme* lexemes, size_t* pos, Variable *vars_table);
static Node* GetAssignment      (Lexeme* lexemes, size_t* pos, Variable *vars_table);
static Node* GetCondition       (Lexeme* lexemes, size_t* pos, Variable *vars_table);
static Node* GetExpression      (Lexeme* lexemes, size_t* pos, Variable *vars_table);
static Node* GetTerm            (Lexeme* lexemes, size_t* pos, Variable *vars_table);
static Node* GetFactor          (Lexeme* lexemes, size_t* pos, Variable *vars_table);

static void SyntaxERROR(const char* message, size_t pos, LexemeType expected, LexemeType found);

static void SyntaxERROR(const char* message, size_t pos, LexemeType expected, LexemeType found)
{
    fprintf(stderr, "Syntax error at position %zu: %s (expected %d, found %d)\n",
            pos, message, expected, found);
}

Node* General(Lexeme* lexemes, size_t* pos, Variable *vars_table)
{
    assert(lexemes && pos);

    Node* program_root = nullptr;
    Node* current_node = nullptr;

    while (lexemes[*pos].type != LEX_END)
    {
        Node* new_node = GetStatement(lexemes, pos, vars_table);

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

Node* ParseBlock(Lexeme* lexemes, size_t* pos, Variable *vars_table)
{
    assert(lexemes && pos);

    Node* block_root = nullptr;
    Node* current_node = nullptr;

    while (lexemes[*pos].type != LEX_RBRACE && lexemes[*pos].type != LEX_END)
    {
        Node* new_node = GetStatement(lexemes, pos, vars_table);

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

Node* GetStatement(Lexeme* lexemes, size_t* pos, Variable *vars_table)
{
    assert(lexemes);
    assert(pos);

    Node* node = nullptr;

    if (lexemes[*pos].type == LEX_OPERATOR)
    {
        switch (lexemes[*pos].value.optr)
        {
            case OP_IF:
                node = GetIf(lexemes, pos, vars_table);
                break;

            case OP_WHILE:
                node = GetWhile(lexemes, pos, vars_table);
                break;

            case OP_PRINT:
                node = GetPrint(lexemes, pos, vars_table);
                if (lexemes[*pos].type != LEX_SEMICOLON)
                {
                    SyntaxERROR("Expected ';' after print", *pos, LEX_SEMICOLON, lexemes[*pos].type);
                    return nullptr;
                }
                (*pos)++;
                break;

            case OP_SCAN:
                node = GetScan(lexemes, pos, vars_table);
                if (lexemes[*pos].type != LEX_SEMICOLON)
                {
                    SyntaxERROR("Expected ';' after scan", *pos, LEX_SEMICOLON, lexemes[*pos].type);
                    return nullptr;
                }
                (*pos)++;
                break;

            case OP_ASSIGN:
            case OP_SEMICOLON:
            default:
                break;
        }
    }
    else if (lexemes[*pos].type == LEX_VAR &&
             lexemes[*pos+1].type == LEX_OPERATOR &&
             lexemes[*pos+1].value.optr == OP_ASSIGN)
    {
        node = GetAssignment(lexemes, pos, vars_table);
        if (lexemes[*pos].type != LEX_SEMICOLON)
        {
            SyntaxERROR("Expected ';' after assignment", *pos, LEX_SEMICOLON, lexemes[*pos].type);
            return nullptr;
        }
        (*pos)++;
    }

    if (node != nullptr)
    {
        Node* semicolon_node = NewNode(OPERATOR, OPERATOR_VALUE(OP_SEMICOLON), node, nullptr);
        return semicolon_node;
    }

    return nullptr;
}

Node* GetIf(Lexeme* lexemes, size_t* pos, Variable *vars_table)
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

    Node* cond = GetCondition(lexemes, pos, vars_table);
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

    Node* body = ParseBlock(lexemes, pos, vars_table);
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

Node* GetWhile(Lexeme* lexemes, size_t* pos, Variable *vars_table) //FIXME
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

    Node* cond = GetCondition(lexemes, pos, vars_table);
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

    Node* body = ParseBlock(lexemes, pos, vars_table);
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

Node* GetPrint(Lexeme* lexemes, size_t* pos, Variable *vars_table)
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

    Node* expr = GetExpression(lexemes, pos, vars_table);
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

Node* GetScan(Lexeme* lexemes, size_t* pos, Variable *vars_table)
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

Node* GetAssignment(Lexeme* lexemes, size_t* pos, Variable *vars_table)
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

    Node* expr = GetExpression(lexemes, pos, vars_table);
    if (!expr)
    {
        SyntaxERROR("Expected expression in assignment", *pos, LEX_NUM, lexemes[*pos].type);
        return nullptr;
    }

    return _ASSIGN(var, expr);
}

Node* GetCondition(Lexeme* lexemes, size_t* pos, Variable *vars_table)
{
    assert(lexemes);
    assert(pos);

    Node* left = GetExpression(lexemes, pos, vars_table);
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

    Node* right = GetExpression(lexemes, pos, vars_table);
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

Node* GetExpression(Lexeme* lexemes, size_t* pos, Variable *vars_table)
{
    assert(lexemes);
    assert(pos);

    Node* node = GetTerm(lexemes, pos, vars_table);
    if (!node)
    {
        return nullptr;
    }

    while (lexemes[*pos].type == LEX_OPERATION &&
          (lexemes[*pos].value.oper == ADD || lexemes[*pos].value.oper == SUB))
    {
        Operation op = lexemes[*pos].value.oper;
        (*pos)++;
        Node* right = GetTerm(lexemes, pos, vars_table);
        if (!right)
        {
            return nullptr;
        }

        node = (op == ADD) ? _ADD(node, right) : _SUB(node, right);
    }

    return node;
}

Node* GetTerm(Lexeme* lexemes, size_t* pos, Variable *vars_table)
{
    assert(lexemes);
    assert(pos);

    Node* node = GetFactor(lexemes, pos, vars_table);
    if (!node)
    {
        return nullptr;
    }

    while (lexemes[*pos].type == LEX_OPERATION &&
          (lexemes[*pos].value.oper == MUL || lexemes[*pos].value.oper == DIV))
    {
        Operation op = lexemes[*pos].value.oper;
        (*pos)++;
        Node* right = GetFactor(lexemes, pos, vars_table);
        if (!right)
        {
            return nullptr;
        }

        node = (op == MUL) ? _MUL(node, right) : _DIV(node, right);
    }

    return node;
}

Node* GetFactor(Lexeme* lexemes, size_t* pos, Variable *vars_table)
{
    assert(lexemes);
    assert(pos);

    Node* expr = nullptr;

    switch (lexemes[*pos].type)
    {
        case LEX_NUM:
            return _NUM(lexemes[(*pos)++].value.num);

        case LEX_VAR:
            return _VAR(lexemes[(*pos)++].value.var);

        case LEX_LBRACKET:
            (*pos)++;
            expr = GetExpression(lexemes, pos, vars_table);
            if (!expr)
            {
                return nullptr;
            }
            if (lexemes[*pos].type != LEX_RBRACKET)
            {
                SyntaxERROR("Expected ')'", *pos, LEX_RBRACKET, lexemes[*pos].type);
                return nullptr;
            }
            (*pos)++;
            return expr;
        case LEX_SEMICOLON:
            break;
        //FIXME
        case LEX_END:
        case LEX_RBRACKET:
        case LEX_LBRACE:
        case LEX_RBRACE:
        case LEX_OPERATION:
        case LEX_OPERATOR:
        default:
            SyntaxERROR("Expected number, variable or '('", *pos, LEX_NUM, lexemes[*pos].type);
            return expr;
    }

    return nullptr;
}
