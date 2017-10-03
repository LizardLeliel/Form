#ifndef STACK_HEADER

#include "inttypes.h"


typedef unsigned char byte;
typedef int32_t size32_t;

/* An enumeration of run-time data types. All data types in Form should
 *  be 32 bits (even char; although strings will be treated like a special
 *  array type*/
typedef enum data_type {
    f_nil       = 0x0000,

    f_numeric   = 0x0001,
    f_composite = 0x0002,
    f_other     = 0x0004,

    f_32int     = 0x0008,
    f_32float   = 0x0010,
    f_bool      = 0x0020,
    f_char      = 0x0040,

    f_table     = 0x0080,
    f_array     = 0x0100,
    f_string    = 0x0200,

    f_ref       = 0x0400,
    f_var       = 0x0800,

} dataType_t;


// The stack data structure that this stack-oriented program uses
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

// The actual stack that is used by the language
extern stack_t* STACK;
//extern stack_t _bottom;

// Must be called for STACK to start pointing at _bottom
void stackIni();
//void freeStack();

// Raise error if at bottom of stack
void shouldNotBeBottom();

// Push and pop things on the stack 
void pushStack(dataType_t dataType, void* data);
void dropStack();
size32_t popStack(dataType_t* outType);

// Inline this?
data_t popData();


#define STACK_HEADER
#endif