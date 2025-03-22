#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <errno.h>
#include <string.h>

#include <sys/stat.h>

#include "bl.h"

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
