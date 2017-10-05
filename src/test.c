
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

#include "test.h"
#include "stack.h"
#include "build.h"
#include "hash.h"
#include "instructions.h"

// The macro below is an integer representing the last 20 bits being 1. Or:
//  00000000 00001111 11111111 11111111
#define bit20 1048575

#define GO_THROUGH(tracer) while(tracer) {\
    printf("%d ", tracer->instruction);   \
        tracer = tracer->next;            \
    } printf("\n");



void runChecks() {
    int32_t sampInt = 423;
    float sampFloat = 1.5f;

    //printf("%lu\n", sizeof(hash_t)*bit20);

    // Simple push test
    pushStack(f_32int|f_numeric, (void*)&sampInt);
    printf("%p\n", STACK->next);
    printf("Checking pushing: %d %s\n", *(int32_t*)(STACK->data),
            *(int32_t*)(STACK->data) == 423 ? "Pass" : "Fail");

    // Simple pop test
    dropStack();
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

    char* ourSymbol1 = "Why?";
    char* ourSymbol2 = "KonohanaSakuya";
    char* ourSymbol3 = "ourSymbol3";


    printf("%-20s %30lu\n",
            ourSymbol1, hashFunction(strlen(ourSymbol1), ourSymbol1));
    printf("%-20s %30lu\n",
            ourSymbol2, hashFunction(strlen(ourSymbol2), ourSymbol2));
    printf("%-20s %30lu\n",
            ourSymbol3, hashFunction(strlen(ourSymbol3), ourSymbol3));

    printf("&'d %-16s %30lu\n",
            ourSymbol1, bit20 & hashFunction(strlen(ourSymbol1), ourSymbol1));
    printf("&'d %-16s %30lu\n",
            ourSymbol2, bit20 & hashFunction(strlen(ourSymbol2), ourSymbol2));
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
    pushStack(f_32int|f_numeric, &sampInt);
    pushStack(f_32int|f_numeric, &sampInt);

    EXEC_INSTRUCTION[add]();

    printf("\nThe stack after pushing 423 twice and adding: %d\n",
           *(uint32_t*)STACK->data);

    pushStack(f_32float|f_numeric, &sampFloat);
    pushStack(f_32float|f_numeric, &sampFloat);

    EXEC_INSTRUCTION[add]();

    printf("\nThe stack after pushing 1.5f twice and adding: %f\n",
           *(float*)STACK->data);

    // And now that there _should_ be a float and a whole number on top, let's
    //  see how it handles with that!
    EXEC_INSTRUCTION[add]();
    printf("The stack after adding the results that are on the stack: %f\n",
            *(float*)STACK->data);

    // We checked two ints, two floats, the top a float and the next an int,
    //  now let's check for an int on top then a float
    pushStack(f_32int|f_numeric, &sampInt);
    EXEC_INSTRUCTION[add]();

    printf("Pushing 423 on the stack then adding to the float on the stack: %f\n",
            *(float*)STACK->data);

    pushStack(f_32int|f_numeric, &sampInt);
    EXEC_INSTRUCTION[sub]();
    printf("Pushing 423 on the stack then subtracting: %f\n",
            *(float*)STACK->data);

    pushStack(f_32int|f_numeric, &sampInt);
    EXEC_INSTRUCTION[mul]();
    printf("Pushing 423 on the stack then multiplying: %f\n",
            *(float*)STACK->data);

    pushStack(f_32int|f_numeric, &sampInt);
    EXEC_INSTRUCTION[divs]();
    printf("Pushing 423 on the stack then dividing: %f\n",
            *(float*)STACK->data);

    pushStack(f_32int|f_numeric, &sampInt);
    EXEC_INSTRUCTION[lessthen]();
    printf("Pushing 423 on the stack then comparing less then: %d\n",
            *(uint32_t*)STACK->data);

    return;
}

