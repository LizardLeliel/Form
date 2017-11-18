#include "build.h"
#include "runtime.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <stdio.h>

program_build_t prepareBuild()
{
    program_build_t programBuild;

    // Initalize the hash
    programBuild.tokenHash          = makeTokenHash();
    programBuild.constantDataList   = makeConstantDataList();

    // Intialize the pointers
    programBuild.programTop         = malloc(sizeof(function_header_t));
    programBuild.currentInstruction = malloc(sizeof(instruction_t));

    programBuild.programTop->head   = programBuild.currentInstruction;
    programBuild.programTop->next   = NULL;

    programBuild.lastFunction       = programBuild.programTop;
    programBuild.mainLast           = programBuild.currentInstruction;

    programBuild.currentInstruction->instruction = nop;

    // Initialize the misc variables
    programBuild.onMain = true;
    programBuild.functionAmmount = 1;

    return programBuild;
}

token_hash_t makeTokenHash()
{
    int tries = 3;
    token_hash_t tokenHash;

    tokenHash.cleanupList.top = NULL;
    tokenHash.hash = calloc(maxArrayVal + 1, sizeof(hash_bucket_t*));

    // If calloc failed, try again with a smaller size
    while (tokenHash.hash == NULL)
    {
        maxArrayVal >>= 1;
        tokenHash.hash = calloc(maxArrayVal + 1, sizeof(hash_bucket_t*));

        if (--tries == 0)
        {
            perror
            ("Unable to allocate memory for hash table while scanning\n");
        }
    }

    memset(tokenHash.typeCount, 0, sizeof tokenHash.typeCount);

    return tokenHash;
}

constant_data_list_t makeConstantDataList()
{
    constant_data_list_t constantDataList;

    constantDataList.top   = NULL;
    constantDataList.depth = 0;

    return constantDataList;
}

void freeHash(token_hash_t* tokenHash)
{
    hash_bucket_list_node_t* tracer;
    hash_bucket_list_node_t* toFree;
    tracer = tokenHash->cleanupList.top;
  
    while (tracer != NULL)
    {
        free(tracer->entry->symbol);
        free(tracer->entry);
        toFree = tracer;
        tracer = tracer->next;
        free(toFree);
    }
    free(tokenHash->hash);
}

// The algorithm used is a variation of the sdbm algoritn
//  (http://www.cse.yorku.ca/~oz/hash.html)
unsigned long hashFunction(size_t wordLength, const char* symbol) 
{
    unsigned long hashed_value = 0;
    int c = 0;

    do 
    {
        hashed_value = symbol[c] + (hashed_value << 6) 
                     + (hashed_value << 16) - hashed_value;
    } while (++c < wordLength);

    return hashed_value;
}

hash_bucket_t* getBucket(token_hash_t* tokenHash,
                         hash_type_t   hashedType,
                         size_t        symbolSize,
                         const char*   symbolName)
{
    unsigned long index = maxArrayVal & hashFunction(symbolSize, symbolName);

    if (tokenHash->hash[index] == NULL)
    {
        return NULL;
    }

    hash_bucket_t* tracer = tokenHash->hash[index];

    while (tracer != NULL)
    {
        if (tracer->symbolLength == symbolSize 
            && memcmp(tracer->symbol, symbolName, symbolSize) == 0) 
        {
            return tracer;
        }
        tracer = tracer->next;
    }

    return NULL;
}

unsigned int getHashID(token_hash_t* tokenHash,
                       hash_type_t   toHashType, 
                       size_t        symbolSize,
                       const char*   symbolName) 
{

    unsigned long index = maxArrayVal & hashFunction(symbolSize, symbolName);

    // If hash entry is empty, set a new one
    // To do: make a function for creating hash entries
    if (tokenHash->hash[index] == NULL) 
    {
        (tokenHash->hash[index]) = malloc(sizeof(hash_bucket_t));

        // Set values
        (tokenHash->hash[index])->hashedType   = toHashType;
        (tokenHash->hash[index])->contents.ID  = ++(tokenHash->typeCount[toHashType]);
        (tokenHash->hash[index])->next         = NULL;
        (tokenHash->hash[index])->symbolLength = symbolSize;

        (tokenHash->hash[index])->symbol
            = memcpy(malloc(symbolSize), symbolName, symbolSize);

        pushToList(tokenHash, tokenHash->hash[index]);

        return tokenHash->typeCount[toHashType];
    }

    // Else, check each node in list to see if symbol already exists. Start
    //  with a dummy list node (Is there a simplier way to do this?)
    hash_bucket_t  dummy;
    hash_bucket_t* tracer = &dummy;

    tracer->next = tokenHash->hash[index];

    do 
    {
        tracer = tracer->next;
        // If the words are the same length and are the same
        if (tracer->symbolLength == symbolSize) 
        {
            if (memcmp(tracer->symbol, symbolName, symbolSize) == 0) 
            {
                return tracer->contents.ID;
            }
        }

    } while (tracer->next != NULL);

    // If trace->next == null, then we need to make a new hash entry
    //  (because one does not exist)
    tracer = tracer->next = malloc(sizeof(hash_bucket_t));
    tracer->hashedType    = toHashType;
    tracer->symbolLength  = symbolSize;
    tracer->contents.ID   = ++tokenHash->typeCount[toHashType];

    tracer->symbol = memcpy(malloc(symbolSize), symbolName, symbolSize);

    return tokenHash->typeCount[toHashType];
}

bool peakHash(token_hash_t* tokenHash,
              hash_type_t   hashedType,
              size_t        symbolSize,
              const char*   symbolName)
{
    return getBucket(tokenHash, hashedType, symbolSize, symbolName) == NULL 
        ? false
        : true;
}

// Adds to list of hash buckets (for freeing later)
void pushToList(token_hash_t* tokenHash,
                hash_bucket_t* slot)
{
    if (tokenHash->cleanupList.top == NULL)
    {
        tokenHash->cleanupList.top = malloc(sizeof(hash_bucket_list_node_t));

        tokenHash->cleanupList.top->entry = slot;
        tokenHash->cleanupList.top->next  = NULL;
    }
    else
    {
        hash_bucket_list_node_t* newNode 
            = malloc(sizeof(hash_bucket_list_node_t));
        newNode->next = tokenHash->cleanupList.top;
        newNode->entry = slot;
        tokenHash->cleanupList.top = newNode;
    }
}

int64_t pushConstantData(constant_data_list_t* constantDataList,
                      data_type_t type,
                      void* data)
{

    constant_data_list_node_t* newNode 
        = malloc(sizeof (constant_data_list_node_t));

    newNode->type = type;
    newNode->data = data;
    newNode->eventualIndex = (constantDataList->depth);
        
    ++(constantDataList->depth);

    if (constantDataList->top == NULL)
    {    
        constantDataList->top = newNode;
    }
    else
    {
        newNode->next         = constantDataList->top;
        constantDataList->top = newNode;       
    }

    return newNode->eventualIndex;
}

// 
// 

unsigned int nextIndex(constant_data_list_t* constantDataList)
{
    return constantDataList->depth + 1;
}

// Returns a dummy instruction
instruction_t* dummyInstruction() 
{
    instruction_t* dummy = malloc(sizeof(instruction_t));
    dummy->instruction   = nop;
    //dummy->argSize       = 0;
    //dummy->args          = NULL;
    dummy->arg1          = 0;
    dummy->arg2          = 0;
    dummy->next          = NULL;
    return dummy;
}

// Adds a new instruction to end of current instruction sequence.
void appendInstruction(program_build_t*  programBuild,
                       instructionType_t newInstruct,
                       int32_t           arg1,
                       int64_t           arg2)
{

    instruction_t* newInstructNode = malloc(sizeof(instruction_t));
    newInstructNode->instruction   = newInstruct;
    //newInstructNode->argSize       = argSize; // In bytes
    newInstructNode->next          = NULL;
    newInstructNode->arg1          = arg1;
    newInstructNode->arg2          = arg2;

    programBuild->currentInstruction->next = newInstructNode;
    programBuild->currentInstruction       = newInstructNode;

    if (programBuild->onMain == true) 
    {
        programBuild->mainLast = newInstructNode;
    }
}


void makeNewFunction(program_build_t* programBuild) 
{
    // Allocates and initialize memory
    function_header_t* newFunction = malloc(sizeof(function_header_t));
    newFunction->next = NULL;
    newFunction->head = dummyInstruction();

    // Assigns build pointer functions 
    programBuild->currentInstruction = newFunction->head;
    programBuild->lastFunction->next = newFunction;
    programBuild->lastFunction       = newFunction;

    programBuild->onMain = false;
}

// Places return instruction on end of the instruction queue,
//  and configures things back to main
void endFunction(program_build_t* programBuild) 
{
    instruction_t* endInstruct = calloc(1, sizeof(instruction_t));
    endInstruct->instruction   = returns;

    programBuild->currentInstruction->next = endInstruct;
    programBuild->currentInstruction       = programBuild->mainLast;

    programBuild->onMain = true;
}



program_context_t returnProgram(program_build_t* programBuild)
{
    // Build program.
    program_context_t program;
    program.functionStack.depth = 0;

    // Create a function array
    program.code = malloc(sizeof(instruction_t*) 
                               * programBuild->functionAmmount);

    int i = 0;
    function_header_t* tracer = programBuild->programTop;

    // Valgrind doesn't like us using pointer arthmetic. 
    while (tracer != NULL)
    {
        program.code[i] = tracer->head;
        ++i;
        tracer = tracer->next;
    }

    size_t bankSize = programBuild->constantDataList.depth;
    program.staticDataBank.size     = bankSize;
    
    program.staticDataBank.dataBank = calloc(bankSize, sizeof (static_data_t));

    constant_data_list_node_t* static_data_tracer 
        = programBuild->constantDataList.top;

    while (static_data_tracer != NULL)
    {

        program.staticDataBank.dataBank[static_data_tracer->eventualIndex].type
            = static_data_tracer->type;

        program.staticDataBank.dataBank[static_data_tracer->eventualIndex].data 
            = static_data_tracer->data;

        static_data_tracer = static_data_tracer->next;
    }

    return program;
}


