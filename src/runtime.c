#include "runtime.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>


// Stack starts initialized with a null node
stack_t _bottom =
{
    f_nil,
    NULL,
    NULL,
};

stack_t* STACK;

/* Set STACK to initialy point to bottom (note; I couldn't find a
 *  way to statically set STACK to point to _bottom; & is a runtime
 *  operator, and the value of &_bottom isn't known at compile time
 */
void stackIni() 
{
    STACK = &_bottom;
}

void shouldNotBeBottom() 
{
    // Try changing later. Make better warning error for user
    if (STACK->next == NULL) perror("Stack underflow");
}

// Pushes data onto the stack.
// Can we change it so all data isn't 32-bit?
void pushStack(data_type_t dataType, void* data) 
{
    stack_t* newNode = malloc(sizeof(stack_t));
    newNode->data    = malloc(32);
    memcpy(newNode->data, data, 32);

    newNode->type    = dataType;
    newNode->next    = STACK;
    STACK            = newNode;

}

// Pops data and frees it
void dropStack() {
    shouldNotBeBottom();

    stack_t* oldNode = STACK;
    STACK            = STACK->next;
    free(oldNode->data); // ?
    free(oldNode);
}

//! Todo: Fix popping behaviour
// Pops data from the stack, frees dynamic memory associated
//  with it, then returns that value.
// Wait, it pops a value of size 32 when we store it as
//  a void pointer?
size32_t popStack(data_type_t* outType) 
{
    shouldNotBeBottom();

    if (outType != NULL) *outType = STACK->type;

    size32_t returnVal = *(size32_t*)STACK->data;
    stack_t* freeNode  = STACK;

    STACK = STACK->next;

    free(freeNode->data);
    free(freeNode);
    return returnVal;
}

data_t popData()
{
    shouldNotBeBottom();
    data_type_t outType;
    size32_t data = popStack(&outType);

    data_t returnStruct 
        = {.dataType = outType, .data = data};
    return returnStruct;
}



// Arguements for instructions loaded in here
//void* instructionArgs;

// A pointer of the current instruction being executed
instruction_t* CURRENT_INSTRUCTION;
function_stack_t FUNCTION_STACK;

// Refactor this into a not-global-variable.
program_context_t* GLOBAL_CONTEXT;

// This is an array of function pointers. These functions
//  are called during runtime.
void (*EXEC_INSTRUCTION[instruction_ammount])()=
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

// This is an array of function pointers. These functions
//  are called during runtime.
void (*EXEC_INSTRUCTION[instruction_ammount])();

void pushFunction(instruction_t* returnInstruction)
{
    if (FUNCTION_STACK.head == NULL)
    {
        FUNCTION_STACK.head = malloc(sizeof(function_stack_node_t));
        FUNCTION_STACK.head->next = NULL;
        FUNCTION_STACK.head->returnInstruction = returnInstruction;
        ++FUNCTION_STACK.depth;
    }
    else
    {
        function_stack_node_t* newFunction = malloc(sizeof(function_stack_t));
        newFunction->next                  = FUNCTION_STACK.head;
        newFunction->returnInstruction     = returnInstruction;
        FUNCTION_STACK.head                = newFunction;
    }
}

void returnFromFunction()
{
    if (FUNCTION_STACK.head == NULL)
    {
        perror("Function stack underflow");
    }
    CURRENT_INSTRUCTION = FUNCTION_STACK.head->returnInstruction;

    function_stack_node_t* freeThis = FUNCTION_STACK.head;
    FUNCTION_STACK.head = FUNCTION_STACK.head->next;
    --FUNCTION_STACK.depth;
    free(freeThis);
    return;
}

// No operation
void i_nop() 
{
    return;
}

void i_add() 
{
    size32_t evaluation;
    data_t operandB = popData();
    data_t operandA = popData();
    data_type_t type = prepareOperands(&operandA, &operandB);

    if (type == f_32int)
    {
        evaluation = operandA.data + operandB.data;
        pushStack(type | f_numeric, &evaluation);
    }
    else if (type == f_32float)
    {
        //evaluation
        float value 
            = interpretAsFloat(operandA.data)
            + interpretAsFloat(operandB.data);
        evaluation = interpretAsInt(value);
        pushStack(type | f_numeric, &evaluation);

    }
}

void i_sub()
{
    size32_t evaluation;
    data_t operandB = popData();
    data_t operandA = popData();
    data_type_t type = prepareOperands(&operandA, &operandB);
    if (type == f_32int)
    {
        evaluation = operandA.data - operandB.data;
        pushStack(type | f_numeric, &evaluation);
    }
    else if (type == f_32float)
    {
        float value 
            = interpretAsFloat(operandA.data)
            - interpretAsFloat(operandB.data);
        evaluation = interpretAsInt(value);
        pushStack(type | f_numeric, &evaluation);

    }
}
void i_mul()
{
    size32_t evaluation;
    data_t operandB = popData();
    data_t operandA = popData();
    data_type_t type = prepareOperands(&operandA, &operandB);
    if (type == f_32int)
    {
        evaluation = operandA.data * operandB.data;
        pushStack(type | f_numeric, &evaluation);
    }
    else if (type == f_32float)
    {
        float value 
            = interpretAsFloat(operandA.data)
            * interpretAsFloat(operandB.data);
        evaluation = interpretAsInt(value);
        pushStack(type | f_numeric, &evaluation);

    }
}

void i_divs()
{
    size32_t evaluation;
    data_t operandB = popData();
    data_t operandA = popData();
    data_type_t type = prepareOperands(&operandA, &operandB);
    if (type == f_32int)
    {
        if (operandB.data == 0) 
        {
            perror("Division by zero");
        }
        evaluation = operandA.data / operandB.data;
        pushStack(type | f_numeric, &evaluation);
    }
    else if (type == f_32float)
    {
        if (*(float*)&(operandB.data) == 0)
        {
            perror("Division by zero");
        }
        float value 
            = interpretAsFloat(operandA.data)
            / interpretAsFloat(operandB.data);
        evaluation = interpretAsInt(value);
        pushStack(type | f_numeric, &evaluation);
    }
}

void i_mod()
{
    size32_t evaluation;
    data_t operandB = popData();
    data_t operandA = popData();
    data_type_t type = prepareOperands(&operandA, &operandB);
    if (type == f_32int)
    {
        if (operandB.data == 0) 
        {
            perror("Division by zero");
        }
        evaluation = operandA.data % operandB.data;
        pushStack(type | f_numeric, &evaluation);
    }
    else if (type == f_32float)
    {
        perror("Moding a float");
    }
}

void i_lessthen()
{
    size32_t evaluation;
    data_t operandB = popData();
    data_t operandA = popData();
    data_type_t type = prepareOperands(&operandA, &operandB);
    if (type == f_32int)
    {
        evaluation = operandA.data < operandB.data;
        pushStack(type | f_numeric, &evaluation);
    }
    else if (type == f_32float)
    {
        size32_t value 
            = interpretAsFloat(operandA.data)
            < interpretAsFloat(operandB.data);
        evaluation = value;
        pushStack(f_numeric | f_bool, &evaluation);
    }
}

void i_push()
{
    data_type_t type = ((data_type_t*)CURRENT_INSTRUCTION->args)[0];

    pushStack(type, CURRENT_INSTRUCTION->args + sizeof(data_type_t));

    // Arg 1: type
    // Arg 2: data
}

void i_call()
{
    unsigned int functionIndex = *(unsigned int*)CURRENT_INSTRUCTION->args;

    pushFunction(CURRENT_INSTRUCTION);

    CURRENT_INSTRUCTION = GLOBAL_CONTEXT->functions[functionIndex];
}

void i_returns()
{
    returnFromFunction();
    return;
}

void i_print()
{
    data_t value = popData();

    if (value.dataType & f_32float)
    {
        printf("%f\n", interpretAsFloat(value.data));
    }
    else if (value.dataType & f_32int)
    {
        printf("%d\n", value.data);
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
    FUNCTION_STACK.depth = 0;
    FUNCTION_STACK.head = NULL;

    GLOBAL_CONTEXT = &program;

    CURRENT_INSTRUCTION = program.functions[0];
    while (CURRENT_INSTRUCTION != NULL)
    {
        EXEC_INSTRUCTION[CURRENT_INSTRUCTION->instruction]();
        CURRENT_INSTRUCTION = CURRENT_INSTRUCTION->next;
    }
}
