#include "build.h"
#include "instructions.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <stdio.h>

/* Four important pointers for the building bytecode out of
 *  scanned input. The four pointers are the top of the function linked
 *  list, the end of the function linked list, the end of main's
 *  instruction list, and the instruction list currently appending to.
 */
struct build_pointer_struct buildPointers = {NULL, NULL, NULL, NULL};

// A number for keeping track of how many functions are declared
unsigned int functionNumber = 1;

/* A boolean to determine whether the current function build sdequence
 *  is the main function or not
 */
bool onMain = true;

// Initialized buildPointers appriopriatly
void instructionIni() 
{
    buildPointers.programTop       = malloc(sizeof(function_header_t));
    buildPointers.currentInstruct  = malloc(sizeof(instruction_t));

    buildPointers.programTop->head = buildPointers.currentInstruct;
    buildPointers.programTop->next = NULL;

    buildPointers.lastFunction     = buildPointers.programTop;
    buildPointers.mainLast         = buildPointers.currentInstruct;

    buildPointers.currentInstruct->instruction = nop;
}

void freeInstructions()
{
    function_header_t* tracer = buildPointers.programTop;
    function_header_t* toFree;
    while (tracer != NULL)
    {
        instruction_t* instructionTracer = tracer->head;
        instruction_t* instructionToFree;

        while (instructionTracer != NULL)
        {
            free(instructionTracer->args);
            instructionToFree = instructionTracer;
            instructionTracer = instructionTracer->next;
            free(instructionToFree);
        }
        free(tracer->head);
        toFree = tracer;
        tracer = tracer->next;
        free(toFree);
    }
}

// Adds a new instruction to end of current instruction sequence.
void appendInstruction(instructionType_t newInstruct,
                       size_t argSize, void* args) 
{
    instruction_t* newInstructNode = malloc(sizeof(instruction_t));
    newInstructNode->instruction   = newInstruct;
    newInstructNode->argSize       = argSize; // In bytes
    newInstructNode->next          = NULL;


    if (argSize == 0) 
    {
        newInstructNode->args = NULL; // Copying memory wouldn't be nessecary
    }
    else 
    {
        newInstructNode->args = malloc(argSize);
        memcpy(newInstructNode->args, args, argSize);
    }

    buildPointers.currentInstruct->next = newInstructNode;
    buildPointers.currentInstruct       = newInstructNode;

    if (onMain == true) 
    {
        buildPointers.mainLast = newInstructNode;
    }
}

// Returns a dynamically allocated dummy instruction
instruction_t* dummyInstruction() 
{
    // Calloc is used since it returns zero-filled memory (NULL)
    instruction_t* dummy = malloc(sizeof(instruction_t));
    dummy->instruction = nop;
    dummy->argSize     = 0;
    dummy->args        = NULL;
    dummy->next        = NULL;
    return dummy;
}

void makeNewFunction() 
{
    // Allocates and initialize memory
    function_header_t* newFunction = malloc(sizeof(function_header_t));
    newFunction->next = NULL;
    newFunction->head = dummyInstruction();

    // Assigns buildPointerFunctions 
    buildPointers.currentInstruct    = newFunction->head;
    buildPointers.lastFunction->next = newFunction;
    buildPointers.lastFunction       = newFunction;

    onMain = false;
}

/* Places return instruction on end of the instruction queue,
 *  and configures things back to main
 */
void endFunction() 
{
    instruction_t* endInstruct = calloc(1, sizeof(instruction_t));
    endInstruct->instruction = returns;

    buildPointers.currentInstruct->next = endInstruct;
    buildPointers.currentInstruct       = buildPointers.mainLast;

    onMain = true;
}

program_context_t returnProgram()
{
    // Build program.
    program_context_t program;
    program.functionStack.depth = 0;

    program.functions = malloc(sizeof(instruction_t*) * functionNumber);

    int i = 0;
    function_header_t* tracer = buildPointers.programTop;

    while (tracer != NULL)
    {
        program.functions[i] = tracer->head;
        ++i;
        tracer = tracer->next;
    }

    return program;
    //return &(buildPointers.programTop->head);
}


#include "hash.h"

#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <stdio.h>



runtime_hash_t RUNTIME_HASH;

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
    unsigned long hashed_value = 0;
    int c = 0;

    do {
        hashed_value = symbol[c] + (hashed_value << 6) + (hashed_value << 16) - hashed_value;
    } while (++c < wordLength);

    return hashed_value;
}


void hashIni() {
    static int tries = 3;
    //HASH = calloc(maxArrayVal+1, sizeof(hash_t));
    RUNTIME_HASH.hash = calloc(maxArrayVal + 1, sizeof(hash_bucket_t*));
    // RUNTIME_HASH.typeCount = {0, 0, 0, 0, 0};
    memset(RUNTIME_HASH.typeCount, 0, sizeof RUNTIME_HASH.typeCount);

    /* if malloc/calloc fails, try again with smaller ammounts */
    while (RUNTIME_HASH.hash == NULL) 
    {
        maxArrayVal >>= 1;
        RUNTIME_HASH.hash = calloc(maxArrayVal + 1, sizeof(hash_bucket_t*));

        if (--tries == 0) 
        {
            perror
            ("Unable to allocate memory for hash table while scanning\n");
        }
    } 
}

unsigned int getHashID(hashType_t toHashType, size_t symbolSize,
                       const char* symbolName) {

    unsigned long index = maxArrayVal & hashFunction(symbolSize, symbolName);

    // If hash entry is empty, set a new one
    //! To do: make a function for creating hash entries
    if (RUNTIME_HASH.hash[index] == NULL) {
        (RUNTIME_HASH.hash[index]) = malloc(sizeof(hash_bucket_t));

        // Set values
        (RUNTIME_HASH.hash[index])->hashedType  = toHashType;
        (RUNTIME_HASH.hash[index])->contents.ID = ++(RUNTIME_HASH.typeCount[toHashType]);
        (RUNTIME_HASH.hash[index])->next        = NULL;

        (RUNTIME_HASH.hash[index])->symbol
            = memcpy(malloc(symbolSize), symbolName, symbolSize);

        pushToList(RUNTIME_HASH.hash[index]);

        return RUNTIME_HASH.typeCount[toHashType];
    }

    // Else, check each node in list to see if symbol already exists. Start
    //  with a dummy list node (Is there a simplier way to do this?)
    hash_bucket_t* tracer = malloc(sizeof(hash_bucket_t));
    hash_bucket_t* freeThisDummy = tracer;
    tracer->next = RUNTIME_HASH.hash[index];

    do {
        tracer = tracer->next;
        // If the words are the same length and are the same
        if (tracer->symbolLength != symbolSize) {
            if (memcmp(tracer->symbol, symbolName, symbolSize) == 0) {
                free(freeThisDummy);
                return tracer->contents.ID;
            }
        }

    } while (tracer->next != NULL);

    free(freeThisDummy);

    // If trace->next == null, then we need to make a new hash entry
    //  (because one does not exist)
    tracer = tracer->next = malloc(sizeof(hash_bucket_t));
    tracer->hashedType    = toHashType;
    tracer->symbolLength  = symbolSize;
    tracer->contents.ID   = ++RUNTIME_HASH.typeCount[toHashType];

    tracer->symbol = memcpy(malloc(symbolSize), symbolName, symbolSize);

    return ++RUNTIME_HASH.typeCount[toHashType];
}

// Adds to list of hash buckets (for freeing later)
void pushToList(hash_bucket_t* slot)
{
    if (RUNTIME_HASH.cleanupList.top == NULL)
    {
        RUNTIME_HASH.cleanupList.top = malloc(sizeof(hash_bucket_list_node_t));

        RUNTIME_HASH.cleanupList.top->entry = slot;
        RUNTIME_HASH.cleanupList.top->next  = NULL;
    }
    else
    {
        hash_bucket_list_node_t* newNode 
            = malloc(sizeof(hash_bucket_list_node_t));
        newNode->next = RUNTIME_HASH.cleanupList.top;
        newNode->entry = slot;
        RUNTIME_HASH.cleanupList.top = newNode;
    }
}

void freeHash()
{
    hash_bucket_list_node_t* tracer;
    hash_bucket_list_node_t* toFree;
    tracer = RUNTIME_HASH.cleanupList.top;
    while (tracer != NULL)
    {
        free(tracer->entry->symbol);
        free(tracer->entry);
        toFree = tracer;
        tracer = tracer->next;
        free(toFree);
    }

    free(RUNTIME_HASH.hash);
}

bool isWhiteSpace(char c)
{
    return (c == ' ' || c == '\n' || c == '\t' || c == 26);
}

char* trim(const char* string)
{
    char* duplicate = malloc(strlen(string));
    unsigned int index = 0;
    char* reader = string;
    while (isWhiteSpace(*reader))
    {
        ++reader;
    }

    while (!isWhiteSpace(*reader))
    {
        duplicate[index] = *reader;
        ++index; ++reader;
    }
    duplicate[index] = '\0';
    return duplicate;
}