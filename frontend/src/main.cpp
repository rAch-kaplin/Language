#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "file_read.h"
#include "tree_func.h"
#include "logger.h"
#include "lexical_analysis.h"
#include "syntax_analysis.h"
#include "arg_parser.h"
#include "../common/colors.h"

const size_t MAX_VARS = 10;

int main(int argc, const char* argv[]) //TODO not const
{
    printf(GREEN "\nStart main! ============================================================================\n\n" RESET);

    clock_t start = clock();

    const char* log_file = "logfile.log";
    LoggerInit(LOGL_DEBUG, log_file, DEFAULT_MODE);

    ArgOption options[] = { {"-i",  "--input", true, nullptr, false},
                            {"-o", "--output", true, nullptr, false} };

    if (ParseArguments(argc, argv, options, sizeof(options) / sizeof(options[0])) != PARSE_OK)
    {
        fprintf(stderr, "ParseArg error\n");
        return 1;
    }

    const char* file_expr = options[INPUT].argument;
    if (file_expr == nullptr)
    {
        fprintf(stderr, RED "Can't open file_expr, please input -i *.txt\n" RESET);
        return 1;
    }


    ReadProgram(file_expr);
    // if (node_G == nullptr)
    // {
    //     fprintf(stderr, "ERROR! check log file\n");
    // }

    FreeVarsTable();
    LoggerDeinit();

    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    printf(MAGENTA "Execution time: %f seconds\n\n" RESET, time_spent);

    printf(GREEN "End main! ==============================================================================\n\n" RESET);
}
