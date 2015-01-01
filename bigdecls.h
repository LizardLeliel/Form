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

    // Misc operations (function call, return output, end program)
    call,
    returns,
    print,
    endProg,

} instructionType_t;

/* an enumeration of data types */
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
