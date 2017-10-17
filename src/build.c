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

// Returns a dummy instruction
instruction_t* dummyInstruction() 
{
    instruction_t* dummy = malloc(sizeof(instruction_t));
    dummy->instruction   = nop;
    dummy->argSize       = 0;
    dummy->args          = NULL;
    dummy->next          = NULL;
    return dummy;
}

// Adds a new instruction to end of current instruction sequence.
void appendInstruction(program_build_t* programBuild,
                       instructionType_t newInstruct,
                       size_t argSize, 
                       void* args) 
{
    instruction_t* newInstructNode = malloc(sizeof(instruction_t));
    newInstructNode->instruction   = newInstruct;
    newInstructNode->argSize       = argSize; // In bytes
    newInstructNode->next          = NULL;


    if (argSize == 0) 
    {
        // Make sure that NULL args aren't
        //  free()'d.
        newInstructNode->args = NULL; // Copying memory wouldn't be nessecary
    }
    else 
    {
        newInstructNode->args = malloc(argSize);
        memcpy(newInstructNode->args, args, argSize);
    }

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

    return program;
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


unsigned int getHashID(token_hash_t* tokenHash,
                       hashType_t toHashType, 
                       size_t symbolSize,
                       const char* symbolName) 
{

    unsigned long index = maxArrayVal & hashFunction(symbolSize, symbolName);

    // If hash entry is empty, set a new one
    // To do: make a function for creating hash entries
    if (tokenHash->hash[index] == NULL) 
    {
        (tokenHash->hash[index]) = malloc(sizeof(hash_bucket_t));

        // Set values
        (tokenHash->hash[index])->hashedType  = toHashType;
        (tokenHash->hash[index])->contents.ID = ++(tokenHash->typeCount[toHashType]);
        (tokenHash->hash[index])->next        = NULL;

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
        if (tracer->symbolLength != symbolSize) 
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

    return ++tokenHash->typeCount[toHashType];
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


