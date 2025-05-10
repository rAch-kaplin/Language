#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <math.h>

#include "color.h"
#include "proccessor.h"
#include "stack.h"
#include "debug_proc.h"
#include "logger.h"
#include "CommonProcAssem.h"

const char* Run(stack *stk, stack *retAddrStk, CPU *proc)
{
    FillingCodeArray(proc);
    size_t count_command = sizeof(command_code) / sizeof(command_code[0]);

#if 0
    for (int i = 0; i < size_buffer; i++)
    {
        printf(COLOR_MAGENTA "%d " COLOR_RESET, proc->code[i]);
    }
    printf("\n");
#endif

    bool next = true;
    while (next)
    {
        stackElem cmd = proc->code[proc->IP];

        switch (cmd)
        {
            case CMD_FUNC:
            {
                GetProcInstruction(cmd, proc);
                ProcessingFuncs(retAddrStk, proc, true, false);
                break;
            }

            case CMD_RET:
            {
                GetProcInstruction(cmd, proc);
                ProcessingFuncs(retAddrStk, proc, false, true);
                break;
            }

            case CMD_PUSH:
            case CMD_POP:
            {
                ProcessingStackCommands(proc, stk, cmd);
                break;
            }

            case CMD_ADD:
            {
                GetProcInstruction(cmd, proc);
                TwoElemStackOperation(stk, [](stackElem val1, stackElem val2) { return val2 + val1;} );
                break;
            }

            case CMD_SUB:
            {
                GetProcInstruction(cmd, proc);
                TwoElemStackOperation(stk, [](stackElem val1, stackElem val2) { return val2 - val1;} );
                break;
            }

            case CMD_MUL:
            {
                GetProcInstruction(cmd, proc);
                TwoElemStackOperation(stk, [](stackElem val1, stackElem val2) { return val2 * val1;} );
                break;
            }

            case CMD_DIV:
            {
                GetProcInstruction(cmd, proc);
                TwoElemStackOperation(stk, [](stackElem val1, stackElem val2) { return val2 / val1;} );
                break;
                //TODO: lamda
            }

            case CMD_SQRT:
            {
                GetProcInstruction(cmd, proc);
                SingleStackOperation(stk, sqrt);
                break;
            }

            case CMD_SIN:
            {
                GetProcInstruction(cmd, proc);
                SingleStackOperation(stk, sin);
                break;
            }

            case CMD_COS:
            {
                GetProcInstruction(cmd, proc);
                SingleStackOperation(stk, cos);
                break;
            }

            case CMD_IN:
            {
                GetProcInstruction(cmd, proc);
                ProcessingInputOut(stk, true, false);
                break;
            }

            case CMD_OUT:
            {
                GetProcInstruction(cmd, proc);
                ProcessingInputOut(stk, false, true);
                break;
            }

            case CMD_JMP:
            {
                DoJmp(proc, cmd);
                break;
            }

            case CMD_JB:
            {
                CONDITIONAL_JMP(<);
                break;
            }

            case CMD_JBE:
            {
                CONDITIONAL_JMP(<=);
                break;
            }

            case CMD_JA:
            {
                CONDITIONAL_JMP(>);
                break;
            }

            case CMD_JAE:
            {
                CONDITIONAL_JMP(>=);
                break;
            }

            case CMD_JE:
            {
                CONDITIONAL_JMP(==);
                break;
            }

            case CMD_JNE:
            {
                CONDITIONAL_JMP(!=);
                break;
            }

            case CMD_HLT:
            {
                GetProcInstruction(cmd, proc);
                LOG(LOGL_DEBUG, "");
                next = false;
                break;
            }

            default:
            {
                printf(COLOR_RED "%d " COLOR_RESET, cmd);
                printf("\n");
                return "Unknow command:(";
                break;
            }
        }

        IpCounter(proc, cmd,  (int)count_command);
    }

    free(proc->code);
    return NULL;
}

void SingleStackOperation(stack *stk, double (*operation)(double))
{
    stackElem value = 0;
    LOG(LOGL_DEBUG, "");
    stackPop(stk, &value);
    double result = operation((double)value);
    LOG(LOGL_DEBUG, "sqrt = %d\n", (int)result);
    stackPush(stk, (stackElem)result);
}

void TwoElemStackOperation(stack *stk, stackElem (*operation)(stackElem val1, stackElem val2))
{
    LOG(LOGL_DEBUG, "");
    stackElem val_1 = 0, val_2 = 0;
    stackPop(stk, &val_1);
    stackPop(stk, &val_2);
    stackPush(stk, operation(val_1, val_2));
}

int FillingCodeArray(CPU *proc)
{
    FILE* bin_file = fopen("../CPU/assembler/bin_code.txt", "rb");
    if (bin_file == nullptr)
    {
        fprintf(stderr, "bin_code can't open\n");
        return 1;
    }

    size_t file_size = GetBinFileSize(bin_file);
    size_t num_elements = file_size / sizeof(int);

    proc->code = (int*)calloc(num_elements + 1, sizeof(int));
    assert(proc->code);

    size_t elements_read = fread(proc->code, sizeof(int), num_elements, bin_file);
    if (elements_read != num_elements)
    {
        fprintf(stderr, "Error: read %zu elements, expected %zu\n", elements_read, num_elements);
    }

    fclose(bin_file);
    return (int)num_elements;
}

size_t GetBinFileSize(FILE *bin_file)
{
    fseek(bin_file, 0, SEEK_END);
    long file_size = ftell(bin_file);
    rewind(bin_file);

    return (size_t)file_size;
}

void IpCounter(CPU *proc, stackElem cmd, int count_command)
{
    for (int i = 0; i < count_command; i++)
    {
        if (cmd == command_code[i].cmd_code)
        {
            switch (cmd)
            {
                case CMD_FUNC:
                case CMD_RET:
                case CMD_JMP:
                case CMD_JB:
                case CMD_JBE:
                case CMD_JA:
                case CMD_JAE:
                case CMD_JE:
                case CMD_JNE:
                    return;

                default:
                    proc->IP += 1 + command_code[i].quantity_args;
                    break;
            }
        }
    }
}

int ProcessingStackCommands(CPU *proc, stack *stk, int cmd)
{
    int TypeArg = proc->code[proc->IP + 1];
    bool IsPush = false, IsPushr = false, IsPopr = false, IsPushm = false,
         IsPopm = false, IsPushmComplex = false, IsPopmComplex = false;

    if (cmd == CMD_PUSH)
    {
        switch (TypeArg)
        {
            case digit:
                IsPush = true;
                break;
            case regist:
                IsPushr = true;
                break;
            case memory:
                IsPushm = true;
                break;
            case complex_memory:
                IsPushmComplex = true;
                break;
            default:
                break;
        }
    }

    if (IsPush)
    {
        stackElem value = proc->code[proc->IP + 2];
        GetProcInstruction(cmd, proc, value);
        LOG(LOGL_DEBUG, "");
        stackPush(stk, value);
    }

    else if (IsPushr)
    {
        stackElem value = proc->registers[proc->code[proc->IP + 2]];
        GetProcInstruction(cmd, proc, value);
        LOG(LOGL_DEBUG, "");
        stackPush(stk, value);
    }

    else if (IsPushm)
    {
        stackElem value = proc->RAM[proc->code[proc->IP + 2]];
        GetProcInstruction(cmd, proc, value);
        LOG(LOGL_DEBUG, "Pushm ");
        stackPush(stk, value);
    }

    else if (IsPushmComplex)
    {
        int reg = proc->registers[proc->code[proc->IP + 2]];
        int val_ram = proc->RAM[proc->code[proc->IP + 3]];
        int value = reg + val_ram;
        GetProcInstruction(cmd, proc, value);
        LOG(LOGL_DEBUG, "Pushm ");
        stackPush(stk, value);
        proc->IP += 1;
    }

    if (cmd == CMD_POP)
    {
        switch (TypeArg)
        {
            case regist:
                IsPopr = true;
                break;
            case memory:
                IsPopm = true;
                break;
            case complex_memory:
                IsPopmComplex = true;
                break;
            default:
                break;
        }
    }

    if (IsPopr)
    {
        stackElem value = 0;
        GetProcInstruction(cmd, proc, value);
        LOG(LOGL_DEBUG, "");
        stackPop(stk, &value);
        proc->registers[proc->code[proc->IP + 2]] = value;
    }

    else if (IsPopm)
    {
        stackElem value = 0;
        GetProcInstruction(cmd, proc, value);
        LOG(LOGL_DEBUG, "Popm ");
        stackPop(stk, &value);
        proc->RAM[proc->code[proc->IP + 2]] = value;
    }

    else if (IsPopmComplex)
    {
        stackElem value = 0;
        int reg = proc->registers[proc->code[proc->IP + 2]];
        int val_ram = proc->RAM[proc->code[proc->IP + 3]];
        LOG(LOGL_DEBUG, "Popm ");
        stackPop(stk, &value);
        proc->RAM[reg + val_ram] = value;
        proc->IP += 1;
    }

    return 0;
}

void ProcessingInputOut(stack *stk, bool Input, bool Out)
{
    LOG(LOGL_DEBUG, "");

    if (Input)
    {
                stackElem input_value = 0;
                printf("Enter a value: ");
                scanf("%d", &input_value);
                stackPush(stk, input_value);
    }

    else if (Out)
    {
                stackElem val = 0;
                stackPop(stk, &val);
                printf("Out: %d\n", val);
    }
}

void DoJmp(CPU *proc, int cmd)
{
    proc->IP = proc->code[proc->IP + 1];
    GetProcInstruction(cmd, proc, proc->IP);
    LOG(LOGL_DEBUG, "JMP to %d ", proc->IP);
}

void ProcessingFuncs(stack *retAddrStk, CPU *proc, bool call, bool ret)
{
    LOG(LOGL_DEBUG, "");

    if (call)
    {
        stackPush(retAddrStk, proc->IP + 2);
        proc->IP = proc->code[proc->IP + 1];
    }

    else if (ret)
    {
        int retAddr = 0;
        stackPop(retAddrStk, &retAddr);
        proc->IP = retAddr;
    }
}

int CtorProc(stack *stk, stack *retAddrStk)
{
    stackCtor(stk, 8);
    stackCtor(retAddrStk, 8);
    return 0;
}
