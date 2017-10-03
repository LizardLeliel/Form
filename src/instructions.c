#include "instructions.h"
#include "stack.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

// Arguements for instructions loaded in here
void* instructionArgs;

// A pointer of the current instruction being executed
instruction_t* CURRENT_INSTRUCTION;


// This is an array of function pointers. These functions
//  are called during runtime.
void (*EXEC_INSTRUCTION[instruction_ammount])() =
{
    i_nop, // No operation

    // Standard arthmetic opertaions
    i_add, 
    i_sub,
    i_mul,
    i_divs,
    i_mod,

    // Standard bitwise operations
    i_nop,
    i_nop,
    i_nop,
    i_nop,
    i_nop,
    i_nop,

    // Standard comparison operations
    i_lessthen,
    i_nop,
    i_nop,
    i_nop,
    i_nop,
    i_nop,

    // Boolean logic operations
    i_nop,
    i_nop,
    i_nop,

    // Increment and deincremetn operations
    i_nop,
    i_nop,

    // Standard stack-related operations
    i_push,
    i_nop,
    i_nop,
    i_nop,
    i_nop,

    // Form goto operations
    i_nop,
    i_nop,

    // Variable-related operations
    i_nop,
    i_nop,
    i_nop,

    // Misc operations 
    i_nop,
    i_nop,
    i_print,
    i_nop
};

// Macro for primitive operations
#define C_OPERATE(DATA_TYPE, OPERATION)              \
    *(DATA_TYPE*)toPush =                            \
    (DATA_TYPE)topArg OPERATION (DATA_TYPE)bottomArg

// No operation
void i_nop() 
{
    return;
}

void i_add() 
{
    size32_t evaluation;
    data_t operandB = popData();
    data_t operandA = popData();
    dataType_t type = prepareOperands(&operandA, &operandB);

    if (type == f_32int)
    {
        evaluation = operandA.data + operandB.data;
        pushStack(type | f_numeric, &evaluation);
    }
    else if (type == f_32float)
    {
        //evaluation
        float value 
            = interpretAsFloat(operandA.data)
            + interpretAsFloat(operandB.data);
        evaluation = interpretAsInt(value);
        pushStack(type | f_numeric, &evaluation);

    }
}

void i_sub()
{
    size32_t evaluation;
    data_t operandB = popData();
    data_t operandA = popData();
    dataType_t type = prepareOperands(&operandA, &operandB);
    if (type == f_32int)
    {
        evaluation = operandA.data - operandB.data;
        pushStack(type | f_numeric, &evaluation);
    }
    else if (type == f_32float)
    {
        float value 
            = interpretAsFloat(operandA.data)
            - interpretAsFloat(operandB.data);
        evaluation = interpretAsInt(value);
        pushStack(type | f_numeric, &evaluation);

    }
}
void i_mul()
{
    size32_t evaluation;
    data_t operandB = popData();
    data_t operandA = popData();
    dataType_t type = prepareOperands(&operandA, &operandB);
    if (type == f_32int)
    {
        evaluation = operandA.data * operandB.data;
        pushStack(type | f_numeric, &evaluation);
    }
    else if (type == f_32float)
    {
        float value 
            = interpretAsFloat(operandA.data)
            * interpretAsFloat(operandB.data);
        evaluation = interpretAsInt(value);
        pushStack(type | f_numeric, &evaluation);

    }
}

void i_divs()
{
    size32_t evaluation;
    data_t operandB = popData();
    data_t operandA = popData();
    dataType_t type = prepareOperands(&operandA, &operandB);
    if (type == f_32int)
    {
        if (operandB.data == 0) 
        {
            perror("Division by zero");
        }
        evaluation = operandA.data / operandB.data;
        pushStack(type | f_numeric, &evaluation);
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
        pushStack(type | f_numeric, &evaluation);
    }
}

void i_mod()
{
    size32_t evaluation;
    data_t operandB = popData();
    data_t operandA = popData();
    dataType_t type = prepareOperands(&operandA, &operandB);
    if (type == f_32int)
    {
        if (operandB.data == 0) 
        {
            perror("Division by zero");
        }
        evaluation = operandA.data % operandB.data;
        pushStack(type | f_numeric, &evaluation);
    }
    else if (type == f_32float)
    {
        perror("Moding a float");
    }
}

void i_lessthen()
{
    size32_t evaluation;
    data_t operandB = popData();
    data_t operandA = popData();
    dataType_t type = prepareOperands(&operandA, &operandB);
    if (type == f_32int)
    {
        evaluation = operandA.data < operandB.data;
        pushStack(type | f_numeric, &evaluation);
    }
    else if (type == f_32float)
    {
        size32_t value 
            = interpretAsFloat(operandA.data)
            < interpretAsFloat(operandB.data);
        evaluation = value;
        pushStack(f_numeric | f_bool, &evaluation);
    }
}

void i_push()
{
    dataType_t type = ((dataType_t*)CURRENT_INSTRUCTION->args)[0];

    pushStack(type, CURRENT_INSTRUCTION->args + sizeof(dataType_t));

    // Arg 1: type
    // Arg 2: data
}

void i_print()
{
    data_t value = popData();

    if (value.dataType & f_32float)
    {
        printf("%f\n", interpretAsFloat(value.data));
    }
    else if (value.dataType & f_32int)
    {
        printf("%d\n", value.data);
    }
}

size32_t interpretAsInt(float value)
{
    any32_t result;
    result.as_f = value;
    return result.as_i;
}

float interpretAsFloat(size32_t operandValue)
{
    any32_t value;
    value.as_i = operandValue;       
    return value.as_f;
}

dataType_t prepareOperands(data_t* operandA, data_t* operandB)
{
    if ((operandA->dataType & f_32int) 
        && (operandB->dataType & f_32int))
    {
        return f_32int;
    }
    // If they're both not ints, then we want to cast any ints to floats
    else if (operandA->dataType & f_32int)
    {
        // cast it to a float, then reinterpret cast it.
        any32_t data;
        data.as_f = (float)operandA->data;
        operandA->data = data.as_i;
    }
    else if (operandB->dataType & f_32int)
    {
        any32_t data;
        data.as_f = (float)operandB->data;
        operandB->data = data.as_i;
    }

    return f_32float;
} 


void execute(instruction_t** program)
{
    CURRENT_INSTRUCTION = *program;
    while (CURRENT_INSTRUCTION != NULL)
    {
        EXEC_INSTRUCTION[CURRENT_INSTRUCTION->instruction]();
        CURRENT_INSTRUCTION = CURRENT_INSTRUCTION->next;
    }
}
