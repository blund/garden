#include <stdio.h>
#include <stdlib.h>

/*
  assert and read_file are stolen from the LuaJIT-Remake :)
 */

void release_assert(const char *assertionExpr,
                    const char *assertionFile,
                    unsigned int assertionLine,
                    const char* assertionFunction) {
  fprintf(stderr, "%s:%u: %s: Assertion `%s' failed.\n", assertionFile, assertionLine, assertionFunction, assertionExpr);
  abort();
}

#define bl_assert(expr)							\
  ((expr) ? ((void)0)							\
   : release_assert(#expr, __FILE__, __LINE__, __extension__ __PRETTY_FUNCTION__))

#define assert bl_assert

float frand(float min, float max) {
    float scale = rand() / (float) RAND_MAX; /* [0, 1.0] */
    return min + scale * ( max - min );      /* [min, max] */
}
