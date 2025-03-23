#include "stdint.h"
#include "malloc.h"

#include <glad/glad.h>

#include "../bl.h"
#include "waves.h"

void create_waves(thing *t) {
  waves *w = w = malloc(sizeof(waves));

  t->data = w;
  //t->type = THING_TYPE_WAVES;

  const int n_per_side = 64;
  const int array_size = 3 * n_per_side * n_per_side;

  const int squares_per_side = n_per_side - 1;
  const int num_cells = squares_per_side * squares_per_side;
  const int index_count = num_cells * 6;

  w->n_points = array_size+1;
  w->n_indices = index_count+1;

  w->points = malloc(w->n_points*sizeof(float));
  w->indices = malloc(w->n_indices*sizeof(float));
  
  float start = -1;
  float end   = 1;

  float len  = end - start;
  float step = len / (n_per_side - 1);

  for (int x = 0; x < n_per_side; x++) {
    for (int y = 0; y < n_per_side; y++) {
      int i = 3 * (x + y*n_per_side);
      w->points[i + 0] = -1.0f + x * step; // x
      w->points[i + 1] = frand(0, 0.05);              // y
      w->points[i + 2] = -1.0f + y * step; // <
    }
  }

  int idx = 0;

  for (int y = 0; y < squares_per_side; y++) {
    for (int x = 0; x < squares_per_side; x++) {
      int top_left     = y * n_per_side + x;
      int top_right    = top_left + 1;
      int bottom_left  = top_left + n_per_side;
      int bottom_right = bottom_left + 1;

      // Triangle 1
      w->indices[idx++] = top_left;
      w->indices[idx++] = bottom_left;
      w->indices[idx++] = bottom_right;

      // Triangle 2
      w->indices[idx++] = top_left;
      w->indices[idx++] = bottom_right;
      w->indices[idx++] = top_right;
    }
  }

  bind_waves(t);
}

void bind_waves(thing *t) {
  waves* w = (waves*)t->data;

  // Set up VAO for thing
  glGenVertexArrays(1, &t->vao);
  glGenBuffers(1, &t->ebo);
  glGenBuffers(1, &t->vbo);

  glBindVertexArray(t->vao);

  glBindBuffer(GL_ARRAY_BUFFER, t->vbo);
  glBufferData(GL_ARRAY_BUFFER, w->n_points*sizeof(float), w->points, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, t->ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, w->n_indices*sizeof(float), w->indices, GL_STATIC_DRAW);
}

void render_waves(thing *t) {
  // glDrawArrays(GL_POINTS, 0, array_size / 3);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glBindVertexArray(t->vao);
  glDrawElements(GL_TRIANGLES, ((waves*)t->data)->n_indices, GL_UNSIGNED_INT, 0);
  
}
