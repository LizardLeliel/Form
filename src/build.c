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
    programBuild.currentInstruction = dummyInstruction();//malloc(sizeof(instruction_node_t));

    programBuild.programTop->head   = programBuild.currentInstruction;
    programBuild.programTop->next   = NULL;
    programBuild.programTop->depth  = 1;

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
    tracer = tokenHash->cleanupList.top;
  
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
void pushToList(token_hash_t*  tokenHash,
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
                        data_type_t            type,
                        void*                  data)
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


unsigned int nextIndex(constant_data_list_t* constantDataList)
{
    return constantDataList->depth + 1;
}

// Returns a dummy instruction
instruction_node_t* dummyInstruction() 
{
    instruction_node_t* dummy = malloc(sizeof(instruction_node_t));
    dummy->instruction   = nop;
    dummy->arg1          = 0;
    dummy->arg2          = 0;
    dummy->next          = NULL;
    return dummy;
}

// Adds a new instruction to end of current instruction sequence.
void appendInstruction(program_build_t*   programBuild,
                       instruction_type_t newInstruct,
                       int32_t            arg1,
                       int64_t            arg2)
{

    instruction_node_t* newInstructNode = malloc(sizeof(instruction_node_t));
    newInstructNode->instruction        = newInstruct;
    newInstructNode->next               = NULL;
    newInstructNode->arg1               = arg1;
    newInstructNode->arg2               = arg2;

    if (programBuild->onMain == true)
    {
        programBuild->programTop->depth += 1;
    }
    else
    {
        programBuild->lastFunction->depth += 1;
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

    newFunction->next  = NULL;
    newFunction->head  = dummyInstruction();
    newFunction->depth = 1;

    // Assigns build pointer functions 
    programBuild->currentInstruction = newFunction->head;
    programBuild->lastFunction->next = newFunction;
    programBuild->lastFunction       = newFunction;
    programBuild->functionAmmount   += 1;

    programBuild->onMain = false;
}

// Places return instruction on end of the instruction queue,
//  and configures things back to main
void endFunction(program_build_t* programBuild) 
{
    //instruction_node_t* endInstruct = calloc(1, sizeof(instruction_node_t));
    //endInstruct->instruction   = returns;
    //programBuild->currentInstruction->next = endInstruct;
    appendInstruction(programBuild, returns, 0, 0);

    programBuild->currentInstruction->next = NULL;
    programBuild->currentInstruction       = programBuild->mainLast;
    programBuild->onMain                   = true;
}

instruction_t convertInstructionNode(instruction_node_t* instruction)
{
    instruction_t flatInstruction;

    flatInstruction.instruction = instruction->instruction;
    flatInstruction.arg1        = instruction->arg1;
    flatInstruction.arg2        = instruction->arg2;

    return flatInstruction;
}

program_context_t returnProgram(program_build_t* programBuild)
{
    // Move into formLang.flex somehow later - appending the end program 
    //  instruction to end of program
    if (programBuild->onMain == false)
    {
        puts("Error: reached EOF while parsing function");
        exit(0);
    }

    appendInstruction(programBuild, endProg, 0, 0);

    // Build program.
    program_context_t program;
    program.functionStack.depth = 0;

    // Initialize the function array
    //printf("function functionAmmount: %d\n", programBuild->functionAmmount);

    program.code = calloc(programBuild->functionAmmount,
                          sizeof(instruction_t*));


    //printf("program.code: %p\n", program.code);
    // array[2] = *(array + 2);
    // array[2][3] = *(*(array + 2) + 2)
    // array[2][3] = *(pointer-in-array + 2)

    // Fill out the function array
    unsigned int functionIndex = 0;
    function_header_t* tracer  = programBuild->programTop;

    while (tracer != NULL)
    {

        //printf("depth size: %zu\n", tracer->depth);


        program.code[functionIndex] = calloc(tracer->depth, sizeof(instruction_t));


        //printf("Function index, pointer: %zu %p\n", tracer->depth, program.code);

        instruction_node_t* instructionTracer = tracer->head;
        unsigned int instructionIndex         = 0;

        while (instructionTracer != NULL)
        {
            (program.code[functionIndex])[instructionIndex]
                = convertInstructionNode(instructionTracer);

            instruction_node_t* freeInstruction = instructionTracer;
            instructionTracer                   = instructionTracer->next;
            ++instructionIndex;
            free(freeInstruction);
        }

        ++functionIndex;
        tracer = tracer->next;
    }

    //Initialize the static data bank
    size_t bankSize                 = programBuild->constantDataList.depth;
    program.staticDataBank.size     = bankSize;
    program.staticDataBank.dataBank = calloc(bankSize, sizeof (static_data_t));

    // Fill out the static data bank
    constant_data_list_node_t* staticDataTracer 
        = programBuild->constantDataList.top;

    while (staticDataTracer != NULL)
    {
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
