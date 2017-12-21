#include "runtime.h"
#include "instructions.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

// Raise error if at bottom of runtime data stack
void shouldNotBeBottom(stack_t** dataStack) 
{
    // Try changing later. Make better warning error for user
    if ((*dataStack)->next == NULL) 
    {
        puts("Stack underflow in shouldNotBeBottom()");
        // Todo: print helpful debugging info
        exit(1);
    }
}

// Push data on the run-time stack.
void pushStack(stack_t**   dataStack, 
               data_type_t dataType, 
               int64_t     data) 
{
    stack_t* newNode = malloc(sizeof(stack_t));
    newNode->data    = data;
    newNode->type    = dataType;
    newNode->next    = *dataStack;
    *dataStack       = newNode;

}

// Deletes the top of the stack
void dropStack(stack_t** dataStack) {
    shouldNotBeBottom(dataStack);

    stack_t* oldNode = *dataStack;
    *dataStack       = (*dataStack)->next;
    free(oldNode);
}

// Pops the stack, returns its value and puts the data type into outType
int64_t popStack(stack_t** dataStack, data_type_t* outType) 
{
    shouldNotBeBottom(dataStack);

    // Fetch the data.
    if (outType != NULL) *outType = (*dataStack)->type;
    int64_t returnVal = (*dataStack)->data;

    // Free the top of the stack.
    stack_t* freeNode = *dataStack;
    *dataStack = (*dataStack)->next;
    free(freeNode); 

    return returnVal;
}

// Pops the stack, then puts the data into a data_t data struct
data_t popData(stack_t** dataStack)
{
    shouldNotBeBottom(dataStack);

    // Get values from popping the stack.
    data_type_t outType;
    int64_t data = popStack(dataStack, &outType);

    // Wrap the dta into a data_t struct.
    data_t returnStruct 
        = {.dataType = outType, .data = data};

    return returnStruct;
}

// This is an array of function pointers. These functions
//  are called during runtime.
void (*EXEC_INSTRUCTION[instruction_ammount])(program_context_t*) =
{
    FORM_NOP, // No operation

    // Standard arthmetic opertaions
    FORM_ADD, 
    FORM_SUB,
    FORM_MUL,
    FORM_DIV,
    FORM_MOD,

    // Standard bitwise operations
    FORM_NOP,
    FORM_NOP,
    FORM_NOP,
    FORM_NOP,
    FORM_NOP,
    FORM_NOP,

    // Standard comparison operations
    FORM_LESS_THEN,
    FORM_LESS_THEN_EQ,
    FORM_GREATER_THEN,
    FORM_GREATER_THEN_EQ,
    FORM_EQ,
    FORM_INEQ,

    // Boolean logic operations
    FORM_LOG_AND,
    FORM_LOG_OR,
    FORM_LOG_NOT,

    // Increment and deincremetn operations
    FORM_NOP,
    FORM_NOP,

    // Standard stack-related operations
    FORM_PUSH,
    FORM_NOP,
    FORM_NOP,
    FORM_NOP,
    FORM_NOP,

    // Form goto operations
    FORM_GOTO,
    FORM_COND_GOTO,

    // Variable-related operations
    FORM_NOP,
    FORM_NOP,
    FORM_NOP,

    // Misc operations 
    FORM_CALL,
    FORM_RETURN,
    FORM_PRINT,
    FORM_NOP
};

// Calls a new function.
void pushFunction(function_stack_t* functionStack, 
                  unsigned int callingFunction,
                  unsigned int instructionDestination)
{
    if (functionStack->head == NULL)
    {
        functionStack->head
            = malloc(sizeof(function_stack_node_t));
        functionStack->head->next             = NULL;
        functionStack->head->functionIndex    = callingFunction;
        functionStack->head->instructionIndex = instructionDestination;
        ++(functionStack->depth);
    }
    else
    {
        function_stack_node_t* newFunction = malloc(sizeof(function_stack_t));
        newFunction->next                  = functionStack->head;
        newFunction->functionIndex         = callingFunction;
        newFunction->instructionIndex      = instructionDestination;
        functionStack->head                = newFunction;
    }
}

// Sets execution back to previous function.
void returnFromFunction(program_context_t* program)
{
    // Check for underflow.
    if (program->functionStack.head == NULL)
    {
        perror("Function stack underflow (no function to return to");
        exit(1);
    }

    // Sets next instructions to go back to previous function
    program->nextFunctionIndex    = program->functionStack.head->functionIndex;
    program->nextInstructionIndex = program->functionStack.head->instructionIndex;

    // Frees node from function stack.
    function_stack_node_t* freeThis = program->functionStack.head;
    program->functionStack.head     = program->functionStack.head->next;
    --program->functionStack.depth;
    free(freeThis);
}

// If either operand is a float, converts the other operand to 
//  a float and returns f_float.
// Else, if they're both integer types, leave them be and return
//  f_int
data_type_t prepareOperands(data_t* operandA, data_t* operandB)
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
        any64_t data;
        data.as_f = (double)operandA->data;
        operandA->data = data.as_i;
    }
    else if (operandB->dataType & f_32int)
    {
        any64_t data;
        data.as_f = (double)operandB->data;
        operandB->data = data.as_i;
    }

    return f_32float;
} 

// Executes the program.
void execute(program_context_t program)
{   

    // Create a dummy stack bottom.
    stack_t bottom =
    {
        f_nil,
        0,
        NULL,
    };
    program.dataStack = &bottom;

    // Initialize values so execution starts at main.
    program.currentInstruction      = program.code[0][0];
    program.currentInstructionIndex = 0;
    program.currentFunctionIndex    = 0;
    program.nextFunctionIndex       = program.currentFunctionIndex;

    // Iterate through each instruction until the end of the program is reached.
    while (program.currentInstruction.instruction != i_endprog)
    {
        // Set the default next instruction.
        program.nextInstructionIndex = program.currentInstructionIndex + 1;

        // Execute the next instruction.
        EXEC_INSTRUCTION[program.currentInstruction.instruction](&program);

        // Fetch the next instruction.
        program.currentInstruction 
            = program.code[program.nextFunctionIndex][program.nextInstructionIndex];

        // Update current instruction/function markers.
        program.currentFunctionIndex    = program.nextFunctionIndex;
        program.currentInstructionIndex = program.nextInstructionIndex;
    }
}
