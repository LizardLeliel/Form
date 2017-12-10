#ifndef BUILD_HEADER
#define BUILD_HEADER

#include <stdbool.h>

#include "runtime.h"
#include "tokenhash.h"

// A wrapper for hash bucket list
typedef struct instruction_node
{
    instruction_type_t       instruction;
    int32_t                  arg1;
    int64_t                  arg2;
    struct instruction_node* next;
} instruction_node_t;

// A list node representing the beginning of a function
typedef struct function_header
{
    instruction_node_t*     head;
    struct function_header* next;
    size_t                  depth;
} function_header_t;

// List node to hold scanned constant data
typedef struct constant_data_list_node
{
    unsigned int                    eventualIndex;
    data_type_t                     type;
    void*                           data;
    struct constant_data_list_node* next;
} constant_data_list_node_t;

// Wrapper struct for scanned constant data list.
typedef struct constant_data_list
{
    constant_data_list_node_t* top;
    unsigned int               depth;
} constant_data_list_t;


// A struct for building a list of instructions and tracking
//  static data before converting to a program object.
typedef struct program_build
{
    // The hash
    token_hash_t         tokenHash;
    constant_data_list_t constantDataList;

    // Important pointers for building
    function_header_t*  programTop;
    function_header_t*  lastFunction;
    instruction_node_t* mainLast;
    instruction_node_t* currentInstruction;

    // Other
    bool         onMain;
    unsigned int functionAmmount;
} program_build_t;


// Returns an empty program build object.
program_build_t prepareBuild();

// Allocates a constant data list.
constant_data_list_t makeConstantDataList();

// Push new data to the constant data stack, returns which
//  index it'll be stored at during runtime.
int64_t pushConstantData(constant_data_list_t* constantDataList,
                         data_type_t           type,
                         void*                 data);

// Makes a dummy head
instruction_node_t* dummyInstruction();

// Adds a new instruction to buildPointers.currentInstruct
void appendInstruction(program_build_t*  programBuild,
                       instruction_type_t newInstruct,
                       int32_t           arg1,
                       int64_t           arg2);

// Adds a new function header to the end of the function queue.
void makeNewFunction(program_build_t* programBuild);

// Places return instruction on end of the instruction queue,
//  and sets things back to main.
void endFunction(program_build_t* programBuild);

// Takes an instruction node and converts it to an instruction data struct
instruction_t convertInstructionNode(instruction_node_t* instruction);

// takes a programBuild object and creates a complete 
//  initialized program context object ready for execution.
program_context_t returnProgram(program_build_t* program);

//#define BUILD_HEADER
#endif
