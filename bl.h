#ifndef BL_H
#define BL_H

const char* read_file(const char* filename);
void _release_assert(const char *assertionExpr, const char *assertionFile,
                    unsigned int assertionLine, const char *assertionFunction);
float random_float(float min, float max);


#ifdef BL_IMPL

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>



/*
  assert and read_file are stolen from the LuaJIT-Remake :)
 */

void _release_assert(const char *assertionExpr,
                    const char *assertionFile,
                    unsigned int assertionLine,
                    const char* assertionFunction) {
  fprintf(stderr, "%s:%u: %s: Assertion `%s' failed.\n", assertionFile, assertionLine, assertionFunction, assertionExpr);
  abort();
}

#define bl_assert(expr)							\
  ((expr) ? ((void)0)							\
   : _release_assert(#expr, __FILE__, __LINE__, __extension__ __PRETTY_FUNCTION__))

#define assert bl_assert

const char* read_file(const char* filename) {
    size_t size;
    char* data = 0;

    struct stat st;
    if (stat(filename, &st) != 0)
    {
        fprintf(stderr, "Failed to access file '%s' for file size, errno = %d (%s)\n",
                filename, errno, strerror(errno));
        abort();
    }
    size = (size_t)(st.st_size);

    data = (char *)malloc(sizeof(char) * size + 1);

    // Make sure we terminate this scary C string :)
    data[size] = '\0';

    FILE* fp = fopen(filename, "r");
    if (fp == 0)
    {
        fprintf(stderr, "Failed to open file '%s' for read, errno = %d (%s)\n",
                filename, errno, strerror(errno));
        abort();
    }

    size_t bytesRead = fread(data, sizeof(char), size, fp);
    assert(bytesRead == size);

    {
        // just to sanity check we have reached eof
        uint8_t _c;
        assert(fread(&_c, sizeof(uint8_t), 1 /*numElements*/, fp) == 0);
        assert(feof(fp));
    }

    fclose(fp);

    return data;
}

float random_float(float min, float max) {
    float scale = rand() / (float) RAND_MAX; /* [0, 1.0] */
    return min + scale * ( max - min );      /* [min, max] */
}

#endif

#endif
