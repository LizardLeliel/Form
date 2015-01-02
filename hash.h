#ifndef HASH_HEADER
#include "bigdecls.h"

#include <stdlib.h>

#ifndef byte
 #define byte unsigned char
#endif

/* The hashing function */
unsigned long hashFunction(size_t wordLength, char* symbol);

/* A struct suitable for keeping track of declared
 *  variables, labels, and function names.
 */
typedef struct hash_bucket_slot {
    byte ID;
    hashType_t hashedTyped;
    char* symbol;
    struct hash_bucket_slot* next;
} hashBucket_t;

 //! Change it so the only element is a hash bucket pointer
typedef hashBucket_t** hash_t;

extern hash_t* HASH;

/* Hash table initalization function */
void hashIni();

/* Set a new entry in a hash */
void setHashEntry
(
    hashType_t  toHashType,
    size_t      symbolSize,
    const char* symbolName
);



#define HASH_HEADER
#endif
