#include "instructions.h"
#include "stack.h"
#include "bigdecls.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

/* Will contain args for arguemented instructions
 *  (so we don't need to worry about passing functions,
 *  because most of the instructions don't require arguements,
 *  and we don't want to waste time passing them.
 * Instructions with args should be able to ask to have them
 *  loaded into this space
 */
 // Is this the best implementation???
void* instructionArgs;

//typedef dataType_t data_t;
//typedef data_t dataType_t;

/* A pointer of the current instruction being executed */
instruction_t* CURRENT_INSTRUCTION;



/* The instruction array (a function array).
 *  Each instruction in our program will be an enum,
 *  and will be used as an index in this array, whiich will
 *  be called and run
 */
 // This is an array of function pointers
void (*EXEC_INSTRUCTION[instruction_ammount])() =
{
    i_nop, // No operation
    i_add, // Addidtion
    i_sub,
    i_mul,
    i_divs,
    i_mod,
    i_nop,
    i_nop,
    i_nop,
    i_nop,
    i_nop,
    i_nop,
    i_lessthen
    // Fill the rest with i_nop, slowly fill with functions
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

// Intstruction - addidtion
void i_add() 
{
    // The functions popStack() checks for underflow
    //  adjust later to do types which are not numeric
    size32_t evaluation;
    data_t operandB = popData();
    data_t operandA = popData();
    dataType_t type = floatCast(&operandA, &operandB);

    if (type == f_32int)
    {
        evaluation = operandA.data + operandB.data;
        pushStack(type | f_numeric, &evaluation);
    }
    else if (type == f_32float)
    {
        //evaluation
        float value 
            = *(float*)&(operandA.data)
            + *(float*)&(operandB.data);
        evaluation = *(size32_t*)&value;
        // printf("Operands: %f %f \n", *(float*)&(operandA.data), 
        //     *(float*)&(operandB.data));
        // printf("Evaluation: %f \n", *(float*)&evaluation);
        pushStack(type | f_numeric, &evaluation);

    }
}

void i_sub()
{
    size32_t evaluation;
    data_t operandB = popData();
    data_t operandA = popData();
    dataType_t type = floatCast(&operandA, &operandB);
    if (type == f_32int)
    {
        evaluation = operandA.data - operandB.data;
        pushStack(type | f_numeric, &evaluation);
    }
    else if (type == f_32float)
    {
        float value 
            = *(float*)&(operandA.data)
            - *(float*)&(operandB.data);
        evaluation = *(size32_t*)&value;
        pushStack(type | f_numeric, &evaluation);

    }
}
void i_mul()
{
    size32_t evaluation;
    data_t operandB = popData();
    data_t operandA = popData();
    dataType_t type = floatCast(&operandA, &operandB);
    if (type == f_32int)
    {
        evaluation = operandA.data * operandB.data;
        pushStack(type | f_numeric, &evaluation);
    }
    else if (type == f_32float)
    {
        float value 
            = *(float*)&(operandA.data)
            * *(float*)&(operandB.data);
        evaluation = *(size32_t*)&value;
        pushStack(type | f_numeric, &evaluation);

    }
}
void i_divs()
{
    size32_t evaluation;
    data_t operandB = popData();
    data_t operandA = popData();
    dataType_t type = floatCast(&operandA, &operandB);
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
            = *(float*)&(operandA.data)
            / *(float*)&(operandB.data);
        evaluation = *(size32_t*)&value;
        pushStack(type | f_numeric, &evaluation);
    }
}

void i_mod()
{
    size32_t evaluation;
    data_t operandB = popData();
    data_t operandA = popData();
    dataType_t type = floatCast(&operandA, &operandB);
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
        perror("moding a float");
    }
}


void i_lessthen()
{
    size32_t evaluation;
    data_t operandB = popData();
    data_t operandA = popData();
    dataType_t type = floatCast(&operandA, &operandB);
    if (type == f_32int)
    {
        evaluation = operandA.data < operandB.data;
        pushStack(type | f_numeric, &evaluation);
    }
    else if (type == f_32float)
    {
        size32_t value 
            = *(float*)&(operandA.data)
            < *(float*)&(operandB.data);
        evaluation = *(size32_t*)&value;
        pushStack(f_numeric | f_bool, &evaluation);
    }
}

dataType_t floatCast(data_t* operandA, data_t* operandB)
{
    // All the ands!
    if ((operandA->dataType & f_32int) 
        && (operandB->dataType & f_32int))
    {
        return f_32int;
    }
    // If they're both not ints, then we want to cast any ints to floats
    else if (operandA->dataType & f_32int)
    {
        // cast it to a float, then reinterpret cast it.
        float h = (float)operandA->data;
        operandA->data = *(size32_t*)&h;
    }
    else if (operandB->dataType & f_32int)
    {
        float h = (float)operandB->data;
        operandB->data = *(size32_t*)&h;
    }
    // So if neither are ints, nothing is done.
    //printf("Floats? %f %f\n", 
    //   *(float*)&(operandA->data), 
    //    *(float*)&(operandB->data));
    return f_32float;
} 



