#ifndef BASIC_OPERATIONS_HEADER
#define BASIC_OPERATIONS_HEADER

#include "runtime.h"

void i_nop(program_context_t*);
void i_add(program_context_t*);
void i_sub(program_context_t*);
void i_mul(program_context_t*);
void i_divs(program_context_t*);
void i_mod(program_context_t*);
void i_lessthen(program_context_t*);
void i_greaterthen(program_context_t*);
void i_eq(program_context_t*);

// BASIC_OPERATIONS_HEADER
#endif