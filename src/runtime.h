#ifndef RUNTIME_HEADER
#define RUNTIME_HEADER

#include <stdlib.h>
#include <inttypes.h>

// An enumeration representing run-time data types.
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

// Enumeration representing the instruction to be executed.
//  The enumeration's intergral value is used as an index
//  to a function array holding the implementation for
//  the instructions.
typedef enum intsruction_set 
{
    // No operation
    i_nop = 0,

    // Standard arthemtic operations
    i_add,  // 1
    i_sub,  // 2
    i_mul,  // 3
    i_div,  // 4 
    i_mod,  // 5

    // Standard bitwise operations
    i_bitwand,   // 6
    i_bitwor,    // 7
    i_bitwnot,   // 8
    i_bitwleft,  // 9
    i_bitwright, // 10
    i_bitwxor,   // 11

    // Standard comparison operations
    i_lessthen,      // 12
    i_lesstheneq,    // 13
    i_greaterthen,   // 14
    i_greatertheneq, // 15
    i_eq,            // 16
    i_ineq,          // 17

    // Boolean logic operations
    i_logand, // 18
    i_logor,  // 19
    i_lognot, // 20

    // Increment and deincrement operations
    i_incr, // 21
    i_decr, // 22

    // Standard stack-related operations
    i_push,  // 23
    i_drop,  // 24
    i_pick,  // 25
    i_del,   // 26
    i_dup,   // 27
    i_rot,   // 28

    // Form goto operations
    i_goto,     // 29
    i_condgoto, // 30

    // Variable-related operations
    i_scope,   // 31
    i_assigns, // 32
    i_get,     // 33

    // Special operations (function call, return, output, end program)
    i_call,    // 34
    i_return,  // 35
    i_print,   // 36
    i_endprog, // 37

    // This isn't an instruction; this is just as the size of the enumeration
    //  if this enum's size changes, this will reflect that.
    instruction_ammount // 38

} instruction_type_t;
// Struct representing ddata that can be stored.

typedef struct data
{
    data_type_t type;
    int64_t     data;
} data_t;


// Stack which stores runtime data.
typedef struct data_stack_node
{
    data_t data;
    struct data_stack_node* next;
} data_stack_node_t;

typedef struct data_stack 
{
    data_stack_node_t* top;
    size_t             depth;
} data_stack_t;



// Represents an executable instruction and its arguments.
typedef struct instruction 
{
    instruction_type_t  instruction;
    int32_t             arg1;
    int64_t             arg2;
} instruction_t;



// Stack node representing the stack of called functions.
typedef struct function_stack_node
{
    struct function_stack_node* next;
    unsigned int                functionIndex;
    unsigned int                instructionIndex;
} function_stack_node_t;

// Wrapper for the function stack.
typedef struct function_stack
{
    function_stack_node_t* head;
    unsigned int           depth;
} function_stack_t;


// Struct to hold composite data type
//  (ie dicts, arrays, string, etc.)
typedef struct static_data
{
    data_type_t type;
    void*       data;
} static_data_t;

// Holds all static data the program may use.
typedef struct static_data_bank
{
    static_data_t* dataBank;
    size_t         size;
} static_data_bank_t;


// Holds all needed data for the program to execute.
typedef struct program_context
{
    // array of functions. Higher-level index is function index,
    //  lower-level index is instruction index. code[0] is main.
    instruction_t** code; 

    // Run-time data.
    static_data_bank_t staticDataBank;
    data_stack_t       dataStack;
    instruction_t      currentInstruction;

    // Values used to track which instruction gets executed next.
    function_stack_t functionStack;
    unsigned int     currentFunctionIndex;
    unsigned int     currentInstructionIndex;
    unsigned int     nextInstructionIndex;
    unsigned int     nextFunctionIndex;

} program_context_t;

// Used for easier implementation of casting.
typedef union any64 
{
    int64_t as_i;
    double  as_f;
} any64_t;

static const unsigned int maxDepth = 50;

// Note: this has to included here.
#include "instructions.h"

// Array of functions implementing the instructions.
// Note: this has to be defined after instruction.h is included.
void (*EXEC_INSTRUCTION[instruction_ammount])(program_context_t*);

// Raise error if at bottom of runtime data stack
void shouldNotBeBottom(data_stack_t* dataStack);

// Push data on the run-time stack.
void pushStack(data_stack_t*   dataStack, 
               data_type_t dataType, 
               int64_t     data);


// Deletes the top of the stack
void dropStack(data_stack_t* dataStack);

// Pops the stack, then puts the data into a data_t data struct
data_t popData(data_stack_t* dataStack);

data_t pickData(data_stack_t* dataStack, int popData);

// Calls a new function.
void pushFunction(function_stack_t* functionStack,
                  unsigned int callingFunction,
                  unsigned int instructionDestination);

// Sets execution back to previous function. 
void returnFromFunction(program_context_t* program);

// If either operand is a float, converts the other operand to 
//  a float and returns f_float.
// Else, if they're both integer types, leave them be and return
//  f_int
data_type_t prepareOperands(data_t* operandA, data_t* operandB);

// Executes the program.
void execute(program_context_t program);

// RUNTIME_HEADER
#endif

