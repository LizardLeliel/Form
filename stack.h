#ifndef STACK_HEADER

#include "bigdecls.h"
#include "inttypes.h"

typedef unsigned char byte;
typedef int32_t size32_t;

/* The stack data stryctyre that this stack-oriented program uses */
typedef struct stack_node {
    dataType_t type;
    void* data;
    struct stack_node* next;

} stack_t;

/* The actual stack that is used by the language */
extern stack_t* STACK;
//extern stack_t _bottom;

/* Must be called for STACK to start pointing at _bottom */
void stackIni();

/* Raise error if at bottom of stack */
void shouldNotBeBottom();

/* Push and pop things on the stack */
void pushStack(dataType_t dataType, void* data);
void dropStack();
size32_t popStack(dataType_t* outType);




#define STACK_HEADER
#endif
