#ifndef THING_H
#define THING_H

#include "stdint.h"
#include "stdio.h"

#include <glad/glad.h>

typedef struct thing {
  uint32_t ebo, vao, vbo;
  uint32_t shader_program;
  float* data;
} thing;

void compile_shader(thing *t, const char *vs_path, const char *fs_path);

#endif
