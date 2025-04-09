#include "stdint.h"
#include "malloc.h"

#include <GL/glew.h>

#include "../bl.h"
#include "waves.h"
#include "../state.h"

void create_waves(thing *t) {
  waves_t *w = w = malloc(sizeof(waves_t));

  t->data = w;
  t->type = THING_TYPE_WAVES;

  const float start = -30;
  const float end   = 30;
  const int n_per_side = 2*1024;
  const int array_size = 3 * n_per_side * n_per_side;

  const int squares_per_side = n_per_side - 1;
  const int num_cells = squares_per_side * squares_per_side;
  const int index_count = num_cells * 6;

  w->n_points = array_size+1;
  w->n_indices = index_count+1;

  w->points = malloc(w->n_points*sizeof(float));
  w->indices = malloc(w->n_indices*sizeof(float));

  float len  = end - start;
  float step = len / (n_per_side - 1);

  for (int x = 0; x < n_per_side; x++) {
    for (int y = 0; y < n_per_side; y++) {
      int i = 3 * (x + y*n_per_side);
      w->points[i + 0] = start + x * step; // x
      w->points[i + 1] = 0.01; //frand(0, 0.05); // y
      w->points[i + 2] = start + y * step; // z
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
  waves_t* w = (waves_t*)t->data;

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
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, w->n_indices*sizeof(uint32_t), w->indices, GL_STATIC_DRAW);
}

void render_waves(global_state* state, thing *thing, vec3 ripple_origin, float ripple_start_time) {
  glUseProgram(thing->shader_program);

  mat4_identity(state->model); // move cube right and into the scene
  set_uniforms(thing, state->proj, state->view, state->model, state->time);
  
  int ripple_origin_location = glGetUniformLocation(thing->shader_program, "u_ripple_origin");
  glUniform3fv(ripple_origin_location, 1, ripple_origin);

  int ripple_start_time_location = glGetUniformLocation(thing->shader_program, "u_ripple_start_time");
  glUniform1f(ripple_start_time_location, ripple_start_time);
  
  glBindVertexArray(thing->vao);
  glDrawElements(GL_TRIANGLES, ((waves_t*)thing->data)->n_indices, GL_UNSIGNED_INT, 0);
}
