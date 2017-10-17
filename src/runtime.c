#include "runtime.h"
#include "basicOperations.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>


void shouldNotBeBottom(stack_t** dataStack) 
{
    // Try changing later. Make better warning error for user
    if ((*dataStack)->next == NULL) perror("Stack underflow");
}

// Pushes data onto the stack.
// Can we change it so all data isn't 32-bit?
void pushStack(stack_t** dataStack, data_type_t dataType, void* data) 
{
    stack_t* newNode = malloc(sizeof(stack_t));
    newNode->data    = malloc(4);

    memcpy(newNode->data, data, 4);

    newNode->type    = dataType;
    newNode->next    = *dataStack;
    *dataStack       = newNode;

}

// Pops data and frees it
void dropStack(stack_t** dataStack) {
    shouldNotBeBottom(dataStack);

    stack_t* oldNode = *dataStack;
    *dataStack        = (*dataStack)->next;
    free(oldNode->data); // ?
    free(oldNode);
}

//! Todo: Fix popping behaviour
// Pops data from the stack, frees dynamic memory associated
//  with it, then returns that value.
// Wait, it pops a value of size 32 when we store it as
//  a void pointer?
size32_t popStack(stack_t** dataStack, data_type_t* outType) 
{
    shouldNotBeBottom(dataStack);

    if (outType != NULL) *outType = (*dataStack)->type;

    size32_t returnVal = *(size32_t*)(*dataStack)->data;
    stack_t* freeNode  = (*dataStack);

    *dataStack = (*dataStack)->next;

    free(freeNode->data);
    free(freeNode);
    return returnVal;
}

// Like popStack, but puts the data into a data_t struct.
data_t popData(stack_t** dataStack)
{
    shouldNotBeBottom(dataStack);
    data_type_t outType;
    size32_t data = popStack(dataStack, &outType);

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
    i_nop,
    i_nop,
    i_nop,
    i_nop,
    i_nop,

    // Boolean logic operations
    i_nop,
    i_nop,
    i_nop,

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


void pushFunction(function_stack_t* function_stack, 
                  instruction_t* returnInstruction)
{
    if (function_stack->head == NULL)
    {
        function_stack->head = malloc(sizeof(function_stack_node_t));
        function_stack->head->next = NULL;
        function_stack->head->returnInstruction = returnInstruction;
        ++(function_stack->depth);
    }
    else
    {
        function_stack_node_t* newFunction = malloc(sizeof(function_stack_t));
        newFunction->next                  = function_stack->head;
        newFunction->returnInstruction     = returnInstruction;
        function_stack->head               = newFunction;
    }
}

void returnFromFunction(program_context_t* program)
{
    if (program->functionStack.head == NULL)
    {
        perror("Function stack underflow");
    }
    program->currentInstruction 
        = program->functionStack.head->returnInstruction;

    function_stack_node_t* freeThis = program->functionStack.head;
    program->functionStack.head      = program->functionStack.head->next;
    --program->functionStack.depth;
    free(freeThis);
    return;
}

// Move this to basicOPerations.c
void i_lessthen(program_context_t* program)
{
    size32_t evaluation;
    data_t operandB = popData(&(program->dataStack));
    data_t operandA = popData(&(program->dataStack));
    data_type_t type = prepareOperands(&operandA, &operandB);
    if (type == f_32int)
    {
        evaluation = operandA.data < operandB.data;
        pushStack(&(program->dataStack), type | f_numeric, &evaluation);
    }
    else if (type == f_32float)
    {
        size32_t value 
            = interpretAsFloat(operandA.data)
            < interpretAsFloat(operandB.data);
        evaluation = value;
        pushStack(&(program->dataStack), f_numeric | f_bool, &evaluation);
    }
}

void i_push(program_context_t* program)
{
    data_type_t type = ((data_type_t*)program->currentInstruction->args)[0];

    pushStack(&(program->dataStack), 
              type, 
              program->currentInstruction->args + sizeof(data_type_t));
    // Arg 1: type
    // Arg 2: data
}

void i_call(program_context_t* program)
{
    unsigned int functionIndex 
        = *(unsigned int*)program->currentInstruction->args;

    pushFunction(&(program->functionStack), program->currentInstruction);

    program->currentInstruction 
        = program->code[functionIndex];
}

void i_returns(program_context_t* program)
{
    returnFromFunction(program);
    return;
}

void i_print(program_context_t* program)
{
    data_t value = popData(&(program->dataStack));

    if (value.dataType & f_32float)
    {
        printf("%f\n", interpretAsFloat(value.data));
    }
    else if (value.dataType & f_32int)
    {
        printf("%d\n", value.data);
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
}

size32_t interpretAsInt(float value)
{
    any32_t result;
    result.as_f = value;
    return result.as_i;
}

float interpretAsFloat(size32_t operandValue)
{
    any32_t value;
    value.as_i = operandValue;       
    return value.as_f;
}

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
        any32_t data;
        data.as_f = (float)operandA->data;
        operandA->data = data.as_i;
    }
    else if (operandB->dataType & f_32int)
    {
        any32_t data;
        data.as_f = (float)operandB->data;
        operandB->data = data.as_i;
    }

    return f_32float;
} 


void execute(program_context_t program)
{   
    // Refactor into function?
    program.functionStack.depth = 0;
    program.functionStack.head  = NULL;

    // Stack starts initialized with a null node
    stack_t bottom =
    {
        f_nil,
        NULL,
        NULL,
    };

    program.dataStack = &bottom;
    program.currentInstruction = program.code[0];

    while (program.currentInstruction != NULL)
    {
        EXEC_INSTRUCTION[program.currentInstruction->instruction](&program);
        program.currentInstruction = program.currentInstruction->next;
    }
}
