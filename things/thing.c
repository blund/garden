
#include "stdlib.h"
#include "stdio.h"

#ifdef __EMSCRIPTEN__
  #include <GLES3/gl3.h>
#else
  #include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>

#include "read_file.h"
#include "thing.h"

void set_uniforms(thing *t, mat4 projection, mat4 view, mat4 model, float time) {
  int projection_location = glGetUniformLocation(t->shader_program, "projection");
  glUniformMatrix4fv(projection_location, 1, GL_FALSE,
		     (const float *)projection);
    
  int view_location = glGetUniformLocation(t->shader_program, "view");
  glUniformMatrix4fv(view_location, 1, GL_FALSE, (const float *)view);

  int model_location = glGetUniformLocation(t->shader_program, "model");
  glUniformMatrix4fv(model_location, 1, GL_FALSE, (const float *)model);

  int time_location = glGetUniformLocation(t->shader_program, "u_time");
  glUniform1f(time_location, time);
}

void compile_shader(thing *t, const char *vs_path, const char *fs_path) {
  // create shader...
  int  success;
  char info_log[512];
  // load and compile vertex shader!
  const char* vertex_shader_code = read_file(vs_path);
  unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  // printf("'%s'\n", vertex_shader_code);
  glShaderSource(vertex_shader, 1, &vertex_shader_code, NULL);
  glCompileShader(vertex_shader);

  // check if compilation failed :)
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
  if(!success) {
    glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
    fprintf(stderr, "Failed compiling vertex shader: %s\n", info_log);
    return;
  }
  
  // load and compile fragment shader!
  const char *fragment_shader_code = read_file(fs_path);
  // printf("'%s'\n", fragment_shader_code);
  unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_shader_code, NULL);
  glCompileShader(fragment_shader);

  // check if compilation failed :)
  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
  if(!success) {
    glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
    fprintf(stderr, "Failed compiling fragment shader: %s\n", info_log);
    return;
  }

  // compile entire shader program
  unsigned int shader_program = glCreateProgram();
  glAttachShader(shader_program, vertex_shader);
  glAttachShader(shader_program, fragment_shader);
  glLinkProgram(shader_program);

  glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
  if(!success) {
    glGetProgramInfoLog(shader_program, 512, NULL, info_log);
    fprintf(stderr, "Failed linking shader program : %s\n", info_log);
  }

  // delete shaders now that program is linked!
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  free((void*)vertex_shader_code);
  free((void *)fragment_shader_code);

  t->shader_program = shader_program;
}
