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
void* instructionArgs;

/* A pointer of the current instruction being executed */
instruction_t* CURRENT_INSTRUCTION;

/* The instruction array (a function array).
 *  Each instruction in our program will be an enum,
 *  and will be used as an index in this array, whiich will
 *  be called and run
 */
void (*EXEC_INSTRUCTION[instruction_ammount])() =
{
    i_nop, // No operation
    i_add, // Addidtion
    // Fill the rest with i_nop, slowly fill with functions


};

#define C_OPERATE(DATA_TYPE, OPERATION)              \
    *(DATA_TYPE*)toPush =                            \
    (DATA_TYPE)topArg OPERATION (DATA_TYPE)bottomArg

void i_nop() {
    return;
}
void i_add() {

    // The functions popStack() checks for underflow
    dataType_t argTypes = 0;

    //! Todo: Fix casting issues
    size32_t topArg    = popStack(&argTypes);
    size32_t bottomArg = popStack(&argTypes);

    void* toPush = malloc(32);

    // Write stack manipulation
    switch (argTypes & (f_numeric|f_composite|f_other)) {

    //! Fix floats (related to casting issues)
      case f_numeric:
        if (argTypes & f_32float) {
            printf("Float float float!\n");
            C_OPERATE(float, +);
            printf("toPush: %f\n", *(float*)toPush);
            printf("Our way: %f\n", (float)topArg + (float)bottomArg);
            printf("Top arg? %f\n", (float)topArg);
            pushStack(f_32float | f_numeric, toPush);}
        else {
            C_OPERATE(int32_t, +);
            pushStack(f_32int | f_numeric, toPush);
        }
        break;

      case f_composite:
        // Write append code
        break;

      default:
        puts("Error: can't not perform \"+\" operator on unmatching types or \
              or types which are not numeric/composite");
        exit(1);
    }



    return;
}




