#ifndef _HTREE_FUNC
#define _HTREE_FUNC

enum CodeError
{
    OK                  = 0x00,
    INVALID_ARGUMENT    = 0x01,
    INVALID_FORMAT      = 0x02,
    MEM_ALLOC_FAIL      = 0x03,
    FILE_NOT_OPEN       = 0x04,
    INVALID_OPERATION   = 0x05,
    INVALID_NODE_TYPE   = 0x06,
};

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
    OP_ASSIGN,
    OP_IF,
    OP_WHILE,
    // OP_BLOCK,
    // OP_FUNC_CALL,
} Operator;

typedef enum Operation
{
    ADD,
    SUB,
    MUL,
    DIV,
    POW,

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
} Variable;

/*************************OPERATION INFO*******************************************************************/

typedef struct
{
    Operation op;
    const char* symbol;
} Operation_t;

static const Operation_t operations[] =
{
    {ADD,  "+"},
    {SUB,  "-"},
    {MUL,  "*"},
    {DIV,  "/"},
    {POW,  "^"},
    {EQ,  "=="},
    {NEQ, "!="},
    {LT,   "<"},
    {LE,  "<="},
    {GT,   ">"},
    {GE,  ">="}
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
    {OP_ASSIGN, "="},
    {OP_IF,     "if"},
    {OP_WHILE,  "while"}
};

const size_t size_of_operators = sizeof(operators) / sizeof(operators[0]);


/***********************************************************************************************************/

Node* NewNode(NodeType type, NodeValue value, Node* left, Node* right);
//CodeError CreateNode(Node **dest, const char *data, Node *parent);
NodeType DetectNodeType(const char *str);

// CodeError TreeDumpDot (Node* root);
// CodeError TreeDumpDot2(Node* root);

void FreeTree(Node** node);

#endif // _HTREE_FUNC
