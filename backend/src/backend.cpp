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

static CodeError    TranslateOperator       (const Node *node, NameTable *name_table, AsmFile *asm_file);
static void         TranslateIf             (const Node *node, NameTable *name_table, AsmFile *asm_file);
static void         TranslateWhile          (const Node *node, NameTable *name_table, AsmFile *asm_file);
static void         TranslateExpression     (const Node *node, NameTable *name_table, AsmFile *asm_file);
static void         TranslateAssign         (const Node *node, NameTable *name_table, AsmFile *asm_file);
static void         TranslatePrint          (const Node *node, NameTable *name_table, AsmFile *asm_file);
static void         TranslatePopVar         (const Node *node, AsmFile *asm_file, NameTable *name_table);
static void         TranslatePushNodeValue  (const Node *node, AsmFile *asm_file);
static void         TranslateScan           (const Node *node, AsmFile *asm_file);
static void         TranslateSqrt           (const Node *node, NameTable *name_table, AsmFile *asm_file);

static void         TranslateFuncDef        (const Node *node, NameTable *name_table, AsmFile *asm_file);
static void         TranslateFuncCall       (const Node *node, NameTable *name_table, AsmFile *asm_file);

void FreeAsmFile(AsmFile* asm_file);
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

CodeError AssemblyTree(const Node *root, NameTable *name_table, const char *file_asm)
{
    assert(root);

    AsmFile asm_file = {};
    asm_file.asm_buffer = (char*)calloc(asm_buffer_size, sizeof(char));
    if (asm_file.asm_buffer == nullptr)
    {
        LOG(LOGL_ERROR, "Memory allocate error");
        return MEM_ALLOC_FAIL;
    }

    _DLOG("Translate OPERATOR");
    _WRITE_ASM(&asm_file, "\ncall volleyball:\n");
    _WRITE_ASM(&asm_file, "hlt\n\n");
    TranslateOperator(root, name_table, &asm_file);

    FILE *fp = fopen(file_asm, "wt");
    fprintf(fp, "%s\n", asm_file.asm_buffer);

    FreeAsmFile(&asm_file);

    return OK;
}

CodeError TranslateOperator(const Node *node, NameTable *name_table, AsmFile *asm_file)
{
    assert(asm_file);  //FIXME

    if (node == nullptr) return OK;

    if (node->type == OPERATOR)
    {
        switch (node->value.optr)
        {
            case OP_SEMICOLON:
            {
                _DLOG("case OP_SEMICOLON");
                TranslateOperator(node->left,  name_table, asm_file);
                TranslateOperator(node->right, name_table, asm_file);
                break; //TODO handle errors
            }
            case OP_IF:
            {
                _DLOG("case OP_IF");
                TranslateIf(node, name_table, asm_file);
                break;
            }
            case OP_WHILE:
            {
                _DLOG("case OP_WHILE");
                TranslateWhile(node, name_table, asm_file);
                break;
            }
            case OP_PRINT:
            {
                _DLOG("case OP_PRINT"); //FIXME _D и __ нельзя
                TranslatePrint(node, name_table, asm_file);
                break;
            }
            case OP_SCAN:
            {
                _DLOG("case OP_SCAN");
                TranslateScan(node, asm_file);
                break;
            }
            case OP_ASSIGN:
            {
                _DLOG("case OP_ASSIGN");
                TranslateAssign(node, name_table, asm_file);
                break;
            }
            case OP_FUNC_DEF:
            {
                _DLOG("case OP_FUNC_DEF");
                TranslateFuncDef(node, name_table, asm_file);
                break;
            }
            case OP_FUNC_CALL:
            {
                _DLOG("case OP_FUNC_CALL");
                TranslateFuncCall(node, name_table, asm_file);
                break;
            }

            default:
                break; //FIXME
        }
    }

    return OK;
}

void TranslateIf(const Node *node, NameTable *name_table, AsmFile *asm_file)
{
    assert(node);
    assert(asm_file);

    size_t if_id = asm_file->endif++;

    TranslateExpression(node->left->right, name_table, asm_file);
    TranslateExpression(node->left->left,  name_table, asm_file);

    _WRITE_ASM(asm_file, "\n;===============  IF  ============== \n");

    switch (node->left->value.oper)
    {
        case EQ:  _WRITE_ASM(asm_file, "jne if_skip_block_%zu:\n",  if_id); break;
        case NEQ: _WRITE_ASM(asm_file, "je if_skip_block_%zu:\n",   if_id); break;
        case LT:  _WRITE_ASM(asm_file, "jae if_skip_block_%zu:\n",  if_id); break;
        case LE:  _WRITE_ASM(asm_file, "ja if_skip_block_%zu:\n",   if_id); break;
        case GT:  _WRITE_ASM(asm_file, "jbe if_skip_block_%zu:\n",  if_id); break;
        case GE:  _WRITE_ASM(asm_file, "jb if_skip_block_%zu:\n",   if_id); break;

        case ADD:
        case SUB:
        case MUL:
        case DIV:
        default:
            fprintf(stderr, "Unknown operator");
            return;
    }

    TranslateOperator(node->right, name_table, asm_file);

    _WRITE_ASM(asm_file, "if_skip_block_%zu:\n", if_id);
}

void TranslateWhile(const Node *node, NameTable *name_table, AsmFile *asm_file)
{
    assert(node);
    assert(asm_file);

    size_t while_id = asm_file->endwhile++;
    _DLOG("TranslateWhile");

    _WRITE_ASM(asm_file, "while_start_%zu:\n", while_id);

    TranslateExpression(node->left->right, name_table, asm_file);
    TranslateExpression(node->left->left,  name_table, asm_file);

    _WRITE_ASM(asm_file, "\n;===============  WHILE  ============== \n");

    switch (node->left->value.oper)
    {
        case EQ:  _WRITE_ASM(asm_file, "je while_end_%zu:\n",   while_id); break;
        case NEQ: _WRITE_ASM(asm_file, "jne while_end_%zu:\n",  while_id); break;
        case LT:  _WRITE_ASM(asm_file, "jb while_end_%zu:\n",   while_id); break;
        case LE:  _WRITE_ASM(asm_file, "jbe while_end_%zu:\n",  while_id); break;
        case GT:  _WRITE_ASM(asm_file, "ja while_end_%zu:\n",   while_id); break;
        case GE:  _WRITE_ASM(asm_file, "jae while_end_%zu:\n",  while_id); break;

        case ADD:
        case SUB:
        case MUL:
        case DIV:
        default:
            fprintf(stderr, "Unknown operator in while condition");
            return;
    }

    TranslateOperator(node->right, name_table, asm_file);

    _WRITE_ASM(asm_file, "jmp while_start_%zu:\n", while_id);

    _WRITE_ASM(asm_file, "while_end_%zu:\n", while_id);
}

void TranslateScan(const Node *node, AsmFile *asm_file)
{
    assert(node);
    assert(asm_file);

    _DLOG("TranslateScan");
    _WRITE_ASM(asm_file, "\n;===============  SCAN  ============== \n");
    _WRITE_ASM(asm_file, "in\n");

    if (node->left->type != VAR)
    {
        fprintf(stderr, "ERROR: Variable node expected for POP\n");
        return;
    }

    _WRITE_ASM(asm_file, "pop [%d]\n", (int)node->left->value.var);
}

void TranslateSqrt(const Node *node, NameTable *name_table, AsmFile *asm_file)
{
    assert(node);
    assert(asm_file);

    _DLOG("TranslateSqrt");
    _WRITE_ASM(asm_file, "\n;===============  SQRT  ============== \n");

    _WRITE_ASM(asm_file, "push [%d]\n", node->left->value.var);
    _WRITE_ASM(asm_file, "sqrt\n", (int)node->left->value.var);

    if (node->left->type != VAR)
    {
        fprintf(stderr, "ERROR: Variable node expected for POP\n");
        return;
    }

    //_WRITE_ASM(asm_file, "pop [%d]\n", (int)node->left->value.var);
}

void TranslatePrint(const Node *node, NameTable *name_table, AsmFile *asm_file)
{
    assert(node);
    assert(asm_file);

    _DLOG("TranslatePrint");
    TranslateExpression(node->left, name_table, asm_file);
    _WRITE_ASM(asm_file, "\n;===============  PRINT  ============== \n");
    _WRITE_ASM(asm_file, "out\n");
}

void TranslateExpression(const Node *node, NameTable *name_table, AsmFile *asm_file)
{
    assert(node);
    assert(asm_file);

    _DLOG("TranslateExpression");
    if (node->type == OPERATION)
    {
        TranslateExpression(node->left,  name_table, asm_file);
        TranslateExpression(node->right, name_table, asm_file);

        switch (node->value.oper)
        {
            case ADD:
                _DLOG("case ADD");
                _WRITE_ASM(asm_file, "add\n");
                break;
            case SUB:
                _DLOG("case SUB");
                _WRITE_ASM(asm_file, "sub\n");
                break;
            case MUL:
                _DLOG("case MUL");
                _WRITE_ASM(asm_file, "mul\n");
                break;
            case DIV:
                _DLOG("case DIV");
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

void  TranslatePushNodeValue(const Node *node, AsmFile *asm_file)
{
    assert(node);
    assert(asm_file);

    _DLOG("TranslatePushNodeValue");

    if (node->type == VAR)
    {
        _WRITE_ASM(asm_file, "push [%d]\n", (int)node->value.var);
    }
    else if (node->type == NUM)
    {
        _WRITE_ASM(asm_file, "push %d\n", (int)node->value.num);
    }
    else
    {
        fprintf(stderr, "ERROR: Unknown node type in push\n");
    }
}

void TranslatePopVar(const Node *node, AsmFile *asm_file, NameTable *name_table)
{
    assert(node);
    assert(asm_file);

    _DLOG("TranslatePopVar");
    if (node->type != VAR)
    {
        fprintf(stderr, "ERROR: Variable node expected for POP\n");
        return;
    }

    _WRITE_ASM(asm_file, "pop [%d]\n", (int)node->value.var);
}

void TranslateAssign(const Node *node, NameTable *name_table, AsmFile *asm_file)
{
    assert(node);
    assert(asm_file);

    _DLOG("TranslateAssign");
    if (node->right->value.optr == OP_SQRT)
    {
        TranslateSqrt(node->right, name_table, asm_file);
    }
    else
    {
        TranslateExpression(node->right, name_table, asm_file);
    }

    TranslatePopVar    (node->left, asm_file, name_table);
}

void TranslateFuncDef(const Node *node, NameTable *name_table, AsmFile *asm_file)
{
    assert(node);
    assert(asm_file);

    const char* func_name = name_table->func_table[node->left->value.func].name;
    _DLOG("TranslateFuncDef %s", func_name);

    _WRITE_ASM(asm_file, "\n;=== FUNCTION DEF %s ===\n", func_name);
    _WRITE_ASM(asm_file, "%s:\n", func_name);

    TranslateOperator(node->right, name_table, asm_file);
    _WRITE_ASM(asm_file, "ret \n");
}

void TranslateFuncCall(const Node *node, NameTable *name_table, AsmFile *asm_file)
{
    assert(node);
    assert(asm_file);

    const char* func_name = name_table->func_table[node->left->value.func].name;
    _DLOG("TranslateFuncCall %s", func_name);

    _WRITE_ASM(asm_file, "\n;=== FUNCTION CALL %s ===\n", func_name);
    if (name_table->func_table[node->left->value.func].name == "volleyball")
    {
        return;
    }
    _WRITE_ASM(asm_file, "call %s:\n", func_name);
}

void TranslateRet(const Node *node, AsmFile *asm_file)
{
    assert(node);
    assert(asm_file);

    _DLOG("TranslateRet");
    _WRITE_ASM(asm_file, "\n;=== RETURN ===\n");

    if (node->left)
    {
        TranslateExpression(node->left, nullptr, asm_file);
    }

    _WRITE_ASM(asm_file, "ret\n");
}



