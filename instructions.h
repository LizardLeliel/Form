#ifndef INSTRUCTIONS_HEADER
#include "bigdecls.h"

/* Datatype suitable for storing an intstruction */
typedef struct instruction_node {
    instructionType_t instruction;
    int argSize;
    void* args;

} instruction_t;

/* Execute a chain of instructions */
void executeInternal(instruction_t** program);

#define INSTRUCTIONS_HEADER
#endif
