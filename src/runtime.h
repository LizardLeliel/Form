#ifndef RUNTIME_HEADER
#define RUNTIME_HEADER

#include <stdlib.h>
#include <inttypes.h>

//
// BIG TODO: CHANGE POP DATA TO BE SIZE INDEPENDENT
// Compiling can still be size agnostic - 32 bit int
//  which is an index of where we're saving all our strings.
//

// Typedefs
typedef unsigned char byte;
typedef int32_t size32_t;

/* An enumeration of run-time data types. All data types in Form should
 *  be 32 bits (even char; although strings will be treated like a special
 *  array type
 */
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
    lesstheneq,
    greaterthen,
    greatertheneq,
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
    int64_t data;
    struct stack* next;
} stack_t;

// Major to do: get rid of this, make
//  this into actual_data.
typedef struct data
{
    data_type_t dataType;
    int64_t data;
} data_t;

// Represents an executable instruction and its args.
// Todo: move args and instruction into a consistent-sized
//  struct.
typedef struct instruction 
{
    instructionType_t instruction;
    //size_t argSize;
    //void* args;
    int32_t arg1;
    int32_t arg2;
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

    // Static data struct

} program_context_t;

// Used for easier implementation of casting.
typedef union any64 
{
    int64_t as_i;
    double  as_f;
} any64_t;

static const unsigned int maxDepth = 50;

#include "basicOperations.h"

// Remove externs sometime - test.h can extern them.
// Or we can keep it global for now.
extern void (*EXEC_INSTRUCTION[instruction_ammount])(program_context_t*);

// Implement this sometime.
// void freeStack();

// Raise error if at bottom of stack
void shouldNotBeBottom(stack_t** dataStack);

// Push and pop things on the stack 
void pushStack(stack_t** dataStack, 
               data_type_t dataType, 
               int64_t data);

// Deletes the top of the stack
void dropStack(stack_t** dataStack);

// Pops the stack, returns its value and puts the data type into outType
int64_t popStack(stack_t** dataStack, data_type_t* outType);

// Pops the stack, then puts the data into a data_t data structe
data_t popData();

// Calls a new function
void pushFunction(function_stack_t* functionStack,
                  instruction_t* returnInstruction);
// Ends a function
void returnFromFunction(program_context_t* program);

void i_push(program_context_t*);
void i_call(program_context_t*);
void i_returns(program_context_t*);
void i_print(program_context_t*);
 
// Reinterpration procedures for arthmetic.
int64_t interpretAsInt(double value);
double interpretAsFloat(int64_t operandValue);

// If either operand is a float, converts the other operand to 
//  a float and returns f_float.
// Else, if they're both integer types, leave them be and return
//  f_int
data_type_t prepareOperands(data_t* operandA, data_t* operandB);

// Execute a chain of instructions
void execute(program_context_t program);

// RUNTIME_HEADER
#endif

