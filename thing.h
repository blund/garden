#ifndef THING_H
#define THING_H

#include "stdint.h"

#include <glad/glad.h>

#include "linalg.h"

typedef enum {
    THING_TYPE_WAVES,
} thing_type;

typedef struct thing {
  thing_type type;

  uint32_t ebo, vao, vbo;
  uint32_t shader_program;

} thing;

void compile_shader(thing *t, const char *vs_path, const char *fs_path);

void set_uniforms(thing *t, mat4 projection, mat4 view, mat4 model, float time);

#endif
