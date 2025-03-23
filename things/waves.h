#ifndef WAVES_H
#define WAVES_H

#include "stdint.h"

#include "../thing.h"

typedef struct {
  float *points;
  uint32_t *indices;

  int n_points;
  int n_indices;
} waves;

void create_waves(thing*t);
void bind_waves(thing *t);
void render_waves(thing *t);

#endif
