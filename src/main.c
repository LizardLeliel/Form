#include <stdio.h>
#include <stdlib.h>

#include "lex.yy.h"
#include "build.h"
#include "runtime.h"
//#include "test.c"

extern void initializeYYLEXProgramBuilder();
extern program_context_t finishYYLEXBuild();

//extern int returnTestX(void);

int main(int argc, char** argv)
{
    //printf("Hello world!\n");


    //printf("Testing testx: %d", returnTestX());
//#define test

    initializeYYLEXProgramBuilder();

#ifdef test
//#include "test.h"
    //runChecks();

    // token_hash_t testHash = makeTokenHash();
    // getHashID(&testHash, h_labelName, 5, "Hello");
    // printf("Get \"Hello\"? %s\n", peakHash(&testHash, h_labelName, 5, "Hello") ? "true" : "false");
    // printf("Get \"dne\"? %s\n", peakHash(&testHash, h_labelName, 3, "dne") ? "true" : "false");
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



    //freeHash();
    //freeInstructions();
    return 0;
}

