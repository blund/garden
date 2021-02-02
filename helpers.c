#include <malloc.h>
#include <sys/stat.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string.h>
#include <stdlib.h>

#include <stb/stb_image.h> // Laster kun inn funksjonsdeklarasjoner

#include "helpers.h"
#include "linalg.h"


char *loadFile(char *file_name)
{
  /* declare a file pointer */
  FILE    *infile;
  char    *buffer;
  long    numbytes;
 
  /* open an existing file for reading */
  infile = fopen(file_name, "r");
 
  /* quit if the file does not exist */
  // if(infile == NULL)
  //   return 1;
 
  /* Get the number of bytes */
  fseek(infile, 0L, SEEK_END);
  numbytes = ftell(infile);
 
  /* reset the file position indicator to 
     the beginning of the file */
  fseek(infile, 0L, SEEK_SET);	
 
  /* grab sufficient memory for the 
     buffer to hold the text */
  //buffer = (char*)calloc(numbytes+1,1);
  buffer = (char *)malloc(1024);
  buffer[numbytes] = 0;
 
  /* memory error */
  // if(buffer == NULL)
  //   return 1;
 
  /* copy all the text into the buffer */
  fread(buffer, sizeof(char), numbytes, infile);
  fclose(infile);
 
  /* confirm we have read the file by
     outputing it to the console */
 
  /* free the memory we used for the buffer */
  return buffer;
  //free(buffer);


}

int loadTexture(char *texture_path) {
  //
  // Load texture file
  //
  glActiveTexture(GL_TEXTURE0);
  unsigned int texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


  int width, height, nr_channels;
  unsigned char *data = stbi_load(texture_path, &width, &height, &nr_channels, 0);
  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    printf("Klarte ikke å laste inn bildet ved %s!\n", texture_path);
  }


  stbi_image_free(data);

  return texture;
}


int loadShader(char *shader_name, int shader_type) {
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
    printf("Failed to compile shader: %s\n", shader_name);
    printf("\t%s", infoLog);
    return -1;
  }
  free((char *)shader_source);

  return shader_id;
}

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
    printf("Failed to link fragment shader");
    printf("%s", infoLog);
  } else {
    printf("\t Fikk kompilert shader med  %s and %s\n", fs_path, vs_path);
  }
  
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  return shader_program;
}

void inline setMat4(int shader, char *param, mat4 value) {
  glUniformMatrix4fv(glGetUniformLocation(shader, param), 1, GL_FALSE, value);
}

void inline setInt(int shader, char *param, int value) {
  glUniform1i(glGetUniformLocation(shader, param), value);
}


void inline setVec3(int shader, char *param, vec3 value) {
  glUniform3fv(glGetUniformLocation(shader, param), 1, value);
}
