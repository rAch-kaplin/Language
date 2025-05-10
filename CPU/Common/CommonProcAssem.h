#ifndef _HCOMMON_PROC_ASSEM
#define _HCOMMON_PROC_ASSEM

enum CodeError
{
    FILE_NOT_OPEN = 10,
    UNKNOW_COMMAND = 20,
    UNKNOW_LABEL = 30,
    FILE_READ_ERROR = 40,
    ASSEM_ERROR = 50,
    ARG_TYPE_ERROR = 60,
    ITS_OK = 0
};

enum command
{
    CMD_PUSH =      1,
    CMD_ADD =       2,
    CMD_SUB =       3,
    CMD_OUT =       4,
    CMD_DIV =       5,
    CMD_MUL =       6,

    CMD_POP =      12,

    CMD_JMP =      21,
    CMD_SQRT =     31,
    CMD_SIN =      32,
    CMD_COS =      33,

    CMD_JB =       41,
    CMD_JBE =      42,
    CMD_JA =       43,
    CMD_JAE =      44,
    CMD_JE =       45,
    CMD_JNE =      46,

    CMD_FUNC =     50,
    CMD_RET =      51,

    CMD_IN =       60,

    CMD_HLT =      -1
};

enum reg_t
{
    ax = 0,
    bx = 1,
    cx = 2,
    dx = 3
};
//FIXME: enum uppercase

enum type_arg
{
    digit =          1,
    regist =         2,
    memory =         6,
    complex_memory = 7,
};

typedef struct
{
    const char *reg_name;
    int number_reg;
} Registers;

typedef struct
{
    const char *cmd_name;
    const int cmd_code;
    int quantity_args;
} CommandCode;

enum num_of_args
{
    ZERO_ARG  = 0,
    ONE_ARG   = 1,
    TWO_ARG   = 2,
    THREE_ARG = 3,
};

static const CommandCode command_code[] = { {"push",    CMD_PUSH,  TWO_ARG },
                                            {"add",     CMD_ADD,   ZERO_ARG},
                                            {"sub",     CMD_SUB,   ZERO_ARG},
                                            {"out",     CMD_OUT,   ZERO_ARG},
                                            {"div",     CMD_DIV,   ZERO_ARG},
                                            {"mul",     CMD_MUL,   ZERO_ARG},
                                            {"hlt",     CMD_HLT,   ZERO_ARG},
                                            {"pop",     CMD_POP,   TWO_ARG },
                                            {"jmp",     CMD_JMP,   ONE_ARG },
                                            {"sqrt",    CMD_SQRT,  ZERO_ARG},
                                            {"sin",     CMD_SIN,   ZERO_ARG},
                                            {"cos",     CMD_COS,   ZERO_ARG},
                                            {"jb",      CMD_JB,    ONE_ARG },
                                            {"jbe",     CMD_JBE,   ONE_ARG },
                                            {"ja",      CMD_JA,    ONE_ARG },
                                            {"jae",     CMD_JAE,   ONE_ARG },
                                            {"je",      CMD_JE,    ONE_ARG },
                                            {"jne",     CMD_JNE,   ONE_ARG },
                                            {"call",    CMD_FUNC,  ONE_ARG },
                                            {"ret",     CMD_RET,   ZERO_ARG},
                                            {"in",      CMD_IN,    ZERO_ARG}};

int CompileArg(const char *str);

#endif //HCOMMON_PROC_ASSEM
