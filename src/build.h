#ifndef BUILD_HEADER
#include <stdbool.h>

#include "runtime.h"

// The max size of the array used by the hash.
// Must be 2^n-1
#define MAX_HASH_ARRAY_SIZE 0xFFFF;

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
    hash_type_t         hashedType;
    char*               symbol;
    size_t              symbolLength;
    struct hash_bucket* next;
    union 
    {
        unsigned int   ID;
        instruction_t* gotoDestination;
    } contents;

} hash_bucket_t;

// These two structs are used to store allocated
//  hash buckets in a list for later deallocation.
typedef struct hash_bucket_list_node
{
    hash_bucket_t*                entry;
    struct hash_bucket_list_node* next; 
} hash_bucket_list_node_t;

typedef struct hash_bucket_list
{
    hash_bucket_list_node_t* top;
} hash_bucket_list_t;

// A list node to keep track of allocated hash buckets
//  (for later deallocation)
typedef struct token_hash
{  
    hash_bucket_t**    hash;
    unsigned int       typeCount[h_gotoMemory+1];
    hash_bucket_list_t cleanupList;
    size_t             size;
} token_hash_t;

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

// Allocates and initializes an empty token hash.
token_hash_t makeTokenHash();

// Allocates a constant data list.
constant_data_list_t makeConstantDataList();

// Deletes the token hash 
void freeHash(token_hash_t* tokenHash);

// The hashing function to calculate a key's hash value.
// The algorithm used is a variation of the sdbm algorithm
//  (http://www.cse.yorku.ca/~oz/hash.html)
unsigned long hashFunction(size_t wordLength, const char* symbol);

// Get a key's corresponding bucket
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

// Checks for the existance of a token
bool peakHash(token_hash_t* tokenHash,
              hash_type_t   hashedType,
              size_t        symbolSize,
              const char*   symbolName);

// bool setHash(token_hash_t* tokenHash,
//              hash_type_t   hashedType,
//              size_t        symbolSize,
//              const char*   symbolName);

// Adds a new hash bucket to a hash object's bucket list.
void pushToList(token_hash_t*  tokenHash, 
                hash_bucket_t* slot);

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

#define BUILD_HEADER
#endif
