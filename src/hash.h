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

/* The hashing function */
unsigned long hashFunction(size_t wordLength, const char* symbol);

/* A struct suitable for keeping track of declared
 *  variables, labels, and function names.
 */
typedef struct hash_bucket {
    hashType_t hashedType;
    char* symbol;
    size_t symbolLength;
    struct hash_bucket* next;

    union {
        unsigned int ID;
        instruction_t* noArgInstruction;
    } contents;

} hash_bucket_t;


 //! Change it so the only element is a hash bucket pointer
typedef hash_bucket_t** hash_t;

extern hash_t HASH;

/* Hash table initalization function */
void hashIni();
void freeHash();

/* Get a token's id */
unsigned int getHashID
(
    hashType_t  toHashType,
    size_t      symbolSize,
    const char* symbolName
);

typedef struct hash_bucket_list_node
{
    hash_bucket_t* entry;
    struct hash_bucket_list_node* next; 
} hash_bucket_list_node_t;


typedef struct hash_bucket_list_wrapper
{
    hash_bucket_list_node_t* top;
} hash_bucket_list_t;

void pushToList(hash_bucket_t* slot);

#define HASH_HEADER
#endif