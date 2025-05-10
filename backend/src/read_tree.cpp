#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

#include "tree_func.h"
#include "file_read.h"
#include "logger.h"
#include "DSL.h"

const size_t MAX_LEN_BUF = 4856;

static CodeError ParseNode                  (Node** node, char** buffer);
static CodeError HandleOperatorNode         (Node** node, char** buffer);
static CodeError HandleOperationNode        (Node** node, char** buffer);
static CodeError HandleVarNode              (Node** node, char** buffer);
static CodeError HandleNumNode              (Node** node, char** buffer);

static void      SkipSpace              (char** buffer);
static NodeType  DetectType             (const char* str);
static Operator  GetOperatorFromStr     (const char* str);
static Operation GetOperationFromStr    (const char* str);

void FixParents(Node* node)
{
    assert(node);

    if (node->left)  (node->left)->parent  = node;
    if (node->right) (node->right)->parent = node;
}

void FixTree(Node* node)
{
    LOG(LOGL_DEBUG, "FixParent node: %p", node);
    if (!node) return;

    FixParents(node);

    if (node->left)  FixTree(node->left);
    if (node->right) FixTree(node->right);
}

Node* LoadTreeFromFile(const char* filename)
{
    size_t file_size = 0;
    char* buffer = ReadProgramToBuffer(filename, &file_size);
    if (!buffer)
    {
        LOG(LOGL_ERROR, "Failed to read file: %s", filename);
        return nullptr;
    }

    Node* root = nullptr;
    char* buffer_ptr = buffer;
    CodeError err = ParseNode(&root, &buffer_ptr);

    free(buffer);

    if (err != OK)
    {
        FreeTree(&root);
        return nullptr;
    }

    return root;
}

static CodeError ParseNode(Node** node, char** buffer)
{
    SkipSpace(buffer);

    if (**buffer != '{')
    {
        LOG(LOGL_ERROR, "Expected '{', got '%c'", **buffer);
        return INVALID_FORMAT;
    }
    (*buffer)++;

    SkipSpace(buffer);

    char type_str[MAX_LEN_BUF] = {};
    if (sscanf(*buffer, "%[^:]", type_str) != 1)
    {
        LOG(LOGL_ERROR, "Failed to read node type\n");
        return INVALID_FORMAT;
    }
    *buffer += strlen(type_str);

    SkipSpace(buffer);

    if (**buffer != ':')
    {
        LOG(LOGL_ERROR, "Expected ':', got '%c'\n", **buffer);
        return INVALID_FORMAT;
    }
    (*buffer)++;

    SkipSpace(buffer);

    if (**buffer != '"')
    {
        LOG(LOGL_ERROR, "Expected '\"', got '%c'\n", **buffer);
        return INVALID_FORMAT;
    }
    (*buffer)++;

    NodeType type = DetectType(type_str);
    switch (type)
    {
        case OPERATOR:
            return HandleOperatorNode(node, buffer);
        case OPERATION:
            return HandleOperationNode(node, buffer);
        case VAR:
            return HandleVarNode(node, buffer);
        case NUM:
            return HandleNumNode(node, buffer);
        default:
            LOG(LOGL_ERROR, "Unknown node type: %s\n", type_str);
            return INVALID_FORMAT;
    }
}

static CodeError HandleOperatorNode(Node** node, char** buffer)
{
    char op_str[MAX_LEN_BUF] = {};
    if (sscanf(*buffer, "%[^\"]", op_str) != 1)
    {
        LOG(LOGL_ERROR, "Failed to read operator\n");
        return INVALID_FORMAT;
    }
    *buffer += strlen(op_str);

    if (**buffer != '"')
    {
        LOG(LOGL_ERROR, "Expected '\"', got '%c'\n", **buffer);
        return INVALID_FORMAT;
    }
    (*buffer)++;

    NodeValue value = {};
    value.optr = GetOperatorFromStr(op_str);
    *node = NewNode(OPERATOR, value, nullptr, nullptr);
    if (!*node) return MEM_ALLOC_FAIL;

    SkipSpace(buffer);

    if (**buffer == '{')
    {
        CodeError err = ParseNode(&((*node)->left), buffer);
        if (err != OK) return err;

        SkipSpace(buffer);

        if (**buffer == '{')
        {
            err = ParseNode(&((*node)->right), buffer);
            if (err != OK) return err;
        }
    }

    SkipSpace(buffer);

    if (**buffer != '}')
    {
        LOG(LOGL_ERROR, "Expected '}', got '%c'\n", **buffer);
        return INVALID_FORMAT;
    }
    (*buffer)++;

    return OK;
}

static CodeError HandleOperationNode(Node** node, char** buffer)
{
    char oper_str[MAX_LEN_BUF] = {};
    if (sscanf(*buffer, "%[^\"]", oper_str) != 1)
    {
        LOG(LOGL_ERROR, "Failed to read operation\n");
        return INVALID_FORMAT;
    }
    *buffer += strlen(oper_str);

    if (**buffer != '"')
    {
        LOG(LOGL_ERROR, "Expected '\"', got '%c'\n", **buffer);
        return INVALID_FORMAT;
    }
    (*buffer)++;

    NodeValue value = {};;
    value.oper = GetOperationFromStr(oper_str);
    *node = NewNode(OPERATION, value, nullptr, nullptr);
    if (!*node) return MEM_ALLOC_FAIL;

    SkipSpace(buffer);

    if (**buffer == '{')
    {
        CodeError err = ParseNode(&((*node)->left), buffer);
        if (err != OK) return err;

        SkipSpace(buffer);

        if (**buffer == '{')
        {
            err = ParseNode(&((*node)->right), buffer);
            if (err != OK) return err;
        }
    }

    SkipSpace(buffer);

    if (**buffer != '}')
    {
        LOG(LOGL_ERROR, "Expected '}', got '%c'\n", **buffer);
        return INVALID_FORMAT;
    }
    (*buffer)++;

    return OK;
}

static CodeError HandleVarNode(Node** node, char** buffer)
{
    char var_str[MAX_LEN_BUF] = {};
    if (sscanf(*buffer, "%[^\"]", var_str) != 1)
    {
        LOG(LOGL_ERROR, "Failed to read variable name\n");
        return INVALID_FORMAT;
    }
    *buffer += strlen(var_str);

    if (**buffer != '"')
    {
        LOG(LOGL_ERROR, "Expected '\"', got '%c'\n", **buffer);
        return INVALID_FORMAT;
    }
    (*buffer)++;

    Variable* vars_table = GetVarsTable();
    size_t var_pos = AddVartable(vars_table, var_str, strlen(var_str));

    NodeValue value = {};;
    value.var = var_pos;
    *node = NewNode(VAR, value, nullptr, nullptr);
    if (!*node) return MEM_ALLOC_FAIL;

    SkipSpace(buffer);

    if (**buffer != '}')
    {
        LOG(LOGL_ERROR, "Expected '}', got '%c'\n", **buffer);
        return INVALID_FORMAT;
    }
    (*buffer)++;

    return OK;
}

static CodeError HandleNumNode(Node** node, char** buffer)
{
    char num_str[MAX_LEN_BUF] = {};
    if (sscanf(*buffer, "%[^\"]", num_str) != 1)
    {
        LOG(LOGL_ERROR, "Failed to read number\n");
        return INVALID_FORMAT;
    }
    *buffer += strlen(num_str);

    if (**buffer != '"')
    {
        LOG(LOGL_ERROR, "Expected '\"', got '%c'\n", **buffer);
        return INVALID_FORMAT;
    }
    (*buffer)++;

    NodeValue value = {};;
    value.num = atof(num_str);
    *node = NewNode(NUM, value, nullptr, nullptr);
    if (!*node) return MEM_ALLOC_FAIL;

    SkipSpace(buffer);

    if (**buffer != '}')
    {
        LOG(LOGL_ERROR, "Expected '}', got '%c'\n", **buffer);
        return INVALID_FORMAT;
    }
    (*buffer)++;

    return OK;
}

static void SkipSpace(char** buffer)
{
    while (isspace(**buffer)) (*buffer)++;
}

static NodeType DetectType(const char* str)
{
    if (strcmp(str, "OPERATOR")  == 0)  return OPERATOR;
    if (strcmp(str, "OPERATION") == 0)  return OPERATION;
    if (strcmp(str, "VAR")       == 0)  return VAR;
    if (strcmp(str, "NUM")       == 0)  return NUM;

    LOG(LOGL_ERROR, "Unknown node type: %s", str);
    return NUM; //FIXME
}

static Operator GetOperatorFromStr(const char* str) //FIXME
{
    if (strcmp(str, ";")     == 0)     return OP_SEMICOLON;
    if (strcmp(str, "if")    == 0)     return OP_IF;
    if (strcmp(str, "while") == 0)     return OP_WHILE;
    if (strcmp(str, "print") == 0)     return OP_PRINT;
    if (strcmp(str, "input") == 0)     return OP_SCAN;
    if (strcmp(str, "=")     == 0)     return OP_ASSIGN;

    LOG(LOGL_ERROR, "Unknown operator: %s\n", str);
    return OP_SEMICOLON;
}

static Operation GetOperationFromStr(const char* str)
{
    if (strcmp(str, "+")   == 0)  return ADD;
    if (strcmp(str, "-")   == 0)  return SUB;
    if (strcmp(str, "*")   == 0)  return MUL;
    if (strcmp(str, "/")   == 0)  return DIV;
    if (strcmp(str, "==")  == 0)  return EQ;
    if (strcmp(str, "!=")  == 0)  return NEQ;
    if (strcmp(str, "<")   == 0)  return LT;
    if (strcmp(str, "<=")  == 0)  return LE;
    if (strcmp(str, ">")   == 0)  return GT;
    if (strcmp(str, ">=")  == 0)  return GE;

    LOG(LOGL_ERROR, "Unknown operation: %s\n", str);
    return ADD;  // FIXME
}

