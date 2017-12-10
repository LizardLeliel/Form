#ifndef TOKENHASH_HEADER
#define TOKENHASH_HEADER

// #include "build.h"

#include <stdbool.h>
#include <stdlib.h>

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
    hash_type_t         hashType;
    char*               symbol;
    size_t              symbolLength;
    struct hash_bucket* next;    
    unsigned int        ID;

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

// Allocates and initializes an empty token hash.
token_hash_t makeTokenHash();

// Deletes the token hash 
void freeHash(token_hash_t* tokenHash);

// The hashing function to calculate a key's hash value.
// The algorithm used is a variation of the sdbm algorithm
//  (http://www.cse.yorku.ca/~oz/hash.html)
unsigned long hashFunction(size_t wordLength, const char* symbol);

// Creates hash bucket for new key, intializes it with next value
//  as displayed in array if nextID is true, or 0 if false. Bool returned
//  is false if key already exists (and therefore fails) or true
//  if key does not already exist.
bool createHashBucket(token_hash_t* tokenHash,
                      hash_type_t   hashedType,
                      size_t        symbolSize,
                      const char*   symbolName,
                      bool          nextID);

// Get a key's corresponding bucket
hash_bucket_t* getBucket(token_hash_t* tokenHash,
                         hash_type_t   hashedType,
                         size_t        symbolSize,
                         const char*   symbolName);

// Returns the value associated with the key
unsigned int getHashValue(token_hash_t* tokenHash,
                          hash_type_t   toHashType,
                          size_t        symbolSize,
                          const char*   symbolName);

// Sets the value for a key. The bool returns true if the bucket 
//  has existed previously.
bool setHashValue(token_hash_t* tokenHash,
                  hash_type_t   toHashType,
                  size_t        symbolSize,
                  const char*   symbolName,
                  unsigned int  newID);



// Checks for the existance of a token
bool peakHash(token_hash_t* tokenHash,
              hash_type_t   hashedType,
              size_t        symbolSize,
              const char*   symbolName);


// Adds a new hash bucket to a hash object's bucket list.
void pushToList(token_hash_t*  tokenHash, 
                hash_bucket_t* slot);

// TOKENHASH_HEADER
#endif