#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "tree_func.h"
#include "file_read.h"
#include "logger.h"
#include "lexical_analysis.h"
#include "syntax_analysis.h"

Node* NewNode(NodeType type, NodeValue value, Node* left, Node* right)
{
    Node* node = (Node*)calloc(1, sizeof(Node));
    if (!node) return nullptr;

    node->type = type;
    node->value = value;

    node->left = left;
    node->right = right;

    if (left)  left->parent  = node;
    if (right) right->parent = node;

    return node;
}

#if 0
CodeError CreateNode(Node **dest, const char *data, Node *parent)
{
    if (dest == nullptr || data == nullptr)
        return INVALID_ARGUMENT;

    Node* node = (Node*)calloc(1, sizeof(Node));
    if (!node)
        return MEM_ALLOC_FAIL;

    NodeType type = DetectNodeType(data);
    NodeValue value = {};

    switch (type)
    {
        case OP:
        {
            bool found = false;
            for (size_t i = 0; i < sizeof(operations) / sizeof(operations[0]); i++)
            {
                if (strcmp(data, operations[i].symbol) == 0)
                {
                    value.op = operations[i].op;
                    found = true;
                    break;
                }
            }
            if (!found) return INVALID_OPERATION;
            break;
        }

        case VAR:
        {
            value.var = AddVartable(GetVarsTable(), data, strlen(data));
            break; //FIXME errors overflow
        }

        case NUM:
            value.num = atof(data);
            break;

        case FUNC:
            break; //TODO

        default:
            return INVALID_NODE_TYPE;
    }

    node->type = type;
    node->value = value;
    node->left = nullptr;
    node->right = nullptr;
    node->parent = parent;

    *dest = node;

    return OK;
}
#endif

NodeType DetectNodeType(const char* str) //FIXME
{
    if (!str || strlen(str) == 0)
        return VAR;

    for (size_t i = 0; i < size_of_operators; i++)
    {
        if (strcmp(str, operators[i].keyword) == 0)
            return OPERATOR;
    }

    for (size_t i = 0; i < size_of_operations; i++)
    {
        if (strcmp(str, operations[i].symbol) == 0)
            return OPERATION;
    }

    bool is_number = true;
    size_t len = strlen(str);
    size_t start = (str[0] == '-') ? 1 : 0;

    for (size_t i = start; i < len; i++)
    {
        if (!isdigit(str[i]) && str[i] != '.')
        {
            is_number = false;
            break;
        }
    }

    if (is_number)
        return NUM;

    return VAR;
}


void FreeTree(Node** node)
{
    if (! node || ! *node) return;

    FreeTree(&(*node)->left);
    FreeTree(&(*node)->right);
    free(*node);
    *node = nullptr;
}
