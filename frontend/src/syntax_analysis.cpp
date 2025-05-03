#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "syntax_analysis.h"
#include "lexical_analysis.h"
#include "logger.h"
#include "DSL.h"

static Node* ParseBlock         (Lexeme* lexemes, size_t* pos);
static Node* GetStatement       (Lexeme* lexemes, size_t* pos);
static Node* GetIf              (Lexeme* lexemes, size_t* pos);
static Node* GetWhile           (Lexeme* lexemes, size_t* pos);
static Node* GetPrint           (Lexeme* lexemes, size_t* pos);
static Node* GetScan            (Lexeme* lexemes, size_t* pos);
static Node* GetAssignment      (Lexeme* lexemes, size_t* pos);
static Node* GetCondition       (Lexeme* lexemes, size_t* pos);
static Node* GetExpression      (Lexeme* lexemes, size_t* pos);
static Node* GetTerm            (Lexeme* lexemes, size_t* pos);
static Node* GetFactor          (Lexeme* lexemes, size_t* pos);

static void SyntaxERROR(const char* message, size_t pos, LexemeType expected, LexemeType found);

#define _SEMICOLON(node)   NewNode(OPERATOR, OPERATOR_VALUE(OP_SEMICOLON), node, nullptr)

static void SyntaxERROR(const char* message, size_t pos, LexemeType expected, LexemeType found)
{
    fprintf(stderr, "Syntax error at position %zu: %s (expected %d, found %d)\n",
            pos, message, expected, found);
}

Node* General(Lexeme* lexemes, size_t* pos)
{
    assert(lexemes && pos);

    Node* program_root = nullptr;
    Node* current_node = nullptr;

    while (lexemes[*pos].type != LEX_END)
    {
        Node* new_node = GetStatement(lexemes, pos);

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

Node* ParseBlock(Lexeme* lexemes, size_t* pos)
{
    assert(lexemes && pos);

    Node *node = GetStatement(lexemes, pos);
    if (!node)
    {
        LOG(LOGL_ERROR, "SetStatement() return nullptr");
        return nullptr;
    }

    return node;
}

Node* GetStatement(Lexeme* lexemes, size_t* pos)
{
    assert(lexemes);
    assert(pos);

    Node* node = nullptr;

    if (lexemes[*pos].type == LEX_OPERATOR)
    {
        switch (lexemes[*pos].value.optr)
        {
            case OP_IF:
                LOG(LOGL_DEBUG, "Start GetStatement ---> OP_IF (block)");
                node = GetIf(lexemes, pos);
                LOG(LOGL_DEBUG, "End GetStatement ---> OP_IF : node = %p", node);
                break;

            case OP_WHILE:
                LOG(LOGL_DEBUG, "GetStatement ---> OP_WHILE (rally)");
                node = GetWhile(lexemes, pos);
                break;

            case OP_PRINT:
                LOG(LOGL_DEBUG, "GetStatement ---> OP_PRINT");
                node = GetPrint(lexemes, pos);
                if (lexemes[*pos].type != LEX_SEMICOLON)
                {
                    SyntaxERROR("Expected ';' after print", *pos, LEX_SEMICOLON, lexemes[*pos].type);
                    return nullptr;
                }
                (*pos)++;
                break;

            case OP_SCAN:
                LOG(LOGL_DEBUG, "GetStatement ---> OP_SCAN");
                node = GetScan(lexemes, pos);
                if (lexemes[*pos].type != LEX_SEMICOLON)
                {
                    SyntaxERROR("Expected ';' after scan", *pos, LEX_SEMICOLON, lexemes[*pos].type); //FIXME
                    return nullptr;
                }
                (*pos)++;
                break;

            case OP_ASSIGN:
            default:
                break;
        }
    }

    else if (lexemes[*pos].type == LEX_VAR &&
             lexemes[*pos+1].type == LEX_OPERATOR &&
             lexemes[*pos+1].value.optr == OP_ASSIGN)
    {
        LOG(LOGL_DEBUG, "GetStatement ---> OP_ASSIGNMENT");
        node = GetAssignment(lexemes, pos);
        LOG(LOGL_DEBUG, "End GetStatement ---> OP_ASSIGNMENT : node = %p", node);
        if (lexemes[*pos].type != LEX_SEMICOLON)
        {
            SyntaxERROR("Expected ';' after assignment", *pos, LEX_SEMICOLON, lexemes[*pos].type);
            return nullptr;
        }
        (*pos)++;
    }


    LOG(LOGL_DEBUG, "NODE RETURN FROM GETSTATEMENT : node = %p", node);
    return _SEMICOLON(node);
}

Node* GetIf(Lexeme* lexemes, size_t* pos)
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

    LOG(LOGL_DEBUG, "Start GetCondition");
    Node* cond = GetCondition(lexemes, pos);
    if (!cond)
    {
        return nullptr;
    }
    LOG(LOGL_DEBUG, "End GetCondition");

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

    LOG(LOGL_DEBUG, "Start Body General");
    Node* body = ParseBlock(lexemes, pos);
    if (!body)
    {
        return nullptr;
    }
    LOG(LOGL_DEBUG, "End Body General");

    if (lexemes[*pos].type != LEX_RBRACE)
    {
        SyntaxERROR("Expected '}' after if body", *pos, LEX_RBRACE, lexemes[*pos].type);
        return nullptr;
    }
    (*pos)++;

    return _IF(cond, body);
}

Node* GetWhile(Lexeme* lexemes, size_t* pos)
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

    Node* cond = GetCondition(lexemes, pos);
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

    Node* body = General(lexemes, pos);
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

    return _WHILE(cond, body);
}

Node* GetPrint(Lexeme* lexemes, size_t* pos)
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

    Node* expr = GetExpression(lexemes, pos);
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

Node* GetAssignment(Lexeme* lexemes, size_t* pos)
{
    assert(lexemes);
    assert(pos);

    LOG(LOGL_DEBUG, "Get var init");
    Node* var = _VAR(lexemes[*pos].value.var);
    (*pos)++;

    if (lexemes[*pos].type != LEX_OPERATOR || lexemes[*pos].value.optr != OP_ASSIGN)
    {
        SyntaxERROR("Expected '=' in assignment", *pos, LEX_OPERATOR, lexemes[*pos].type);
        return nullptr;
    }
    (*pos)++;

    LOG(LOGL_DEBUG, "Start GetExpression");
    Node* expr = GetExpression(lexemes, pos);
    if (!expr)
    {
        SyntaxERROR("Expected expression in assignment", *pos, LEX_NUM, lexemes[*pos].type);
        return nullptr;
    }
    LOG(LOGL_DEBUG, "End GetExpression");

    LOG(LOGL_DEBUG, "Init var success");

    return _ASSIGN(var, expr);
}

Node* GetCondition(Lexeme* lexemes, size_t* pos)
{
    assert(lexemes);
    assert(pos);

    Node* left = GetExpression(lexemes, pos);
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

    Node* right = GetExpression(lexemes, pos);
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

Node* GetExpression(Lexeme* lexemes, size_t* pos)
{
    assert(lexemes);
    assert(pos);

    Node* node = GetTerm(lexemes, pos);
    if (!node)
    {
        LOG(LOGL_ERROR, "GerTerm() return nullptr");
        return nullptr;
    }

    while (lexemes[*pos].type == LEX_OPERATION &&
          (lexemes[*pos].value.oper == ADD || lexemes[*pos].value.oper == SUB))
    {
        Operation op = lexemes[*pos].value.oper;
        (*pos)++;
        Node* right = GetTerm(lexemes, pos);
        if (!right)
        {
            LOG(LOGL_ERROR, "GerTerm() return nullptr");
            return nullptr;
        }

        node = (op == ADD) ? _ADD(node, right) : _SUB(node, right);
    }

    LOG(LOGL_DEBUG, "node from GetExpression = %p", node);
    return node;
}

Node* GetTerm(Lexeme* lexemes, size_t* pos)
{
    assert(lexemes);
    assert(pos);

    Node* node = GetFactor(lexemes, pos);
    if (!node)
    {
        LOG(LOGL_ERROR, "GerFactor() return nullptr");
        return nullptr;
    }

    while (lexemes[*pos].type == LEX_OPERATION &&
          (lexemes[*pos].value.oper == MUL || lexemes[*pos].value.oper == DIV))
    {
        Operation op = lexemes[*pos].value.oper;
        (*pos)++;
        Node* right = GetFactor(lexemes, pos);
        if (!right)
        {
            LOG(LOGL_ERROR, "GerFactor() return nullptr");
            return nullptr;
        }

        node = (op == MUL) ? _MUL(node, right) : _DIV(node, right);
    }

    return node;
}

Node* GetFactor(Lexeme* lexemes, size_t* pos)
{
    assert(lexemes);
    assert(pos);

    //printf(" at position %zu: %s (found %d)\n", *pos, lexemes[*pos].type);

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
            Node* expr = GetExpression(lexemes, pos);
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
        }
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
            return nullptr;
    }

    return nullptr;
}
