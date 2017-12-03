#include "build.h"
#include "runtime.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <stdio.h>

// Returns an empty program build object.
program_build_t prepareBuild()
{
    program_build_t programBuild;

    // Initalize the hash
    programBuild.tokenHash          = makeTokenHash();
    programBuild.constantDataList   = makeConstantDataList();

    // Intialize pointers
    programBuild.programTop         = malloc(sizeof(function_header_t));
    programBuild.currentInstruction = dummyInstruction();
    programBuild.lastFunction       = programBuild.programTop;
    programBuild.mainLast           = programBuild.currentInstruction;

    // Initializes the main function
    programBuild.programTop->head   = programBuild.currentInstruction;
    programBuild.programTop->next   = NULL;
    programBuild.programTop->depth  = 1;

    // Initialize the misc variables
    programBuild.onMain = true;
    programBuild.functionAmmount = 1;

    return programBuild;
}

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

// Allocates a constant data list.
constant_data_list_t makeConstantDataList()
{
    constant_data_list_t constantDataList;

    constantDataList.top   = NULL;
    constantDataList.depth = 0;

    return constantDataList;
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

// Push new data to the constant data stack, returns which
//  index it'll be stored at during runtime.
int64_t pushConstantData(constant_data_list_t* constantDataList,
                        data_type_t            type,
                        void*                  data)
{
    // Initialize constant data node.
    constant_data_list_node_t* newNode 
        = malloc(sizeof (constant_data_list_node_t));
    newNode->type = type;
    newNode->data = data;
    newNode->eventualIndex = (constantDataList->depth);
        
    ++(constantDataList->depth);

    // Add the new node
    newNode->next         = constantDataList->top;
    constantDataList->top = newNode;       

    return newNode->eventualIndex;
}


// Returns a dummy instruction
instruction_node_t* dummyInstruction() 
{
    instruction_node_t* dummy = malloc(sizeof(instruction_node_t));
    dummy->instruction        = i_nop;
    dummy->arg1               = 0;
    dummy->arg2               = 0;
    dummy->next               = NULL;
    return dummy;
}

// Adds a new instruction to end of current instruction sequence.
void appendInstruction(program_build_t*   programBuild,
                       instruction_type_t newInstruct,
                       int32_t            arg1,
                       int64_t            arg2)
{
    // Initialize the instruction pointer
    instruction_node_t* newInstructNode = malloc(sizeof(instruction_node_t));
    newInstructNode->instruction        = newInstruct;
    newInstructNode->next               = NULL;
    newInstructNode->arg1               = arg1;
    newInstructNode->arg2               = arg2;

    // Determine if we need to increase the main function's depth
    //  or the last-function-added's depther
    if (programBuild->onMain == true)
    {
        programBuild->programTop->depth += 1;
    }
    else
    {
        programBuild->lastFunction->depth += 1;
    }

    // Append instruction to end of function.
    programBuild->currentInstruction->next = newInstructNode;
    programBuild->currentInstruction       = newInstructNode;

    // Save the last instruction in main for when we finish
    //  scanning a function.
    if (programBuild->onMain == true) 
    {
        programBuild->mainLast = newInstructNode;
    }
}

// Adds a new function header to the end of the function queue.
void makeNewFunction(program_build_t* programBuild) 
{
    // Allocates and initialize memory
    function_header_t* newFunction = malloc(sizeof(function_header_t));
    newFunction->next  = NULL;
    newFunction->head  = dummyInstruction();
    newFunction->depth = 1;

    // Assigns build pointer functions 
    programBuild->currentInstruction = newFunction->head;
    programBuild->lastFunction->next = newFunction;
    programBuild->lastFunction       = newFunction;

    // set other values.
    programBuild->functionAmmount += 1;
    programBuild->onMain           = false;
}

// Places return instruction on end of the instruction queue,
//  and sets things back to main.
void endFunction(program_build_t* programBuild) 
{
    // Append the return instruction to end of function
    appendInstruction(programBuild, i_return, 0, 0);

    // End the function's instruction list
    programBuild->currentInstruction->next = NULL;

    // Set things back onto main
    programBuild->currentInstruction = programBuild->mainLast;
    programBuild->onMain             = true;
}

// Takes an instruction node and converts it to an instruction data struct
instruction_t convertInstructionNode(instruction_node_t* instruction)
{
    instruction_t flatInstruction;

    flatInstruction.instruction = instruction->instruction;
    flatInstruction.arg1        = instruction->arg1;
    flatInstruction.arg2        = instruction->arg2;

    return flatInstruction;
}

// takes a programBuild object and creates a complete 
//  initialized program context object ready for execution.
program_context_t returnProgram(program_build_t* programBuild)
{
    // Append the end program instruction
    appendInstruction(programBuild, i_endprog, 0, 0);

    // Initialize the program object.
    program_context_t program;
    program.functionStack.depth = 0;

    // Initialize the function array.
    program.code = calloc(programBuild->functionAmmount,
                          sizeof(instruction_t*));

    // Fill out the function array
    unsigned int functionIndex = 0;
    function_header_t* tracer  = programBuild->programTop;

    while (tracer != NULL)
    {
        // Allocate enough memory for the function to fit into an array
        program.code[functionIndex] 
            = calloc(tracer->depth, sizeof(instruction_t));

        // Beging the tracer and index marker
        instruction_node_t* instructionTracer = tracer->head;
        unsigned int instructionIndex         = 0;

        while (instructionTracer != NULL)
        {
            // Convert the instruction node and save it in the proper
            //  place int he array.
            (program.code[functionIndex])[instructionIndex]
                = convertInstructionNode(instructionTracer);

            // Deallocate the instruction node and move to next node.    
            instruction_node_t* freeInstruction = instructionTracer;
            instructionTracer                   = instructionTracer->next;
            ++instructionIndex;
            free(freeInstruction);
        }

        // Update tracer and index marker.
        ++functionIndex;
        tracer = tracer->next;
    }

    //Initialize the static data bank
    size_t bankSize                 = programBuild->constantDataList.depth;
    program.staticDataBank.size     = bankSize;
    program.staticDataBank.dataBank = calloc(bankSize, sizeof(static_data_t));

    // Fill out the static data bank, using another tracer.
    constant_data_list_node_t* staticDataTracer 
        = programBuild->constantDataList.top;

    while (staticDataTracer != NULL)
    {
        // Assigning type here may be redundant
        program.staticDataBank.dataBank[staticDataTracer->eventualIndex].type
            = staticDataTracer->type;
        program.staticDataBank.dataBank[staticDataTracer->eventualIndex].data 
            = staticDataTracer->data;
        staticDataTracer = staticDataTracer->next;
    }

    // Initialize function stack
    program.functionStack.depth = 0;
    program.functionStack.head  = NULL;

    return program;
}
