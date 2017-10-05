#ifndef HASH_HEADER
#include "instructions.h"

#include <stdlib.h>

#ifndef byte
 #define byte unsigned char
#endif

// Enumeration of hashed data types
typedef enum hash_type {
    h_nothing = 0,     // = 0
    h_variableName,    // = 1
    h_functionName,    // = 2
    h_labelName,       // = 3
    h_gotoMemory,      // = 4
} hashType_t;     

/* A struct suitable for keeping track of declared
 *  variables, labels, and function names.
 */
typedef struct hash_bucket 
{
    hashType_t hashedType;
    char* symbol;
    size_t symbolLength;
    struct hash_bucket* next;

    union 
    {
        unsigned int ID;
        instruction_t* noArgInstruction;
    } contents;

} hash_bucket_t;

// These two structs and function are used in implementing
//  deallocation of hash buckets
typedef struct hash_bucket_list_node
{
    hash_bucket_t* entry;
    struct hash_bucket_list_node* next; 
} hash_bucket_list_node_t;

typedef struct hash_bucket_list
{
    hash_bucket_list_node_t* top;
} hash_bucket_list_t;


// 0xFFFF = 2^20 - 1 = 1048575
static unsigned long maxArrayVal = 0xFFFF;
typedef struct runtime_hash
{  
    hash_bucket_t** hash;
    unsigned int typeCount[h_gotoMemory+1];
    hash_bucket_list_t cleanupList;
} runtime_hash_t;

// For testing
// extern hash_t HASH;

// The hashing function 
unsigned long hashFunction(size_t wordLength, const char* symbol);

void hashIni();
void freeHash();

// Set element of hash
unsigned int getHashID(hashType_t  toHashType,
                       size_t      symbolSize,
                       const char* symbolName);


void pushToList(hash_bucket_t* slot);

#define HASH_HEADER
#endif