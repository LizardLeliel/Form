#include "runtime.h"
#include "basicOperations.h"

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
    i_nop, // No operation

    // Standard arthmetic opertaions
    i_add, 
    i_sub,
    i_mul,
    i_divs,
    i_mod,

    // Standard bitwise operations
    i_nop,
    i_nop,
    i_nop,
    i_nop,
    i_nop,
    i_nop,

    // Standard comparison operations
    i_lessthen,
    i_lesstheneq,
    i_greaterthen,
    i_greatertheneq,
    i_eq,
    i_ineq,

    // Boolean logic operations
    i_logand,
    i_logor,
    i_lognot,

    // Increment and deincremetn operations
    i_nop,
    i_nop,

    // Standard stack-related operations
    i_push,
    i_nop,
    i_nop,
    i_nop,
    i_nop,

    // Form goto operations
    i_nop,
    i_nop,

    // Variable-related operations
    i_nop,
    i_nop,
    i_nop,

    // Misc operations 
    i_call,
    i_returns,
    i_print,
    i_nop
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

void i_push(program_context_t* program)
{
    data_type_t type = program->currentInstruction.arg1;
    int64_t     data = program->currentInstruction.arg2;

    pushStack(&(program->dataStack), type, data);
}

void i_call(program_context_t* program)
{
    unsigned int functionIndex 
        = program->currentInstruction.arg2;

    pushFunction(&(program->functionStack), 
                 program->currentFunctionIndex,
                 program->nextInstructionIndex);

    program->nextFunctionIndex    = functionIndex;
    program->nextInstructionIndex = 0;

}

void i_returns(program_context_t* program)
{
    returnFromFunction(program);
}

void i_print(program_context_t* program)
{
    shouldNotBeBottom(&(program->dataStack));

    data_t value = popData(&(program->dataStack));

    // Determine how to print based on type.
    if (value.dataType & f_32float)
    {
        printf("%f\n", interpretAsFloat(value.data));
    }
    else if (value.dataType & f_32int)
    {
        printf("%ld\n", value.data);
    }
    else if (value.dataType & f_bool)
    {
        if (value.data == 0)
        {
            printf("False\n");
        }
        else
        {
            printf("True\n");
        }
    }
    else if (value.dataType & f_string)
    {
        printf("%s\n", program->staticDataBank.dataBank[value.data].data);
    }
    
}

// Reinterpretation casting functions.
int64_t interpretAsInt(double value)
{
    any64_t result;
    result.as_f = value;
    return result.as_i;
}
double interpretAsFloat(int64_t value)
{
    any64_t result;
    result.as_i = value;       
    return result.as_f;
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
    while (program.currentInstruction.instruction != endProg)
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
