#ifndef STACK_HEADER
#define STACK_HEADER

#include "bigdecls.h"
#include "inttypes.h"

typedef unsigned char byte;
typedef int32_t size32_t;

/* The stack data stryctyre that this stack-oriented program uses */
typedef struct data_node 
{
    dataType_t type;
    void* data;
    struct data_node* next;

} dataNode_t;

// This is for scoping! Each called function will get
//  its own stack for users to push and pop from!
typedef struct scoping_node
{
    dataNode_t* dataTop;
    struct scoping_node* nextScope;
} scopingNode_t;

// Scoping stack of data stacks wrapper.
typedef struct program_stack
{
    scopingNode_t* currentScope;
    int scopeDepth; // Neccesary?
} programStack_t;

/* A union suitable for storing 32 bits of storage. Used to avoid
 * casting hassle */
// This may be removed soon, with generic arthmetic casting
typedef union floatint_union 
{
    int32_t as_i;
    float   as_f;
} intfloat_t;

typedef struct data
{
    dataType_t type;
    void* data;
} data_t;

/* The actual stack that is used by the language */
// Or do something better. Plus, we'll need to make it
//  so individual functions may have seperate stack spaces
//extern stack_t* STACK;
//extern stack_t _bottom;

/* Must be called for STACK to start pointing at _bottom */
programStack_t newProgramStack();
void deleteProgramStack(programStack_t* programStack);

void pushScoping(programStack_t* programStack);
void popScoping(programStack_t* programStack);

void pushData
(
    programStack_t* programStack, 
    dataType_t      dataType,
    void*           data
);

data_t popData(programStack_t* programStack);


// /* Push and pop things on the stack */
// void pushStack(dataType_t dataType, void* data);
// void dropStack();
// size32_t popStack(dataType_t* outType);
// // Inline this?
// data_t popData();

/* Raise error if at bottom of stack */
void shouldNotBeBottom(programStack_t* programStack);


// STACK_HEADER
#endif
