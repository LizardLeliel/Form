#ifndef HASH_HEADER
#include "bigdecls.h"
<<<<<<< HEAD
#include "instructions.h"
=======
>>>>>>> fbaab0080dc700dae1c8ddc879a0d3c19c65ef50

#include <stdlib.h>

#ifndef byte
 #define byte unsigned char
#endif

/* The hashing function */
<<<<<<< HEAD
unsigned long hashFunction(size_t wordLength, const char* symbol);
=======
unsigned long hashFunction(size_t wordLength, char* symbol);
>>>>>>> fbaab0080dc700dae1c8ddc879a0d3c19c65ef50

/* A struct suitable for keeping track of declared
 *  variables, labels, and function names.
 */
typedef struct hash_bucket_slot {
<<<<<<< HEAD
    hashType_t hashedType;
    char* symbol;
    size_t symbolLength;
    struct hash_bucket_slot* next;

   union {
        unsigned int ID;
        instruction_t* noArgInstruction;
    } contents;

=======
    byte ID;
    hashType_t hashedTyped;
    char* symbol;
    struct hash_bucket_slot* next;
>>>>>>> fbaab0080dc700dae1c8ddc879a0d3c19c65ef50
} hashBucket_t;

 //! Change it so the only element is a hash bucket pointer
typedef hashBucket_t** hash_t;

<<<<<<< HEAD
extern hash_t HASH;
=======
extern hash_t* HASH;
>>>>>>> fbaab0080dc700dae1c8ddc879a0d3c19c65ef50

/* Hash table initalization function */
void hashIni();

<<<<<<< HEAD
/* Get a token's id */
unsigned int getHashID
=======
/* Set a new entry in a hash */
void setHashEntry
>>>>>>> fbaab0080dc700dae1c8ddc879a0d3c19c65ef50
(
    hashType_t  toHashType,
    size_t      symbolSize,
    const char* symbolName
);



#define HASH_HEADER
#endif
