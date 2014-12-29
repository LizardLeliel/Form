#ifndef BUILD_HEADER
#include "instructions.h"

/* A struct suitable for creating a list of functions */
typedef struct function_header_struct {
    instruction_t* head;
    struct function_header_struct* next;

} functionHead_t;

struct build_pointer_struct {
    functionHead_t* programTop;
    functionHead_t* lastFunction;
    instruction_t*  mainLast;
    instruction_t*  currentInstruct;

};

extern struct build_pointer_struct buildPointers;
extern unsigned int functionNumber;

/* Prepare struct buildPointers */
void instructionIni();

/* Adds a new instruction to buildPointers.currentInstruct */
void appendInstruction
(
    instructionType_t newInstruct,
    size_t argSize,
    void* args
);

/* Creates an appriopriate ARRAY for the program;
 *  it also prepares other structures for use
 */
instruction_t** returnProgram();

#define BUILD_HEADER
#endif
