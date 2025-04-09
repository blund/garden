#include "stdint.h"
#include "malloc.h"


#ifdef __EMSCRIPTEN__
  #include <GLES3/gl3.h>
#else
  #include <GL/glew.h>
#endif

#include "string.h"
#include "math.h"

#include "../bl.h"
#include "land.h"
#include "../state.h"

void create_land(thing *t) {
  land_t *land = land = malloc(sizeof(land_t));

  t->data = land;
  t->type = THING_TYPE_LAND;

  const float start = -20.0;
  const float end   = 20.0;
  const int n_per_side = 32;
  const int array_size = 3 * n_per_side * n_per_side;

  const int squares_per_side = n_per_side - 1;
  const int num_cells = squares_per_side * squares_per_side;
  const int index_count = num_cells * 6;

  land->n_points = array_size+1;
  land->n_indices = index_count+1;

  land->points = malloc(land->n_points*sizeof(float));
  land->indices = malloc(land->n_indices*sizeof(float));

  float len  = end - start;
  float step = len / (n_per_side - 1);
  
  float land_radius = 30;     // how far out the island spreads
  float land_height = 2;     // how tall the island is
  float inner_flat_radius = 3.0f; // fully flat zone in center
  vec3 center = {0.0f, 0.0f};  // center of the grid (assuming centered at 0)

  for (int x = 0; x < n_per_side; x++) {
    for (int y = 0; y < n_per_side; y++) {
      int i = 3 * (x + y * n_per_side);

      float px = start + x * step;
      float pz = start + y * step;

      float dx = px - center[0];
      float dz = pz - center[1];
      float dist = sqrtf(dx * dx + dz * dz);

      float height = 0.01f;

      if (dist < land_radius) {
	if (dist < inner_flat_radius) {
	  // Flat pancake top
	  height += land_height;
	} else {
	  // Jagged rim
	  float edge = (dist - inner_flat_radius) / (land_radius - inner_flat_radius);
	  float h = 1.0f - edge;

	  // Jagged bump using sine waves (or random noise)
	  float jagged = 0.5f + 0.5f * sinf(px * 40.0f + pz * 35.0f);
	  height += land_height * h * jagged;
	}
      }

      land->points[i + 0] = px;
      land->points[i + 1] = height-1.5;
      land->points[i + 2] = pz;
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
      land->indices[idx++] = top_left;
      land->indices[idx++] = bottom_left;
      land->indices[idx++] = bottom_right;

      // Triangle 2
      land->indices[idx++] = top_left;
      land->indices[idx++] = bottom_right;
      land->indices[idx++] = top_right;
    }
  }

  bind_land(t);
}

void bind_land(thing *t) {
  land_t* land = (land_t*)t->data;

  // Set up VAO for thing
  glGenVertexArrays(1, &t->vao);
  glGenBuffers(1, &t->ebo);
  glGenBuffers(1, &t->vbo);

  glBindVertexArray(t->vao);

  glBindBuffer(GL_ARRAY_BUFFER, t->vbo);
  glBufferData(GL_ARRAY_BUFFER, land->n_points*sizeof(float), land->points, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, t->ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, land->n_indices*sizeof(uint32_t), land->indices, GL_STATIC_DRAW);
}

void render_land(global_state* state, thing *thing) {
  glUseProgram(thing->shader_program);

  mat4_identity(state->model); // move cube right and into the scene
  set_uniforms(thing, state->proj, state->view, state->model, state->time);
  
  glBindVertexArray(thing->vao);
  glDrawElements(GL_TRIANGLES, ((land_t*)thing->data)->n_indices, GL_UNSIGNED_INT, 0);
}
