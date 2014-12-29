#include "stack.h"
#include "bigdecls.h"

#include "stdlib.h"
#include "stdio.h"

/* Stack starts initialized with a null node */
stack_t _bottom =
{
    nil,
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

void ShouldNotBeBottom() {
    if (STACK->data == nil) perror("Stack underflow");
}

/* Pushes data onto the stack */
void pushStack(dataType_t dataType, byte* data) {
    if (dataSize[dataType] == 0) {
        printf("Fix behaviour for variable-sized data pushStack()!\n");
        exit(1);
    }
    else {
        stack_t* newNode = malloc(dataSize[dataType]);
        newNode->data    = data;
        newNode->type    = dataType;
        newNode->next    = STACK;
        STACK            = newNode;
    }
}

/* Pops data and frees it */
void popStack() {
    ShouldNotBeBottom();
    stack_t* newNode = STACK;
    STACK = STACK->next;
    free(newNode);
}



