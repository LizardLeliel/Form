#include "instructions.h"
#include "stack.h"
#include "bigdecls.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>

// Note: rename to "vm" possible, or something else?

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

#define C_OPERATE(DATA_TYPE, OPERATION)             \
    *(DATA_TYPE*)toPush =                            \
    (DATA_TYPE)topArg OPERATION (DATA_TYPE)bottomArg; \

void i_nop() {
    return;
}
void i_add() {

    // The functions popStack() checks for underflow
    instructionType_t argTypes = 0;

    size32_t topArg    = popStack(&argTypes);
    size32_t bottomArg = popStack(&argTypes);

    void* toPush = malloc(32);

    // Write stack manipulation
    //! Todo: Make better switch case
    switch (argTypes) {
    case f_nil:
        perror("You tried adding nothing\n");
        break;

    case f_32int:
        C_OPERATE(int32_t, +)
        pushStack(f_32int,  toPush);
        break;

    case f_32float: case f_32int|f_32float:
        C_OPERATE(float, +)
        pushStack(f_32float, toPush);
        break;

    }



    return;
}





