#include "stdint.h"
#include "malloc.h"
#include "string.h"

#ifdef __EMSCRIPTEN__
  #include <GLES3/gl3.h>
#else
  #include <GL/glew.h>
#endif

#include "../bl.h"
#include "light.h"

void create_light(thing *t) {
  light_t *light = malloc(sizeof(light_t));

  t->data = light;
  t->type = THING_TYPE_LIGHT;

  light->pos[0] = 0;
  light->pos[1] = 3;
  light->pos[2] = -40;
  
  // Position      // Normals
  float vertices[] = {
    // Front face
    -0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,
    0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,
    0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,
    -0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,

    // Back face
    -0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,
    0.5f, -0.5f, -0.5f,   0.0f,  0.0f, -1.0f,
    0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,   0.0f,  0.0f, -1.0f,
  };

  memcpy(light->vertices, vertices, 48*sizeof(float));
  
  unsigned int indices[] = {
    // Front face
    0, 1, 2, 2, 3, 0,
    // Right face
    1, 5, 6, 6, 2, 1,
    // Back face
    5, 4, 7, 7, 6, 5,
    // Left face
    4, 0, 3, 3, 7, 4,
    // Top face
    3, 2, 6, 6, 7, 3,
    // Bottom face
    4, 5, 1, 1, 0, 4
  };

  memcpy(light->indices, indices, 36*sizeof(float));
 
  bind_light(t);
}

void bind_light(thing *t) {
  light_t* light = (light_t*)t->data;

  // Set up VAO for thing
  glGenVertexArrays(1, &t->vao);
  glGenBuffers(1, &t->vbo);
  glGenBuffers(1, &t->ebo);

  glBindVertexArray(t->vao);

  glBindBuffer(GL_ARRAY_BUFFER, t->vbo);
  glBufferData(GL_ARRAY_BUFFER, 48*sizeof(float), light->vertices, GL_STATIC_DRAW);

  // Element buffer
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, t->ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36*sizeof(float), light->indices, GL_STATIC_DRAW);
  
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3*sizeof(float)));
  glEnableVertexAttribArray(1);

}

void render_light(global_state* state, thing *thing) {
  glUseProgram(thing->shader_program);

  light_t* light = thing->data;

  mat4_translate(state->model, light->pos);
  
  set_uniforms(thing, state->proj, state->view, state->model, state->time);

  glBindVertexArray(thing->vao);
  glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void*)0);
}
