#include "stack.h"
#include "bigdecls.h"

#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "stdint.h"

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
void stackIni() 
{
    STACK = &_bottom;
}

void shouldNotBeBottom() 
{
    // Try changing later. Make better warning error for user
    if (STACK->next == NULL) perror("Stack underflow");
}

/* Pushes data onto the stack */
// Can we change it so all data isn't 32-bit?
void pushStack(dataType_t dataType, void* data) 
{
    stack_t* newNode = malloc(sizeof(stack_t));
    newNode->data    = malloc(32);
    memcpy(newNode->data, data, 32);

    newNode->type    = dataType;
    newNode->next    = STACK;
    STACK            = newNode;

}

/* Pops data and frees it */
void dropStack() {
    shouldNotBeBottom();

    stack_t* oldNode = STACK;
    STACK            = STACK->next;
    free(oldNode);
}

//! Todo: Fix popping behaviour
// Pops data from the stack, frees dynamic memory associated
//  with it, then returns that value.
size32_t popStack(dataType_t* outType) 
{
    shouldNotBeBottom();

    if (outType != NULL) *outType = STACK->type;

    size32_t returnVal = *(size32_t*)STACK->data;
    stack_t* freeNode  = STACK;

    STACK = STACK->next;

    free(freeNode);
    return returnVal;
}


data_t popData()
{
    shouldNotBeBottom();
    dataType_t outType;
    size32_t data = popStack(&outType);

    data_t returnStruct 
        = {.dataType = outType, .data = data};
    return returnStruct;
}

