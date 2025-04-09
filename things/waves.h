#ifndef WAVES_H
#define WAVES_H

#include "stdint.h"

#include "../thing.h"
#include "../state.h"

typedef struct {
  float *points;
  uint32_t *indices;

  uint32_t n_points;
  uint32_t n_indices;
} waves_t;

void create_waves(thing*t);
void bind_waves(thing *t);
void render_waves(global_state* s, thing *t, vec3 ripple_origin, float ripple_start_time);

#endif
