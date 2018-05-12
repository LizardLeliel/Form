#include "runtime.h"
#include "instructions.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

// Raise error if at bottom of runtime data stack
// Just return a 1 or 0, let the above function print the error
void shouldNotBeBottom(data_stack_t* dataStack) 
{
    // Try changing later. Make better warning error for user
    if (dataStack->top == NULL) 
    {
        puts("Stack underflow in shouldNotBeBottom()");
        // Todo: print helpful debugging info
        exit(1);
    }
}

// Push data on the run-time stack.
void pushStack(data_stack_t* dataStack, 
               data_type_t   dataType, 
               int64_t       data) 
{
    data_stack_node_t* newNode = malloc(sizeof(data_stack_node_t));

    newNode->data.data = data;
    newNode->data.type = dataType;
    newNode->next      = dataStack->top;
    dataStack->top     = newNode;
    ++(dataStack->depth);
}

// Deletes the top of the stack
void dropStack(data_stack_t* dataStack) {
    shouldNotBeBottom(dataStack);

    data_stack_node_t* oldNode = dataStack->top;
    dataStack->top             = dataStack->top->next;
    free(oldNode);

    --(dataStack->depth);
}

// Pops the stack, then puts the data into a data_t data struct
data_t popData(data_stack_t* dataStack)
{

    // Put the data into a data_t struct.
    data_t returnStruct         = dataStack->top->data;
    data_stack_node_t* freeNode = dataStack->top;
    dataStack->top              = dataStack->top->next;
    free(freeNode); 

    --(dataStack->depth);
    return returnStruct;
}

// Pick data here?

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
    FORM_PICK,
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
    if ((operandA->type & f_32int) 
        && (operandB->type & f_32int))
    {
        return f_32int;
    }
    // If they're both not ints, then we want to cast any ints to floats
    else if (operandA->type & f_32int)
    {
        // cast it to a float, then reinterpret cast it.
        any64_t data;
        data.as_f = (double)operandA->data;
        operandA->data = data.as_i;
    }
    else if (operandB->type & f_32int)
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
