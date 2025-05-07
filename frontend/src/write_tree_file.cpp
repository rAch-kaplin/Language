#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "tree_func.h"

static void WriteTree(FILE* file, const Node* node, int indent);
static void WriteTab(int level, FILE* file);
static const char* GetOperatorStr(Operator optr);
static const char* GetOperationStr(Operation oper);

static void WriteTree(FILE* file, const Node* node, int indent)
{
    if (!node) return;

    WriteTab(indent, file);

    switch (node->type)
    {
        case OPERATOR:
            fprintf(file, "{OPERATOR:\"%s\"\n", GetOperatorStr(node->value.optr));
            break;
        case OPERATION:
            fprintf(file, "{OPERATION:\"%s\"\n", GetOperationStr(node->value.oper));
            break;
        case VAR:
            fprintf(file, "{VAR:\"%zu\"}\n", node->value.var);
            return;
        case NUM:
            fprintf(file, "{NUM:\"%lg\"}\n", node->value.num);
            return;
        default:
            return;
    }

    WriteTree(file, node->left, indent + 1);
    WriteTree(file, node->right, indent + 1);

    WriteTab(indent, file);
    fprintf(file, "}\n");
}

static void WriteTab(int level, FILE* file)
{
    for (int i = 0; i < level; i++)
    {
        fprintf(file, "\t");
    }
}

static const char* GetOperatorStr(Operator optr)
{
    switch (optr)
    {
        case OP_SEMICOLON: return ";";
        case OP_IF:        return "if";
        case OP_WHILE:     return "while";
        case OP_PRINT:     return "print";
        case OP_SCAN:      return "input";
        case OP_ASSIGN:    return "=";
        default:           return "unknown";
    }
}

static const char* GetOperationStr(Operation oper)
{
    switch (oper)
    {
        case ADD:       return "+";
        case SUB:       return "-";
        case MUL:       return "*";
        case DIV:       return "/";
        case EQ:        return "==";
        case NEQ:       return "!=";
        case LT:        return "<";
        case LE:        return "<=";
        case GT:        return ">";
        case GE:        return ">=";
        default:        return "unknown";
    }
}

void SaveTreeToFile(const char* filename, const Node* root)
{
    FILE* file = fopen(filename, "w");
    if (!file)
    {
        fprintf(stderr, "Failed to open file");
        return;
    }

    WriteTree(file, root, 0);
    fclose(file);
}
