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

void i_nop() {
    return;
}
void i_add() {
    shouldNotBeBottom();

    // Write stack manipulation
    switch (STACK->type) {
        case f_int:;

            int32_t highest = *(int32_t*)STACK->data;
            dropStack();
            shouldNotBeBottom();
            int32_t nextHighest = *(int32_t*)STACK->data;
            dropStack();
            int32_t result = highest + nextHighest;
            pushStack(f_int, &result);
            break;
        default:
            break;
    }

    return;
}





