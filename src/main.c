#include <stdio.h>
#include <stdlib.h>

#include "build.h"
#include "runtime.h"
//#include "test.c"

extern void yylex();
extern FILE* yyin;

extern void initializeYYLEXProgramBuilder();
extern program_context_t finishYYLEXBuild();

//extern int returnTestX(void);

int main(int argc, char** argv)
{
    printf("Hello world!\n");

    stackIni();
    //instructionIni();
    //hashIni();

    //printf("Testing testx: %d", returnTestX());
//#define test

    initializeYYLEXProgramBuilder();

#ifdef test
#include "test.h"
    runChecks();
#endif

#ifndef test
    if (argc >= 2)
    {   
        yyin = fopen(argv[1], "r");
        if (yyin == NULL)
        {
            perror("File does not exist");
        }
        yylex();
        execute(finishYYLEXBuild());
    }
    else
    {
        printf("Please provide a file\n");
        return 0;
    }
#endif
#undef test



    //freeHash();
    //freeInstructions();
    return 0;
}

