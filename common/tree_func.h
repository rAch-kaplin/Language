#ifndef _HTREE_FUNC
#define _HTREE_FUNC

#include "common.h"

/**************************ENUMS**************************************************************************/

typedef enum NodeType
{
    NUM,
    VAR,
    OPERATOR,
    OPERATION,
} NodeType;

typedef enum Operator
{
    OP_SEMICOLON,
    OP_ASSIGN,
    OP_IF,
    OP_WHILE,
    //OP_BREAK,
    //OP_CONTINUE,
    OP_SCAN,
    OP_PRINT,
    // OP_BLOCK,
    // OP_FUNC_CALL,
} Operator;

typedef enum Operation
{
    ADD,
    SUB,
    MUL,
    DIV,

    EQ,     // ==
    NEQ,    // !=
    LT,     // <
    LE,     // <=
    GT,     // >
    GE      // >=
} Operation;

/*************************NODE******************************************************************************/

typedef union NodeValue
{
    double num;
    size_t var;
    Operator optr;
    Operation oper;
} NodeValue;

typedef struct Node
{
    NodeType type;
    NodeValue value;

    struct Node* left;
    struct Node* right;
    struct Node* parent;
} Node;

/*************************VARIABLES************************************************************************/

typedef struct Variable
{
    char *name;
    size_t len_name;
    int value;
} Variable;

/*************************OPERATION INFO*******************************************************************/

typedef struct
{
    Operation op;
    const char* symbol;
} Operation_t;

static const Operation_t operations[] =
{
    {ADD,            "+"},
    {SUB,            "-"},
    {MUL,            "*"},
    {DIV,            "/"},
    {EQ,  "scores_equal"},
    {NEQ, "	scores_diff"},
    {LT,        "losing"},
    {LE,    "not_losing"},
    {GT,       "leading"},
    {GE,    "not_behind"},
};

const size_t size_of_operations = sizeof(operations) / sizeof(operations[0]);

/*************************OPERATOR INFO********************************************************************/

typedef struct
{
    Operator optr;
    const char* keyword;
} Operator_t;

static const Operator_t operators[] =
{
    {OP_SEMICOLON,         ";"},
    {OP_ASSIGN,            "="},
    {OP_IF,            "block"},
    {OP_WHILE,         "rally"},
    //{OP_BREAK,       "timeout"},
    //{OP_CONTINUE,     "rotate"},
    {OP_PRINT,         "serve"},
    {OP_SCAN,        "receive"},
};

const size_t size_of_operators = sizeof(operators) / sizeof(operators[0]);


/***********************************************************************************************************/

Node* NewNode(NodeType type, NodeValue value, Node* left, Node* right);
NodeType DetectNodeType(const char *str);

CodeError TreeDumpDot (Node* root);
CodeError TreeDumpDot2(Node* root);

void SaveTreeToFile(const char* filename, const Node* root);
void FreeTree(Node** node);

size_t LookupVar  (Variable *vars_table, const char* name, size_t len_name);
size_t AddVartable(Variable *vars_table, const char* name, size_t len_name);
Variable* GetVarsTable();
void FreeVarsTable();

#endif // _HTREE_FUNC
