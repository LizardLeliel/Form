#ifndef BUILD_HEADER
#include "instructions.h"

/* A struct suitable for creating a list of functions
 *  The top of this function is the function which gets
 *  executed first; and will be outside of any function declaration,
 *  adding new functions will be treated as a queue
 */
typedef struct function_header_struct {
    instruction_t* head;
    struct function_header_struct* next;

} functionHead_t;

<<<<<<< HEAD
// I normally don't be this in-depth with something, but these four pointers
//  are so important I figured I'll do this kind of commenting
=======
>>>>>>> fbaab0080dc700dae1c8ddc879a0d3c19c65ef50
/* A collection of pointers which are important to the contruction
 *  the program's instruction squences
 * -----
 * @programTop: A pointer to the top of function list, or the 'main' function
 * @lastFunction: The last function, new function declarations will be added
 *  after the end
 * @mainLast: The last instruction added to the end of main. Once a function
 *  declaration ends, any new instructions will be added to the end of this,
 *  and any new function declarations will be valid
<<<<<<< HEAD
 * @currentInstruction: The last instruction appened, regardless of lists. Any
=======
 * @currentInstructi: The last instruction appened, regardless of lists. Any
>>>>>>> fbaab0080dc700dae1c8ddc879a0d3c19c65ef50
 *  new instruction will be added to this.
 * -----
 * Keep in mind that both function lists and instruction lists are implemented
 *  as queues; but both are different. There will only be one function queue;
 *  and each point to a different instruction list in memory. There are
 *  instruction queues; as many as there are functions (including "main")
 */
struct build_pointer_struct {
    functionHead_t* programTop;
    functionHead_t* lastFunction;
    instruction_t*  mainLast;
    instruction_t*  currentInstruct;

};

/* The instructions are NULL-initialized; It is currently externed
 *  for test.c (which I usually don't include in git repositories).
 *  It will be removed if its found not to be needed
 */
extern struct build_pointer_struct buildPointers;
/* The ammount of functions currently, including "main". Initialized
 *  to one
 */
extern unsigned int functionNumber;

/* Allocates various memory for the functions queue, and sets initial
 *  values to variables in struct "buildPointers"
 */
void instructionIni();

/* Adds a new instruction to buildPointers.currentInstruct */
void appendInstruction
(
    instructionType_t newInstruct,
    size_t argSize,
    void* args
);

/* Makes a dummy head */
inline instruction_t* dummyInstruction();

/* Adds a new function to the end of the function queue; sets build pointers
 *  to appriopriate values
 */
void makeNewFunction();

/* Things! (Make a better explanation) */
void endFunction();

/* Currently undefined; it will create an appriopriate array
 *  for goto look-up and function calls; as well as return head.
 *  It will also snip dummy instruction queue heads unless function
 *  is emptey; i.e. only contains "return"
 */
instruction_t** returnProgram();

#define BUILD_HEADER
#endif
