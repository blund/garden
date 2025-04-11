#include "stdint.h"
#include "malloc.h"

#include <GL/glew.h>

#include "raycast.h"

void create_raycast(thing *t) {
  raycast *r = malloc(sizeof(raycast));

  t->data = r;
  t->type = THING_TYPE_RAYCAST;

  r->hit_time = 0;

  bind_raycast(t);
}

void bind_raycast(thing *t) {
  raycast* r = (raycast*)t->data;

  // Set up VAO for thing
  glGenVertexArrays(1, &t->vao);
  glGenBuffers(1, &t->vbo);

  glBindVertexArray(t->vao);

  glBindBuffer(GL_ARRAY_BUFFER, t->vbo);
  glBufferData(GL_ARRAY_BUFFER, 3*sizeof(float), r->point, GL_DYNAMIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
}

void render_raycast(global_state* state, thing *thing) {
  glUseProgram(thing->shader_program);

  //raycast *r = thing->data;

  set_uniforms(thing, state->proj, state->view, state->model, state->time);
  //printf("%f %f %f\n", state->proj[0][0], state->view[0][0], state->model[0][0]);

  //glBindBuffer(GL_ARRAY_BUFFER, thing->vbo);
  //glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * sizeof(float), r->point);

  glBindVertexArray(thing->vao);
  glDrawArrays(GL_POINTS, 0, 1);
}
