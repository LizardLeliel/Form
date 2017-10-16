#ifndef RUNTIME_HEADER
#define RUNTIME_HEADER

#include <stdlib.h>
#include <inttypes.h>

typedef unsigned char byte;
typedef int32_t size32_t;

/* An enumeration of run-time data types. All data types in Form should
 *  be 32 bits (even char; although strings will be treated like a special
 *  array type*/
typedef enum data_type {
    f_nil       = 0x0000,

    f_numeric   = 0x0001,
    f_composite = 0x0002,
    f_other     = 0x0004,

    f_32int     = 0x0008,
    f_32float   = 0x0010,
    f_bool      = 0x0020,
    f_char      = 0x0040,

    f_table     = 0x0080,
    f_array     = 0x0100,
    f_string    = 0x0200,

    f_ref       = 0x0400,
    f_var       = 0x0800,

} data_type_t;

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
    instruction_ammount

} instructionType_t;

// The stack data structure that this stack-oriented program uses
typedef struct stack 
{
    data_type_t type;
    void* data;
    struct stack* next;
} stack_t;

// Major to do: get rid of this, make
//  this into actual_data.
typedef struct data
{
    data_type_t dataType;
    size32_t data;
} data_t;

typedef struct instruction 
{
    instructionType_t instruction;
    size_t argSize;
    void* args;
    struct instruction* next;
} instruction_t;

typedef struct function_stack_node
{
    struct function_stack_node* next;
    instruction_t* returnInstruction;
} function_stack_node_t;

// Contains information such as return address
//  and (later) local variables.
typedef struct function_stack
{
    function_stack_node_t* head;
    unsigned int depth;
} function_stack_t;

// This is what's execute
typedef struct program_context
{

    instruction_t**  code; // code[0] is main
    function_stack_t functionStack;
    // Later: make a wrapper struct for stack
    stack_t*         dataStack;

    instruction_t* currentInstruction;

} program_context_t;

// Used for easier implementation of casting.
typedef union any32 
{
    int32_t as_i;
    float   as_f;
} any32_t;

static const unsigned int maxDepth = 50;

#include "basicOperations.h"

// Remove externs sometime - test.h can extern them.
// Or we can keep it global for now.
extern void (*EXEC_INSTRUCTION[instruction_ammount])(program_context_t*);

// The actual stack that is used by the language
//  Extern'd so it can be used by test.c
extern stack_t* STACK;
//extern stack_t _bottom;

// Must be called for STACK to start pointing at _bottom
void stackIni();
//void freeStack();

// Raise error if at bottom of stack
void shouldNotBeBottom(stack_t** dataStack);

// Push and pop things on the stack 
void pushStack(stack_t** dataStack, data_type_t dataType, void* data);
void dropStack(stack_t** dataStack);
size32_t popStack(stack_t** dataStack, data_type_t* outType);

// Inline this?
data_t popData();


void pushFunction(function_stack_t* functionStack,
                  instruction_t* returnInstruction);
void returnFromFunction(program_context_t* program);


// Instruction Functions 
// Commented out instructions are still here to keep the ordering
// void i_nop(program_context_t*);
// void i_add(program_context_t*);
// void i_sub(program_context_t*);
// void i_mul(program_context_t*);
// void i_divs(program_context_t*);
// void i_mod(program_context_t*);
void i_lessthen(program_context_t*);
void i_push(program_context_t*);
void i_call(program_context_t*);
void i_returns(program_context_t*);
void i_print(program_context_t*);
 
size32_t interpretAsInt(float value);
float interpretAsFloat(size32_t operandValue);
data_type_t prepareOperands(data_t* operandA, data_t* operandB);

// Execute a chain of instructions
void execute(program_context_t program);

// RUNTIME_HEADER
#endif

