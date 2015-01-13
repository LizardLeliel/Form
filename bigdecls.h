#ifndef BIG_DECLARATIONS_HEADER
#include <inttypes.h>
#include <stdlib.h>

/* This file contains declarations of big items, such
 *  as enumerations
 */

/* Instruction set enumeration - each member represents
 *  and action that a bytecode instruction can perform.
 *  Any word that is a plural but has no reason to be plural
 *  is done as to avoid naming conflicts
 */
typedef enum intsruction_set {
    // No operation
    nop = 0,

    // Standard arthemtic operations
    add,
    sub,
    mul,
    divs,
    mod,

    // Standard bitwise operations
    bitwand,
    bitwor,
    bitwnot,
    bitwleft,
    bitwright,
    bitwxor,

    // Standard comparison operations
    lessthan,
    lessthaneq,
    morethan,
    morethaneq,
    eq,
    ineq,

    // Boolean logic operations
    logand,
    logor,
    lognot,

    // Increment and deincrement operations
    incr,
    decr,

    // Standard stack-related operations
    push,
    drop,
    over,
    swaps,
    rot,

    // Form goto operations
    gotos,
    congotos,

    // Variable-related operations
    scope,
    assigns,
    get,

    // Misc operations (function call, return, output, end program)
    call,
    returns,
    print,
    endProg,

    // This isn't an instruction; this is just as the size of the enumeration
    //  if this enum's size changes, this will reflect that, and... things
    instruction_ammount,

} instructionType_t;

/* An enumeration of hashed data types */
typedef enum hash_type {
    h_nothing = 0,     // 0
    h_variableName,   // 1
    h_functionName,  // 2
    h_labelName,    // 3
    h_gotoMemory,  // 4
} hashType_t;     // I couldn't help myself with the comments!

/* An enumeration of run-time data types */
typedef enum data_type {
    nil = 0,
    f_int,
    f_ulong,
    f_double,
    f_bool,
    f_char,
    f_var,
    f_table,
    f_array,
    f_ref,
} dataType_t;

/* Array of data sizes */
extern const size_t dataSize[10];

#define BIG_DECLARATIONS_HEADER
#endif
