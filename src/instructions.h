#ifndef INSTRUCTIONS_HEADER
#include "bigdecls.h"
#include "stack.h"

// This is gonna be a long file...

/* Datatype suitable for storing an intstruction */
typedef struct instruction_node 
{
    instructionType_t instruction;
    //! Todo: Delete. Argsize can be the first 2 or 4 bytes of args
    //!  you'll also need to fix every thing
    size_t argSize;
    void* args;
    struct instruction_node* next;

} instruction_t;

// Used for easier implementation of casting.
typedef union floatint_union 
{
    int32_t as_i;
    float   as_f;
} any32_t;

extern void (*EXEC_INSTRUCTION[instruction_ammount])();

/* Instruction Functions */
void i_nop();
void i_add();
void i_sub();
void i_mul();
void i_divs();
void i_mod();
void i_lessthen();
void i_push();
void i_print();
 
size32_t interpretAsInt(float value);
float interpretAsFloat(size32_t operandValue);
dataType_t prepareOperands(data_t* operandA, data_t* operandB);

/* Execute a chain of instructions */
void executeInternal(instruction_t** program);

#define INSTRUCTIONS_HEADER
#endif

//size32_t popStack(dataType_t* outType<);