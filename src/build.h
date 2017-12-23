#ifndef BUILD_HEADER
#define BUILD_HEADER

#include <stdbool.h>

#include "runtime.h"
#include "tokenhash.h"


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

// Might not be nessecary; a function to convert this could be
//  made instead.
typedef struct if_hash_sequence
{
    unsigned int functionNumber;
    unsigned int idNumber;
    unsigned int elseNumber;
    unsigned int scoping; // May not be nessecary
    unsigned int sequence;
} if_hash_sequence_t;

typedef struct if_sequence_tracker
{
    unsigned int sequence; 
    unsigned int functionNumber; // Might be able to be infered
                                 // from the function header.
    unsigned int elifSequence;
    unsigned int scope; // 0, then not in if sequence
    bool         thenFlag; // Might be a boolean?
    bool         elseFlag; // Might also aalso be a boolean?

    unsigned int nextID;
} if_sequence_tracker_t;

// Saves info about the parsing of an if sequence in a previous scope.
typedef struct scope_branch_info
{
    unsigned int scope; // "Can be infered" my notes say.
    bool         thenFlag;
    bool         elseFlag;
    unsigned int id;
    unsigned int sequence; // notes say this can also be inferred.
} scope_branch_info_t;

typedef struct scope_branch_info_node
{
    struct scope_branch_info_node* next;
    scope_branch_info_t            info;
} scope_branch_info_node_t;

typedef struct scope_branch_info_stack
{
    scope_branch_info_node_t* head;
    unsigned int              depth; // Might not be nessecary.
} scope_branch_info_stack_t;


typedef struct instruction_node
{
    instruction_type_t       instruction;
    unsigned int             index;
    int32_t                  arg1;
    int64_t                  arg2;
    hash_bucket_t*           arg2ref;
    struct instruction_node* next;
} instruction_node_t;

// A list node representing the beginning of a function
typedef struct function_header
{
    instruction_node_t*       head;
    struct function_header*   next;
    size_t                    depth;
    if_sequence_tracker_t     ifTracker;
    scope_branch_info_stack_t scopeBranchInfoStack;
} function_header_t;

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
    bool          onMain;
    size_t*       currentDepth;
    unsigned int  functionAmount;
    unsigned int  lineNumber;
} program_build_t;


// Returns an empty program build object.
program_build_t prepareBuild();

// Allocates a constant data list.
constant_data_list_t makeConstantDataList();

// Creates empty if tracker struct
if_sequence_tracker_t  createIfTracker(unsigned int functionNumber);
if_sequence_tracker_t* getTracker(program_build_t* program);

#define uint unsigned int
void hashableIfInfo(char* buffer,
                    uint  fn, // function number
                    uint  seq, // sequence
                    uint  eiseq, // elifsequence
                    uint  scope,
                    uint  tflag, // then flag
                    uint  eflag); //else flag
#undef uint

// Push new data to the constant data stack, returns which
//  index it'll be stored at during runtime.
int64_t pushConstantData(constant_data_list_t* constantDataList,
                         data_type_t           type,
                         void*                 data);

void pushScopeBranchInfo(scope_branch_info_stack_t* infoStack,
                         if_sequence_tracker_t      info);
 
void popScopeBranchInfo(scope_branch_info_stack_t* infoStack,
                        if_sequence_tracker_t*     info);
// Makes a dummy head
instruction_node_t* dummyInstruction();

// Adds a new instruction to buildPointers.currentInstruct
void appendInstruction(program_build_t*   programBuild,
                       instruction_type_t newInstruct,
                       int32_t            arg1,
                       int64_t            arg2);

void attachBucket(program_build_t* programBuild,
                  hash_bucket_t*   bucket);

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
