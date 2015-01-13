#ifndef INSTRUCTIONS_HEADER
#include "bigdecls.h"
#include "stack.h"

// This is gonna be a long file...

/* Datatype suitable for storing an intstruction */
typedef struct instruction_node {
    instructionType_t instruction;
    //! Todo: Delete. Argsize can be the first 2 or 4 bytes of args
    //!  you'll also need to fix every thing
    size_t argSize;
    void* args;
    struct instruction_node* next;

} instruction_t;

extern void (*EXEC_INSTRUCTION[instruction_ammount])();

/* Instruction Functions */
void i_nop();
void i_add();

/* Execute a chain of instructions */
void executeInternal(instruction_t** program);

#define INSTRUCTIONS_HEADER
#endif
