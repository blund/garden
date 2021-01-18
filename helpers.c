#include <malloc.h>
#include <sys/stat.h>

#include <glad/glad.h>
// #include <GLFW/glfw3.h>

// #include <GL/gl.h>

#include <string.h>
#include <stdlib.h>

#include "helpers.h"
#include "linalg.h"


void inline setMat4(int shader, char *param, mat4 value) {
  glUniformMatrix4fv(glGetUniformLocation(shader, param), 1, GL_FALSE, value);
}

void inline setInt(int shader, char *param, int value) {
  glUniform1i(glGetUniformLocation(shader, param), value);
}


void inline setVec3(int shader, char *param, vec3 value) {
  glUniform3fv(glGetUniformLocation(shader, param), 1, value);
}

char *loadFile(char *file_name)
{
  // @merk det som allokeres her må fris manuelt
  FILE    *infile;
  char    *buffer;
  long    numbytes;
 
  infile = fopen(file_name, "r");

  // @utrygg rapporter feil om vi mislykkes her
  fseek(infile, 0L, SEEK_END);
  numbytes = ftell(infile);
  fseek(infile, 0L, SEEK_SET);	
 
  buffer = (char *)malloc(numbytes+1);
  buffer[numbytes] = 0;
 

  fread(buffer, sizeof(char), numbytes, infile);
  fclose(infile);
 
  return buffer;
}

int loadShader(char *shader_name, GLenum shader_type) {
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
    printf("Failed to compile shader: %s\n", shader_name);
    printf("\t%s", infoLog);
    return -1;
  }
  free((char *)shader_source);

  return shader_id;

}




