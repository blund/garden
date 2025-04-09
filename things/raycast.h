#ifndef RAYCAST_H
#define RAYCAST_H

#include "stdint.h"

#include "../state.h"
#include "thing.h"

typedef struct {
  vec3 point;
  float hit_time;
} raycast;

void create_raycast(thing*t);
void bind_raycast(thing *t);
void render_raycast(global_state* s, thing *t);

#endif
