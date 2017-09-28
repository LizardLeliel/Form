#include "hash.h"
#include "bigdecls.h"

#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <stdio.h>

/* Using a macro would be bad; the variable may change. The value MUST be
 *  a number expressable by 2^n-1.
 */
#define initialHashArraySize 1048575
unsigned long maxArrayVal = initialHashArraySize;
#undef initialHashArraySize

/* The hashtable */
hash_t HASH;
hash_bucket_list_t LIST;

// Counts how much of 
unsigned int counters[h_labelName+1] = {0,0,0,0};

/* The hash function is implemented with an array size and not a null-
 *  terminated string because it'll be simplier to determine
 *  the range of the actual symbol (which, as text, is going to include
 *  extra chars, such as the white space on the end), instead of trim the
 *  extra chars and add the new null character and so on.
 * One other note: the whole value isn't used; the first few bits will be &'d,
 *  and used as the hash array index (so the hash array doesn't need to have as
 *  many elements equal to the highest calue of unsigned long; that would use
 *  too much unnesecary memory
 * The algorithm used is a variation of the sdbm algoritn
 *  (http://www.cse.yorku.ca/~oz/hash.html)
 */
unsigned long hashFunction(size_t wordLength, const char* symbol) {
    unsigned long hash = 0;
    int c = 0;

    do {
        hash = symbol[c] + (hash << 6) + (hash << 16) - hash;
    } while (++c < wordLength);

    return hash;
}


void hashIni() {
    static int tries = 3;
    HASH = calloc(maxArrayVal+1, sizeof(hash_t));
    LIST.top = NULL;

    /* if malloc/calloc fails, try again with smaller ammounts */
    while (HASH == NULL) 
    {
        maxArrayVal >>= 1;
        HASH = calloc(maxArrayVal+1, sizeof(hash_t));

        if (--tries == 0) 
        {
            perror
            ("Unable to allocate memory for hash table while scanning\n");
        }
    } 

    //! DELETE EVENTUALLY
    printf("SIZEOF HASH ARRAY: %lu BYTES\n", sizeof(hash_t)*maxArrayVal);

} // function

/* This function will always return an unisnged integer. It first checks in the
 *  hash to see if hash bucket has entries. If not, it'll add a new entry
 */
unsigned int getHashID(hashType_t toHashType, size_t symbolSize,
                       const char* symbolName) {
    // Get hash index
    //! Test to make sure it handles collisions well by hardcoding and index,
    //!  and seeing if it'll behave correctly given 2 different tokens without
    //!  deference NULL
    unsigned long index = maxArrayVal&hashFunction(symbolSize, symbolName);

    // If hash entry is empty, set a new one
    //! To do: make a function for creating hash entries
    if (HASH[index] == NULL) {
        (HASH[index]) = malloc(sizeof(hashBucket_t));

        // Set values
        (HASH[index])->hashedType  = toHashType;
        (HASH[index])->contents.ID = ++(counters[toHashType]);
        (HASH[index])->next        = NULL;

        (HASH[index])->symbol
            = memcpy(malloc(symbolSize), symbolName, symbolSize);

        pushToList(HASH[index]);

        return counters[toHashType];
    }

    // Else, check each node in list to see if symbol already exists. Start
    //  with a dummy list node (Is there a simplier way to do this?)
    hashBucket_t* tracer = malloc(sizeof(hashBucket_t));
    hashBucket_t* freeThisDummy = tracer;
    tracer->next = HASH[index];

    do {
        tracer = tracer->next;
        // If the words are the same length and are the same
        //! Todo: Make it an && condidtional expression
        if (tracer->symbolLength != symbolSize) {
            if (memcmp(tracer->symbol, symbolName, symbolSize) == 0) {
                free(freeThisDummy);
                return tracer->contents.ID;
            }
        }

    } while (tracer->next != NULL);

    // Now's the best time to free!
    free(freeThisDummy);

    // If trace->next == null, then we need to make a new hash entry
    tracer = tracer->next = malloc(sizeof(hashBucket_t));
    tracer->hashedType    = toHashType;
    tracer->symbolLength  = symbolSize;
    tracer->contents.ID   = ++counters[toHashType];

    tracer->symbol = memcpy(malloc(symbolSize), symbolName, symbolSize);

    return counters[toHashType];
}

void pushToList(hashBucket_t* slot)
{
    if (LIST.top == NULL)
    {
        LIST.top = malloc(sizeof(hash_bucket_list_node_t));

        LIST.top->entry = slot;
        LIST.top->next  = NULL;
    }
    else
    {
        hash_bucket_list_node_t* newNode 
            = malloc(sizeof(hash_bucket_list_node_t));
        newNode->next = LIST.top;
        newNode->entry = slot;
        LIST.top = newNode;
    }
}

void freeHash()
{
    // Free hash buckets
    // Free hash bucket list
    // Free hash array
    hash_bucket_list_node_t* tracer;
    hash_bucket_list_node_t* toFree;
    tracer = LIST.top;
    while (tracer != NULL)
    {
        free(tracer->entry);
        toFree = tracer;
        tracer = tracer->next;
        free(toFree);
    }

    free(HASH);

}