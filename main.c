#include <stdio.h>
#include <stdlib.h>
#include "argparse.h"
#include "stack.h"

int main(int argc, char** argv)
{
    printf("Hello world!\n");
    stackIni();

#define test

#ifdef test
 #include "test.h"
    runChecks();
 #undef test
#endif

    int n = argParse(argc, argv);
    return n;
}
