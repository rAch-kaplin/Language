#include <stdio.h>
#include <string.h>

#include "color.h"
#include "stack.h"
#include "logger.h"
#include "proccessor.h"
#include "common.h"
#include "CommonProcAssem.h"

ServiceLines* GetServiceLines()
{
    static ServiceLines service = {"",""};
    return &service;
}

int main(int argc, char *argv[])
{
    printf(COLOR_GREEN "Start main!\n" COLOR_RESET);
    CheckArgsProc(argc, argv);
    loggerInit(LOGL_DEBUG, "../logfile.log");

    struct stack stk = {NULL, 0, 0};
    struct stack retAddrStk = {};
    struct CPU proc = {};

    CtorProc(&stk, &retAddrStk);

    const char* error_run = Run(&stk, &retAddrStk, &proc);
    if (error_run != NULL)
    {
        printf("ERROR! from Run ^, %s\n", error_run);
        free(proc.code);
        stackDtor(&stk);
        stackDtor(&retAddrStk);
        return 1;
    }

    stackDtor(&stk);
    stackDtor(&retAddrStk);

    loggerDeinit();
    printf(COLOR_GREEN "End of main!\n" COLOR_RESET);
    return 0;
}

void CheckArgsProc(int argc, char *argv[])
{
    GetLogger()->color_mode = DEFAULT_MODE;

    const char* color_mode = NULL;

    for (int i = 1; i < argc; i++)
    {

        if ((strcmp(argv[i], "-mode") || strcmp(argv[i], "-m") == 0) && i + 1 < argc)
        {
            color_mode = argv[i + 1];
            if (strcmp(color_mode, "COLOR_MODE") == 0)
            {
                GetLogger()->color_mode = COLOR_MODE;
            }
            i++;
        }
    }
}
