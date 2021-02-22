#include <stdio.h>
#include <stdlib.h>

#include <malloc.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <glad/glad.h>

#include "helpers.h"
#include "shader.h"

int loadShader(const char *shader_name, int shader_type) {
  // Last inn og kompiler fragment-shader-program
  int shader_id = glCreateShader(shader_type);
  const char *shader_source = loadFile(shader_name);
  int len = strlen(shader_source);
  glShaderSource(shader_id, 1, &shader_source, &len);
  glCompileShader(shader_id);

  // check for shader compile errors
  char infoLog[512];
  int success;
  glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader_id, 512, NULL, infoLog);
    printf("Kompilering av shader '%s' feilet:\n", shader_name);
    printf("\t%s\n", infoLog);
    return 0;
  }
  free((char *)shader_source);

  return shader_id;
}


/*
int createShader(char *fs_path, char *vs_path) {

  int fragment_shader = loadShader(fs_path, GL_FRAGMENT_SHADER);
  int vertex_shader   = loadShader(vs_path, GL_VERTEX_SHADER);

  // Link shadere til et shader objekt, og slett shaderene
  int shader_program = glCreateProgram();
  glAttachShader(shader_program, vertex_shader);
  glAttachShader(shader_program, fragment_shader);
  glLinkProgram(shader_program);

  // check for linking errors
  int success;
  char infoLog[512];
  glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shader_program, 512, NULL, infoLog);
    printf("Failed to link fragment shader\n");
    printf("%s", infoLog);
  } else {
    printf("\t Fikk kompilert shader med  %s and %s\n", fs_path, vs_path);
  }
  
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  return shader_program;
}
*/

int compileShader(Shader *shader, int verbose) {
  // Returnerer 0 når den filer

  int fragment_shader = loadShader(shader->f_path, GL_FRAGMENT_SHADER);
  int vertex_shader   = loadShader(shader->v_path, GL_VERTEX_SHADER);
  if (fragment_shader == 0) return 0;
  if (vertex_shader   == 0) return 0;
  
  // Link shadere til et shader objekt, og slett shaderene
  shader->id = glCreateProgram();
  glAttachShader(shader->id, vertex_shader);
  glAttachShader(shader->id, fragment_shader);
  glLinkProgram(shader->id);

  // check for linking errors
  int success;
  char infoLog[512];
  glGetProgramiv(shader->id, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shader->id, 512, NULL, infoLog);
    printf("\t!!! Kompilering av shader '%s' feilet\n", shader->name);
    // printf("%s", infoLog);
    return 0;
  } else {
    if (verbose)
      printf("\t['%s' ble kompilert]\n", shader->name);
  }

  // Hvis vi får kompilet, lagre når filen sist ble endret.
  // @URYDDIG - føler at vi angrepier io fra en del vinkler her, usikker på om det bør struktureres bedre

  struct stat statbuf;
  stat(shader->v_path, &statbuf);
  shader->v_modified = statbuf.st_mtim.tv_nsec;

  stat(shader->f_path, &statbuf);
  shader->f_modified = statbuf.st_mtim.tv_nsec;
  
  
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  return 1;
}


int hotloadShader(Shader *shader) {
  // 1  - ny shader hotloadet
  // 0  - shader ikke endret
  // -1 - feil ved kompilering av ny shader

  
  struct stat statbuf;
  stat(shader->v_path, &statbuf);
  int v_modified_new = statbuf.st_mtim.tv_nsec;

  stat(shader->f_path, &statbuf);
  int f_modified_new = statbuf.st_mtim.tv_nsec;


  // Lagre kritiske verdier
  int tmp_id       = shader->id;
  int tmp_v_m_time = shader->v_modified;
  int tmp_f_m_time = shader->f_modified;
  
  if (v_modified_new != shader->v_modified ||
      f_modified_new != shader->f_modified) {
    if(!compileShader(shader, false)) {
      // Her klarte vi ikke å kompilere den nye shaderen, og endrer derfor ikke id'en. Velger å oppdatere endringsdatoen, så vi ikke forsøker å rekompilere shaderen på nytt før en endring er gjort.
      
      shader->id = tmp_id;
      shader->v_modified = v_modified_new;
      shader->f_modified = f_modified_new;
      printf("\t!!! Klarte ikke å hotloade shader '%s'\n", shader->name);
      return -1;
    }
    
    printf("\t[Hotloadet shader '%s']\n", shader->name);
    return 1;
  }
  
  return 0;
}


void setMat4(int shader, char *param, mat4 value) {
  glUniformMatrix4fv(glGetUniformLocation(shader, param), 1, GL_FALSE, value);
}

void setInt(int shader, char *param, int value) {
  glUniform1i(glGetUniformLocation(shader, param), value);
}


void setVec3(int shader, char *param, vec3 value) {
  glUniform3fv(glGetUniformLocation(shader, param), 1, value);
}
