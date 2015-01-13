#ifndef INSTRUCTIONS_HEADER
#include "bigdecls.h"

<<<<<<< HEAD
// This is gonna be a long file...

/* Datatype suitable for storing an intstruction */
typedef struct instruction_node {
    instructionType_t instruction;
    //! Todo: Delete. Argsize can be the first 2 or 4 bytes of args
    //!  you'll also need to fix every thing
=======
/* Datatype suitable for storing an intstruction */
typedef struct instruction_node {
    instructionType_t instruction;
>>>>>>> fbaab0080dc700dae1c8ddc879a0d3c19c65ef50
    size_t argSize;
    void* args;
    struct instruction_node* next;

} instruction_t;

<<<<<<< HEAD
extern void (*EXEC_INSTRUCTION[instruction_ammount])();

/* Instruction Functions */
void i_nop();
void i_add();

=======
>>>>>>> fbaab0080dc700dae1c8ddc879a0d3c19c65ef50
/* Execute a chain of instructions */
void executeInternal(instruction_t** program);

#define INSTRUCTIONS_HEADER
#endif
