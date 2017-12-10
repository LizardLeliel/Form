#include "tokenhash.h"
// #include "build.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Allocates and initializes an empty token hash.
token_hash_t makeTokenHash()
{
    // Initializes token hash
    token_hash_t tokenHash;
    unsigned int size         = MAX_HASH_ARRAY_SIZE;
    tokenHash.cleanupList.top = NULL;
    tokenHash.hash
        = calloc(size, sizeof(hash_bucket_t*));

    // Tru to allocate the hash array (default ~1mb of ram)
    unsigned int tries = 3;
    while (tokenHash.hash == NULL)
    {
        size >>= 1;
        tokenHash.hash = calloc(size, sizeof(hash_bucket_t*));

        if (--tries == 0)
        {
            perror
            ("Unable to allocate memory for hash table while scanning\n");
            exit(1);
        }
    }
    tokenHash.size = size;

    // Fill the typecount array with default values
    memset(tokenHash.typeCount, 0, sizeof tokenHash.typeCount);

    return tokenHash;
}

// Deletes the token hash 
void freeHash(token_hash_t* tokenHash)
{
    hash_bucket_list_node_t* tracer;
    tracer = tokenHash->cleanupList.top;
  
    // Iterate through the list, deleting each node's entry, symbol,
    //  and the node itself
    while (tracer != NULL)
    {
        hash_bucket_list_node_t* toFree;
        free(tracer->entry->symbol);
        free(tracer->entry);
        toFree = tracer;
        tracer = tracer->next;
        free(toFree);
    }
    free(tokenHash->hash);
}

// The hashing function to calculate a key's hash value.
// The algorithm used is a variation of the sdbm algorithm
//  (http://www.cse.yorku.ca/~oz/hash.html)
unsigned long hashFunction(size_t wordLength, const char* symbol) 
{
    unsigned long hashed_value = 0;
    int c                      = 0;
    do 
    {
        hashed_value = symbol[c] + (hashed_value << 6) 
                     + (hashed_value << 16) - hashed_value;
    } while (++c < wordLength);
    return hashed_value;
}

// Get a key's corresponding bucket
hash_bucket_t* getBucket(token_hash_t* tokenHash,
                         hash_type_t   hashedType,
                         size_t        symbolSize,
                         const char*   symbolName)
{
    // Hash the data.
    unsigned long index = tokenHash->size & hashFunction(symbolSize, symbolName);

    // Return nothing if the bucket doesn't exist.
    if (tokenHash->hash[index] == NULL)
    {
        return NULL;
    }

    // If a bucket exists, search through the list (since they'll be a list
    //  if there's a collision).
    hash_bucket_t* tracer = tokenHash->hash[index];

    while (tracer != NULL)
    {
        if (tracer->symbolLength == symbolSize 
            && tracer->hashedType == hashedType
            && memcmp(tracer->symbol, symbolName, symbolSize) == 0) 
        {
            return tracer;
        }
        tracer = tracer->next;
    }

    // We reached end of list, there is no 
    return NULL;
}

unsigned int getHashID(token_hash_t* tokenHash,
                       hash_type_t   toHashType, 
                       size_t        symbolSize,
                       const char*   symbolName) 
{

    unsigned long index = tokenHash->size & hashFunction(symbolSize, symbolName);

    // If hash element is NULL, create a new one.
    if (tokenHash->hash[index] == NULL) 
    {
        (tokenHash->hash[index]) = malloc(sizeof(hash_bucket_t));

        // Set values.
        (tokenHash->hash[index])->hashedType   = toHashType;
        (tokenHash->hash[index])->contents.ID  = ++(tokenHash->typeCount[toHashType]);
        (tokenHash->hash[index])->next         = NULL;
        (tokenHash->hash[index])->symbolLength = symbolSize;

        (tokenHash->hash[index])->symbol
            = memcpy(malloc(symbolSize), symbolName, symbolSize);

        // Put the bucket into the list for later deallocation.
        pushToList(tokenHash, tokenHash->hash[index]);

        return tokenHash->typeCount[toHashType];
    }

    // Else, check each node in list to see if symbol already exists. Start
    //  with a dummy list node
    hash_bucket_t  dummy;
    hash_bucket_t* tracer = &dummy;

    tracer->next = tokenHash->hash[index];
    do 
    {
        tracer = tracer->next;

        // If the tokens are the same length, type, and are the same
        if (tracer->symbolLength  == symbolSize 
            && tracer->hashedType == toHashType
            && memcmp(tracer->symbol, symbolName, symbolSize) == 0) 
        {
            return tracer->contents.ID;
        }

    } while (tracer->next != NULL);

    // The end of the list is reached, so the element doesn't already 
    //  exist, and needs to be added
    tracer = tracer->next = malloc(sizeof(hash_bucket_t));
    tracer->hashedType    = toHashType;
    tracer->symbolLength  = symbolSize;
    tracer->contents.ID   = ++tokenHash->typeCount[toHashType];
    tracer->symbol        = memcpy(malloc(symbolSize), symbolName, symbolSize);

    return tokenHash->typeCount[toHashType];
}

// Checks for the existance of a token
bool peakHash(token_hash_t* tokenHash,
              hash_type_t   hashedType,
              size_t        symbolSize,
              const char*   symbolName)
{
    return getBucket(tokenHash, hashedType, symbolSize, symbolName) == NULL 
        ? false
        : true;
}

// Adds a new hash bucket to a hash object's bucket list.
void pushToList(token_hash_t*  tokenHash,
                hash_bucket_t* slot)
{
    if (tokenHash->cleanupList.top == NULL)
    {
        tokenHash->cleanupList.top
            = malloc(sizeof(hash_bucket_list_node_t));
        tokenHash->cleanupList.top->entry = slot;
        tokenHash->cleanupList.top->next  = NULL;
    }
    else
    {
        hash_bucket_list_node_t* newNode 
            = malloc(sizeof(hash_bucket_list_node_t));
        newNode->next              = tokenHash->cleanupList.top;
        newNode->entry             = slot;
        tokenHash->cleanupList.top = newNode;
    }
}
