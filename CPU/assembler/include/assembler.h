#ifndef _HASSEMBLER
#define _HASSEMBLER

#include <stdlib.h>
#include <stdio.h>
#include "stack.h"
#include "common.h"
#include "proccessor.h"

const int LABELS_SIZE = 128;
const size_t LABEL_NAME = 20;
const size_t SIZE_ARG = 30;

typedef struct
{
        char name[LABEL_NAME];
        int value;
} Label;

typedef struct Assem
{
    int *code = NULL;
    int offset = 0;
    int CODE_SIZE;
    const char *file_name;
    FILE *file_asm = NULL;
    int label_count = 0;
    char *listing = NULL;

    Label Labels[LABELS_SIZE];
} Assem;

CodeError AssemblyLabels(char **buffer, Assem *assem, int cmd_code);
void CtorAssembly(Assem *assem, char **buffer, size_t *file_size);
int ReadCommand(Assem *assem,  char *cmd);
int GetCommandCode(const char *cmd, size_t count_command);
void CheckArgsAsm(const int argc, const char *argv[], Assem *assem);
int FirstPassFile(char *buffer, Assem *assem);
int FindLabel(Assem *assem, char *cmd);
const char* Assembler(Assem *assem);
CodeError WriteBinFile(Assem *assem);
CodeError AssemblyArgType(Assem *assem, char **buffer, int cmd_code);
void CheckLabels(char *cmd, Assem *assem, int CODE_SIZE);
int FindFunc(Assem *assem, char *cmd);
char * ReadFileToBuffer(Assem *assem, char *buffer, size_t *file_size);
char* SkipSpace(char* current_pos);
void RemoveSpaces(char* str);
CodeError HandleMemoryAccess(char* arg, Assem *assem);
bool IsComplexArgument(const char *arg);

#endif //HASSEMBLER
