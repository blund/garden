#ifndef LIGHT_H
#define LIGHT_H

#include "stdint.h"

#include "../state.h"
#include "../thing.h"

typedef struct {
  float vertices[48];
  uint32_t indices[36];
  vec3 pos;
} light_t;

void create_light(thing*t);
void bind_light(thing *t);
void render_light(global_state* s, thing *t);

#endif
