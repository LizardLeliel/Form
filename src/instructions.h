#ifndef INSTRUCTIONS_HEADER
#define INSTRUCTIONS_HEADER

#include "runtime.h"

// These functions are given special style
//  since they're meant to be access from an array.
void FORM_NOP(program_context_t*);
void FORM_ADD(program_context_t*);
void FORM_SUB(program_context_t*);
void FORM_MUL(program_context_t*);
void FORM_DIV(program_context_t*);
void FORM_MOD(program_context_t*);

void FORM_LESS_THEN(program_context_t*);
void FORM_LESS_THEN_EQ(program_context_t*);
void FORM_GREATER_THEN(program_context_t*);
void FORM_GREATER_THEN_EQ(program_context_t*);
void FORM_EQ(program_context_t*);
void FORM_INEQ(program_context_t*);

void FORM_LOG_AND(program_context_t*);
void FORM_LOG_OR(program_context_t*);
void FORM_LOG_NOT(program_context_t*);

void FORM_PUSH(program_context_t*);
void FORM_CALL(program_context_t*);
void FORM_RETURN(program_context_t*);
void FORM_PRINT(program_context_t*);

// Reinterpration functions for arthmetic instructions.
int64_t interpretAsInt(double value);
double interpretAsFloat(int64_t value);

// INSTRUCTIONS_HEADER
#endif