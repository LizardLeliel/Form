#ifndef INSTRUCTIONS_HEADER

#include "stdlib.h"

#include "stack.h"

// Each enumeration's integer representation
//  corresponds to an index in a function pointer
//  array.
typedef enum intsruction_set 
{
    // No operation
    nop = 0,

    // Standard arthemtic operations
    add,
    sub,
    mul,
    divs,
    mod,

    // Standard bitwise operations
    bitwand,
    bitwor,
    bitwnot,
    bitwleft,
    bitwright,
    bitwxor,

    // Standard comparison operations
    lessthen,
    lessthaneq,
    morethan,
    morethaneq,
    eq,
    ineq,

    // Boolean logic operations
    logand,
    logor,
    lognot,

    // Increment and deincrement operations
    incr,
    decr,

    // Standard stack-related operations
    push,
    drop,
    over,
    swaps,
    rot,

    // Form goto operations
    gotos,
    congotos,

    // Variable-related operations
    scope,
    assigns,
    get,

    // Misc operations (function call, return, output, end program)
    call,
    returns,
    print,
    endProg,

    // This isn't an instruction; this is just as the size of the enumeration
    //  if this enum's size changes, this will reflect that, and... things
    instruction_ammount,

} instructionType_t;

typedef struct instruction 
{
    instructionType_t instruction;
    size_t argSize;
    void* args;
    struct instruction* next;

} instruction_t;

// Used for easier implementation of casting.
typedef union any32 
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

// Execute a chain of instructions
void execute(instruction_t** program);

#define INSTRUCTIONS_HEADER
#endif

