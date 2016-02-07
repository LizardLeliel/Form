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


    dataType_t topType, bottomType, bothTypes;
    any32_t    topArg, bottomArg;

    topArg.as_i    = popStack(&topType);
    bottomArg.as_i = popStack(&bottomType);
    bothTypes      = topType | bottomType;

    void* toPush = malloc(32);

    // Write stack manipulation
    switch (bothTypes & (f_numeric|f_composite|f_other)) {

    // Numerically add
      case f_numeric:
        if (!(bothTypes & f_32float)) {
            *(int32_t*)toPush = topArg.as_i + bottomArg.as_i;
            pushStack(f_numeric | f_32int, toPush);
            break;
        }

        // The next two should convert integers to floats implicitly
        if (!(topType & f_32float)) {
            topArg.as_f = topArg.as_i; }
        else if (!(bottomType & f_32float)) {
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
}
