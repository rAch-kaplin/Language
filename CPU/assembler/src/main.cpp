#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "color.h"
#include "common.h"
#include "CommonProcAssem.h"
#include "assembler.h"

int main(const int argc, const char *argv[])
{
    struct Assem assem = {};
    CheckArgsAsm(argc, argv, &assem);

    const char* error_assem = Assembler(&assem);
    if (error_assem != NULL)
    {
        printf("ERROR! from Assembler: %s \n", error_assem);
        free(assem.code);
        return ASSEM_ERROR;
    }

    WriteBinFile(&assem);

    printf(COLOR_GREEN "End of main_assembler!\n" COLOR_RESET);
    return 0;
}

CodeError WriteBinFile(Assem *assem)
{
    FILE *bin_file = fopen("../CPU/assembler/bin_code.txt", "wb"); // FIXME: передавай через argv
    if (bin_file == NULL)
    {
        fprintf(stderr, "Error bin_file not open\n");
        return FILE_NOT_OPEN;
    }

    fwrite(assem->code, sizeof(int), (size_t)assem->CODE_SIZE, bin_file);
    free(assem->code);
    fclose(bin_file);
    return ITS_OK;
}

void CheckArgsAsm(const int argc, const char *argv[], Assem *assem)
{
    for (int i = 1; i < argc; i++)
    {
        if ((strcmp(argv[i], "--file") || strcmp(argv[i], "-f") == 0) && i + 1 < argc)
        {
            assem->file_name = argv[i + 1];
            i++;
        }
    }
}
