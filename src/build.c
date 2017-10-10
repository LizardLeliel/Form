#include "build.h"
#include "instructions.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

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


