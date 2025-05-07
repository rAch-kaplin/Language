#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

#include "tree_func.h"
#include "file_read.h"
#include "logger.h"

const size_t MAX_LEN_BUF = 4856;

static CodeError ParseNode                  (Node** node, char** buffer);
static CodeError HandleOperatorNode         (Node** node, char** buffer);
static CodeError HandleOperationNode        (Node** node, char** buffer);
static CodeError HandleVarNode              (Node** node, char** buffer);
static CodeError HandleNumNode              (Node** node, char** buffer);

static CodeError SkipWhitespace         (char** buffer);
static NodeType  DetectType             (const char* str);
static Operator GetOperatorFromStr      (const char* str);
static Operation GetOperationFromStr    (const char* str);

Node* LoadTreeFromFile(const char* filename)
{
    size_t file_size = 0;
    char* buffer = ReadProgramToBuffer(filename, &file_size);
    if (!buffer)
    {
        LOG(LOGL_ERROR, "Failed to read file: %s\n", filename);
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
    CodeError err = SkipWhitespace(buffer);
    if (err != OK) return err;

    if (**buffer != '{')
    {
        LOG(LOGL_ERROR, "Expected '{', got '%c'\n", **buffer);
        return INVALID_FORMAT;
    }
    (*buffer)++;

    err = SkipWhitespace(buffer);
    if (err != OK) return err;

    char type_str[MAX_LEN_BUF] = {};
    if (sscanf(*buffer, "%[^:]", type_str) != 1)
    {
        LOG(LOGL_ERROR, "Failed to read node type\n");
        return INVALID_FORMAT;
    }
    *buffer += strlen(type_str);

    err = SkipWhitespace(buffer);
    if (err != OK) return err;

    if (**buffer != ':')
    {
        LOG(LOGL_ERROR, "Expected ':', got '%c'\n", **buffer);
        return INVALID_FORMAT;
    }
    (*buffer)++;

    err = SkipWhitespace(buffer);
    if (err != OK) return err;

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

    NodeValue value;
    value.optr = GetOperatorFromStr(op_str);
    *node = NewNode(OPERATOR, value, nullptr, nullptr);
    if (!*node) return MEM_ALLOC_FAIL;

    CodeError err = SkipWhitespace(buffer);
    if (err != OK) return err;

    if (**buffer == '{')
    {
        err = ParseNode(&((*node)->left), buffer);
        if (err != OK) return err;

        err = SkipWhitespace(buffer);
        if (err != OK) return err;

        if (**buffer == '{')
        {
            err = ParseNode(&((*node)->right), buffer);
            if (err != OK) return err;
        }
    }

    err = SkipWhitespace(buffer);
    if (err != OK) return err;

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

    NodeValue value;
    value.oper = GetOperationFromStr(oper_str);
    *node = NewNode(OPERATION, value, nullptr, nullptr);
    if (!*node) return MEM_ALLOC_FAIL;

    CodeError err = SkipWhitespace(buffer);
    if (err != OK) return err;

    if (**buffer == '{')
    {
        err = ParseNode(&((*node)->left), buffer);
        if (err != OK) return err;

        err = SkipWhitespace(buffer);
        if (err != OK) return err;

        if (**buffer == '{')
        {
            err = ParseNode(&((*node)->right), buffer);
            if (err != OK) return err;
        }
    }

    err = SkipWhitespace(buffer);
    if (err != OK) return err;

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
        LOG(LOGL_ERROR, "Failed to read variable\n");
        return INVALID_FORMAT;
    }
    *buffer += strlen(var_str);

    if (**buffer != '"')
    {
        LOG(LOGL_ERROR, "Expected '\"', got '%c'\n", **buffer);
        return INVALID_FORMAT;
    }
    (*buffer)++;

    NodeValue value;
    value.var = (size_t)atoi(var_str);
    *node = NewNode(VAR, value, nullptr, nullptr);
    if (!*node) return MEM_ALLOC_FAIL;

    CodeError err = SkipWhitespace(buffer);
    if (err != OK) return err;

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

    NodeValue value;
    value.num = atof(num_str);
    *node = NewNode(NUM, value, nullptr, nullptr);
    if (!*node) return MEM_ALLOC_FAIL;

    CodeError err = SkipWhitespace(buffer);
    if (err != OK) return err;

    if (**buffer != '}')
    {
        LOG(LOGL_ERROR, "Expected '}', got '%c'\n", **buffer);
        return INVALID_FORMAT;
    }
    (*buffer)++;

    return OK;
}

static CodeError SkipWhitespace(char** buffer)
{
    while (isspace(**buffer))
    {
        (*buffer)++;
        if (**buffer == '\0') return INVALID_FORMAT;
    }
    return OK;
}

static NodeType DetectType(const char* str)
{
    if (strcmp(str, "OPERATOR") == 0)  return OPERATOR;
    if (strcmp(str, "OPERATION") == 0) return OPERATION;
    if (strcmp(str, "VAR") == 0) return VAR;
    if (strcmp(str, "NUM") == 0) return NUM;

    LOG(LOGL_ERROR, "Unknown node type: %s\n", str);
    return NUM; //FIXME
}

static Operator GetOperatorFromStr(const char* str)
{
    for (size_t i = 0; i < size_of_operators; i++)
    {
        if (strcmp(str, operators[i].keyword) == 0)
        {
            return operators[i].optr;
        }
    }

    LOG(LOGL_ERROR, "Unknown operator: %s\n", str);
    return OP_SEMICOLON;
}

static Operation GetOperationFromStr(const char* str)
{
    for (size_t i = 0; i < size_of_operations; i++)
    {
        if (strcmp(str, operations[i].symbol) == 0)
        {
            return operations[i].op;
        }
    }

    LOG(LOGL_ERROR, "Unknown operation: %s\n", str);
    return ADD;  //FIXME
}
