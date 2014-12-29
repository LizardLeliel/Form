#include "build.h"
#include "instructions.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/* Three important pointers for the building bytecode out of
 *  scanned input. The three pointers are the top of the function linked
 *  list, the end of the function linked list, the end of main's
 *  instruction list, and the instruction list currently appending too.
 *  They are all initalized to NULL
 */
struct build_pointer_struct buildPointers = {NULL, NULL, NULL, NULL};

/* A number for keeping track of how many functions are declared */
unsigned int functionNumber = 1;
bool onMain = true;

void instructionIni() {
    buildPointers.programTop       = malloc(sizeof(functionHead_t));
    buildPointers.currentInstruct  = malloc(sizeof(instruction_t));
    buildPointers.programTop->head = buildPointers.currentInstruct;
    buildPointers.programTop->next = NULL;
    buildPointers.lastFunction     = buildPointers.programTop;
    buildPointers.mainLast         = buildPointers.currentInstruct;

    buildPointers.currentInstruct->instruction = nop;
}

/* This function MAY be changed later on, if I change instruction_t */
void appendInstruction(instructionType_t newInstruct, size_t argSize, void* args) {

    instruction_t* newInstructNode = malloc(sizeof(instruction_t));
    newInstructNode->instruction   = newInstruct;
    newInstructNode->argSize       = argSize; // In bytes
    newInstructNode->next          = NULL;

    if (argSize == 0) {
        newInstructNode->args = NULL; // Copying memory wouldn't be nessecary
    }
    else {
        newInstructNode->args = malloc(argSize);
        memcpy(newInstructNode->args, args, argSize);
    }

    buildPointers.currentInstruct->next = newInstructNode;
    buildPointers.currentInstruct       = newInstructNode;


}






