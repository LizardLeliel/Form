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
        // reintrepret_cast<> would be kind of nice for this, even
        //  it may be just as wordy.

        //evaluation
        float value 
            = *(float*)&(operandA.data)
            + *(float*)&(operandB.data);
        // Why is zero being pushed???
        evaluation = *(size32_t*)&value;
        
        printf("Operands: %f %f \n", *(float*)&(operandA.data), 
            *(float*)&(operandB.data));
        printf("Evaluation: %f \n", *(float*)&evaluation);
        pushStack(type | f_numeric, &evaluation);

    }


/*
    dataType_t topType, bottomType, bothTypes;
    any32_t    topArg, bottomArg;

    topArg.as_i    = popStack(&topType);
    bottomArg.as_i = popStack(&bottomType);
    bothTypes      = topType | bottomType;

    void* toPush = malloc(32); //Ack, the memcpy in pushStack will make
                               // this a leak! D:

    // Write stack manipulation
    switch (bothTypes & (f_numeric|f_composite|f_other)) 
    {

    // Numerically add
      case f_numeric:
        if (!(bothTypes & f_32float)) 
        {
            *(int32_t*)toPush = topArg.as_i + bottomArg.as_i;
            pushStack(f_numeric | f_32int, toPush);
            break;
        }

        // The next two should convert integers to floats implicitly
        if (!(topType & f_32float)) 
        {
            topArg.as_f = topArg.as_i; 
        }
        else if (!(bottomType & f_32float)) 
        {
            bottomArg.as_f = bottomArg.as_i;
        }

        *(float*)toPush = topArg.as_f + bottomArg.as_f;
        pushStack(f_32float | f_numeric, toPush);
        break;

      case f_composite:
        // Write append code
        break;

      default:
        puts("Error: can't not perform \"+\" operator on unmatching types or \
              on types which are not numeric/composite");
        exit(1);
    }

    return;
*/
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
    printf("Floats? %f %f\n", 
        *(float*)&(operandA->data), 
        *(float*)&(operandB->data));
    return f_32float;
} 




