#include "build.h"
#include "instructions.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/* Four important pointers for the building bytecode out of
 *  scanned input. The three pointers are the top of the function linked
 *  list, the end of the function linked list, the end of main's
 *  instruction list, and the instruction list currently appending too.
 *  They are all initalized to NULL
 */
struct build_pointer_struct buildPointers = {NULL, NULL, NULL, NULL};

/* A number for keeping track of how many functions are declared */
unsigned int functionNumber = 1;

/* A boolean to determine whether the current function build sdequence
 *  is the main function or not
 */
bool onMain = true;

/* Initialized buildPointers appriopriatly */
void instructionIni() 
{
    buildPointers.programTop       = malloc(sizeof(functionHead_t));
    buildPointers.currentInstruct  = malloc(sizeof(instruction_t));

    buildPointers.programTop->head = buildPointers.currentInstruct;
    buildPointers.programTop->next = NULL;

    buildPointers.lastFunction     = buildPointers.programTop;
    buildPointers.mainLast         = buildPointers.currentInstruct;

    buildPointers.currentInstruct->instruction = nop;
}

void freeInstructions()
{
    // outer
    functionHead_t* tracer = buildPointers.programTop;
    functionHead_t* toFree;
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

/* Adds a new instruction to end of the instruction sequence.
 *  Note: I may change instruction_t such that argsize is not a member,
 *  and dynamicly-sized variables will have their size as the first byte (or
 *  two, or three). Most instructions do not require arguements, the few that
 *  do usually have fixed argSize; only for push values like strings and arrays
 */
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

/* Returns a dynamically allocated dummy instruction */
instruction_t* dummyInstruction() 
{
    /* The function is calloc because every value in dummy is 0 -
     *  instructionType (nop, which is 0), argSize (0), args (NULL),
     *  and next (also NULL)
     */
    return calloc(1, sizeof(instruction_t));
}

/* The function has no parameters since function names are dealt with
 *  in hash.c It also sets buildPointers.currenInstruct/lastFunction,
 *  and onMain
 */
void makeNewFunction() 
{
    /* Allocates and initialize memory */
    functionHead_t* newFunction = malloc(sizeof(functionHead_t));
    newFunction->next = NULL;
    newFunction->head = dummyInstruction();

    /* Assigns buildPointerFunctions */
    buildPointers.currentInstruct    = newFunction->head;
    buildPointers.lastFunction->next = newFunction;
    buildPointers.lastFunction       = newFunction;

    onMain = false;
}

/* Places appriopriate instruction on end of the instruction queue,
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

instruction_t** returnProgram()
{
    return &(buildPointers.programTop->head);
}


