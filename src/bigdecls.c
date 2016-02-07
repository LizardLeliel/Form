#include "bigdecls.h"
#include "inttypes.h"
#include "stdlib.h"

/* Datasize array - defined here so it may be
 * externed in the header file.
 */
const size_t dataSize[10] =
{
    0,                  // Nil data
    sizeof(int32_t),    // Signed int
    sizeof(uint64_t),   // Unisgned long
    sizeof(double),     // Double
    sizeof(char),       // Boolean
    sizeof(char),       // Character
    0,                  // Var - variable size
    0,                  // Table - variable size
    0,                  // Array - variable size
    0                   // Ref   - variable size
};
