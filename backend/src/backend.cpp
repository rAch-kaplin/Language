#include "backend.h"
#include "logger.h"
#include "common.h"

const size_t asm_buffer_size = 16048;

#define _WRITE_ASM(asm_file_ptr, ...)                                                           \
    do {                                                                                        \
        if ((size_t)(asm_file_ptr)->cur_len < asm_buffer_size)                                  \
        {                                                                                       \
            int _written = snprintf((asm_file_ptr)->asm_buffer + (asm_file_ptr)->cur_len,       \
                                  asm_buffer_size - (size_t)(asm_file_ptr)->cur_len,            \
                                  __VA_ARGS__);                                                 \
            if (_written > 0) (asm_file_ptr)->cur_len += _written;                              \
        }                                                                                       \
    } while (0)

static CodeError    TranslateOperator       (const Node *node, AsmFile *asm_file);
static void         TranslateIf             (const Node *node, AsmFile *asm_file);
static void         TranslateWhile          (const Node *node, AsmFile *asm_file);
static void         TranslateScan           (const Node *node, AsmFile *asm_file);
static void         TranslatePrint          (const Node *node, AsmFile *asm_file);
static void         TranslateExpression     (const Node *node, AsmFile *asm_file);
static void         TranslatePushNodeValue  (const Node *node, AsmFile *asm_file);
static void         TranslatePopVar         (const Node *node, AsmFile *asm_file);
static void         TranslateAssign         (const Node *node, AsmFile *asm_file);

//TODO add logs

void FreeAsmFile(AsmFile* asm_file)
{
    if (!asm_file) return;

    if (asm_file->asm_buffer)
    {
        free(asm_file->asm_buffer);
        asm_file->asm_buffer = nullptr;
    }
}

CodeError AssemblyTree(const Node *root, const char *file_asm)
{
    assert(root);

    AsmFile asm_file = {};
    asm_file.asm_buffer = (char*)calloc(asm_buffer_size, sizeof(char));
    if (asm_file.asm_buffer == nullptr)
    {
        LOG(LOGL_ERROR, "Memory allocate error");
        return MEM_ALLOC_FAIL;
    }

    TranslateOperator(root, &asm_file);

    FILE *fp = fopen(file_asm, "w+");
    fwrite(asm_file.asm_buffer, asm_buffer_size, sizeof(char), fp);

    FreeAsmFile(&asm_file);

    return OK;
}

CodeError TranslateOperator(const Node *node, AsmFile *asm_file)
{
    assert(node);
    assert(asm_file);

    if (node->type == OPERATOR)
    {
        switch (node->value.optr)
        {
            case OP_SEMICOLON:
            {
                TranslateOperator(node->left, asm_file);
                TranslateOperator(node->left, asm_file);
                break; //TODO handle errors
            }
            case OP_IF:
            {
                TranslateIf(node, asm_file);
                break;
            }
            case OP_WHILE:
            {
                TranslateWhile(node, asm_file);
                break;
            }
            case OP_PRINT:
            {
                TranslatePrint(node, asm_file);
                break;
            }
            case OP_SCAN:
            {
                TranslateScan(node, asm_file);
                break;
            }
            case OP_ASSIGN:
            {
                TranslateAssign(node, asm_file);
                break;
            }

            default:
                break; //FIXME
        }
    }

    return OK;
}

void TranslateIf(const Node *node, AsmFile *asm_file)
{
    assert(node);
    assert(asm_file);

    TranslateExpression(node->left->right, asm_file);
    TranslateExpression(node->left->left , asm_file);

    switch (node->left->value.oper)
    {
        case EQ:
            _WRITE_ASM(asm_file, "je endif_%zu:\n", asm_file->endif);
            break;
        case NEQ:
           _WRITE_ASM(asm_file, "jne endif_%zu:\n", asm_file->endif);
            break;
        case LT:
            _WRITE_ASM(asm_file, "jb endif_%zu:\n", asm_file->endif);
            break;
        case LE:
            _WRITE_ASM(asm_file, "jbe endif_%zu:\n", asm_file->endif);
            break;
        case GT:
            _WRITE_ASM(asm_file, "ja endif_%zu:\n", asm_file->endif);
            break;
        case GE:
            _WRITE_ASM(asm_file, "jae endif_%zu:\n", asm_file->endif);
            break;

        case SUB:
        case ADD:
        case MUL:
        case DIV:
            break;
        default:
            fprintf(stderr, "Unknow operation");
            return;
    }

    TranslateOperator(node->right, asm_file);
    (asm_file->endif)++;
}

void TranslateWhile(const Node *node, AsmFile *asm_file)
{
    assert(node);
    assert(asm_file);

    size_t while_counter = 0;

    _WRITE_ASM(asm_file, "while_start_%zu:\n", while_counter);

    TranslateExpression(node->left , asm_file);

    _WRITE_ASM(asm_file, "endwhile_%zu:\n", asm_file->endwhile);

    TranslateOperator(node->right, asm_file);

    _WRITE_ASM(asm_file, "jmp while_start%zu:\n", while_counter);
    _WRITE_ASM(asm_file, "endwhile_%zu:\n"      , while_counter);

    while_counter++;
}

void TranslateScan(const Node *node, AsmFile *asm_file)
{
    assert(node);
    assert(asm_file);

    _WRITE_ASM(asm_file, "in\n");
}

void TranslatePrint(const Node *node, AsmFile *asm_file)
{
    assert(node);
    assert(asm_file);

    TranslateExpression(node->left, asm_file);
    _WRITE_ASM(asm_file, "out\n");
}

void TranslateExpression(const Node *node, AsmFile *asm_file)
{
    assert(node);
    assert(asm_file);

    if (node->type == OPERATION)
    {
        TranslateExpression(node->left,  asm_file);
        TranslateExpression(node->right, asm_file);

        switch (node->value.oper)
        {
            case ADD:
                _WRITE_ASM(asm_file, "add\n");
                break;
            case SUB:
                _WRITE_ASM(asm_file, "sub\n");
                break;
            case MUL:
                _WRITE_ASM(asm_file, "mul\n");
                break;
            case DIV:
                _WRITE_ASM(asm_file, "div\n");
                break;

            case EQ:
            case NEQ:
            case LT:
            case LE:
            case GT:
            case GE:
                break;
            default:
                fprintf(stderr, "ERROR: Unknown operation in expression\n");
                break;
        }
    }
    else
    {
        TranslatePushNodeValue(node, asm_file);
    }
}

void TranslatePushNodeValue(const Node *node, AsmFile *asm_file)
{
    assert(node);
    assert(asm_file);

    _WRITE_ASM(asm_file, "push ");

    if (node->type == VAR)
    {
        Variable* vars_table = GetVarsTable();

        _WRITE_ASM(asm_file, "[%d]\n", vars_table[node->value.var].value);
    }
    else if (node->type == NUM)
    {
        _WRITE_ASM(asm_file, "%d\n", (int)node->value.num);
    }
    else
    {
        fprintf(stderr, "ERROR: Unknown node type in push\n");
    }
}

void TranslatePopVar(const Node *node, AsmFile *asm_file)
{
    assert(node);
    assert(asm_file);

    if (node->type != VAR)
    {
        fprintf(stderr, "ERROR: Variable node expected for POP\n");
        return;
    }

    Variable* vars_table = GetVarsTable();

    _WRITE_ASM(asm_file, "pop [%d]\n", vars_table[node->value.var].value);
}

void TranslateAssign(const Node *node, AsmFile *asm_file)
{
    assert(node);
    assert(asm_file);

    TranslateExpression(node->right, asm_file);
    TranslatePopVar    (node->left, asm_file);
}

