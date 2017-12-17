#include "build.h"
#include "runtime.h"
#include "tokenhash.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <stdio.h>

// Returns an empty program build object.
program_build_t prepareBuild()
{
    program_build_t programBuild;

    // Initalize the hash
    programBuild.tokenHash          = makeTokenHash();
    programBuild.constantDataList   = makeConstantDataList();

    // Intialize pointers
    programBuild.programTop         = malloc(sizeof(function_header_t));
    programBuild.currentInstruction = dummyInstruction();
    programBuild.lastFunction       = programBuild.programTop;
    programBuild.mainLast           = programBuild.currentInstruction;

    // Initialize main function's scope info stack
    programBuild.programTop->scopeBranchInfoStack.depth = 0;
    programBuild.programTop->scopeBranchInfoStack.head  = NULL;

    // Initializes the main function
    programBuild.programTop->head      = programBuild.currentInstruction;
    programBuild.programTop->next      = NULL;
    programBuild.programTop->depth     = 1;
    programBuild.programTop->ifTracker 
        = createIfTracker(programBuild.programTop->depth);

    // Initialize the misc variables
    programBuild.onMain         = true;
    programBuild.functionAmount = 1;
    programBuild.lineNumber     = 1;

    return programBuild;
}

// Allocates a constant data list.
constant_data_list_t makeConstantDataList()
{
    constant_data_list_t constantDataList;

    constantDataList.top   = NULL;
    constantDataList.depth = 0;

    return constantDataList;
}

if_sequence_tracker_t createIfTracker(unsigned int functionNumber)
{
    if_sequence_tracker_t tracker;

    tracker.sequence       = 0;
    tracker.functionNumber = functionNumber;
    tracker.elifSequence   = 0;
    tracker.scope          = 0;
    tracker.thenFlag       = false;
    tracker.elseFlag       = false;
    tracker.nextID         = 0;

    return tracker;;
}

if_sequence_tracker_t* getTracker(program_build_t* program)
{
    if (program->onMain == true)
    {
        return &(program->programTop->ifTracker);
    }
    else
    {
        return &(program->lastFunction->ifTracker);
    }
}

// Push new data to the constant data stack, returns which
//  index it'll be stored at during runtime.
int64_t pushConstantData(constant_data_list_t* constantDataList,
                        data_type_t            type,
                        void*                  data)
{
    // Initialize constant data node.
    constant_data_list_node_t* newNode 
        = malloc(sizeof (constant_data_list_node_t));
    newNode->type = type;
    newNode->data = data;
    newNode->eventualIndex = (constantDataList->depth);
        
    ++(constantDataList->depth);

    // Add the new node
    newNode->next         = constantDataList->top;
    constantDataList->top = newNode;       

    return newNode->eventualIndex;
}

// These two functions are untested.
void pushScopeBranchInfo(scope_branch_info_stack_t* infoStack,
                         if_sequence_tracker_t      info) 
{
    scope_branch_info_node_t* newInfo 
        = malloc(sizeof (scope_branch_info_node_t));

    newInfo->info.scope    = info.scope;
    newInfo->info.thenFlag = info.thenFlag;
    newInfo->info.elseFlag = info.elseFlag;
    newInfo->info.id       = info.nextID;
    newInfo->info.sequence = info.sequence;

    newInfo->next     = infoStack->head;
    infoStack->head   = newInfo;
    infoStack->depth += 1;
    return;
}
void popScopeBranchInfo(scope_branch_info_stack_t* infoStack,
                        if_sequence_tracker_t*     info)
{
    if (infoStack->head == NULL)
    {
        // Think about why this error may happen and if this could happen,
        //  change the error message.
        perror("Fatal Error: no scope to return in popScopeBranchInfo");
        exit(1);
    }

    scope_branch_info_t revertInfo = infoStack->head->info;

    info->scope    = revertInfo.scope;
    info->thenFlag = revertInfo.thenFlag;
    info->elseFlag = revertInfo.elseFlag;
    info->nextID   = revertInfo.id;
    info->sequence = revertInfo.sequence;

    scope_branch_info_node_t* freeNode = infoStack->head;
    infoStack->head                    = infoStack->head->next;
    free(freeNode);

    return;
}

// Returns a dummy instruction
instruction_node_t* dummyInstruction() 
{
    instruction_node_t* dummy = malloc(sizeof(instruction_node_t));
    dummy->instruction        = i_nop;
    dummy->index              = 0;
    dummy->arg1               = 0;
    dummy->arg2               = 0;
    dummy->next               = NULL;
    return dummy;
}

// Adds a new instruction to end of current instruction sequence.
void appendInstruction(program_build_t*   programBuild,
                       instruction_type_t newInstruct,
                       int32_t            arg1,
                       int64_t            arg2)
{
    // Initialize the instruction pointer
    instruction_node_t* newInstructNode = malloc(sizeof(instruction_node_t));
    newInstructNode->instruction        = newInstruct;
    newInstructNode->next               = NULL;
    newInstructNode->arg1               = arg1;
    newInstructNode->arg2               = arg2;

    // Determine if we need to increase the main function's depth
    //  or the last-function-added's depther
    if (programBuild->onMain == true)
    {
        programBuild->programTop->depth += 1;
        newInstructNode->index            = programBuild->programTop->depth;
    }
    else
    {
        programBuild->lastFunction->depth += 1;
        newInstructNode->index             = programBuild->lastFunction->depth;
    }

    // Append instruction to end of function.
    programBuild->currentInstruction->next = newInstructNode;
    programBuild->currentInstruction       = newInstructNode;

    // Save the last instruction in main for when we finish
    //  scanning a function.
    if (programBuild->onMain == true)  
    {
        programBuild->mainLast = newInstructNode;
    }
}

// Adds a new function header to the end of the function queue.
void makeNewFunction(program_build_t* programBuild) 
{
    // set other values.
    programBuild->functionAmount += 1;
    programBuild->onMain          = false;

    // Allocates and initialize memory
    function_header_t* newFunction = malloc(sizeof(function_header_t));
    newFunction->next              = NULL;
    newFunction->head              = dummyInstruction();
    newFunction->depth             = 1;
    newFunction->ifTracker         
        = createIfTracker(programBuild->functionAmount);

    // Assigns build pointer functions 
    programBuild->currentInstruction = newFunction->head;
    programBuild->lastFunction->next = newFunction;
    programBuild->lastFunction       = newFunction;

}

// Places return instruction on end of the instruction queue,
//  and sets things back to main.
void endFunction(program_build_t* programBuild) 
{
    // Append the return instruction to end of function
    appendInstruction(programBuild, i_return, 0, 0);

    // End the function's instruction list
    programBuild->currentInstruction->next = NULL;

    // Set things back onto main
    programBuild->currentInstruction = programBuild->mainLast;
    programBuild->onMain             = true;
}

// Takes an instruction node and converts it to an instruction data struct
instruction_t convertInstructionNode(instruction_node_t* instruction)
{
    instruction_t flatInstruction;

    flatInstruction.instruction = instruction->instruction;
    flatInstruction.arg1        = instruction->arg1;
    flatInstruction.arg2        = instruction->arg2;

    return flatInstruction;
}

// takes a programBuild object and creates a complete 
//  initialized program context object ready for execution.
program_context_t returnProgram(program_build_t* programBuild)
{
    // Append the end program instruction
    appendInstruction(programBuild, i_endprog, 0, 0);

    // Initialize the program object.
    program_context_t program;
    program.functionStack.depth = 0;

    // Initialize the function array.
    program.code = calloc(programBuild->functionAmount,
                          sizeof(instruction_t*));

    // Fill out the function array
    unsigned int functionIndex = 0;
    function_header_t* tracer  = programBuild->programTop;

    while (tracer != NULL)
    {
        // Allocate enough memory for the function to fit into an array
        program.code[functionIndex] 
            = calloc(tracer->depth, sizeof(instruction_t));

        // Beging the tracer and index marker
        instruction_node_t* instructionTracer = tracer->head;
        unsigned int instructionIndex         = 0;

        while (instructionTracer != NULL)
        {
            // Convert the instruction node and save it in the proper
            //  place int he array.
            (program.code[functionIndex])[instructionIndex]
                = convertInstructionNode(instructionTracer);

            // Deallocate the instruction node and move to next node.    
            instruction_node_t* freeInstruction = instructionTracer;
            instructionTracer                   = instructionTracer->next;
            ++instructionIndex;
            free(freeInstruction);
        }

        // Update tracer and index marker.
        ++functionIndex;
        tracer = tracer->next;
    }

    //Initialize the static data bank
    size_t bankSize                 = programBuild->constantDataList.depth;
    program.staticDataBank.size     = bankSize;
    program.staticDataBank.dataBank = calloc(bankSize, sizeof(static_data_t));

    // Fill out the static data bank, using another tracer.
    constant_data_list_node_t* staticDataTracer 
        = programBuild->constantDataList.top;

    while (staticDataTracer != NULL)
    {
        // Assigning type here may be redundant
        program.staticDataBank.dataBank[staticDataTracer->eventualIndex].type
            = staticDataTracer->type;
        program.staticDataBank.dataBank[staticDataTracer->eventualIndex].data 
            = staticDataTracer->data;
        staticDataTracer = staticDataTracer->next;
    }

    // Initialize function stack
    program.functionStack.depth = 0;
    program.functionStack.head  = NULL;

    return program;
}
