#ifndef THING_H
#define THING_H

#include "stdint.h"

#include "../linalg.h"

// @TODO this should be generated from the file in things..
typedef enum {
    THING_TYPE_LAND,
    THING_TYPE_LIGHT,
    THING_TYPE_WAVES,
    THING_TYPE_RAYCAST,
} thing_type;

typedef struct thing {
  thing_type type;

  uint32_t ebo, vao, vbo;
  uint32_t shader_program;

  void* data;

} thing;

void compile_shader(thing *t, const char *vs_path, const char *fs_path);

void set_uniforms(thing *t, mat4 projection, mat4 view, mat4 model, float time);

#endif
