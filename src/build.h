#ifndef BUILD_HEADER
#include <stdbool.h>

#include "runtime.h"


// Represents the type of data stored in a hash bucket
typedef enum hash_type {
    h_nothing = 0,     
    h_variableName,    
    h_functionName,    
    h_labelName,       
    h_gotoMemory,      
} hash_type_t;     


// Stores the corresponding id of a matched symbol
//  (variable, function name, etc.)
typedef struct hash_bucket 
{
    hash_type_t hashedType;
    char* symbol;
    size_t symbolLength;
    struct hash_bucket* next;
    union 
    {
        unsigned int ID;
        instruction_t* gotoDestination;
    } contents;

} hash_bucket_t;

// These two structs are used to store allocated
//  hash buckets in a list for later deallocation.
typedef struct hash_bucket_list_node
{
    hash_bucket_t* entry;
    struct hash_bucket_list_node* next; 
} hash_bucket_list_node_t;

typedef struct hash_bucket_list
{
    hash_bucket_list_node_t* top;
} hash_bucket_list_t;


// A structur suitable for storing values related to
//  different tokens
typedef struct token_hash
{  
    hash_bucket_t** hash;
    // Move typeCount out?
    unsigned int typeCount[h_gotoMemory+1];
    hash_bucket_list_t cleanupList;
} token_hash_t;

// A list node representing the beginning of a function
typedef struct function_header
{
    instruction_t* head;
    struct function_header* next;
} function_header_t;




typedef struct constant_data_list_node
{
    unsigned int eventualIndex;
    data_type_t type;
    void* data;
    struct constant_data_list_node* next;
} constant_data_list_node_t;

typedef struct constant_data_list
{
    constant_data_list_node_t* top;
    unsigned int depth;
} constant_data_list_t;



// parser
// tokenizer

// A struct meant to represent code before
//  its converted to a bytecode array.
typedef struct program_build
{
    // The hash
    token_hash_t         tokenHash;
    constant_data_list_t constantDataList;

    // Important pointers for building
    function_header_t* programTop;
    function_header_t* lastFunction;
    instruction_t*     mainLast;
    instruction_t*     currentInstruction;

    // Other
    bool onMain;
    unsigned int functionAmmount;
} program_build_t;


// The max size of the array.
// Note: turn this into a macro.
static unsigned long maxArrayVal = 0xFFFF;

// Returns an empty program build object.
// Todo: rename?
program_build_t prepareBuild();


// Note: make tries and hash type a parameter
// Allocates and initializes a new token hash.
token_hash_t makeTokenHash();
constant_data_list_t makeConstantDataList();

// Todo: GARBAGE CLEAN

// Free a token Hash
void freeHash(token_hash_t* tokenHash);

// The hashing function 
unsigned long hashFunction(size_t wordLength, const char* symbol);

hash_bucket_t* getBucket(token_hash_t* tokenHash,
                         hash_type_t   hashedType,
                         size_t        symbolSize,
                         const char*   symbolName);



// Returns the ID of a token (regardless if it exists
//  in the hash already or not)
unsigned int getHashID(token_hash_t* tokenHash,
                       hash_type_t   toHashType,
                       size_t        symbolSize,
                       const char*   symbolName);

bool peakHash(token_hash_t* tokenHash,
              hash_type_t   hashedType,
              size_t        symbolSize,
              const char*   symbolName);





// Adds a new hash bucket to a hash object's bucket list.
void pushToList(token_hash_t*  tokenHash, 
                hash_bucket_t* slot);

// Push new data to the constant data stack
int64_t pushConstantData(constant_data_list_t* constantDataList,
                      data_type_t type,
                      void* data);

unsigned int nextIndex(constant_data_list_t* constantDataList);

// Makes a dummy head
inline instruction_t* dummyInstruction();

// Adds a new instruction to buildPointers.currentInstruct
void appendInstruction(program_build_t*  programBuild,
                       instructionType_t newInstruct,
                       int32_t           arg1,
                       int64_t           arg2);


// Adds a new function header to the end of the function queue.
void makeNewFunction(program_build_t* programBuild);

// Ends a function
void endFunction(program_build_t* programBuild);

// takes a programBuild object and creates a complete 
//  initialized program context object ready for execution
program_context_t returnProgram(program_build_t* program);


#define BUILD_HEADER
#endif
