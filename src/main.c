#include <stdio.h>
#include <stdlib.h>

#include "stack.h"
#include "build.h"
#include "hash.h"
#include "instructions.h"
//#include "test.c"

extern void yylex(void);
extern FILE* yyin;

int main(int argc, char** argv)
{
    printf("Hello world!\n");

    stackIni();
    instructionIni();
    hashIni();


//#define test

#ifdef test
#include "test.h"
    runChecks();
 #undef test
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
        execute(returnProgram());
    }
    else
    {
        printf("Please provide a file\n");
    }
#endif

    freeHash();
    freeInstructions();
    return 0;
}

