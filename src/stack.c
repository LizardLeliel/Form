#include "stack.h"
#include "bigdecls.h"

#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "stdint.h"

/* Stack starts initialized with a null node */
/*stack_t _bottom =
{
    f_nil,
    NULL,
    NULL,
};*/

/* Define STACK */
//stack_t* STACK;

/* Set STACK to initialy point to bottom (note; I couldn't find a
 *  way to statically set STACK to point to _bottom; & is a runtime
 *  operator, and the value of &_bottom isn't known at compile time
 */
// void stackIni() 
// {
//     STACK = &_bottom;
// }


programStack_t newProgramStack()
{
    programStack_t programStack 
        = {NULL, 0};
    // The first scope would be the main/global scope
    pushScoping(&programStack);
}

void deleteProgramStack(programStack_t* programStack)
{
    while (programStack->currentScope != NULL)
    {
        popScoping(programStack);
    }
}

void pushScoping(programStack_t* programStack)
{
    ++(programStack->scopeDepth);
    scopingNode_t* newScope = malloc(sizeof(scopingNode_t));
    newScope->nextScope     = programStack->currentScope;
    newScope->dataTop       = malloc(sizeof(dataNode_t));
    newScope->dataTop->next = NULL;
    newScope->dataTop->data = NULL;
    newScope->dataTop->type = f_nil;
}

void popScoping(programStack_t* programStack)
{
    --(programStack->scopeDepth);
    // Freeing all the data in this scope
    dataNode_t* tracer           = programStack->currentScope->dataTop;
    dataNode_t* freeThisDataNode = tracer;

    while (tracer != NULL)
    {
        free(tracer->data);
        freeThisDataNode = tracer;
        tracer = tracer->next;
        free(freeThisDataNode);
    }

    scopingNode_t* freeThisScopeNode = programStack->currentScope;
    programStack->currentScope       = programStack->currentScope->nextScope;
    free(freeThisScopeNode);
}

data_t popData(programStack_t* programStack)
{
    // Hmm 
    shouldNotBeBottom(programStack);
    dataNode_t* dataNode = programStack->currentScope->dataTop;
    programStack->currentScope->dataTop 
        = programStack->currentScope->dataTop->next;

    data_t returnData = {dataNode->type, dataNode->data};
    free(dataNode);
    return returnData;
}

void shouldNotBeBottom(programStack_t* programStack) 
{
    // Try changing later. Make better warning error for user
    if (programStack->currentScope->dataTop->next == NULL) 
    {
        perror("Stack underflow");
    }
}

