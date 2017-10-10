#include "instructions.h"
#include "stack.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

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
