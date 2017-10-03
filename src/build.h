#ifndef BUILD_HEADER
#include "instructions.h"


/* A struct suitable for creating a list of functions
 *  The top of this function is the function which gets
 *  executed first; and will be outside of any function declaration,
 *  adding new functions will be treated as a queue
 */
typedef struct function_header
{
    instruction_t* head;
    struct function_header* next;

} function_header_t;


/* A collection of pointers which are important to the contruction
 *  the program's instruction squences
 * -----
 * programTop: A pointer to the top of function list, or the 'main' function
 * lastFunction: The last function, new function declarations will be added
 *  after the end
 * mainLast: The last instruction added to the end of main. Once a function
 *  declaration ends, any new instructions will be added to the end of this,
 *  and any new function declarations will be valid
 * currentInstruction: The last instruction appened, regardless of lists. Any
 *  new instruction will be added to this.
 */
struct build_pointer_struct 
{
    function_header_t* programTop;
    function_header_t* lastFunction;
    instruction_t*  mainLast;
    instruction_t*  currentInstruct;

};

// This is extern'd for test.c
extern struct build_pointer_struct buildPointers;

// The ammount of functions, including main.
extern unsigned int functionNumber;

/* Allocates various memory for the functions queue, and sets initial
 *  values to variables in struct "buildPointers"
 */
void instructionIni();
void freeInstructions();

// Adds a new instruction to buildPointers.currentInstruct
void appendInstruction
(
    instructionType_t newInstruct,
    size_t argSize,
    void* args
);

// Makes a dummy head
inline instruction_t* dummyInstruction();

/* Adds a new function to the end of the function queue; sets build pointers
 *  to appriopriate values
 */
void makeNewFunction();

// Ends a function
void endFunction();

/* Returns head of main. it eventually will create an appriopriate array
 *  for goto look-up and function calls; as well as return head.
 *  It will also snip dummy instruction queue heads unless function
 *  is emptey; i.e. only contains "return"
 */
instruction_t** returnProgram();

#define BUILD_HEADER
#endif
