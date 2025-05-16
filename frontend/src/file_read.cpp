#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "file_read.h"
#include "logger.h"
#include "lexical_analysis.h"
#include "syntax_analysis.h"
#include "tree_func.h"

const size_t BUFFER_SIZE = 10;

Node* ReadProgram(const char *file_program, NameTable *name_table)
{
    size_t lexeme_count = 0;
    Lexeme *lexeme_array = InitLexemeArray(file_program, &lexeme_count, name_table);
    if (lexeme_array == nullptr)
    {
        LOG(LOGL_ERROR, "Lexeme_array was not allocated");
        return nullptr;
    }

    // for (size_t i = 0; i < lexeme_count; i++)
    // {
    //     if (lexeme_array[i].type == LEX_VAR) {
    //         printf("VAR lexeme: raw value = %zu\n", lexeme_array[i].value.var);
    //     }
    // }

    PrintLexemes(lexeme_array, lexeme_count, name_table);

    size_t cur = 0;
    LOG(LOGL_DEBUG, "Start General() func");
    Node *node_G = General(lexeme_array, &cur, name_table);

    DeinitLexemes(lexeme_array);

    return node_G;
    //return nullptr;
}


size_t GetSizeFile(FILE *file_program)
{
    assert(file_program);

    if (fseek(file_program, 0, SEEK_END) != 0)
    {
        LOG(LOGL_ERROR, "fseek() Error!");
        return 0;
    }
    size_t file_size = (size_t)ftell(file_program);
    rewind(file_program);

    return file_size;
}

char* ReadProgramToBuffer(const char *program_name, size_t *file_size)
{
    FILE *file_program = fopen(program_name, "r");
    if (file_program == nullptr)
    {
        LOG(LOGL_ERROR, "Can't open file: %s", file_program);
        return nullptr;
    }

    *file_size = GetSizeFile(file_program);
    char *buffer = (char*)calloc(*file_size + 2, sizeof(char));
    if (buffer == nullptr)
    {
        LOG(LOGL_ERROR, "Memory allocation failed");
        fclose(file_program);
        return nullptr;
    }

    size_t read = fread(buffer, sizeof(char), *file_size, file_program);
    if (read != *file_size)
    {
        free(buffer);
        fclose(file_program);
        return nullptr;
    }

    fclose(file_program);

    return buffer;
}
