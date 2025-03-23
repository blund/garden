#ifndef WAVES_H
#define WAVES_H

#include "stdint.h"
#include "malloc.h"


#include "../bl.h"
#include "../thing.h"

typedef struct {
  float *points;
  uint32_t *indices;

  // float points[array_size];
  // uint32_t indices[index_count];
  int n_points;
  int n_indices;
} waves;

void create_waves(thing*t);
void bind_waves(thing *t);
void render_waves(thing *t);

#endif
