#include "instructions.h"
#include "runtime.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

// No operation
void i_nop(program_context_t* program) 
{
    return;
}

void i_add(program_context_t* program) 
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

void i_sub(program_context_t* program)
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
void i_mul(program_context_t* program)
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

void i_divs(program_context_t* program)
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

void i_mod(program_context_t* program)
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
void i_lessthen(program_context_t* program)
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
void i_lesstheneq(program_context_t* program)
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
void i_greaterthen(program_context_t* program)
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

void i_greatertheneq(program_context_t* program)
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

void i_eq(program_context_t* program)
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

void i_ineq(program_context_t* program)
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

void i_logand(program_context_t* program)
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

void i_logor(program_context_t* program)
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

void i_lognot(program_context_t* program)
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

void i_push(program_context_t* program)
{
    data_type_t type = program->currentInstruction.arg1;
    int64_t     data = program->currentInstruction.arg2;

    pushStack(&(program->dataStack), type, data);
}

void i_call(program_context_t* program)
{
    unsigned int functionIndex 
        = program->currentInstruction.arg2;

    pushFunction(&(program->functionStack), 
                 program->currentFunctionIndex,
                 program->nextInstructionIndex);

    program->nextFunctionIndex    = functionIndex;
    program->nextInstructionIndex = 0;
}

void i_returns(program_context_t* program)
{
    returnFromFunction(program);
}

void i_print(program_context_t* program)
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