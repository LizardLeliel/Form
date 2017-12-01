#include <stdio.h>
#include <stdlib.h>

#include "lex.yy.h"
#include "build.h"
#include "runtime.h"
//#include "test.c"

extern void initializeYYLEXProgramBuilder();
extern program_context_t finishYYLEXBuild();


int main(int argc, char** argv)
{

//#define test

    initializeYYLEXProgramBuilder();

#ifdef test
//#include "test.h"
    //runChecks();

#endif

#ifndef test
    if (argc >= 2)
    {   
        yyin = fopen(argv[1], "r");
        if (yyin == NULL)
        {
            perror("File does not exist");
            exit(0);
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

    return 0;
}

