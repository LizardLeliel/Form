#include "stack.h"
#include "bigdecls.h"

#include "stdlib.h"
#include "stdio.h"
#include "string.h"

/* Stack starts initialized with a null node */
stack_t _bottom =
{
    f_nil,
    NULL,
    NULL,
};

/* Define STACK */
stack_t* STACK;

/* Set STACK to initialy point to bottom (note; I couldn't find a
 *  way to statically set STACK to point to _bottom; & is a runtime
 *  operator, and the value of &_bottom isn't known at compile time
 */
void stackIni() {
    STACK = &_bottom;
}

void shouldNotBeBottom() {
    if (STACK->next == NULL) perror("Stack underflow");
}

/* Pushes data onto the stack */
void pushStack(dataType_t dataType, void* data) {
    if (dataSize[dataType] == 0) {
        printf("Fix behaviour for variable-sized data pushStack()!\n");
        exit(1);
    }
    else {
        stack_t* newNode = malloc(sizeof(stack_t));
        newNode->data = malloc(dataSize[dataType]);
        memcpy(newNode->data, data, dataSize[dataType]);
        // The next is kept, because you'll never know what may happen...
        //newNode->data    = data;
        newNode->type    = dataType;
        newNode->next    = STACK;
        STACK            = newNode;
    }
}

/* Pops data and frees it */
void dropStack() {
    shouldNotBeBottom();

    stack_t* newNode = STACK;
    STACK = STACK->next;
    free(newNode);
}

size32_t popStack(instructionType_t* outType) {
    shouldNotBeBottom();

    if (outType != NULL) *outType |= STACK->type;

    size32_t returnVal = *(size32_t*)STACK->data;
    stack_t* freeNode = STACK;

    STACK = STACK->next;

    free(freeNode);
    return returnVal;
}


