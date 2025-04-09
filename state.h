#ifndef STATE_H
#define STATE_H

#include "linalg.h"

typedef struct {
  mat4 proj;
  mat4 view;
  mat4 model;
  float time;
} global_state;

#endif
