#ifndef LAND_H
#define LAND_H

#include "stdint.h"

#include "../thing.h"
#include "../state.h"

typedef struct {
  float *points;
  uint32_t *indices;

  int n_points;
  int n_indices;
} land_t;

void create_land(thing*t);
void bind_land(thing *t);
void render_land(global_state* s, thing *t);

#endif
