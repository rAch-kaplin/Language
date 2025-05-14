#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <ctype.h>

#include "logger.h"
#include "color.h"
#include "CommonProcAssem.h"
#include "assembler.h"

void HandlSizeArg(int *CODE_SIZE, char **current_pos);
bool IfEndFile(char *current_pos);
size_t GetAsmFileSize(Assem *assem);
char* SkipComment(char* current_pos);

const char* Assembler(Assem *assem)
{
    char *buffer = NULL;
    size_t file_size = 0;

    CtorAssembly(assem, &buffer, &file_size);

    char *current_pos = buffer;
    const size_t count_command = sizeof(command_code) / sizeof(command_code[0]);

    //assem->listing = (char*)calloc(assem->CODE_SIZE * 30, sizeof(char));
    char *initial_listing = assem->listing;
    //assem->listing += snprintf(assem->listing, assem->CODE_SIZE, "\n\t  Number \t\tCode \t\t Text\n");

    for (int i = 1;;i++)
    {
        current_pos = SkipSpace     (current_pos);
        current_pos = SkipComment   (current_pos);

        if (IfEndFile(current_pos)) break;

        char cmd[SIZE_ARG] = "";
        if (sscanf(current_pos, "%29s", cmd) != 1)
        {
            printf("the string incorrect\n");
            return NULL;
        }

        current_pos += strlen(cmd);

        int cmd_code = GetCommandCode(cmd, count_command);

        //assem->listing += snprintf(assem->listing, assem->CODE_SIZE, "\n\t\t %03d \t\t %03d \t\t %s ", i, cmd_code, cmd);

        switch (cmd_code)
        {
            case CMD_PUSH:
            case CMD_POP:
            {
                //assem->listing += snprintf(assem->listing, assem->CODE_SIZE, "\t\t %03d \t\t %03d \t\t\t %s\n", i, cmd_code, cmd);
                CodeError error = AssemblyArgType(assem, &current_pos, cmd_code);
                if (error == ARG_TYPE_ERROR)
                {
                    free(buffer);
                    return COLOR_RED "not correct arg" COLOR_RESET;
                }
                break;
            }

            case CMD_RET:
            case CMD_ADD:
            case CMD_SUB:
            case CMD_OUT:
            case CMD_DIV:
            case CMD_MUL:
            case CMD_HLT:
            case CMD_IN:
            case CMD_SQRT:
            {
                //assem->listing += snprintf(assem->listing, assem->CODE_SIZE, "\n");
                assem->code[assem->offset++] = cmd_code;
                break;
            }
            case CMD_FUNC:
            case CMD_JMP:
            case CMD_JA:
            case CMD_JAE:
            case CMD_JB:
            case CMD_JBE:
            case CMD_JE:
            case CMD_JNE:
            {
                //assem->listing += snprintf(assem->listing, assem->CODE_SIZE, "\t\t %03d \t\t %03d \t\t\t %s\n", i, cmd_code, cmd);
                CodeError error = AssemblyLabels(&current_pos, assem, cmd_code);
                if (error)
                {
                    return "LABEL ERROR!";
                }
                break;
            }

            default:
            {
                if (FindLabel(assem, cmd) == -1)
                {
                    fprintf(stderr, "Unknow command: %s\n", cmd);
                    return NULL;
                }
                break;
            }
        }
    }

    //FILE *file_list = fopen("listing.txt", "w");
    //fprintf(file_list, "%s", initial_listing);
    //fclose(file_list);
#if 0
    for (int i = 0; i < assem->CODE_SIZE + 1; i++)
    {
        printf("%d ", assem->code[i]);
    }
    printf("\n");
#endif

    free(buffer);
    free(initial_listing);
    return NULL;
}

CodeError AssemblyLabels(char **buffer, Assem *assem, int cmd_code)
{
    assert(buffer != nullptr);
    assert(assem != nullptr);

    assem->code[assem->offset++] = cmd_code;
    char *current_pos = *buffer;

    current_pos = SkipSpace(current_pos);

    char label[SIZE_ARG] = "";
    if (sscanf(current_pos, "%29s", label) != 1)
    {
        printf("the string incorrect\n");
        return ARG_TYPE_ERROR;
    }

    //printf(COLOR_GREEN "%s\n" COLOR_RESET, label);

    int label_index = FindLabel(assem, label);
    if (label_index != -1)
    {
        assem->code[assem->offset++] = label_index;
    }
    else
    {
        printf(COLOR_RED "Error: Label not found\n" COLOR_RESET);
        return UNKNOW_LABEL;
    }

    //assem->listing += snprintf(assem->listing, assem->CODE_SIZE, "%s:", label);
    current_pos += strlen(label);
    *buffer = current_pos + 1;

    return ITS_OK;
}

int CompileArg(const char *str)
{
    Registers ArrayRegs[] = { {"ax", 0},
                              {"bx", 1},
                              {"cx", 2},
                              {"dx", 3}};

    int count_regs = sizeof(ArrayRegs) / sizeof(ArrayRegs[0]);
    for (int i = 0; i < count_regs; i++)
    {
        if (strcmp(str, ArrayRegs[i].reg_name) == 0)
        {
            return ArrayRegs[i].number_reg;
        }
    }
    return -1;
}

int GetCommandCode(const char *cmd, size_t count_command)
{
    for (size_t i = 0; i < count_command; i++)
    {
        if (strcmp(cmd, command_code[i].cmd_name) == 0)
        {
            return command_code[i].cmd_code;
        }
    }

    return 0;
}

void CtorAssembly(Assem *assem, char **buffer, size_t *file_size)
{
    assem->file_asm = fopen(assem->file_name, "r");
    if (assem->file_asm == nullptr)
    {
        fprintf(stderr, "file_asm can't open\n");
        return;
    }

    *buffer = ReadFileToBuffer(assem, *buffer, file_size);
    assert(buffer != nullptr);
    fclose(assem->file_asm);

    assem->CODE_SIZE = FirstPassFile(*buffer, assem);

    assem->code = (int*)calloc((size_t)assem->CODE_SIZE + 1, sizeof(int));
}

int ReadCommand(Assem *assem, char *cmd)
{
    if (fscanf(assem->file_asm, "%19s", cmd) != 1)
    {
        if (feof(assem->file_asm))
        {
            return -1;
        }

        return FILE_READ_ERROR;
    }
    return ITS_OK;
}

int FirstPassFile(char *buffer, Assem *assem)
{
    int CODE_SIZE = 0;
    char *current_pos = buffer;
    while(true)
    {
        current_pos = SkipSpace(current_pos);

        if (*current_pos == '\0')
        {
            break;
        }

        char cmd[SIZE_ARG] = "";
        if (sscanf(current_pos, "%29s", cmd) != 1)
        {
            printf("the string incorrect\n");
            return -1;
        }

        current_pos += strlen(cmd);

        size_t count_command = sizeof(command_code) / sizeof(command_code[0]);
        int cmd_code = GetCommandCode(cmd, count_command);

        switch (cmd_code)
        {
            case CMD_PUSH:
            case CMD_POP:
            {
                HandlSizeArg(&CODE_SIZE, &current_pos);
                break;
            }

            case CMD_SQRT:
            case CMD_ADD:
            case CMD_SUB:
            case CMD_OUT:
            case CMD_DIV:
            case CMD_MUL:
            case CMD_RET:
            case CMD_HLT:
            case CMD_IN:
            {
                CODE_SIZE += 1;
                break;
            }
            case CMD_FUNC:
            case CMD_JA:
            case CMD_JAE:
            case CMD_JB:
            case CMD_JBE:
            case CMD_JE:
            case CMD_JNE:
            case CMD_JMP:
            {
                char label[SIZE_ARG] = "";
                sscanf(current_pos, "%29s", label);
                current_pos += strlen(label) + 1;
                CODE_SIZE += 2;
                break;
            }

            default:
            {
                break;
            }
        }

        CheckLabels(&cmd[0], assem, CODE_SIZE);
    }

    return CODE_SIZE;
}

void HandlSizeArg(int *CODE_SIZE, char **current_pos)
{
    char arg[SIZE_ARG] = "";
    sscanf(*current_pos, "%29[^\n]", arg);
    if (IsComplexArgument(arg))
    {
        *CODE_SIZE += 4;
    }
    else
    {
        *CODE_SIZE += 3;
    }
}

bool IsComplexArgument(const char *arg)
{
    return (strchr(arg, '+') != NULL);
}

void CheckLabels(char *cmd, Assem *assem, int CODE_SIZE)
{
    if (strcmp(&cmd[strlen(cmd) - 1], ":") == 0)
    {
        cmd[strlen(cmd) - 1] = '\0';
        strcpy(assem->Labels[assem->label_count].name, cmd);
        assem->Labels[assem->label_count].value = CODE_SIZE;
        assem->label_count++;
    }
}

int FindLabel(Assem *assem, char *cmd)
{
    cmd[strlen(cmd) - 1] = '\0';
    for (int i = 0; i < LABELS_SIZE; i++)
    {
        if (strcmp(assem->Labels[i].name, cmd) == 0)
        {
            return assem->Labels[i].value;
        }
    }
    return -1;
}

int FindFunc(Assem *assem, char *cmd)
{
    for (int i = 0; i < LABELS_SIZE; i++)
    {
        if (strcmp(assem->Labels[i].name, cmd) == 0)
        {
            return assem->Labels[i].value;
        }
    }
    return -1;
}

CodeError AssemblyArgType(Assem *assem, char **buffer, int cmd_code)
{
    assem->code[assem->offset++] = cmd_code;

    char *current_pos = *buffer;
    current_pos = SkipSpace(current_pos);

    char arg[SIZE_ARG] = "";
    char type_arg[SIZE_ARG] = "";

    sscanf(current_pos, "%29s", type_arg);

    if (strchr(type_arg, '['))
    {
        if (sscanf(current_pos, "%29[^\n]", arg) != 1)
        {
            printf("the string incorrect\n");
            return ARG_TYPE_ERROR;
        }
    }

    else
    {
        if (sscanf(current_pos, "%29s", arg) != 1)
        {
            printf("the string incorrect\n");
            return ARG_TYPE_ERROR;
        }
    }
    //assem->listing += snprintf(assem->listing, assem->CODE_SIZE, "%s", arg);
    current_pos += strlen(arg);
    *buffer = current_pos;

    size_t size_arg = strlen(arg);

    if (arg[0] == '-' && size_arg > 1 && isdigit(arg[1]))
    {
        assem->code[assem->offset++] = digit;
        assem->code[assem->offset++] = atoi(arg);
    }

    else if (isdigit(arg[0]))
    {
        assem->code[assem->offset++] = digit;
        assem->code[assem->offset++] = atoi(arg);
    }

    else if ((arg[0] == '[' || arg[size_arg - 1] == ']'))
    {
        CodeError error = HandleMemoryAccess(&arg[0], assem);
        if (error != ITS_OK)
        {
            return ARG_TYPE_ERROR;
        }
    }

    else if (!isdigit(arg[0]) && !isdigit(arg[1]))
    {
        int reg = CompileArg(arg);
        if (reg == -1)
        {
            return ARG_TYPE_ERROR;
        }
        assem->code[assem->offset++] = regist;
        assem->code[assem->offset++] = reg;
    }

    return ITS_OK;
}

size_t GetAsmFileSize(Assem *assem)
{
    fseek(assem->file_asm, 0, SEEK_END);
    size_t file_size = (size_t)ftell(assem->file_asm);
    rewind(assem->file_asm);

    return file_size;
}

char * ReadFileToBuffer(Assem *assem, char *buffer, size_t *file_size)
{
    *file_size = GetAsmFileSize(assem);

    buffer = (char*)calloc(*file_size + 1, sizeof(char));
    assert(buffer);

    fread(buffer, 1, *file_size, assem->file_asm);
    return buffer;
}

char* SkipSpace(char* current_pos)
{
    while (isspace(*current_pos))
    {
        current_pos++;
    }
    return current_pos;
}

char* SkipComment(char* current_pos)
{
    if (*current_pos == ';')
    {
        while (*current_pos != '\n' && *current_pos != '\0')
        {
            current_pos++;
        }

        return current_pos;
    }

    return current_pos;
}

CodeError HandleMemoryAccess(char* arg, Assem *assem)
{
    size_t size_arg = strlen(arg);

    arg[size_arg - 1] = '\0';
    char inner_arg[SIZE_ARG] = {};
    strncpy(inner_arg, arg + 1, size_arg - 2);
    inner_arg[size_arg - 2] = '\0';
    RemoveSpaces(inner_arg);

    char* plus_pos = strchr(inner_arg, '+');
    if (plus_pos)
    {
        *plus_pos = '\0';
        char* left_part = inner_arg;
        char* right_part = plus_pos + 1;

        int reg = CompileArg(left_part);
        int num = atoi(right_part);

        if (reg != -1)
        {
            assem->code[assem->offset++] = complex_memory;
            assem->code[assem->offset++] = reg;
            assem->code[assem->offset++] = num;
            return ITS_OK;
        }

        reg = CompileArg(right_part);
        num = atoi(left_part);

        if (reg != -1)
        {
            assem->code[assem->offset++] = complex_memory;
            assem->code[assem->offset++] = reg;
            assem->code[assem->offset++] = num;
            return ITS_OK;
        }
    }

    else
    {
        int reg = CompileArg(inner_arg);
        if (reg != -1)
        {
            assem->code[assem->offset++] = memory;
            assem->code[assem->offset++] = reg;
            return ITS_OK;
        }

        int num = atoi(inner_arg);
        assem->code[assem->offset++] = memory;
        assem->code[assem->offset++] = num;

        return ITS_OK;
    }
    return ARG_TYPE_ERROR;
}

void RemoveSpaces(char* str)
{
    char* dest = str;
    for (char* src = str; *src != '\0'; src++)
    {
        if (!isspace(*src))
        {
            *dest = *src;
            dest++;
        }
    }
    *dest = '\0';
}

bool IfEndFile(char *current_pos)
{
    if (*current_pos == '\0')
        return true;
    return false;
}
