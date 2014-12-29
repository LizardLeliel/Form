#ifndef STACK_HEADER

#include "bigdecls.h"

#define byte unsigned char

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
void pushStack(dataType_t dataType, byte* data);
void popStack();

#define STACK_HEADER
#endif
