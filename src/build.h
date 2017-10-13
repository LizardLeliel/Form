#ifndef BUILD_HEADER
#include <stdbool.h>
#include "runtime.h"

typedef unsigned char byte;

// Hashing-related data types
// Enumeration of hashed data types
typedef enum hash_type {
    h_nothing = 0,     // = 0
    h_variableName,    // = 1
    h_functionName,    // = 2
    h_labelName,       // = 3
    h_gotoMemory,      // = 4
} hashType_t;     

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

/* A struct suitable for keeping track of declared
 *  variables, labels, and function names.
 */
typedef struct hash_bucket 
{
    hashType_t hashedType;
    char* symbol;
    size_t symbolLength;
    struct hash_bucket* next;

    union 
    {
        unsigned int ID;
        instruction_t* noArgInstruction;
    } contents;

} hash_bucket_t;

// These two structs and function are used in implementing
//  deallocation of hash buckets
typedef struct hash_bucket_list_node
{
    hash_bucket_t* entry;
    struct hash_bucket_list_node* next; 
} hash_bucket_list_node_t;

typedef struct hash_bucket_list
{
    hash_bucket_list_node_t* top;
} hash_bucket_list_t;


// 0xFFFF = 2^20 - 1 = 1048575
// This is literally not a runtime hash.
// Rename it!!!
static unsigned long maxArrayVal = 0xFFFF;
typedef struct token_hash
{  
    hash_bucket_t** hash;
    unsigned int typeCount[h_gotoMemory+1];
    hash_bucket_list_t cleanupList;
} token_hash_t;






typedef struct program_build
{
    // The hash
    token_hash_t       tokenHash;

    // Important pointers for building
    function_header_t* programTop;
    function_header_t* lastFunction;
    instruction_t*     mainLast;
    instruction_t*     currentInstruction;

    // Other
    bool onMain;
    unsigned int functionAmmount;
} program_build_t;




program_build_t prepareBuild();








// This is extern'd for test.c
// extern struct build_pointer_struct buildPointers;

// The ammount of functions, including main.
// extern unsigned int functionNumber;



/* Allocates various memory for the functions queue, and sets initial
 *  values to variables in struct "buildPointers".
 * Should be changed to "buildInit"
 */
void instructionIni();
//void hashIni();

// Make tries and hash type a parameter
token_hash_t makeTokenHash();

// Change this so it only reflects what is in build.h.
// That, and things have been changed to program_context_t.
void freeInstructions();
void freeHash(token_hash_t* tokenHash);

// Adds a new instruction to buildPointers.currentInstruct
void appendInstruction(program_build_t* programBuild,
                       instructionType_t newInstruct,
                       size_t            argSize,
                       void*             args);

// Makes a dummy head
inline instruction_t* dummyInstruction();

/* Adds a new function to the end of the function queue; sets build pointers
 *  to appriopriate values
 */
void makeNewFunction(program_build_t* programBuild);

// Ends a function
void endFunction(program_build_t* programBuild);

// Change this comment to better reflect the function.
/* Returns head of main. it eventually will create an appriopriate array
 *  for goto look-up and function calls; as well as return head.
 *  It will also snip dummy instruction queue heads unless function
 *  is emptey; i.e. only contains "return"
 */
program_context_t returnProgram(program_build_t* program);



// For testing
// extern hash_t HASH;

// The hashing function 
unsigned long hashFunction(size_t wordLength, const char* symbol);


// Set element of hash
unsigned int getHashID(token_hash_t* tokenHash,
                       hashType_t    toHashType,
                       size_t        symbolSize,
                       const char*   symbolName);


void pushToList(token_hash_t*  tokenHash, 
                hash_bucket_t* slot);

// Utilities
bool isWhiteSpace(char c);
char* trim(const char* string);

#define BUILD_HEADER
#endif
