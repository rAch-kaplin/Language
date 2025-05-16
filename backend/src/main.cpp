#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "file_read.h"
#include "tree_func.h"
#include "logger.h"
#include "arg_parser.h"
#include "../common/colors.h"
#include "backend.h"

void PrintVariablesTable(NameTable *name_table);

int main(int argc, const char* argv[]) //TODO not const
{
    printf(GREEN "\nStart main! ============================================================================\n\n" RESET);

    clock_t start = clock();

    const char* log_file = "../logfile.log";
    LoggerInit(LOGL_DEBUG, log_file, DEFAULT_MODE);

    ArgOption options[] = { {"-i",  "--input", true, nullptr, false},
                            {"-o", "--output", true, nullptr, false} };

    if (ParseArguments(argc, argv, options, sizeof(options) / sizeof(options[0])) != PARSE_OK)
    {
        fprintf(stderr, "ParseArg error\n");
        return 1;
    }

    const char* file_tree = options[INPUT].argument;
    if (file_tree == nullptr)
    {
        fprintf(stderr, RED "Can't open file_tree, please input -i *.txt\n" RESET);
        return 1;
    }

    const char* file_asm = options[OUTPUT].argument;
    if (file_asm == nullptr)
    {
        fprintf(stderr, RED "Can't open file_asm, please input -i *.txt\n" RESET);
        return 1;
    }

    //Variable vars_table[MAX_VARS] = {};
    NameTable name_table = {};

    Node* root = LoadTreeFromFile(file_tree, &name_table);
    if (root == nullptr)
    {
        fprintf(stderr, RED "Error: Failed to load tree from file %s\n" RESET, file_tree);
        return 1;
    }
    FixTree     (root);
    TreeDumpDot (root, &name_table);
    TreeDumpDot2(root, &name_table);

    PrintVariablesTable(&name_table);
    _DLOG("Start Assembly\n");
    AssemblyTree(root, &name_table, file_asm);

    //FreeVarsTable(vars_table);
    FreeNameTable(&name_table);
    FreeTree(&root);

    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    printf(MAGENTA "Execution time: %f seconds\n\n" RESET, time_spent);

    printf(GREEN "End main! ==============================================================================\n\n" RESET);
}

void PrintVariablesTable(NameTable *name_table)
{
    size_t count = 0;

    printf("\n======== VARIABLES TABLE (MAX: %zu) =======\n", MAX_VARS);
    printf("%-5s | %-20s | %-10s\n", "ID", "Name", "Length");
    printf("------------------------------------------\n");

    for (size_t i = 0; i < MAX_VARS; i++)
    {
        if (name_table->vars_table[i].name != nullptr)
        {
            printf("%-5zu | %-20s | %-10zu\n",
                   i,
                   name_table->vars_table[i].name,
                   name_table->vars_table[i].len_name);
            count++;
        }
    }

    printf("------------------------------------------\n");
    printf("Total variables: %zu/%zu\n", count, MAX_VARS);
    printf("==========================================\n\n");
}
