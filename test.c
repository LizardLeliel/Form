
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

#include "test.h"
#include "stack.h"
#include "bigdecls.h"
#include "build.h"
#include "hash.h"
<<<<<<< HEAD
#include "instructions.h"
=======
>>>>>>> fbaab0080dc700dae1c8ddc879a0d3c19c65ef50

// The macro below is an integer representing the last 20 bits being 1. Or:
//  00000000 00001111 11111111 11111111
#define bit20 1048575

#define GO_THROUGH(tracer) while(tracer) {\
    printf("%d ", tracer->instruction);   \
        tracer = tracer->next;            \
    } printf("\n");



void runChecks() {
    int32_t sampInt = 423;

<<<<<<< HEAD
    printf("%u\n", sizeof(hash_t)*bit20);
=======
    printf("%lu\n", sizeof(hash_t)*bit20);
>>>>>>> fbaab0080dc700dae1c8ddc879a0d3c19c65ef50

    // Simple push test
    pushStack(f_int, (void*)&sampInt);
    printf("Checking pushing: %d %s\n", *(int32_t*)(STACK->data),
            *(int32_t*)(STACK->data) == 423 ? "Pass" : "Fail");

    // Simple pop test
<<<<<<< HEAD
    dropStack();
=======
    popStack();
>>>>>>> fbaab0080dc700dae1c8ddc879a0d3c19c65ef50
    printf("Checking popping: %d %s\n", STACK->type,
            STACK->type == 0 ? "Pass" : "Fail");

    // Simple chain build test
    appendInstruction(nop, 0, NULL);
    appendInstruction(mod, 0, NULL);
    appendInstruction(scope, 0, NULL);

    // Make tracer and go through it
    instruction_t* tracer = buildPointers.programTop->head;
    printf("main before a new function was added: ");
    GO_THROUGH(tracer)

    // Start a new function; add instructions
    makeNewFunction();

    appendInstruction(gotos, 0, NULL);
    appendInstruction(logand, 0, NULL);
    appendInstruction(call, 0, NULL);

    // End function; add one more instruction for good measure
    endFunction();

    appendInstruction(sub, 0, NULL);

    // Go through the two chains
    tracer = buildPointers.programTop->head;

    printf("main after function is added: ");
    GO_THROUGH(tracer)

    tracer = buildPointers.programTop->next->head;

    printf("The function that was added: ");
    GO_THROUGH(tracer)

    // Test the hash function
    printf("The hash function and its various results, and their various ");
    printf("results with their\n first two bytes &'d:\n");

    char* ourSymbol1  = "Why?";
    char* ourSymbol2 = "KonohanaSakuya";
    char* ourSymbol3 = "ourSymbol3";


    printf("%-20s %30lu\n",
            ourSymbol1, hashFunction(strlen(ourSymbol1), ourSymbol1));
    printf("%-20s %30lu\n",
            ourSymbol2, hashFunction(strlen(ourSymbol2), ourSymbol2));
<<<<<<< HEAD
    printf("%-20s %30lu\n",
=======
    printf("%-20s %30lx\n",
>>>>>>> fbaab0080dc700dae1c8ddc879a0d3c19c65ef50
            ourSymbol3, hashFunction(strlen(ourSymbol3), ourSymbol3));

    printf("&'d %-16s %30lu\n",
            ourSymbol1, bit20 & hashFunction(strlen(ourSymbol1), ourSymbol1));
    printf("&'d %-16s %30lu\n",
            ourSymbol2, bit20 & hashFunction(strlen(ourSymbol2), ourSymbol2));
<<<<<<< HEAD
    printf("&'d %-16s %30lu\n",
            ourSymbol3, bit20 & hashFunction(strlen(ourSymbol3), ourSymbol3));

    printf("\n\n");


#define printHashResults(x)                      \
    printf("%-30s %u, %u\n",                     \
        x,                                       \
        getHashID(h_variableName, strlen(x), x), \
        getHashID(h_variableName, strlen(x), x)  \
    );

    printHashResults(ourSymbol1)
    printHashResults(ourSymbol2)
    printHashResults(ourSymbol3)
    printHashResults(ourSymbol1)

    // Test our very first coded instruction: i_add
    pushStack(f_int, &sampInt);
    pushStack(f_int, &sampInt);

    EXEC_INSTRUCTION[add]();

    printf("\nThe stack after pushing 423 twice and adding: %d\n",
           *(uint32_t*)STACK->data);

    return ;
=======
    printf("&'d %-16s %30lx\n",
            ourSymbol3, bit20 & hashFunction(strlen(ourSymbol3), ourSymbol3));

    printf("\n\n\n");
    return;
>>>>>>> fbaab0080dc700dae1c8ddc879a0d3c19c65ef50
}


