
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

#include "test.h"
#include "stack.h"
#include "bigdecls.h"
#include "build.h"
#include "hash.h"

#define GO_THROUGH(tracer) while(tracer) {\
    printf("%d ", tracer->instruction);   \
        tracer = tracer->next;            \
    } printf("\n");



void runChecks() {
    int32_t sampInt = 423;

    // Simple push test
    pushStack(f_int, (void*)&sampInt);
    printf("Checking pushing: %d %s\n", *(int32_t*)(STACK->data),
            *(int32_t*)(STACK->data) == 423 ? "Pass" : "Fail");

    // Simple pop test
    popStack();
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

    char* ourSymbol1  = "Bonjour";
    char* ourSymbol2 = "KonohanaSakuya";
    char* ourSymbol3 = "ourSymbol3";

#define bit20 1048575

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

    printf("\n\n\n");
    return;
}


