#include "argparse.h"
<<<<<<< HEAD
//#include "lex.yy.c"
/*
=======
#include "lex.yy.c"

>>>>>>> fbaab0080dc700dae1c8ddc879a0d3c19c65ef50
int argParse(int argc, char** argv) {
    if (argc < 2) {
        printf("Invalid arguements. Format:\n");
        printf("%s [input file]\n", argv[0]);
        return -1;
    }
    else {
        yyin = fopen(argv[1], "r");
        printf("File opened: %s\n", argv[1]);
        int n = yylex();
        return n;
    }
}
<<<<<<< HEAD
*/
=======
>>>>>>> fbaab0080dc700dae1c8ddc879a0d3c19c65ef50
