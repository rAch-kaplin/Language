#include "proccessor.h"
#include "CommonProcAssem.h"

const char* CommandToString(int cmd)
{
    switch (cmd)
    {
        case CMD_PUSH:    return "push";
        case CMD_ADD:     return "add";
        case CMD_SUB:     return "sub";
        case CMD_OUT:     return "out";
        case CMD_DIV:     return "div";
        case CMD_MUL:     return "mul";
        case CMD_HLT:     return "hlt";
        case CMD_POP:     return "pop";
        case CMD_JMP:     return "jmp";
        case CMD_JA:      return "ja";
        case CMD_JB:      return "jb";
        case CMD_JAE:     return "jae";
        case CMD_JBE:     return "jbe";
        case CMD_JE:      return "je";
        case CMD_JNE:     return "jne";
        case CMD_SQRT:    return "sqrt";
        case CMD_SIN:     return "sin";
        case CMD_COS:     return "cos";
        case CMD_FUNC:    return "call";
        case CMD_RET:     return "ret";
        case CMD_IN:      return "in";

        default:          return "Unknown";
    }
}

