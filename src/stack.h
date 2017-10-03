#ifndef STACK_HEADER

#include "bigdecls.h"
#include "inttypes.h"

typedef unsigned char byte;
typedef int32_t size32_t;

/* The stack data structure that this stack-oriented program uses */
typedef struct stack 
{
    dataType_t type;
    void* data;
    struct stack* next;

} stack_t;


// Major to do: get rid of this, make
//  this into actual_data.
typedef struct data
{
    dataType_t dataType;
    size32_t data;
} data_t;

/*
typedef struct actual_data
{
	dataType_t type;
	void* data;
} actual_data_t;
*/

/* The actual stack that is used by the language */
// Or do something better. Plus, we'll need to make it
//  so individual functions may have seperate stack spaces
extern stack_t* STACK;
//extern stack_t _bottom;

/* Must be called for STACK to start pointing at _bottom */
void stackIni();
//void freeStack();

/* Raise error if at bottom of stack */
void shouldNotBeBottom();

/* Push and pop things on the stack */
void pushStack(dataType_t dataType, void* data);
void dropStack();
size32_t popStack(dataType_t* outType);
// Inline this?
data_t popData();


#define STACK_HEADER
#endif