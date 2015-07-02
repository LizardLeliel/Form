#include <stdio.h>
#include <stdlib.h>
#include "argparse.h"
#include "stack.h"
#include "build.h"
#include "hash.h"

int main(int argc, char** argv)
{
    printf("Hello world!\n");

    stackIni();
    instructionIni();
    hashIni();

    printf("%u %u %u\n", 1, 1<<1, 1>>1);

#define test

#ifdef test
 #include "test.h"
    // Make a formal test suite?
    runChecks();
 #undef test
#endif

    //int n = argParse(argc, argv);

    return 0;
}


/* Milestones:
 *  January 8th 2015 at arround 12:17 pm:
 *   - The first stack operation was performed (internally; not from script)
 *
 */
