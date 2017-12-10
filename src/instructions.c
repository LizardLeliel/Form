#include "instructions.h"
#include "runtime.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

// No operation
void FORM_NOP(program_context_t* program) 
{
    return;
}

void FORM_ADD(program_context_t* program) 
{
    int64_t evaluation;
    data_t operandB = popData(&(program->dataStack));
    data_t operandA = popData(&(program->dataStack));
    data_type_t type = prepareOperands(&operandA, &operandB);

    if (type == f_32int)
    {
        evaluation = operandA.data + operandB.data;
        pushStack(&(program->dataStack), type | f_numeric, evaluation);
    }
    else if (type == f_32float)
    {
        //evaluation
        float value 
            = interpretAsFloat(operandA.data)
            + interpretAsFloat(operandB.data);
        evaluation = interpretAsInt(value);
        pushStack(&(program->dataStack), type | f_numeric, evaluation);

    }
}

void FORM_SUB(program_context_t* program)
{
    int64_t evaluation;
    data_t operandB = popData(&(program->dataStack));
    data_t operandA = popData(&(program->dataStack));
    data_type_t type = prepareOperands(&operandA, &operandB);
    if (type == f_32int)
    {
        evaluation = operandA.data - operandB.data;
        pushStack(&(program->dataStack), type | f_numeric, evaluation);
    }
    else if (type == f_32float)
    {
        float value 
            = interpretAsFloat(operandA.data)
            - interpretAsFloat(operandB.data);
        evaluation = interpretAsInt(value);
        pushStack(&(program->dataStack), type | f_numeric, evaluation);

    }
}
void FORM_MUL(program_context_t* program)
{
    int64_t evaluation;
    data_t operandB = popData(&(program->dataStack));
    data_t operandA = popData(&(program->dataStack));
    data_type_t type = prepareOperands(&operandA, &operandB);
    if (type == f_32int)
    {
        evaluation = operandA.data * operandB.data;
        pushStack(&(program->dataStack), type | f_numeric, evaluation);
    }
    else if (type == f_32float)
    {
        float value 
            = interpretAsFloat(operandA.data)
            * interpretAsFloat(operandB.data);
        evaluation = interpretAsInt(value);
        pushStack(&(program->dataStack), type | f_numeric, evaluation);

    }
}

void FORM_DIV(program_context_t* program)
{
    int64_t evaluation;
    data_t operandB = popData(&(program->dataStack));
    data_t operandA = popData(&(program->dataStack));
    data_type_t type = prepareOperands(&operandA, &operandB);
    if (type == f_32int)
    {
        if (operandB.data == 0) 
        {
            perror("Division by zero");
        }
        evaluation = operandA.data / operandB.data;
        pushStack(&(program->dataStack), type | f_numeric, evaluation);
    }
    else if (type == f_32float)
    {
        if (*(float*)&(operandB.data) == 0)
        {
            perror("Division by zero");
        }
        float value 
            = interpretAsFloat(operandA.data)
            / interpretAsFloat(operandB.data);
        evaluation = interpretAsInt(value);
        pushStack(&(program->dataStack), type | f_numeric, evaluation);
    }
}

void FORM_MOD(program_context_t* program)
{
    int64_t evaluation;
    data_t operandB = popData(&(program->dataStack));
    data_t operandA = popData(&(program->dataStack));
    data_type_t type = prepareOperands(&operandA, &operandB);
    if (type == f_32int)
    {
        if (operandB.data == 0) 
        {
            perror("Division by zero");
        }
        evaluation = operandA.data % operandB.data;
        pushStack(&(program->dataStack), type | f_numeric, evaluation);
    }
    else if (type == f_32float)
    {
        perror("Moding a float");
    }
}

// Move this to basicOPerations.c
void FORM_LESS_THEN(program_context_t* program)
{
    int64_t evaluation;
    data_t operandB = popData(&(program->dataStack));
    data_t operandA = popData(&(program->dataStack));
    data_type_t type = prepareOperands(&operandA, &operandB);
    if (type == f_32int)
    {
        evaluation = operandA.data < operandB.data;
        pushStack(&(program->dataStack), f_numeric | f_bool, evaluation);
    }
    else if (type == f_32float)
    {
        int64_t value 
            = interpretAsFloat(operandA.data)
            < interpretAsFloat(operandB.data);
        evaluation = value;
        pushStack(&(program->dataStack), f_numeric | f_bool, evaluation);
    }
}

// Move this to basicOPerations.c
void FORM_LESS_THEN_EQ(program_context_t* program)
{
    int64_t evaluation;
    data_t operandB = popData(&(program->dataStack));
    data_t operandA = popData(&(program->dataStack));
    data_type_t type = prepareOperands(&operandA, &operandB);
    if (type == f_32int)
    {
        evaluation = operandA.data <= operandB.data;
        pushStack(&(program->dataStack), f_numeric | f_bool, evaluation);
    }
    else if (type == f_32float)
    {
        int64_t value 
            = interpretAsFloat(operandA.data)
            <= interpretAsFloat(operandB.data);
        evaluation = value;
        pushStack(&(program->dataStack), f_numeric | f_bool, evaluation);
    }
}

// Move this to basicOPerations.c
void FORM_GREATER_THEN(program_context_t* program)
{
    int64_t evaluation;
    data_t operandB = popData(&(program->dataStack));
    data_t operandA = popData(&(program->dataStack));
    data_type_t type = prepareOperands(&operandA, &operandB);
    if (type == f_32int)
    {
        evaluation = operandA.data > operandB.data;
        pushStack(&(program->dataStack), f_numeric | f_bool, evaluation);
    }
    else if (type == f_32float)
    {
        int64_t value 
            = interpretAsFloat(operandA.data)
            > interpretAsFloat(operandB.data);
        evaluation = value;
        pushStack(&(program->dataStack), f_numeric | f_bool, evaluation);
    }
}

void FORM_GREATER_THEN_EQ(program_context_t* program)
{
    int64_t evaluation;
    data_t operandB = popData(&(program->dataStack));
    data_t operandA = popData(&(program->dataStack));
    data_type_t type = prepareOperands(&operandA, &operandB);
    if (type == f_32int)
    {
        evaluation = operandA.data >= operandB.data;
        pushStack(&(program->dataStack), f_numeric | f_bool, evaluation);
    }
    else if (type == f_32float)
    {
        int64_t value 
            = interpretAsFloat(operandA.data)
            >= interpretAsFloat(operandB.data);
        evaluation = value;
        pushStack(&(program->dataStack), f_numeric | f_bool, evaluation);
    }
}

void FORM_EQ(program_context_t* program)
{
    int64_t evaluation;
    data_t operandB = popData(&(program->dataStack));
    data_t operandA = popData(&(program->dataStack));
    data_type_t type = prepareOperands(&operandA, &operandB);
    if (type == f_32int)
    {
        evaluation = operandA.data > operandB.data;
        pushStack(&(program->dataStack), f_numeric | f_bool, evaluation);
    }
    else if (type == f_32float)
    {
        int64_t value 
            = interpretAsFloat(operandA.data)
            > interpretAsFloat(operandB.data);
        evaluation = value;
        pushStack(&(program->dataStack), f_numeric | f_bool, evaluation);
    }
}

void FORM_INEQ(program_context_t* program)
{
    int64_t evaluation;
    data_t operandB = popData(&(program->dataStack));
    data_t operandA = popData(&(program->dataStack));
    data_type_t type = prepareOperands(&operandA, &operandB);
    if (type == f_32int)
    {
        evaluation = operandA.data != operandB.data;
        pushStack(&(program->dataStack), f_numeric | f_bool, evaluation);
    }
    else if (type == f_32float)
    {
        int64_t value 
            = interpretAsFloat(operandA.data)
            != interpretAsFloat(operandB.data);
        evaluation = value;
        pushStack(&(program->dataStack), f_numeric | f_bool, evaluation);
    }
}

void FORM_LOG_AND(program_context_t* program)
{
    int64_t evaluation;
    data_t operandB = popData(&(program->dataStack));
    data_t operandA = popData(&(program->dataStack));
    data_type_t type = prepareOperands(&operandA, &operandB);
    if (type == f_32int)
    {
        evaluation = operandA.data && operandB.data;
        pushStack(&(program->dataStack), f_numeric | f_bool, evaluation);
    }
    else if (type == f_32float)
    {
        int64_t value 
            = interpretAsFloat(operandA.data)
            && interpretAsFloat(operandB.data);
        evaluation = value;
        pushStack(&(program->dataStack), f_numeric | f_bool, evaluation);
    }
}

void FORM_LOG_OR(program_context_t* program)
{
    int64_t evaluation;
    data_t operandB = popData(&(program->dataStack));
    data_t operandA = popData(&(program->dataStack));
    data_type_t type = prepareOperands(&operandA, &operandB);
    if (type == f_32int)
    {
        evaluation = operandA.data || operandB.data;
        pushStack(&(program->dataStack), f_numeric | f_bool, evaluation);
    }
    else if (type == f_32float)
    {
        int64_t value 
            = interpretAsFloat(operandA.data)
            || interpretAsFloat(operandB.data);
        evaluation = value;
        pushStack(&(program->dataStack), f_numeric | f_bool, evaluation);
    }
}

void FORM_LOG_NOT(program_context_t* program)
{
    int64_t evaluation;
    data_t operand   = popData(&(program->dataStack));
    data_type_t type = operand.dataType;

    if (type & f_32int || type & f_bool)
    {
        evaluation = !operand.data;
        pushStack(&(program->dataStack), f_numeric | f_bool, evaluation);
    }
    else if (type & f_32float)
    {
        evaluation = !interpretAsFloat(operand.data);
        pushStack(&(program->dataStack), f_numeric | f_bool, evaluation);
    }
}

void FORM_PUSH(program_context_t* program)
{
    data_type_t type = program->currentInstruction.arg1;
    int64_t     data = program->currentInstruction.arg2;

    pushStack(&(program->dataStack), type, data);
}

void FORM_CALL(program_context_t* program)
{
    unsigned int functionIndex 
        = program->currentInstruction.arg2;

    pushFunction(&(program->functionStack), 
                 program->currentFunctionIndex,
                 program->nextInstructionIndex);

    program->nextFunctionIndex    = functionIndex;
    program->nextInstructionIndex = 0;
}

void FORM_RETURN(program_context_t* program)
{
    returnFromFunction(program);
}

void FORM_PRINT(program_context_t* program)
{
    shouldNotBeBottom(&(program->dataStack));

    data_t value = popData(&(program->dataStack));

    // Determine how to print based on type.
    if (value.dataType & f_32float)
    {
        printf("%f\n", interpretAsFloat(value.data));
    }
    else if (value.dataType & f_32int)
    {
        printf("%ld\n", value.data);
    }
    else if (value.dataType & f_bool)
    {
        if (value.data == 0)
        {
            printf("False\n");
        }
        else
        {
            printf("True\n");
        }
    }
    else if (value.dataType & f_string)
    {
        printf("%s\n", program->staticDataBank.dataBank[value.data].data);
    }
}

// Reinterpretation casting functions.
int64_t interpretAsInt(double value)
{
    any64_t result;
    result.as_f = value;
    return result.as_i;
}
double interpretAsFloat(int64_t value)
{
    any64_t result;
    result.as_i = value;       
    return result.as_f;
}