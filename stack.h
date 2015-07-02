#ifndef STACK_HEADER

#include "bigdecls.h"
#include "inttypes.h"

typedef unsigned char byte;
typedef int32_t size32_t;

/* The stack data stryctyre that this stack-oriented program uses */
typedef struct stack_node 
{
    dataType_t type;
    void* data;
    struct stack_node* next;

} stack_t;

/* A union suitable for storing 32 bits of storage. Used to avoid
 * casting hassle */
// This may be removed soon, with generic arthmetic casting
typedef union floatint_union 
{
    int32_t as_i;
    float   as_f;
} any32_t;

typedef struct data
{
    dataType_t dataType;
    size32_t data;
} data_t;

/* The actual stack that is used by the language */
// Or do something better. Plus, we'll need to make it
//  so individual functions may have seperate stack spaces
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
// Inline this?
data_t popData();



#define STACK_HEADER
#endif
