#include "hash.h"

#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

/* The hash function is implemented with an array size and not a null-
 *  terminated string because it'll be simplier to determine
 *  the range of the actual symbol (which, as text, is going to include
 *  extra chars, such as the white space on the end), instead of trim the
 *  extra chars and add the new null character and so on.
 * One other note: the whole value isn't used; the first few bits will be &'d,
 *  and used as the hash array index (so the hash array doesn't need to have as
 *  many elements equal to the highest calue of unsigned long; that would use
 *  too much unnesecary memory
 * The algorithm used is a variation of the sdbm algoritn
 *  (http://www.cse.yorku.ca/~oz/hash.html)
 */
unsigned long hashFunction(size_t wordLength, char* symbol) {
    unsigned long hash = 0;
    int c = 0;

    do {
        hash = symbol[c] + (hash << 6) + (hash << 16) - hash;
    } while (++c < wordLength);

    return hash;
}

